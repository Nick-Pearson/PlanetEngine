#pragma once

#include "Platform/Platform.h"
#include <iostream>
#include "Container/String.h"

#define P_LOG(Category, Format, ...) \
	{ \
	String msg =  String::Printf(Format, ##__VA_ARGS__); \
	String LogMsg = String::Printf(TEXT("[%s] %s\n"), #Category , *msg); \
	std::cout << *LogMsg << std::endl; \
	OutputDebugString(*LogMsg); \
	}


#define P_ERROR(Category, Format, ...) P_LOG(Category, Format, ##__VA_ARGS__)