#pragma once   // Ensure this header is included at most once in each translation unit.

/* --------------------------------------------------------------------------
 * Standard-library headers required for player logic, buffers, timing, and I/O.
 * --------------------------------------------------------------------------*/
#include <vector>
#include <iostream>
#include <string>
#include <cmath>
#include <chrono>
#include <queue>
#include <functional>
#include <cctype>
#include <climits>
#include <cfloat> 
#include <list>

#include "common.hpp"   // Shared helpers such as to_rational and argument parsing.
#include "message.hpp"  // Wire-protocol builders and verifiers.

/* --------------------------------------------------------------------------
 * All player-related code lives in the player namespace to avoid collisions.
 * --------------------------------------------------------------------------*/
namespace player{
    /* ----------------------------------------------------------------------
     * Delayed_Message stores a protocol message together with the absolute
     * time at which it should be transmitted.
     * A priority queue ordered by send_time lets the server implement
     * per-player artificial latency deterministically.
     * -------------------------------------------------------------------- */
    class Delayed_Message{
    private:
        std::string message;                           // The message body ready to send.
        std::chrono::steady_clock::time_point send_time; // When the message becomes eligible.
    public:
        // Construct a delayed message that should be sent after “delay”.
        Delayed_Message(const std::string& msg, const std::chrono::seconds delay) {
            message   = msg;
            send_time = std::chrono::steady_clock::now() + delay;
        }

        // Returns true when the message is ready to leave the timer queue.
        bool ready() {
            return std::chrono::steady_clock::now() >= send_time;       
        }

        std::chrono::steady_clock::time_point get_send_time() { return send_time; }
        std::string  get_message()            { return message; }

         /* Strict weak ordering so Delayed_Message can live in std::priority_queue
         * with std::greater<> as the comparator. */
        bool operator<(const Delayed_Message& other) const {
            return send_time < other.send_time;
        }
        bool operator==(const Delayed_Message& other) const {
            return send_time == other.send_time;
        }
        bool operator!=(const Delayed_Message& other) const {
            return !(*this == other);
        }
        bool operator>(const Delayed_Message& other) const {
            return other < *this;
        }
        bool operator<=(const Delayed_Message& other) const {
            return !(*this > other);
        }
        bool operator>=(const Delayed_Message& other) const {
            return !(*this < other);
        }
    };
    
    /* ----------------------------------------------------------------------
     * Player encapsulates all state and behaviour associated with a single
     * TCP connection, including buffers, per-player timers, predictions, and
     * score calculation.
     * -------------------------------------------------------------------- */
    class Player {
    private:
        /* --------------------------- Immutable configuration. --------------------------- */
        std::string player_id = "UNKNOWN";  // Filled after the HELLO message.
        int n;                              // Degree of the polynomial.
        int k;                              // Highest point index allowed in PUT.
        const int m;                        // Target number of valid PUTs per game.
        uint16_t port;
        std::string ip;

        /* --------------------------- Gameplay state. ------------------------------------ */
        std::vector<double> polynomial;     // Coefficients received in COEFF.
        std::vector<double> prediction;     // Current prediction vector, size k+1.
        double penalty = 0.0;               // Accumulated penalty points.

        /* --------------------------- I/O buffers. --------------------------------------- */
        std::list<char> received_buffer;    // Inbound byte stream, line-buffered.

        /* --------------------------- Connection bookkeeping. ---------------------------- */
        bool received_hello = false;               // True once a valid HELLO arrives.
        std::chrono::steady_clock::time_point expiration_date; // Kick-off deadline.

        /* --------------------------- Per-player timers. --------------------------------- */
        std::chrono::seconds delay;                // Artificial latency derived from id.
        std::priority_queue<
            Delayed_Message,
            std::vector<Delayed_Message>,
            std::greater<Delayed_Message>
        > timer_queue;                             // Min-heap ordered by send_time.

        bool put_possible      = false;            // True when client may issue PUT.
        int  coeff_state_end   = 0;                // Bytes until COEFF/STATE done sending.
        int  scoring_end       = INT_MAX;          // Bytes until SCORING done sending.
        bool stop_timer_queue  = false;            // Disables timer processing when true.
        int  m_counter         = 0;                // Number of successful PUTs by this player.
    public:
        std::list<char> send_buffer;        // Outbound bytes waiting to be sent.

        // Constructor fills fixed parameters and sets the HELLO expiration (3 s).
        Player(int N, int K, int M, std::string ip_address, uint16_t p)
        : n(N)
        , k(K)
        , m(M)
        , port(p)
        , ip(ip_address)
        , prediction(k + 1, 0.0)
        , expiration_date(std::chrono::steady_clock::now() + std::chrono::seconds(3))
        {}
        
        /* ------------------------------------------------------------------
         * Lightweight helpers that update internal counters after a send().
         * ---------------------------------------------------------------- */
        void dec_scoring_end(const int val) {
            scoring_end -= val;
        }

