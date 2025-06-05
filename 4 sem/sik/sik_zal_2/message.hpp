#pragma once   // Ensure this header is included only once in any translation unit.

/* --------------------------------------------------------------------------
 * Standard-library headers required for string handling, tokenisation, and
 * numeric limits used when validating incoming messages.
 * --------------------------------------------------------------------------*/
#include <string>
#include <vector>
#include <sstream>
#include <limits>

/* --------------------------------------------------------------------------
 * The message namespace contains helper functions that *build* protocol lines
 * for transmission over the TCP connection.
 * --------------------------------------------------------------------------*/

namespace message {

    /* add_suffix appends “\r\n” so every generated string is a complete,
     * CR-LF-terminated line ready for send(). */
    inline void add_suffix(std::string& msg) {
        msg.push_back('\r');  // Carriage Return (ASCII 13)
        msg.push_back('\n');  // Line Feed (ASCII 10)
    }

    /* HELLO_msg formats the initial greeting sent by a client. */
    inline std::string HELLO_msg(const std::string& player_id) {
        std::string res = "HELLO " + player_id;
        add_suffix(res);
        return res;
    }

    /* COEFF_msg sends the polynomial coefficients from server to client. */
    inline std::string COEFF_msg(const std::vector<std::string>& coeff) {
        std::string res = "COEFF";
        for (const auto& c : coeff) {
            res += ' ';
            res += c;
        }
        add_suffix(res);
        return res;
    }

    /* PUT_msg carries a player’s suggested adjustment to the prediction vector. */
    inline std::string PUT_msg(const std::string& point, const std::string& value) {
        std::string res = "PUT " + point + " " + value;
        add_suffix(res);
        return res;
    }

    /* BAD_PUT_msg tells the client that its previous PUT was malformed. */
    inline std::string BAD_PUT_msg(const std::string& point, const std::string& value) {
        std::string res = "BAD_PUT " + point + " " + value;
        add_suffix(res);
        return res;
    }

    /* STATE_msg publishes the entire prediction vector after a legal PUT. */
    inline std::string STATE_msg(const std::vector<std::string>& values) {
        std::string res = "STATE";
        for (const auto& v : values) {
            res += ' ';
            res += v;
        }
        add_suffix(res);
        return res;
    }

    /* PENALTY_msg imposes a fixed penalty because the client broke a rule. */
    inline std::string PENALTY_msg(const std::string& point, const std::string& value) {
        std::string res = "PENALTY " + point + " " + value;
        add_suffix(res);
        return res;
    }

    /* SCORING_msg ends the game and reports every player’s final score. */
    inline std::string SCORING_msg(const std::vector<std::string>& id, const std::vector<std::string>& score) {
        std::string res = "SCORING";
        size_t n = std::min(id.size(), score.size());
        for (size_t i = 0; i < n; ++i) {
            res += ' ';
            res += id[i];
            res += ' ';
            res += score[i];
        }
        add_suffix(res);
        return res;
    }

} // namespace message

namespace verification {

    /* extract_body verifies that msg starts with prefix and ends with suffix,
     * then returns the substring in between. */
    static bool extract_body(const std::string& msg,
                             const std::string& prefix,
                             const std::string& suffix,
                             std::string& out_body) {
        if (msg.size() <= prefix.size() + suffix.size()) return false;
        if (msg.compare(0, prefix.size(), prefix) != 0) return false;
        if (msg.compare(msg.size() - suffix.size(), suffix.size(), suffix) != 0) return false;
        out_body = msg.substr(prefix.size(),
                              msg.size() - prefix.size() - suffix.size());
        return !out_body.empty();
    }

    /* is_alnum_str returns true only when every character is a letter or digit. */
    static bool is_alnum_str(const std::string& s) {
        if (s.empty()) return false;

        for (char ch : s)
            if (!std::isalnum(static_cast<unsigned char>(ch)))
                return false;

        return true;
    }

    /* parse_rational converts a token to double while enforcing the project’s
     * grammar: optional minus, digits, optional period, up to seven decimals. */
    static bool parse_rational(const std::string& token, double& out_val) {
        size_t pos = token.find('.');
        std::string ip = token.substr(0, pos);
        std::string fp = (pos == std::string::npos ? "" : token.substr(pos + 1));

        if (ip.empty()) return false;

        // Check integer part (allow optional leading minus).
        size_t start = 0;
        if (ip[0] == '-') {
            if (ip.size() == 1) return false;
            start = 1;
        }

        for (size_t i = start; i < ip.size(); ++i) {
            if (!std::isdigit(static_cast<unsigned char>(ip[i]))) return false;
        }

        // Check fractional part.
        if (pos != std::string::npos) {
            if (fp.size() > 7) return false;
            for (char ch : fp) {
                if (!std::isdigit(static_cast<unsigned char>(ch)))
                    return false;
            }
        }

        try {
            out_val = std::stod(token);
        } catch (...) {
            return false;
        }
        return true;
    }



