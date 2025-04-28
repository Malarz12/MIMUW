#include "err.h"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <iomanip>
#include <sstream>

[[noreturn]] void fatal(const std::string &msg) {
    // Print up to first 10 bytes of the raw message in hex and exit.
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    size_t len = msg.size() < 10 ? msg.size() : 10;
    for (size_t i = 0; i < len; ++i) {
        uint8_t byte = static_cast<uint8_t>(msg[i]);
        oss << std::setw(2) << static_cast<unsigned>(byte);
    }
    std::cerr << "ERROR MSG " << oss.str() << std::endl;
    std::exit(1);
}

void error(const std::string &msg) {
    // Print up to first 10 bytes of the raw message in hex.
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    size_t len = msg.size() < 10 ? msg.size() : 10;
    for (size_t i = 0; i < len; ++i) {
        uint8_t byte = static_cast<uint8_t>(msg[i]);
        oss << std::setw(2) << static_cast<unsigned>(byte);
    }
    std::cerr << "ERROR MSG " << oss.str() << std::endl;
}

[[noreturn]] void syserr(const std::string &msg) {
    // Print up to first 10 bytes of the raw message in hex and exit.
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    size_t len = msg.size() < 10 ? msg.size() : 10;
    for (size_t i = 0; i < len; ++i) {
        uint8_t byte = static_cast<uint8_t>(msg[i]);
        oss << std::setw(2) << static_cast<unsigned>(byte);
    }
    std::cerr << "ERROR MSG " << oss.str() << std::endl;
    std::exit(1);
}