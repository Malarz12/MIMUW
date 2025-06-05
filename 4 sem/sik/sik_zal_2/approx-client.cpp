// Standard-library headers that provide the containers, utilities, networking,
// and system-level functions required by this client.
#include <vector>
#include <iostream>
#include <string>
#include <unordered_map>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <queue>
#include <utility>
#include <cmath>
#include <poll.h>
#include <sstream>
#include <csignal> 
#include <list>

// Project-specific headers that define the wire-protocol and helpers.
#include "message.hpp"
#include "player.hpp"
#include "common.hpp"

// Simple compile-time constants that control buffer sizes, time-outs, and the
// number of file descriptors monitored by poll().
#define BUFFER_SIZE 100000
#define TIMEOUT 100
#define CONNECTIONS 2

// The global namespace groups configuration flags, buffers, and game state in
// one place so they are accessible from every function in this translation unit.
namespace global {
    // Connection parameters supplied on the command line.
    std::string player_id = "";
    std::string server_ip = "";
    uint16_t server_port = -1;
    std::string server_port_string;
    int ipv_type = 0;            // 4 means IPv4, 6 means IPv6, 0 lets getaddrinfo decide.
    bool strategy = false;       // False = interactive mode, true = automatic bot.

    // I/O buffers and bookkeeping for partial sends/receives.
    static char buffer[BUFFER_SIZE];
    std::list<char> received_buffer;
    int already_sent = 0;
    std::vector<char> send_buffer;

    // Data structures that hold the polynomial coefficients sent by the server
    // and the client’s own predictions over time.
    std::vector<double> coeffs;
    std::vector<double> prediction;
    std::vector<double> computed_poly;

    // Game-specific state.
    int n;                       // Degree of the polynomial (updated after COEFF message).
    int k;                       // Size of the prediction vector (updated after first STATE).
    bool first_state = true;     // Set to false after the first STATE is processed.
    bool received_scoring = false; // Signals graceful termination once SCORING arrives.
} 


// -----------------------------------------------------------------------------
// Networking helpers
// -----------------------------------------------------------------------------

// connect_tcp creates a blocking TCP socket and tries every address returned by
// getaddrinfo until one succeeds or the list is exhausted.
int connect_tcp() {
    addrinfo hints{};
    hints.ai_socktype = SOCK_STREAM;

     // Restrict address family based on the user’s preference.
    switch (global::ipv_type) {
        case 4:  hints.ai_family = AF_INET;  break;      // IPv4 only.
        case 6:  hints.ai_family = AF_INET6; break;      // IPv6 only.
        default: hints.ai_family = AF_UNSPEC;            // Either IPv4 or IPv6.
    }

    std::string port_str = std::to_string(global::server_port);
    addrinfo* res = nullptr;
    int rv = getaddrinfo(global::server_ip.c_str(), port_str.c_str(),
                         &hints, &res);
    if (rv != 0) fatal(std::string("getaddrinfo: ") + gai_strerror(rv));

    int sock = -1;
    for (addrinfo* p = res; p; p = p->ai_next) {
        sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sock < 0) continue;

        // Stop at the first address that connects successfully.
        if (connect(sock, p->ai_addr, p->ai_addrlen) == 0) break; // OK
        close(sock);
        sock = -1;
    }
    freeaddrinfo(res);

    if (sock < 0) fatal("Unable to connect to server");
    return sock;
}

// make_nonblocking sets the O_NONBLOCK flag on a file descriptor so that reads
// and writes return immediately instead of blocking the calling thread.
int make_nonblocking(int sock_fd) {
    int flags = fcntl(sock_fd, F_GETFL, 0);
    if (flags < 0) return -1;
    return fcntl(sock_fd, F_SETFL, flags | O_NONBLOCK);
}

// -----------------------------------------------------------------------------
// Polynomial utilities
// -----------------------------------------------------------------------------

// compute_poly evaluates the polynomial with the current coefficients at a
// single integer point using the naïve O(n) method.
double compute_poly(int point) {
    double res = 0.0;
    for (int i = 0; i < static_cast<int>(global::coeffs.size()); ++i) {
        res += (std::pow(point, i) * global::coeffs[i]);
    }
    return res;
}

// fully_compute_poly precomputes the polynomial’s value at every index that
// will ever appear in STATE messages, so later differences are cheap.
void fully_compute_poly() {
    for (int i = 0; i < static_cast<int>(global::prediction.size()); ++i) {
        global::computed_poly.push_back(compute_poly(i));
    }
}

// -----------------------------------------------------------------------------
// Robust send/receive helpers
// -----------------------------------------------------------------------------

