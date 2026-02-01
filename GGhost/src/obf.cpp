#include "obf.hpp"
#include "sys.hpp"
#include "spf.hpp"
#include <Windows.h>
#include <psapi.h>
#pragma code_seg(".safetext")
#pragma comment(linker, "/SECTION:.safetext,ER")
namespace ggh {
    namespace obf {
        void sleep(uint32_t m) {
            void* b = GetModuleHandle(0);
            void* a = 0; SIZE_T s = 0; ULONG o;
            sys::ft(b, &a, &s);
            if (!a) return;
            sys::p(GetCurrentProcess(), &a, &s, PAGE_READWRITE, &o);
            LARGE_INTEGER l; l.QuadPart = -(long long)m * 10000;
            spf::delay(&l);
            sys::p(GetCurrentProcess(), &a, &s, PAGE_EXECUTE_READ, &o);
        }
    }
}