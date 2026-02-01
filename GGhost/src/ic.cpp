#include "ic.hpp"
#include "sys.hpp"
#include <Windows.h>

extern "C" void ic_handler();
namespace ggh {
    namespace ic {
        void init(/* 这很同性恋 */) {
            sys::PROCESS_INSTRUMENTATION_CALLBACK_INFORMATION c = { 0, 0, (PVOID)ic_handler };
            sys::s(GetCurrentProcess(), &c);
        }
    }
}