// send_full_msg guarantees that the entire message is written to the socket,
// retrying short writes and handling EAGAIN transparently.
bool send_full_msg(const std::string& msg, const int sock_fd) {
    const char* ptr = msg.data();
    size_t left = msg.size();

    while (left > 0) {
        ssize_t sent = send(sock_fd, ptr, left, 0);
        if (sent > 0) {
            ptr  += sent;
            left -= static_cast<size_t>(sent);
        } else if (sent < 0) {
            // If the kernel’s send buffer is full, spin and try again.
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                continue;
            }
            // Any other error is fatal for this connection.
            std::cerr << "ERROR: can't send msg\r\n";
            close(sock_fd);
            return false;
        } else if (sent == 0) {
            std::cerr << "ERROR: unexpected server disconnect\n";
            close(sock_fd);
            return false;
        }
    }
    return true;
}


// -----------------------------------------------------------------------------
// Protocol parser and dispatcher
// -----------------------------------------------------------------------------

// process_msg interprets a single line-oriented protocol message from the server
// and sends an appropriate response. It returns false if the message format is
// invalid or a fatal error occurs.
bool process_msg(const std::string& msg, const int sock_fd) {
    // Each branch corresponds to one message type defined by the assignment.
    if (msg.starts_with("COEFF")) {
        // The server has sent the polynomial coefficients that define the game.
        if(!verification::verify_COEFF(msg, global::coeffs)) {
            std::cerr << "ERROR: wrong msg format \r\n";
            return false;
        }
        global::n = global::coeffs.size();
        std::cout << "RECEIVED" << " " << msg;

        // The rules require the client to send an initial PUT with value 0.
        std::string reply = message::PUT_msg("0", "0.0");
        if(!send_full_msg(reply, sock_fd)) {
            exit(1);
            return false;
        }
        std::cout << "SENT " << " " << reply;
        return true;
    } else if (msg.starts_with("STATE")) {
        // STATE conveys the current prediction vector for all points 0..k-1.
        if(!verification::verify_STATE(msg, global::prediction)) {
            std::cerr << "ERROR: wrong msg format \r\n";
            return false;
        }
        std::cout << "RECEIVED" << " " << msg;

        // The first STATE message reveals k, the length of the game.
        if (global::first_state) {
            global::first_state = false;
            global::k = global::prediction.size();
            fully_compute_poly();
        }

        // Select the best index and delta according to the scoring heuristic.
        std::pair<int, double> best_put = std::make_pair(0, 0.0);
        for (int i = 0; i < static_cast<int>(global::prediction.size()); i++) {
            double difference = std::min(5.0, std::abs(global::computed_poly[i] - global::prediction[i]));
            if (global::computed_poly[i] < 0) difference *= -1;

            // Evaluate the gain in squared-error by nudging prediction[i] by difference.
            if ((std::pow(global::prediction[i] - global::computed_poly[i], 2)
             - std::pow(global::prediction[i] + difference - global::computed_poly[i], 2)) >
            (std::pow(global::prediction[best_put.first] - global::computed_poly[best_put.first], 2)
             - std::pow(global::prediction[best_put.first] + best_put.second - global::computed_poly[best_put.first], 2))) {
                best_put = std::make_pair(i, difference);
             }
        }

        // Send the chosen PUT back to the server.
        std::string reply = message::PUT_msg(std::to_string(best_put.first), common::to_rational(best_put.second));
        if(!send_full_msg(reply, sock_fd)) {
            std::exit(1);
            return false;
        }
        std::cout << "SENT " << " " << reply;
        return true;
    } else if (msg.starts_with("BAD_PUT")) {
        // BAD_PUT tells the client that its previous PUT was illegal or malformed.
        int out_point;
        double out_value;
        if(!verification::verify_BAD_PUT(msg, out_point, out_value)) {
            std::cerr << "ERROR: wrong msg format \r\n";
            return false;
        }
        std::cout << "RECEIVED" << " " << msg;

        // Per protocol, immediately send a neutral PUT (0, 0).
        std::string reply = message::PUT_msg("0", "0.0");
        if(!send_full_msg(reply, sock_fd)) {
            std::exit(1);
        }
        std::cout << "SENT " << " " << reply;
        return true;
    } else if (msg.starts_with("SCORING")) {
        // SCORING delivers the final results and signals the end of the match.
        std::vector<std::pair<std::string,double>> out_scores;
        if(!verification::verify_SCORING(msg, out_scores)) {
            std::cerr << "ERROR: wrong msg format \r\n";
            return false;
        }
        std::cout << "RECEIVED" << " " << msg;
        global::received_scoring = true;
        return true;
    } else if (msg.starts_with("PENALTY")){
        // PENALTY deducts points because the client violated some rule.
        int out_point;
        double out_value;
        if(!verification::verify_PENALTY(msg, out_point, out_value)) {
            std::cerr << "ERROR: wrong msg format \r\n";
            return false;
        }
        std::cout << "RECEIVED" << " " << msg;
        return true;
    } else {
        // Any unknown prefix indicates a protocol violation.
        std::cerr << "ERROR: wrong msg format \r\n";
        return false;
    }
}

