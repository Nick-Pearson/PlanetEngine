#pragma once

#include "spdlog/spdlog.h"

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO

#define P_TRACE(...) SPDLOG_TRACE(##__VA_ARGS__);
#define P_EDITOR(...) SPDLOG_DEBUG(##__VA_ARGS__);
#define P_LOG(...) SPDLOG_INFO(##__VA_ARGS__);
#define P_WARN(...) SPDLOG_WARN(##__VA_ARGS__);
#define P_ERROR(...) SPDLOG_ERROR(##__VA_ARGS__);