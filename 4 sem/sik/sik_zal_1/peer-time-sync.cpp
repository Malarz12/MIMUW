#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <climits>
#include <string>
#include <chrono>
#include <fcntl.h>
#include <vector>

#include "err.h"
#include "list.h"
#include "message.h"
#include "helpers.h"

using namespace std;
using namespace chrono;
using namespace helpers;

int main(int argc, char *argv[]) {
    // Parse command line arguments into an options structure.
    Options opts;
    parse_arguments(argc, argv, opts);

    // Create and bind a UDP socket to the specified address and port.
    int socket_fd = create_and_bind_socket(opts);

    // Containers for peer addresses and HELLO_REPLY-derived contacts.
    ListOfSockaddr addrList;
    ListOfSockaddr receivedContacts;

    // Send an initial HELLO message if a peer address/port was configured.
    send_initial_hello(socket_fd, opts);


    // Variables tracking synchronization state and timing.
    int synchronization = 255;
    auto start_time = steady_clock::now();
    auto last_START_SYNC = start_time;
    auto offset = milliseconds(0);
    milliseconds T1(0), T2(0), T3(0), T4(0);
    bool leader = false;
    
    // Structures representing current sync source and ongoing sync target.
    Node synchronized_to, synchronizing_to;
    synchronized_to.does_exist = false;
    synchronizing_to.does_exist = false;
    auto now = steady_clock::now();
    Message recvMsg;

    // Enter the main loop handling both incoming messages and scheduled tasks.
    while(true) {
        // If our current sync source has not responded in time, reset.
        check_source_timeout(synchronized_to, leader, synchronization);

        // Every 5 seconds (if not fully synchronized), send SYNC_START.
        try_send_start_sync(socket_fd, addrList, synchronization, last_START_SYNC, start_time, offset);

        // Prepare a buffer for incoming UDP packets.
        vector<uint8_t> recv_buf(655360);
        sockaddr_in sender_addr{};
        socklen_t addr_len = sizeof(sender_addr);

        // Block (non-blocking socket) until data arrives or we loop back.
        ssize_t recv_bytes = recvfrom(socket_fd, recv_buf.data(), recv_buf.size(), 0,
                                      reinterpret_cast<sockaddr*>(&sender_addr), &addr_len);
        if (recv_bytes < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) continue;
            error("recvfrom");
            continue;
        }

        // Attempt to parse the raw bytes into a Message.
        try {
            recvMsg = deserialize(recv_buf.data(), static_cast<size_t>(recv_bytes));
        } catch (const exception &ex) {
            error(string(reinterpret_cast<char*>(recv_buf.data()),
                         static_cast<size_t>(min<ssize_t>(10, recv_bytes))));
            continue;
        }

        // Convert sender's port from network to host order.
        uint16_t sender_port = ntohs(sender_addr.sin_port);
        char ip_buf[INET_ADDRSTRLEN];
        if(!inet_ntop(AF_INET, &sender_addr.sin_addr, ip_buf, sizeof(ip_buf))){
            error("recvfrom");
            continue;
        };
        string sender_ip_str = ip_buf;

        // Drop any packet that appears to originate from ourselves.
        if (sender_ip_str == opts.bind_address && sender_port == opts.port) {
            error("message from self ignored");
            continue;
        }
        // Dispatch based on the message code in the header.
        switch (recvMsg.message) {
            case 1: {
                // HELLO: reply with HELLO_REPLY and remember this peer.
                Message reply = make_HELLO_REPLY(addrList);
                if(!send_message(socket_fd, reply, sender_addr, "HELLO_REPLY")){
                    error("message not send");
                }
                if(!addrList.add(ip_buf, sender_port)){
                    error("not added");
                }
                break;
            }
            case 2: {
                // HELLO_REPLY: validate the contact list, then CONNECT.
                if (sender_ip_str != opts.peer_address || opts.peer_port != sender_port) {
                    error("HELLO_REPLY");
                    break;
                }
                if(!handle_hello_reply_contacts(recvMsg,sender_ip_str, 
                        sender_port, 
                        opts.bind_address, 
                        opts.port, 
                        receivedContacts))
                {
                    error("HELLO_REPLY");
                    break;
                }
                if(!addrList.add(ip_buf, sender_port)) {
                    error("not added");
                }
                if(!send_connects(socket_fd, receivedContacts)) {
                }
                break;
            }
            case 3: {
                // CONNECT: acknowledge with ACK_CONNECT and track sender.
                if(!addrList.add(ip_buf, sender_port)) {
                    error("not added");
                };
                if(!send_message(socket_fd, make_ACK_CONNECT(), sender_addr, "ACK_CONNECT")) {
                    error("message not send");
                }
                break;
            }
            case 4: {
                // ACK_CONNECT: simply add the sender to the list.
                if(!addrList.add(ip_buf, sender_port)) {
                    error("not added");
                };
                break;
            }
            case 11: {
                // SYNC_START: initiate delay request if conditions permit.
                if (leader) {
                    error("is leader");
                    break;
                }

                // If already synchronizing, skip or timeout.
                if (synchronized_to.does_exist && sender_ip_str == synchronized_to.address && sender_port == synchronized_to.port) {
                    synchronized_to.last_heard = steady_clock::now();
                    if (recvMsg.synchronized >= synchronization) {
                        synchronization = 255;
                        synchronized_to.does_exist = false;
                    }
                }

                // Validate peer is known and sync level is acceptable.
                if (synchronizing_to.does_exist) {
                    now = steady_clock::now();
                    if (duration_cast<milliseconds>(now - start_time - T3) > seconds(5)) {
                        synchronizing_to.does_exist = false;
                    }
                    if (synchronizing_to.does_exist) {
                        error("already synchronizing");
                        break;
                    }
                }
                if (!is_known_peer(addrList, sender_ip_str, sender_port)) {
                    error("sender not known");
                    break;
                }
                
                if (recvMsg.synchronized >= 254) {
                    error("too low sync level");
                    break;
                } 
                
                if (synchronized_to.does_exist == true) {
                    if (sender_ip_str == synchronized_to.address && sender_port == synchronized_to.port) {
                        if (recvMsg.synchronized >= synchronization) {
                            error("too low sync level");
                            break;
                        }
                    } else {
                        if (recvMsg.synchronized >= synchronization - 1) {
                            error("too low sync level");
                            break;
                        }
                    }
                }

                // Record timestamps and send DELAY_REQUEST.
                T1 = milliseconds(recvMsg.timestamp);
                T2 = duration_cast<milliseconds>(steady_clock::now() - start_time);
                synchronizing_to.port = sender_port;
                synchronizing_to.address = sender_ip_str;
                synchronizing_to.synchronization = recvMsg.synchronized;
                synchronizing_to.does_exist = true;
                synchronizing_to.last_heard = steady_clock::now();
                
                Message msg = make_DELAY_REQUEST();
                if(!send_message(socket_fd, msg, sender_addr, "DELAY_REQUEST")) {
                    error("message not send");
                    synchronizing_to.does_exist = false;
                };
                
                T3 = duration_cast<milliseconds>(steady_clock::now() - start_time);
                break;
            }      
            case 12: {
                // DELAY_REQUEST: respond with DELAY_RESPONSE.
                if (!is_known_peer(addrList, sender_ip_str, sender_port)) {
                    error("sender not known");
                    break;
                }
                Message msg = make_DELAY_RESPONSE(synchronization, start_time, offset);
                if(!send_message(socket_fd, msg, sender_addr, "DELAY_RESPONSE")) {
                    error("message not send");
                }
                break;
            }
            case 13: {
                // DELAY_RESPONSE: compute one-way delay offset
                if (leader) {
                    synchronizing_to.does_exist = false;
                    error("received by leader");
                    break;
                }
                T4 = milliseconds(recvMsg.timestamp);
                // Validate matching sync request context.
                if (!synchronizing_to.does_exist) {
                    break;
                } else if (sender_ip_str != synchronizing_to.address || sender_port != synchronizing_to.port) {
                    error("wrong sender");
                    break;
                } else if (synchronizing_to.synchronization != recvMsg.synchronized) {
                    synchronizing_to.does_exist = false;
                    error("synchronization has finished");
                    break;
                } else if (duration_cast<milliseconds>(T4 - T1) > seconds(5)) {
                    synchronizing_to.does_exist = false;
                    error("timeout");
                    break;
                } else if (duration_cast<milliseconds>(steady_clock::now() - start_time - T3) > seconds(5)) {
                    synchronizing_to.does_exist = false;
                    error("timeout");
                    break;
                } else if (T1 > T4){
                    // Check for unreasonable delay
                    error("negative delay detected");
                } else {
                    // Compute offset and update sync level
                    offset = duration_cast<milliseconds>((T2 - T1 + T3 - T4) / 2);
                    synchronization = recvMsg.synchronized + 1;

                    synchronizing_to.does_exist = false;
                    synchronized_to.address = sender_ip_str;
                    
                    synchronized_to.port = sender_port;
                    synchronized_to.synchronization = recvMsg.synchronized;
                    synchronized_to.last_heard = steady_clock::now();
                    synchronized_to.does_exist = true;
                }
                break;
            }

            case 21: {
                // LEADER: handle leadership announcement or resignation.
                if (recvMsg.synchronized == 0 && !leader) {
                    leader = true;
                    synchronization = 0;
                    synchronized_to.does_exist = false;
                    synchronizing_to.does_exist = false;
                    offset = milliseconds(0);
                    unsigned int remaining = 2;
                    // Pause briefly before next sync cycle.
                    while (remaining > 0) {
                        remaining = sleep(remaining);
                    }
                } else if (recvMsg.synchronized == 255 && leader) {
                    synchronized_to.does_exist = false;
                    leader = false;
                    synchronization = 255;
                } else {
                    error("invalid LEADER message");
                }
                break;
            }
            case 31: {
                // GET_TIME: provide current time reading.
                Message resp = make_TIME(synchronization, start_time, offset);
                if(!send_message(socket_fd, resp, sender_addr, "TIME")) {
                    error("message not send");
                }
                break;
            }
            default:
                // Unrecognized message code.
                error("wrong message type");
        }
    }

    // Close socket on exit, reporting any error.
    if (close(socket_fd) < 0) {
        syserr("close");
    }
    
    return 0;
}
