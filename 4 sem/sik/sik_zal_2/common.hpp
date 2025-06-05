#pragma once   // Ensure the header is included at most once during compilation.

// -----------------------------------------------------------------------------
// Standard-library headers that supply the utilities, containers, and I/O
// facilities needed to parse command-line arguments and format values.
// -----------------------------------------------------------------------------
#include <unistd.h>    // getopt for option parsing.
#include <cstdlib>     // strtoul for numeric conversion.
#include <cerrno>      // errno for error detection after strtoul.
#include <cstdint>     // Fixed-width integer types such as uint16_t.
#include <iostream>    // std::cerr for error output.
#include <string>
#include <regex>       // std::regex for validating identifiers.
#include <sstream>     // std::ostringstream for string formatting.
#include <iomanip>     // std::setprecision for fixed-point output.
#include <fstream>     // std::ifstream for reading coefficient files.

#include "err.h"       // Project-specific helper that terminates on fatal errors.

// All helper functions live in the common namespace so they can be shared by
// both the client and the server without name collisions.

namespace common {
    // read_port converts a C-string to a 16-bit port number and aborts on error.
    inline uint16_t read_port(const char *str) {
        char *endptr;
        errno = 0;
        unsigned long val = std::strtoul(str, &endptr, 10);
        if (errno != 0 || *endptr != '\0' || val > UINT16_MAX) {
            fatal("not valid port");
        }
        return static_cast<uint16_t>(val);
    }

    // ---------------------------------------------------------------------
    // parse_server_arguments reads command-line flags for the *server* binary
    // and writes validated values back to the caller through reference args.
    // ---------------------------------------------------------------------
    inline void parse_server_arguments(int argc, char *argv[],
                                   uint16_t&    port,     // Defaults to 0.
                                   int&         k,        // Defaults to 100.
                                   int&         n,        // Defaults to 4.
                                   int&         m,        // Defaults to 131.
                                   std::string& filename) // Mandatory option.
    {
        // Track whether each option has already been seen to catch duplicates.
        bool got_p = false, got_k = false, got_n = false,
            got_m = false, got_f = false;

        opterr = 0;                                          // Silence getopt’s own messages.
        int ch;
        while ((ch = getopt(argc, argv, "p:k:n:m:f:")) != -1) {
            switch (ch) {
            case 'p':   // Port on which to listen.
                if (got_p) fatal("ERROR: option -p given more than once");
                port  = read_port(optarg);
                got_p = true;
                break;

            case 'k':   // Number of turns each player gets.
                if (got_k) fatal("ERROR: option -k given more than once");
                k = std::stoi(optarg);
                if (k < 1 || k > 10000)
                    fatal("ERROR: -k must be in range 1…10000");
                got_k = true;
                break;

            case 'n':   // Degree of the hidden polynomial.
                if (got_n) fatal("ERROR: option -n given more than once");
                n = std::stoi(optarg);
                if (n < 1 || n > 8)
                    fatal("ERROR: -n must be in range 1…8");
                got_n = true;
                break;

            case 'm':   // Target cumulative “m” after which the game ends.
                if (got_m) fatal("ERROR: option -m given more than once");
                m = std::stoi(optarg);
                if (m < 1 || m > 12341234)
                    fatal("ERROR: -m must be in range 1…12341234");
                got_m = true;
                break;

            case 'f':   // Path to the file containing polynomial coefficients.
                if (got_f) fatal("ERROR: option -f given more than once");
                filename = optarg;
                got_f = true;
                break;

            default:
                fatal("ERROR: unknown flag");
            }
        }

        // The filename is mandatory because the server must read coefficients.
        if (!got_f)
            fatal("ERROR: option -f (filename) is required");
    }


