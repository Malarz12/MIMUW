#ifndef ERR_H
#define ERR_H

#include <string>

// Fatal error: prints ERROR <msg> and exits with code 1.
[[noreturn]] void fatal(const std::string &msg);

// Non-fatal message error: prints ERROR MSG <hex-data> (up to 10 bytes) and continues.
void error(const std::string &msg);

// System error: prints ERROR <msg>: <errno-string> and exits with code 1.
[[noreturn]] void syserr(const std::string &msg);

#endif // ERR_H