        void dec_coeff_state_end(const int val) {
            coeff_state_end -=val;
        }
        void set_scoring_end(int val) {
            this->scoring_end = val;
        }

        int get_scoring_end() {
            return scoring_end;
        }

        int get_coeff_state_end() {
            return coeff_state_end;
        }

        void inc_m() {
            m_counter++;
        }

        int get_m() {
            return m_counter;
        }

        /* ------------------------------------------------------------------
         * Utility to register a unique player_id and compute the latency delay.
         * ---------------------------------------------------------------- */
        void set_player_id(const std::string& player_id_p) {
            this->player_id = player_id_p;
            this->delay = compute_delay(player_id);
        }

        /* ------------------------------------------------------------------
         * Accept the polynomial sent by the server in the COEFF message.
         * ---------------------------------------------------------------- */
        void set_polynomial(const std::vector<double>& polynomial_p) {
            for (int i = 0; i < static_cast<int>(polynomial_p.size()); ++i) {
                this->polynomial.push_back(polynomial_p[i]);
            }
        }

        /* ------------------------------------------------------------------
         * Update one element of the prediction vector after a valid PUT.
         * ---------------------------------------------------------------- */
        void update_prediction(const int point,const double value) {
            prediction[point] += value;
        }

        /* ------------------------------------------------------------------
         * calculate_score implements the official scoring formula: the sum of
         * squared errors between prediction and the true polynomial plus any
         * accumulated penalties.
         * ---------------------------------------------------------------- */
        double calculate_score() const {
            if (polynomial.size() == 0) {
                return DBL_MAX;
            }
            double res = 0.0;
            for (int i = 0; i <= k; ++i) {
                double temp = 0.0;
                for (int j = 0; j <= n; ++j) {
                    temp += (std::pow(i, j) * polynomial[j]);
                }
                double diff = prediction[i] - temp;
                res += diff * diff;
            }
            res += penalty;
            return res;
        }

        /* ------------------------------------------------------------------
         * Convert the prediction vector to strings with trimmed trailing zeros
         * so they can be embedded directly into STATE messages.
         * ---------------------------------------------------------------- */
        std::vector<std::string> string_predictions() {
            std::vector<std::string> res;
            for (int i = 0; i < static_cast<int>(prediction.size()); ++i) {
                res.push_back(common::to_rational(prediction[i]));
            }
            return res;
        }

        /* ------------------------------------------------------------------
         * Buffer-management helpers push raw bytes into received_buffer and parse
         * complete lines, generating responses and updating global game state.
         * This function is called by the server whenever new bytes arrive.
         * ---------------------------------------------------------------- */
        void push_received_buffer(const std::vector<char>& received_msg) {
            for (int i = 0; i < static_cast<int>(received_msg.size()); ++i) {
                received_buffer.push_back(received_msg[i]);
            }
        }

