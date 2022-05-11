#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#define NOMINMAX // no min()/max() macros
#include <Winsock2.h>
#pragma comment(lib, "Ws2_32")

