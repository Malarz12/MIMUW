#pragma once

// The iostream header provides the standard streams used for reporting errors.
#include <iostream>
// The cstdlib header supplies the std::exit function used for immediate termination.
#include <cstdlib>

/*
 * fatal prints an error message prefixed with “ERROR:”, flushes it to stderr,
 * and then terminates the program with a non-zero status.
 * The [[noreturn]] attribute tells the compiler that this function never returns,
 * which lets it optimize call sites and suppress certain warnings.
 */
[[noreturn]] inline void fatal(const std::string &msg) {
    std::cerr << "ERROR: " << msg << "\n";
    std::exit(1);
}

/*
 * error prints an error message to stderr but does not terminate the program,
 * allowing the caller to decide how to recover or continue execution.
 */
inline void error(const std::string &msg) {
    std::cerr << "ERROR: " << msg << "\n";
}

/*
 * syserr behaves exactly like fatal, but it exists as a semantic distinction
 * for situations where the underlying cause is a system-level failure such as
 * a failed system call.  It also carries the [[noreturn]] attribute.
 */
[[noreturn]] inline void syserr(const std::string &msg) {
    std::cerr << "ERROR: " << msg << "\n";
    std::exit(1);
}
