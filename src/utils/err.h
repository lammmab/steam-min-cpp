// GPT

#pragma once

#include <stdexcept>
#include <sstream>
#include <string>
#include "utils/fmt/format.h"

namespace runtime_err
{
    [[noreturn]] inline void throw_error(
        const char* expr,
        const char* msg,
        const char* file,
        int line,
        const char* func)
    {
        std::ostringstream oss;
        oss << "Runtime error";

        if (expr)
            oss << " | expr: (" << expr << ")";

        if (msg && *msg)
            oss << " | msg: " << msg;

        oss << " | at " << file << ":" << line
            << " | in " << func;

        throw std::runtime_error(oss.str());
    }
}

/* Unconditional runtime failure */
#define RUNTIME_FAIL(msg) \
    ::runtime_err::throw_error(nullptr, msg, __FILE__, __LINE__, __func__)

/* Assert with message */
#define RUNTIME_ASSERT(expr, msg) \
    do { \
        if (!(expr)) { \
            ::runtime_err::throw_error(#expr, msg, __FILE__, __LINE__, __func__); \
        } \
    } while (0)

/* Assert with formatted message */
#define RUNTIME_ASSERTF(expr, fmt_str, ...) \
    do { \
        if (!(expr)) { \
            auto _msg = fmt::format(fmt_str, __VA_ARGS__); \
            ::runtime_err::throw_error(#expr, _msg.c_str(), __FILE__, __LINE__, __func__); \
        } \
    } while (0)

/* Assert without custom message */
#define RUNTIME_CHECK(expr) \
    RUNTIME_ASSERT(expr, "")

/* Null check helper */
#define RUNTIME_ASSERT_NOT_NULL(ptr) \
    RUNTIME_ASSERT((ptr) != nullptr, "null pointer")

/* Range check helper */
#define RUNTIME_ASSERT_RANGE(val, min, max) \
    RUNTIME_ASSERT(((val) >= (min) && (val) <= (max)), "value out of range")