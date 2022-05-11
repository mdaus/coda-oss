#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#define NOMINMAX // no min()/max() macros
#include <windows.h>
#include <Winsock2.h>
#pragma comment(lib, "Ws2_32")
