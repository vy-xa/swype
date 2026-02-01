#include "../include/ggh.hpp"
#include "../include/sys.hpp"
#include "../include/ic.hpp"
#include "../include/obf.hpp"
#include "../include/spf.hpp"
#include <tlhelp32.h>
#include <vector>

namespace ggh {
    void enable_debug() {
        HANDLE hToken;
        if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
            LUID luid;
            if (LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid)) {
                TOKEN_PRIVILEGES tp;
                tp.PrivilegeCount = 1;
                tp.Privileges[0].Luid = luid;
                tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
                AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
            }
            CloseHandle(hToken);
        }
    }
    HANDLE hijack_handle(uint32_t tpid) {
        ULONG sz = 0x10000;
        std::vector<uint8_t> buf(sz);
        sys::qsi(16, buf.data(), sz, &sz);
        while (sz > buf.size()) {
            buf.resize(sz);
            sys::qsi(16, buf.data(), sz, &sz);
        }
        sys::SYSTEM_HANDLE_INFORMATION* hi = (sys::SYSTEM_HANDLE_INFORMATION*)buf.data();
        /* 劫持 csrss.exe*/
        std::vector<uint32_t> targets;
        HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        PROCESSENTRY32W e; e.dwSize = sizeof(e);
        if (Process32FirstW(hSnap, &e)) {
            do {
                if (_wcsicmp(e.szExeFile, L"csrss.exe") == 0 || _wcsicmp(e.szExeFile, L"lsass.exe") == 0) {
                    targets.push_back(e.th32ProcessID);
                }
            } while (Process32NextW(hSnap, &e));
        }
        CloseHandle(hSnap);
        for (ULONG i = 0; i < hi->NumberOfHandles; ++i) {
            sys::SYSTEM_HANDLE_TABLE_ENTRY_INFO& h = hi->Handles[i];
            bool is_target = false;
            for(auto t : targets) if(h.UniqueProcessId == t) { is_target=true; break; }
            if (!is_target) continue;
            if ((h.GrantedAccess & (PROCESS_VM_READ | PROCESS_VM_WRITE)) == (PROCESS_VM_READ | PROCESS_VM_WRITE)) {
                HANDLE proc_h = open(h.UniqueProcessId, PROCESS_DUP_HANDLE);
                if (proc_h) {
                    HANDLE dup_h = 0;
                    sys::dup(proc_h, (HANDLE)(uintptr_t)h.HandleValue, GetCurrentProcess(), &dup_h, 0, 0, 2); // Same access
                    if (dup_h) {
                        if (GetProcessId(dup_h) == tpid) {
                            CloseHandle(proc_h);
                            return dup_h;
                        }
                        CloseHandle(dup_h);
                    }
                    CloseHandle(proc_h);
                }
            }
        }
        return 0;
    }
    void init() {
        enable_debug();
        sys::init();
        spf::init();
        ic::init();
    }
    void read(uint32_t p, uint64_t a, void* b, size_t s) {
        static HANDLE h = 0;
        if (!h || GetProcessId(h) != p) h = hijack_handle(p);
        if (!h) h = open(p, PROCESS_ALL_ACCESS);
        if (h) {
            sys::r(h, a, b, s);
        }
    }
    void write(uint32_t p, uint64_t a, void* b, size_t s) {
        static HANDLE h = 0;
        if (!h || GetProcessId(h) != p) h = hijack_handle(p);
        if (!h) h = open(p, PROCESS_ALL_ACCESS);
        if (h) {
            sys::w(h, a, b, s);
        }
    }
    uint64_t get_module_base(uint32_t pid) {
        HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
        if (hSnap != INVALID_HANDLE_VALUE) {
            MODULEENTRY32W me; me.dwSize = sizeof(me);
            if (Module32FirstW(hSnap, &me)) {
                do {
                    if (wcscmp(me.szModule, L"RobloxPlayerBeta.exe") == 0) {
                        CloseHandle(hSnap);
                        return (uint64_t)me.modBaseAddr;
                    }
                } while (Module32NextW(hSnap, &me));
            }
            CloseHandle(hSnap);
        }
        return 0;
    }
    HANDLE open(uint32_t p, uint32_t a) {
        sys::CLIENT_ID c = { (HANDLE)p, 0 };
        sys::OBJECT_ATTRIBUTES o = { sizeof(o), 0, 0, 0, 0, 0 };
        HANDLE h = 0;
        sys::o(&h, a, &o, &c);
        return h;
    }
    size_t query(HANDLE h, uint64_t a, void* i, size_t s) {
        size_t r = 0;
        sys::q(h, (void*)a, 0, i, s, &r);
        return r;
    }
    void sleep(uint32_t m) {
        obf::sleep(m);
    }
    uint32_t get_pid(const std::wstring& n) {
        HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        PROCESSENTRY32W e; e.dwSize = sizeof(e);
        if (Process32FirstW(h, &e)) {
            do { if (n == e.szExeFile) { CloseHandle(h); return e.th32ProcessID; } } while (Process32NextW(h, &e));
        }
        CloseHandle(h); return 0;
    }
}
