#pragma once
#include <windows.h>

#ifdef AUDIOMANAGER_EXPORTS
#define AUDIOMANAGER_API __declspec(dllexport)
#else
#define AUDIOMANAGER_API __declspec(dllimport)
#endif

extern "C" {
    AUDIOMANAGER_API void GetSpeakerConfiguration(char* buffer, int bufferSize);
}