    // ---------------------------------------------------------------------
    // parse_client_arguments reads flags for the *client* binary, filling in
    // connection parameters and operational mode flags.
    // ---------------------------------------------------------------------
    inline void parse_client_arguments(int argc, char *argv[],
                                   std::string& player_id,
                                   std::string& server_ip,
                                   uint16_t&    server_port,
                                   int&         ipv_type,
                                   bool&        strategy)
    {
        /*  ── znaczniki “już-widziałem” ─────────────────────────── */
        bool got_u = false, got_s = false, got_p = false;   // Required flags
        bool got_4 = false, got_6 = false;                  // Mutually exclusive

        opterr = 0;                                 // Suppress getopt help
        int ch;
        while ((ch = getopt(argc, argv, "u:s:p:46a")) != -1) {
            switch (ch) {
            case 'u':                              // Player identifier.
                if (got_u)  fatal("ERROR: option -u given more than once");
                player_id = optarg;
                got_u = true;
                break;

            case 's':                               // Server IP or hostname.
                if (got_s)  fatal("ERROR: option -s given more than once");
                server_ip = optarg;
                got_s = true;
                break;

            case 'p':            // Server port.
                if (got_p)  fatal("ERROR: option -p given more than once");
                server_port = read_port(optarg);
                if (server_port == 0)               // 0 means invalid here.
                    fatal("ERROR: port must be 1–65535");
                got_p = true;
                break;

            case '4':   // Force IPv4 only.
                if (got_4) fatal("ERROR: option -4 given more than once");
                got_4 = true;
                ipv_type = 4;
                break;

            case '6':   // Force IPv6 only.
                if (got_6) fatal("ERROR: option -6 given more than once");
                got_6 = true;
                ipv_type = 6;
                break;

            case 'a':   // Enable automatic strategy (bot mode).
                strategy = true;
                break;

            default:
                fatal("ERROR: unknown flag");
            }
        }

        // Verify that all mandatory options are present.
        if (!got_u || !got_s || !got_p)
            fatal("ERROR: -u, -s i -p are neccesery");

        // Ensure the user did not request both IPv4 and IPv6 simultaneously.
        if (got_4 && got_6)
            fatal("ERROR: both -4 and -6");

    }

    // verify_client_input validates the semantic correctness of already-parsed
    // client parameters such as identifier format and non-empty server IP.
    inline void verify_client_input(const std::string& player_id,
                                    const std::string& server_ip,
                                    const uint16_t& server_port,    /*server_port*/
                                    const int& ipv_type,    /*ipv_type*/
                                    const bool& strategy    /*strategy*/) {
        
        std::regex rx("^[A-Za-z0-9]+$");
        if (!std::regex_match(player_id, rx)) {
            fatal("ERROR: invalid player_id: must be non-empty and contain only A–Z, a–z, 0–9");
        }
        if (server_ip == "") fatal("ERROR: wrong input");
    }

     // verify_server_input performs the same role for server-side parameters.
    inline void verify_server_input(const uint16_t& port,
                                    const int& k,
                                    const int& n,
                                    const int& m,
                                    const std::string& filename) {

        if (k > 10000 || k < 0) fatal("ERROR: wrong input");
        if (n > 8 || n < 1) fatal("ERROR: wrong input");
        if (m > 12341234 || m < 1) fatal("ERROR: wrong input");
        if (filename == "") fatal("ERROR: wrong input");
    }

    // to_rational converts a floating-point value to a string with up to seven
    // fractional digits, trimming trailing zeros for compactness.
    inline std::string to_rational(double val) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(7) << val;
        std::string s = oss.str();
        auto pos = s.find('.');
        if (pos != std::string::npos) {
            while (s.size() > pos + 1 && s.back() == '0') {
                s.pop_back();
            }
            if (s.back() == '.') {
                s.pop_back();
            }
        }
        return s;
    }

    // get_next_line reads a single line from a file, appends a newline, and
    // returns the result or an empty string on EOF.
    inline std::string get_next_line(std::ifstream& in) {
        std::string line;
        if (std::getline(in, line)) {
            return line + "\n";
        } else {}
            return std::string();
    }   
}    // namespace common