// get_one_msg tries to extract a complete line (terminated by '\n') from the
// receive buffer and returns true only when successful.
bool get_one_msg(std::string& full_msg) {
    auto it = global::received_buffer.begin();
    while (it != global::received_buffer.end() && *it != '\n') {
        ++it;
    }
    if (it == global::received_buffer.end()) {
        return false;
    }

    // Move the characters into a std::string and erase them from the list.
    std::string temp;
    temp.reserve(std::distance(global::received_buffer.begin(), std::next(it)));
    for (auto jt = global::received_buffer.begin(); jt != std::next(it); ++jt) {
        temp.push_back(*jt);
    }
    full_msg = std::move(temp);
    global::received_buffer.erase(global::received_buffer.begin(), std::next(it));
    return true;
}

// push_received_buffer reads as many bytes as possible from the socket and
// appends them to the global list so that get_one_msg can parse them later.
bool push_received_buffer(int sock_fd) {
    ssize_t n = read(sock_fd, global::buffer, BUFFER_SIZE - 1);
    if (n > 0) {
        for (int i = 0; i < static_cast<int>(n); ++i) {
            //std::cout<<global::buffer[i];
            global::received_buffer.push_back(global::buffer[i]);
        }
        return true;
    } else if (n == 0) {
        // A zero-length read means the peer performed an orderly shutdown.
        std:: cerr << "ERROR: unexpected server disconnect\n ";
        close(sock_fd);
        return false;
    } else { // n < 0
        // For EAGAIN the poll loop will retry later; any other errno is fatal.
        if (errno == EAGAIN || errno == EWOULDBLOCK) {

        } else {
            std:: cerr << "ERROR: unexpected server disconnect\n ";
            close(sock_fd);
            return false;
        }
    }
    return true;
}

// -----------------------------------------------------------------------------
// Program entry point
// -----------------------------------------------------------------------------