    /* split_ws splits a string on ASCII whitespace exactly like a shell. */
    static std::vector<std::string> split_ws(const std::string& s) {
        std::istringstream iss(s);
        std::vector<std::string> toks;
        std::string tok;
        while (iss >> tok) toks.push_back(std::move(tok));
        return toks;
    }

    /* Each verify_XXX function checks a specific message type and returns the
     * parsed fields through reference parameters when successful. */
    bool verify_HELLO(const std::string& msg, std::string& out_player_id) {
        std::string body;
        if (!extract_body(msg, "HELLO ", "\r\n", body)) return false;
        if (!is_alnum_str(body)) return false;
        out_player_id = body;
        return true;
    }

    bool verify_COEFF(const std::string& msg, std::vector<double>& out_coeffs) {
        std::string body;
        if (!extract_body(msg, "COEFF ", "\r\n", body)) {
            return false;
        }
        auto toks = split_ws(body);
        if (toks.empty() || toks.size() > 8) {
            return false;
        }
        out_coeffs.clear();
        for (auto& t : toks) {
            double v;
            if (!parse_rational(t, v)) {
                std::cout<<t<<"bb\r\n";
                return false;
            }
            out_coeffs.push_back(v);
        }
        return true;
    }

    bool verify_PUT(const std::string& msg, int& out_point, double& out_value) {
        std::string body;
        if (!extract_body(msg, "PUT ", "\r\n", body)) return false;

        auto toks = split_ws(body);
        if (toks.size() != 2) return false;

        /* --- point --- */
        for (char ch : toks[0]) {
            if (!std::isdigit(static_cast<unsigned char>(ch)))
                return false;
        }
        try {
            out_point = std::stoi(toks[0]);
        } catch (...) {
            return false;
        }

        /* --- value --- */
        return parse_rational(toks[1], out_value);
    }

    bool verify_PENALTY(const std::string& msg, int& out_point, double& out_value) {
        std::string body;
        if (!extract_body(msg, "PENALTY ", "\r\n", body))
            return false;

        auto toks = split_ws(body);
        if (toks.size() != 2)
            return false;

        /* --- point --- */
        for (char ch : toks[0]) {
            if (!std::isdigit(static_cast<unsigned char>(ch)))
                return false;
        }
        try {
            out_point = std::stoi(toks[0]);
        } catch (...) {
            return false;
        }

        /* --- value --- */
        return parse_rational(toks[1], out_value);
    }


    bool verify_BAD_PUT(const std::string& msg, int& out_point, double& out_value) {
        std::string body;
        if (!extract_body(msg, "BAD_PUT ", "\r\n", body))
            return false;

        auto toks = split_ws(body);
        if (toks.size() != 2)
            return false;

        /* --- point --- */
        for (char ch : toks[0]) {  // ← był unsigned char c
            if (!std::isdigit(static_cast<unsigned char>(ch)))
                return false;
        }
        try {
            out_point = std::stoi(toks[0]);
        } catch (...) {
            return false;
        }

        /* --- value --- */
        return parse_rational(toks[1], out_value);
    }

    bool verify_STATE(const std::string& msg, std::vector<double>& out_states) {
        std::string body;
        if (!extract_body(msg, "STATE ", "\r\n", body)) return false;
        auto toks = split_ws(body);
        if (toks.empty()) return false;
        out_states.clear();
        for (auto& t : toks) {
            double v;
            if (!parse_rational(t, v)) return false;
            out_states.push_back(v);
        }
        return true;
    }

    bool verify_SCORING(const std::string& msg, std::vector<std::pair<std::string,double>>& out_scores) {
        std::string body;
        if (!extract_body(msg, "SCORING ", "\r\n", body)) {
            return false;
        }
        auto toks = split_ws(body);
        if (toks.size() < 2 || (toks.size() % 2) != 0) {
            return false;
        }
        out_scores.clear();

        for (size_t i = 0; i < toks.size(); i += 2) {
            // player_id
            if (!is_alnum_str(toks[i])) {
                return false;
            }// result
            double r;
            if (!parse_rational(toks[i+1], r)) {
                return false;
            }
            out_scores.emplace_back(toks[i], r);
        }
        return true;
    }

} // namespace verification