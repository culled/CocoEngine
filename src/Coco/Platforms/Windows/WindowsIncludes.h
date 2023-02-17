#pragma once

// Require at least Windows 7
#define NTDDI_WIN7 0x06010000
#define _WIN32_WINNT_WIN7 0x0601

// Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define NOGDI
#define NODRAWTEXT
#define NOCTLMGR
#define NOFLATSBAPIS

// Use unicode version of functions
#ifndef UNICODE
#define UNICODE
#endif 

#include <Windows.h>
#include <windowsx.h>
#include <shellapi.h>