int main(int argc, char *argv[]) {
    // Parse, verify, and store command-line arguments that configure the client.
    common::parse_client_arguments(argc, argv, global::player_id, global::server_ip, global::server_port, global::ipv_type, global::strategy);
    common::verify_client_input(global::player_id, global::server_ip, global::server_port, global::ipv_type, global::strategy);

    // Ignore SIGPIPE so failed send() calls are reported by errno instead.
    std::signal(SIGPIPE, SIG_IGN);

    // ------------------------------------------------------------------
    // Automatic (bot) mode – the client runs without user intervention.
    // ------------------------------------------------------------------
    if (global::strategy) {
        int sock_fd = connect_tcp();
        std::cout << "CONNECTED WITH "
              << global::server_ip << ":" << global::server_port
              << " (fd=" << sock_fd << ")\r\n";

        // Greet the server with a HELLO that identifies this client.
        std::string msg = message::HELLO_msg(global::player_id); 
        if(!send_full_msg(msg, sock_fd)) {
            return 1;
        }
        std::cout << "SENT" << " " << msg;
        // Main bot loop: read all available data, process messages, and quit
        // gracefully once the final SCORING message is received.
        while (true) {
            if(!push_received_buffer(sock_fd)) {
                return 1;
            }
            while(get_one_msg(msg)) {
                //until we have zero full msg's
                if (!process_msg(msg, sock_fd)) {
                    close(sock_fd);
                    return 1;
                }
                if (global::received_scoring) {
                    close(sock_fd);
                    return 0;
                }
            }
        }
    } 
    // ------------------------------------------------------------------
    // Interactive mode – allows a human to type PUT commands on stdin.
    // ------------------------------------------------------------------
    else {
        // Switch stdin to non-blocking so the poll loop can multiplex it.
        if (make_nonblocking(STDIN_FILENO) < 0) {
            perror("fcntl stdin O_NONBLOCK");               
            return 1;
        }

        // Prepare the pollfd array: index 0 is stdin, index 1 is the socket.
        int sock_fd = connect_tcp();
        if (sock_fd < 0) return 1;
        if (make_nonblocking(sock_fd) < 0) {
            perror("fcntl sock_fd O_NONBLOCK");       
            close(sock_fd);
            return 1;
        }
        
        pollfd fds[CONNECTIONS];
        
        fds[0].fd      = STDIN_FILENO;
        fds[0].events  = POLLIN;
        fds[0].revents = 0;

        fds[1].fd      = sock_fd;
        fds[1].events  = POLLIN;
        fds[1].revents = 0;
        // Send the mandatory HELLO before entering the event loop.
        if (!send_full_msg(message::HELLO_msg(global::player_id), sock_fd)) {
            return 1;
        }

        bool accept_input = false;
        // Repeatedly wait for input, socket data, or a time-out.
        while (true) {
            for (int i = 0; i < CONNECTIONS; ++i) {
                fds[i].revents = 0;
            }
            
            int ready = poll(fds, CONNECTIONS, TIMEOUT);
            if (ready < 0) {
                perror("poll");                                
                return 1;
            }
            if (ready == 0) {                                
                continue;                                    
            }
            // -------------------- Handle user keyboard input. --------------------
            if (fds[0].revents & POLLIN) {
                ssize_t n = read(STDIN_FILENO, global::buffer, BUFFER_SIZE - 1);
                if (n > 0) {
                    global::buffer[n] = '\0';
                    std::string line{global::buffer, static_cast<size_t>(n)}; 
                    static const std::regex re(
                        R"(^\s*([0-9]+)\s+(-?[0-9]+(?:\.[0-9]{1,7})?)\s*$)");

                    std::smatch m;
                    if (!std::regex_match(line, m, re)) {
                        std::cerr << "ERROR: invalid input line " << line << "\r\n";
                        continue;
                    }

                    unsigned long ul = std::stoul(m[1].str());
                    if (ul > std::numeric_limits<unsigned int>::max()) {
                        std::cerr << "ERROR: invalid input line " << line << "\r\n";
                        continue;
                    }
                    unsigned int a = static_cast<unsigned int>(ul);

                    double b;
                    try {
                        b = std::stod(m[2].str());
                    } catch (...) {
                        std::cerr << "ERROR: invalid input line " << line << "\r\n";
                        continue;
                    }

                    std::string new_put =
                        message::PUT_msg(std::to_string(a), common::to_rational(b));

                    global::send_buffer.insert(global::send_buffer.end(),
                                            new_put.begin(), new_put.end());
                    fds[1].events |= POLLOUT;
                }
                else if (n == 0) {
                    std::cerr << "ERROR: stdin closed\n";
                    close(sock_fd);
                    return 1;
                }
                else if (errno != EAGAIN && errno != EWOULDBLOCK) {
                    std::cerr << "ERROR: stdin\n";
                    close(sock_fd);
                    return 1;
                }
            }

            // -------------------- Handle inbound server data. --------------------
            if (fds[1].revents & POLLIN) {
                if(!push_received_buffer(sock_fd)) {
                    return 1;
                }
                std::string msg;
                while (get_one_msg(msg)) {
                    std::cout << "RECEIVED " << msg;
                    if (msg.starts_with("SCORING")) {
                        close(sock_fd);
                        return 0;
                    } 
                    if (msg.starts_with("COEFF")) {
                        accept_input = true;
                    }
                }
            }
            
            // Socket errors reported by poll() terminate the client immediately.
            if (fds[1].revents & (POLLERR | POLLHUP | POLLNVAL)) {
                std::cerr << "ERROR: socket closed or error\n";
                close(sock_fd);
                return 1;
            }

            // -------------------- Handle outbound buffered messages. --------------------
            if (accept_input && fds[1].revents & POLLOUT) {
                size_t left = global::send_buffer.size() - global::already_sent;
                if (left == 0) {
                    fds[1].events &= ~POLLOUT;
                    continue;
                }

                const char* data = global::send_buffer.data() + global::already_sent;
                ssize_t n = send(sock_fd, data, left, 0);
                if (n > 0) {
                    global::already_sent += static_cast<size_t>(n);
                    if (global::already_sent >= static_cast<int>(global::send_buffer.size())) {
                        fds[1].events &= ~POLLOUT; // No more data to send.
                        global::send_buffer.clear();
                        global::already_sent = 0;
                    }
                } else if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
                    // Kernel buffer is still full; try again on the next POLLOUT.
                } else if (n < 0) {
                    std::cerr << "ERROR: couldn't send message \r\n";
                    close(sock_fd);
                    return 1;
                } else if (n == 0) {
                    std:: cerr << "ERROR: unexpected server disconnect\n ";
                    close(sock_fd); 
                    return 1;
                }
            }
        }
        // Close the socket before exiting; this line is unreachable in practice.
        close(sock_fd);
    }
    return 0;
}