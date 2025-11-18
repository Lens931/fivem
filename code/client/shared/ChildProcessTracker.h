#pragma once

#include <windows.h>

namespace childproc
{
        void TrackProcess(HANDLE processHandle, const char* tag = nullptr);
}
