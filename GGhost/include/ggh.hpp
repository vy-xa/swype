#pragma once
#include <Windows.h>
#include <cstdint>
#include <vector>
#include <string>

    namespace ggh {
        void init();
        void read(uint32_t p, uint64_t a, void* b, size_t s);
        void write(uint32_t p, uint64_t a, void* b, size_t s);
        void sleep(uint32_t m);
        uint32_t get_pid(const std::wstring& n);
        HANDLE open(uint32_t p, uint32_t a);
        size_t query(HANDLE h, uint64_t a, void* i, size_t s);
        uint64_t get_module_base(uint32_t pid);
    }
