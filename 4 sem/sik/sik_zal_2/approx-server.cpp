// Standard C++ and POSIX headers that provide containers, algorithm helpers,
// file-descriptor utilities, networking primitives, and system calls.
#include <vector>
#include <list>
#include <unordered_map>
#include <string>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <csignal>
#include <poll.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <inttypes.h>
#include <endian.h>
#include <fstream>
#include <netdb.h>
#include <csignal>

// Project-specific modules that implement the wire protocol, player logic, and
// common helpers shared between client and server.
#include "message.hpp"
#include "player.hpp"
#include "common.hpp"
#include "err.h"

// Compile-time constants that control buffer sizes, poll() limits, and protocol
// thresholds for the entire lifetime of the server.
#define BUFFER_SIZE   100000
#define QUEUE_LENGTH  100000
#define TIMEOUT       100
#define CONNECTIONS   100000
#define CHUNK_SIZE    1024

// The global namespace gathers configuration parameters and runtime state so
// every function in this file can access them without excessive argument lists.
namespace global {
    // Game configuration parsed from the command line.
    uint16_t port = 0;          // TCP port on which the server listens.
    int k = 100;                // Number of turns for each player.
    int n = 4;                  // Degree of the secret polynomial.
    int m = 131;                // Threshold on cumulative “m” after which the game ends.
    std::string filename = "";  // Path to the coefficient file.

    // Dynamic game state that evolves while the program is running.
    int current_m = 0;          // Current total of m contributed by all players.
    bool finish = false;        // True once SCORING is sent and the game is over.
    size_t active_clients = 0;  // Count of connected sockets that are still alive.

    // Containers holding objects that represent connected players and helpers
    // that track arrivals requiring special handling.
    std::unordered_map<int, player::Player> players_map;
    std::list<int> just_connected;

    // next_tick records when the next one-second timer event should fire.
    auto next_tick = std::chrono::steady_clock::now() + std::chrono::seconds(1);
}


// disconnect_client performs a full cleanup when a socket needs to be removed
// from the poll set, freeing resources and updating global counters.
void disconnect_client(int index, pollfd poll_descriptors[], std::unordered_map<int, player::Player>& players_map) {
    int fd = poll_descriptors[index].fd;
    if (fd < 0) return;
    close(fd);
    auto it = players_map.find(fd);
    if (it != players_map.end()) {
        global::current_m -= it->second.get_m();
        players_map.erase(it);
    }
    poll_descriptors[index].fd      = -1;
    poll_descriptors[index].events  = 0;
    poll_descriptors[index].revents = 0;
    --global::active_clients;
    printf("Client %d fully disconnected\n", fd);
}

// get_sorted_players returns two parallel vectors of player IDs and scores
// sorted lexicographically by player_id so the SCORING message is deterministic.
std::pair<std::vector<std::string>, std::vector<std::string>> get_sorted_players(const std::unordered_map<int, player::Player>& players_map) {
    struct Entry {
        std::string id;
        double      score;
    };

    std::vector<Entry> entries;
    entries.reserve(players_map.size());

    for (const auto& kv : players_map) {
        const player::Player& pl = kv.second;
        entries.push_back({ pl.get_player_id(), pl.calculate_score() });
    }

    std::sort(entries.begin(), entries.end(),
        [](auto const& a, auto const& b) {
            return a.id < b.id;
        }
    );

    std::vector<std::string> ids;
    std::vector<std::string> scores;
    ids.reserve(entries.size());
    scores.reserve(entries.size());

    for (auto const& e : entries) {
        ids.push_back(e.id);
        scores.push_back(common::to_rational(e.score));
    }

    return { std::move(ids), std::move(scores) };
}

