#pragma once

#include <ctime>
#include <iostream>

#include "Platform/Platform.h"
#include "Container/String.h"

#define P_LOG(Format, ...) \
    { \
    String msg =  String::Printf(Format, ##__VA_ARGS__); \
    time_t * rawtime = new time_t; \
    struct tm * timeinfo; \
    time(rawtime); \
    timeinfo = localtime(rawtime); \
    char buf[80]; \
    strftime(buf, sizeof(buf), "%X", timeinfo); \
    String LogMsg = String::Printf(TEXT("%s [%s] %s\n"), buf, typeid(*this).name(), *msg); \
    OutputDebugString(*LogMsg); \
    }


#define P_ERROR(Format, ...) P_LOG(Format, ##__VA_ARGS__)