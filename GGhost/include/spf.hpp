#pragma once
#include <Windows.h>
#include <cstdint>

namespace ggh {
    namespace spf {
        extern void* g_ret;
        void init();
        void delay(LARGE_INTEGER* i);
    }
}