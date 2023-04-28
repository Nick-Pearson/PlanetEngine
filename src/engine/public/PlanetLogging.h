#pragma once

#include "spdlog/spdlog.h"

namespace PlanetLogging
{
    extern void init_logging();
}

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO

#define P_TRACE(...) SPDLOG_TRACE(##__VA_ARGS__);
#define P_EDITOR(...) SPDLOG_DEBUG(##__VA_ARGS__);
#define P_LOG(...) SPDLOG_INFO(##__VA_ARGS__);
#define P_WARN(...) SPDLOG_WARN(##__VA_ARGS__);
#define P_ERROR(...) SPDLOG_ERROR(##__VA_ARGS__);
#define P_FATAL(...) \
    { \
        SPDLOG_ERROR(##__VA_ARGS__); \
        exit(1); \
    }
#define P_ASSERT(condition, ...) \
    if (!(condition)) \
    { \
        P_FATAL(##__VA_ARGS__); \
    }