        /* ------------------------------------------------------------------
         * process_received_buffer consumes as many complete protocol messages
         * as possible, validates them, updates internal state, and schedules
         * outbound responses.  The boolean send_message is set when new data
         * has been queued for sending so the poll loop can add POLLOUT.
         * ---------------------------------------------------------------- */
        void process_received_buffer(std::ifstream& file, bool& send_message, int& global_current_m, const bool finish) {
            std::string msg = "";
            if (finish) return;
            auto it = received_buffer.begin();
            while (it != received_buffer.end()) {
                char c = *it;
                msg.push_back(c);
                if (c == '\n') {
                    auto next_it = std::next(it);
                    received_buffer.erase(received_buffer.begin(), next_it);
                    it = received_buffer.begin();
                    if (msg.starts_with("HELLO")) {
                        if (!verification::verify_HELLO(msg, this->player_id)) {
                            std::cerr << "ERROR: bad message from " << ip << ": "<< port << ", " << player_id << ": " << msg << "\n";
                        } else if (received_hello == true) {
                            std::cerr << "ERROR: bad message from " << ip << ": "<< port << ", " << player_id << ": " << msg << "\n";
                        } else {
                            std::cout << this->player_id << " RECEIVED " << msg;
                            received_hello = true;
                            std::string line = common::get_next_line(file);
                            std::cout << "NEW LINE: " << line;// << "\r\n";
                            verification::verify_COEFF(line, polynomial);
                            coeff_state_end = send_buffer.size() + line.size();
                            std::cout << this->player_id << " SENDING: COEFF" << "\r\n";
                            push_send_buffer(line);
                            compute_delay(this->player_id);
                            send_message = true; 
                        }
                    } else if (msg.starts_with("PUT")) {
                        int point;
                        double value;
                        if (!verification::verify_PUT(msg, point, value)) {
                            std::cerr << "ERROR: bad message from " << ip << ": "<< port << ", " << player_id << ": " << msg << "\n";
                        } else if (!put_possible) {
                            if (global_current_m >= m) {
                                return;
                            }
                            std::cout << this->player_id << " " << "RECEIVED " << msg;
                            penalty += 20.0;
                            std::cout << "SENDING: PENALTY" << "\r\n";
                            push_send_buffer(message::PENALTY_msg(std::to_string(point), common::to_rational(value)));
                            if (point < 0 || point > k || value > 5.0 || value < -5.0) {
                                std::cout << this->player_id << " " << "SENDING: BAD_PUT" << "\r\n";
                                push_bad_put_msg(std::to_string(point), common::to_rational(value));
                                penalty += 10.0;
                            } else {
                                send_message = true;
                                update_prediction(point, value);
                                std::cout << this->player_id << " " << "SENDING: STATE" << "\r\n";
                                push_state_msg();
                                global_current_m++;
                                m_counter++;
                                
                                std::vector<std::string> preds = string_predictions();
                                std::cout << this->player_id << " " << " UPDATED PREDICTION:";
                                for (int idx = 0; idx < static_cast<int>(preds.size()); ++idx) {
                                    std::cout << " " << preds[idx];
                                }
                                std::cout << "\r\n";
                            }
                        } else {
                            if (global_current_m >= m) {
                                    return;
                            }
                            std::cout << this->player_id << " " << "RECEIVED " << msg;
                            if (point < 0 || point > k || value > 5.0 || value < -5.0) {
                                std::cout << this->player_id << " " << "SENDING: BAD_PUT" << "\r\n";
                                push_bad_put_msg(std::to_string(point), common::to_rational(value));
                                penalty += 10.0;
                            } else {
                                //correct put msg
                                put_possible = false;
                                update_prediction(point, value);
                                std::cout << this->player_id << " " << "SENDING: STATE" << "\r\n";
                                push_state_msg();
                                global_current_m++;
                                m_counter++;
                                
                                std::vector<std::string> preds = string_predictions();
                                std::cout << this->player_id << " " << " UPDATED PREDICTION:";
                                for (int idx = 0; idx < static_cast<int>(preds.size()); ++idx) {
                                    std::cout << " " << preds[idx];
                                }
                                std::cout << "\r\n";

                                
                            }
                        }
                    } else {
                        std::cerr << "ERROR: bad message from " << ip << ": "<< port << ", " << player_id << ": " << msg << "\n";
                    }
                } else {
                    ++it;
                }
            }
        }

        /* ---------------------------- Lightweight accessors. --------------------------- */
        std::string get_player_id() const {
            return this->player_id;
        }

        /* ---------------------------- Timer-queue helpers. ----------------------------- */
        void stop_timer_queue_fun() {
            stop_timer_queue = true;
        }

        /* process_timer_q dequeues and sends any delayed messages whose time has come. */
        void process_timer_q(bool& send_message) {
            while (!timer_queue.empty() && !stop_timer_queue) {
                Delayed_Message msg = timer_queue.top();
                if (!msg.ready()) {
                    break;
                }
                
                send_message = true;
                if (msg.get_message().starts_with("STATE")) {
                    coeff_state_end = send_buffer.size() + msg.get_message().size();
                }
                push_send_buffer(msg.get_message());
                timer_queue.pop();
            }
        }

        /* push_send_buffer appends a raw string to the outgoing byte queue. */
        void push_send_buffer(const std::string& msg) {
            for (int i = 0; i < static_cast<int>(msg.size()); ++i) {
                send_buffer.push_back(msg[i]);
            }
        }

        /* add_penalty is a helper for ad-hoc penalty accumulation. */
        void add_penalty(const double val) {
            penalty += val;
        }

        /* compute_delay derives the artificial latency from the number of
         * lowercase letters in the player_id, then caches and returns it. */
        std::chrono::seconds compute_delay(const std::string& player_id_p) {
            int lowercase_count = 0;
            for (char c : player_id_p) {
                if (std::islower(static_cast<unsigned char>(c))) {
                    ++lowercase_count;
                }
            }
            this->delay = std::chrono::seconds(lowercase_count);
            return std::chrono::seconds(lowercase_count);
        }

         /* ---------------------------- High-level message builders. --------------------- */
        void set_put_possible(const bool val) {
            put_possible = val;
        }

        bool get_put_possible() {
            return put_possible;
        }

        void push_state_msg() {
            Delayed_Message msg(message::STATE_msg(string_predictions()), std::chrono::seconds(delay));
            timer_queue.push(msg);
        }

        void push_bad_put_msg(const std::string& point, const std::string& value) {
            Delayed_Message msg(message::BAD_PUT_msg(point, value), std::chrono::seconds(1));
            timer_queue.push(msg);
        }

        bool get_received_hello() {
            return received_hello;
        }

        void set_received_hello() {
            received_hello = true;
        }

        bool expired() {
            auto current_time = std::chrono::steady_clock::now();
            return current_time > expiration_date;
        }

        /* ------------------------------------------------------------------
         * Destructor is trivial because all containers clean up automatically.
         * ---------------------------------------------------------------- */
         
        ~Player() = default;  // standardowy, automatyczny destruktor
    };

}