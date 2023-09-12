#pragma once

#ifdef COCO_HIGHDPI_SUPPORT
// Require at least Windows 10 1703 "Redstone 2"
#define NTDDI_WIN10_RS2 0x0A000003
#define _WIN32_WINNT_WIN10 0x0A00
#else
// Require at least Windows 7
#define NTDDI_WIN7 0x06010000
#define _WIN32_WINNT_WIN7 0x0601
#endif

// Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define NOGDI
#define NODRAWTEXT
#define NOCTLMGR
#define NOFLATSBAPIS
#define NOSERVICE

// Use unicode version of functions
#ifndef UNICODE
#define UNICODE
#endif 

#include <Windows.h>
#include <windowsx.h>
#include <shellapi.h>

#ifdef COCO_HIGHDPI_SUPPORT
#include <ShellScalingApi.h>
#endif