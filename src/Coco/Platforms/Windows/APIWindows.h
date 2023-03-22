#pragma once

#ifdef COCO_PLATFORM_WINDOWS
#ifdef COCO_DLL
#if COCO_EXPORT
// DLL export
#define COCOAPI __declspec(dllexport)
#else
// DLL import
#define COCOAPI __declspec(dllimport)
#endif
#else
// Statically linked, no export/import needed
#define COCOAPI
#endif
#endif