// -----------------------------------------------------------------------------
// Program entry point.
// -----------------------------------------------------------------------------
int main(int argc, char *argv[]) {
    // Install basic signal handlers so Ctrl-C exits quickly and SIGPIPE is ignored.
    std::signal(SIGINT, [](int){ _exit(0); });
    std::signal(SIGPIPE, SIG_IGN); 

    // Parse command-line arguments and verify that they satisfy assignment rules.
    common::parse_server_arguments(argc, argv, global::port, global::k, global::n, global::m, global::filename);
    common::verify_server_input(global::port, global::k, global::n, global::m, global::filename);
    std::ifstream file(global::filename);
    if (!file.is_open()) {
        fatal("Nie udało się otworzyć pliku");
        return 1;
    }

    // ----------------------------------------------------------------------
    // Socket setup: create an IPv6 listening socket that also accepts IPv4
    // connections through the IPv4-mapped IPv6 mechanism.
    // ----------------------------------------------------------------------
    int socket_fd = socket(AF_INET6, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        std::cerr << "ERROR: system error \r\n";
        close(socket_fd);
        return 1;
    }

    int off = 0;
    if (setsockopt(socket_fd, IPPROTO_IPV6, IPV6_V6ONLY, &off, sizeof(off)) < 0) {
        std::cerr << "ERROR: system error \r\n";
        close(socket_fd);
        return 1;
    }

    struct sockaddr_in6 server_address6{};
    server_address6.sin6_family   = AF_INET6;
    server_address6.sin6_addr     = in6addr_any;    // Listen on all interfaces.
    server_address6.sin6_port     = htons(global::port); // Convert to network byte order.
    
    if(bind(socket_fd,  reinterpret_cast<sockaddr*> (&server_address6), sizeof(server_address6)) < 0) {
        std::cerr << "ERROR: system error \r\n";
        close(socket_fd);
        return 1;
    }

    if (listen(socket_fd, QUEUE_LENGTH) < 0) {
        std::cerr << "ERROR: system error \r\n";
        close(socket_fd);
        return 1;
    }

    // ----------------------------------------------------------------------
    // Prepare the pollfd array: slot 0 is the listening socket, remaining
    // slots will be filled dynamically as clients connect.
    // ----------------------------------------------------------------------
    struct pollfd poll_descriptors[CONNECTIONS];
    poll_descriptors[0].fd = socket_fd;
    poll_descriptors[0].events = POLLIN;
    for (int i = 1; i < CONNECTIONS; ++i) {
        poll_descriptors[i].fd = -1;
        poll_descriptors[i].events = POLLIN;
    }

    static char buffer[BUFFER_SIZE]; // Shared scratch buffer for recv().
    sockaddr_storage cli_addr; // Holds the peer’s address on accept().
    socklen_t      cli_len; 

    // ----------------------------------------------------------------------
    // Main event loop: multiplex new connections, inbound/outbound traffic,
    // game-timer ticks, and graceful shutdown when SCORING is broadcast.
    // ----------------------------------------------------------------------
    do {
        // Clear revents before the next poll() call.
        for (int i = 0; i < CONNECTIONS; ++i) {
            poll_descriptors[i].revents = 0;
        }
        // -------------------------------------------------- Timer handling.
        auto now = std::chrono::steady_clock::now();
        if (now >= global::next_tick) {
            for (int i = 1; i < CONNECTIONS; ++i) {
                int fd = poll_descriptors[i].fd;
                if (fd == -1) continue;
                bool send_message = false;
                auto it = global::players_map.find(fd);
                it->second.process_timer_q(send_message);
                if(send_message) {
                    poll_descriptors[i].events |= POLLOUT;
                }
            }
            global::next_tick = std::chrono::steady_clock::now() + std::chrono::seconds(1);
        }

        // -------------------------------------------------- Check win condition.
        if (global::current_m >= global::m) {
            std::pair<std::vector<std::string>, std::vector<std::string>> res = get_sorted_players(global::players_map);
            std::vector<std::string> player_id = res.first;
            std::vector<std::string> player_scores = res.second;
            std::string msg = message::SCORING_msg(player_id, player_scores);
            global::finish = true;
            for (int i = 1; i < CONNECTIONS; ++i) {
                int fd = poll_descriptors[i].fd;
                if (fd == -1) continue;

                auto it = global::players_map.find(fd);
                if (it == global::players_map.end()) {
                    continue;
                }
                player::Player& pl = it->second;

                pl.send_buffer.insert(pl.send_buffer.end(), msg.begin(), msg.end());
                pl.set_scoring_end(pl.send_buffer.size());
                poll_descriptors[i].events |= POLLOUT;
            }
        }

        // -------------------------------------------------- Wait for descriptors to change state.
        int poll_status = poll(poll_descriptors, CONNECTIONS, TIMEOUT);
        if (poll_status == -1 ) {
            std::cerr << "ERROR: unkown error \r\n";
            close(socket_fd);
            return 1;
        } else if (poll_status > 0) {
            // The listening socket reports errors that can only be fatal.
            if (poll_descriptors[0].revents & (POLLERR|POLLHUP|POLLNVAL)) {
                std:: cerr << "ERROR: unexpected error \r\n";
                close(poll_descriptors[0].fd);
                return 1;
            }
            // -------------------------------------------------- Accept new clients.
            if (!global::finish && (poll_descriptors[0].revents & POLLIN)) {
                cli_len = sizeof(cli_addr);
                int client_fd = accept(poll_descriptors[0].fd,
                                    reinterpret_cast<sockaddr*>(&cli_addr),
                                    &cli_len);
                if (client_fd < 0) {
                   std::cerr << "ERROR: couldn't accept new client \r\n";
                } else {
                    // Convert the socket to non-blocking mode.
                    int flags = fcntl(client_fd, F_GETFL, 0);
                    if (flags < 0) { 
                        perror("fcntl GETFL"); 
                    }
                    if (fcntl(client_fd, F_SETFL, flags | O_NONBLOCK) < 0) {
                        std::cerr << "ERROR:  system error \r\n";
                        close(client_fd);
                    }
                     // Find an empty slot in the poll array.
                    bool accepted = false;
                    int slot;
                    for (int i = 1; i < CONNECTIONS; ++i) {
                        if (poll_descriptors[i].fd == -1) {
                            poll_descriptors[i].fd     = client_fd;
                            poll_descriptors[i].events = POLLIN;
                            global::active_clients++;
                            accepted = true;
                            slot = i;
                            break;
                        }
                    }
                    if (!accepted) {
                        close(client_fd);
                        printf("too many clients\n");
                    } else {
                        // Print the numeric address for logging purposes.
                        char host[NI_MAXHOST];
                        char service[NI_MAXSERV];
                        std::string ip = "UNKNOWN";
                        uint16_t port = -1;
                        if (getnameinfo(reinterpret_cast<sockaddr*>(&cli_addr), cli_len,
                                        host, sizeof(host),
                                        service, sizeof(service),
                                        NI_NUMERICHOST | NI_NUMERICSERV) == 0) {
                            std::cout << "Accepted connection from " 
                                    << host << ":" << service 
                                    << " (fd=" << client_fd << ")\n";

                            ip = host;
                            port = static_cast<uint16_t>(std::stoi(service));
                        } else {
                            std::cout << "Accepted connection (fd=" 
                                    << client_fd << ")\n";
                        }
                        
                        // Create a Player object for this descriptor.
                        global::players_map.emplace(client_fd, player::Player(global::n, global::k, global::m, ip, port));
                        global::just_connected.push_back(client_fd);
                        // If the game has already finished, close the client immediately.
                        if (global::finish) {
                            disconnect_client(slot, poll_descriptors, global::players_map);
                        }
                    }
                }
            }

            // -------------------------------------------------- Service each active client.
            for (int i = 1; i < CONNECTIONS; ++i) {
                int fd = poll_descriptors[i].fd;
                if (fd == -1) continue;  // wolny slot

                auto it = global::players_map.find(fd);
                if (it == global::players_map.end()) {
                    continue;
                }
                player::Player& pl = it->second;

                // Fatal socket state changes are handled first.
                if (poll_descriptors[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
                    std::cerr << "ERROR: socket closed or error\n";
                    disconnect_client(i, poll_descriptors, global::players_map);
                    continue;
                }
                // -------------------- Read side: process inbound data.
                if (poll_descriptors[i].revents & (POLLIN)) {
                    ssize_t n = read(fd, buffer, BUFFER_SIZE);

                    if (n > 0 && !global::finish) {
                        // ► normalny odczyt danych
                        bool send_message = false;
                        pl.push_received_buffer(std::vector<char>(buffer, buffer + n));
                        pl.process_received_buffer(file, send_message, global::current_m, global::finish);
                        if (send_message) {
                            poll_descriptors[i].events |= POLLOUT;
                        }
                    }
                    else if (n == 0 || (poll_descriptors[i].revents & POLLHUP)) {
                        disconnect_client(i, poll_descriptors, global::players_map);
                    } else if (n < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
                        disconnect_client(i, poll_descriptors, global::players_map);
                    }
                }
                
                // -------------------- Write side: drain pending responses.
                if (poll_descriptors[i].revents & POLLOUT) {
                    std::vector<char> tmp;
                    tmp.reserve(std::min<size_t>(pl.send_buffer.size(), CHUNK_SIZE));
                    auto tmp2 = pl.send_buffer.begin();
                    for (size_t cnt = 0; cnt < CHUNK_SIZE && tmp2 != pl.send_buffer.end(); ++cnt, ++tmp2) {
                        tmp.push_back(*tmp2);
                    }
                    ssize_t n = send(fd, tmp.data(), tmp.size(), 0);
                    if (n > 0) {
                        pl.dec_coeff_state_end(n);
                        pl.dec_scoring_end(n);
                        // Remove the bytes that were just transmitted.
                        ssize_t to_erase = std::min<ssize_t>(n, pl.send_buffer.size());
                        auto erase_end = pl.send_buffer.begin();
                        for (ssize_t j = 0; j < to_erase; ++j) {
                            ++erase_end;
                        }
                        // Allow the client to send PUTs again once COEFF/STATE ends are reached.
                        pl.send_buffer.erase(pl.send_buffer.begin(), erase_end);
                        if (pl.get_coeff_state_end() <= 0) {
                            pl.set_put_possible(true);
                        }
                        // Close the connection after SCORING is fully delivered.
                        if (global::finish && pl.get_scoring_end() <= 0) {
                            disconnect_client(i, poll_descriptors, global::players_map);
                        }
                        if (pl.send_buffer.size() <= 0) {
                            poll_descriptors[i].events &= ~POLLOUT;
                        }
                    } else if (n == 0) {
                        disconnect_client(i, poll_descriptors, global::players_map);
                        continue;
                    } else if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
                        //full kernel buffer
                    }
                    else {
                        disconnect_client(i, poll_descriptors, global::players_map);
                    }
                }
                // A second guard for HUP/ERR that may appear without POLLIN/OUT.
                if (poll_descriptors[i].revents & (POLLHUP | POLLERR)) {
                    disconnect_client(i, poll_descriptors, global::players_map);
                }
            }
        }
        // -------------------------------------------------- Enforce HELLO time-outs for newcomers.
        auto jt = global::just_connected.begin();
        while (jt != global::just_connected.end()) {
            int fd = *jt;
            int slot = -1;
            for (int i = 1; i < CONNECTIONS; ++i) {
                if (poll_descriptors[i].fd == fd) {
                    slot = i;
                    break;
                }
            }
            auto map_it = global::players_map.find(fd);
            if (map_it == global::players_map.end() || slot < 0) {
                jt = global::just_connected.erase(jt);
                continue;
            }

            player::Player& pl = map_it->second;
            if (!pl.get_received_hello() && pl.expired()) {
                disconnect_client(slot, poll_descriptors, global::players_map);
                jt = global::just_connected.erase(jt);
            }
            else if (pl.get_received_hello()) {
                jt = global::just_connected.erase(jt);
            }
            else {
                ++jt;
            }
        }
        // -------------------------------------------------- Reset state when the round ends.
        if (global::finish && global::active_clients == 0) {
            std::cout << "GAME HAS ENDED \r\n";
            sleep(1);                       // Give the OS time to flush logs.
            global::finish = false;
            global::players_map.clear();
            global::current_m = 0;
            global::just_connected.clear();
            global::active_clients = 0;
            std::cout << "NEW GAME \r\n";
        }
    } while(true);
    // This line is theoretically unreachable but cleans up on unusual exits.
    close(socket_fd);
    return 0;
}