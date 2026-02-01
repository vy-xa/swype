#include "sys.hpp"
#include "spf.hpp"
#include <vector>
#pragma code_seg(".safetext")
#pragma comment(linker, "/SECTION:.safetext,ER")
extern "C" void spf_invoke_5(void* t, void* g, uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4, uint64_t a5);
extern "C" void spf_invoke_7(void* t, void* g, uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4, uint64_t a5, uint64_t a6, uint64_t a7);
namespace ggh {
    namespace sys {
        uint32_t s_r=0, s_w=0, s_p=0, s_d=0, s_s=0, s_o=0, s_q=0, s_qsi=0, s_dup=0;
        void* g_gad=0;
        uint32_t fs(void* f) {
            uint8_t* p = (uint8_t*)f;
            if (p[0]==0x4C && p[1]==0x8B && p[2]==0xD1 && p[3]==0xB8) return *(uint32_t*)(p+4);
            return 0;
        }
        void* fg(void* m) {
            IMAGE_DOS_HEADER* d=(IMAGE_DOS_HEADER*)m;
            IMAGE_NT_HEADERS* n=(IMAGE_NT_HEADERS*)((uint8_t*)m+d->e_lfanew);
            IMAGE_SECTION_HEADER* s=IMAGE_FIRST_SECTION(n);
            for(int i=0;i<n->FileHeader.NumberOfSections;++i){
                if(memcmp(s[i].Name,".text",5)==0){
                    uint8_t* b=(uint8_t*)m+s[i].VirtualAddress;
                    for(size_t j=0;j<s[i].Misc.VirtualSize;++j)
                        if(b[j]==0x0F && b[j+1]==0x05 && b[j+2]==0xC3) return b+j;
                }
            }
            return 0;
        }
        void init() {
            HMODULE n = GetModuleHandleA("ntdll.dll");
            s_r = fs(GetProcAddress(n, "NtReadVirtualMemory"));
            s_w = fs(GetProcAddress(n, "NtWriteVirtualMemory"));
            s_p = fs(GetProcAddress(n, "NtProtectVirtualMemory"));
            s_d = fs(GetProcAddress(n, "NtDelayExecution"));
            s_s = fs(GetProcAddress(n, "NtSetInformationProcess"));
            s_o = fs(GetProcAddress(n, "NtOpenProcess"));
            s_q = fs(GetProcAddress(n, "NtQueryVirtualMemory"));
            s_qsi = fs(GetProcAddress(n, "NtQuerySystemInformation"));
            s_dup = fs(GetProcAddress(n, "NtDuplicateObject"));
            g_gad = fg(n);
        }
        void r(HANDLE h, uint64_t a, void* b, size_t s) {
            g_ssn = s_r; g_addr = (uint64_t)g_gad;
            spf_invoke_5((void*)ggh_sys, ggh::spf::g_ret, (uint64_t)h, a, (uint64_t)b, s, 0);
        }
        void w(HANDLE h, uint64_t a, void* b, size_t s) {
            g_ssn = s_w; g_addr = (uint64_t)g_gad;
            spf_invoke_5((void*)ggh_sys, ggh::spf::g_ret, (uint64_t)h, a, (uint64_t)b, s, 0);
        }
        void p(HANDLE h, void** a, SIZE_T* s, ULONG n, ULONG* o) {
            g_ssn = s_p; g_addr = (uint64_t)g_gad;
            auto f = (long(*)(HANDLE,void**,SIZE_T*,ULONG,ULONG*))ggh_sys;
            f(h,a,s,n,o);
        }
        void d(LARGE_INTEGER* i) {
            g_ssn = s_d; g_addr = (uint64_t)g_gad;
            auto f = (long(*)(BOOLEAN,LARGE_INTEGER*))ggh_sys;
            f(0,i);
        }
        void s(HANDLE h, PROCESS_INSTRUMENTATION_CALLBACK_INFORMATION* i) {
            g_ssn = s_s; g_addr = (uint64_t)g_gad;
            auto f = (long(*)(HANDLE,int,void*,ULONG))ggh_sys;
            f(h,40,i,sizeof(*i));
        }
        void o(PHANDLE h, ACCESS_MASK a, OBJECT_ATTRIBUTES* b, CLIENT_ID* c) {
            g_ssn = s_o; g_addr = (uint64_t)g_gad;
            auto f = (long(*)(PHANDLE,ACCESS_MASK,OBJECT_ATTRIBUTES*,CLIENT_ID*))ggh_sys;
            f(h,a,b,c);
        }
        void q(HANDLE h, void* a, int c, void* b, size_t s, size_t* r) {
            g_ssn = s_q; g_addr = (uint64_t)g_gad;
            auto f = (long(*)(HANDLE,void*,int,void*,size_t,size_t*))ggh_sys;
            f(h,a,c,b,s,r);
        }
        void qsi(int c, void* b, ULONG l, ULONG* r) {
            g_ssn = s_qsi; g_addr = (uint64_t)g_gad;
            auto f = (long(*)(int,void*,ULONG,ULONG*))ggh_sys;
            f(c,b,l,r);
        }
        void dup(HANDLE s, HANDLE sh, HANDLE t, PHANDLE th, ACCESS_MASK a, ULONG attr, ULONG o) {
            g_ssn = s_dup; g_addr = (uint64_t)g_gad;
            spf_invoke_7((void*)ggh_sys, ggh::spf::g_ret, (uint64_t)s, (uint64_t)sh, (uint64_t)t, (uint64_t)th, (uint64_t)a, (uint64_t)attr, (uint64_t)o);
        }
        void ft(void* m, void** a, SIZE_T* z) {
            IMAGE_DOS_HEADER* d=(IMAGE_DOS_HEADER*)m;
            IMAGE_NT_HEADERS* n=(IMAGE_NT_HEADERS*)((uint8_t*)m+d->e_lfanew);
            IMAGE_SECTION_HEADER* s=IMAGE_FIRST_SECTION(n);
            for(int i=0;i<n->FileHeader.NumberOfSections;++i){
                if(memcmp(s[i].Name,".text",5)==0){
                    *a = (uint8_t*)m+s[i].VirtualAddress;
                    *z = s[i].Misc.VirtualSize;
                    return;
                }
            }
            *a = 0; *z = 0;
        }
    }
}
