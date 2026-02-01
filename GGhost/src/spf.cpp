#include "spf.hpp"
#include "sys.hpp"
#include <vector>
#pragma code_seg(".safetext")
#pragma comment(linker, "/SECTION:.safetext,ER")
extern "C" void spf_invoke(void* t, void* g, uint64_t a1, uint64_t a2);
extern "C" void ggh_sys();
extern "C" uint32_t g_ssn;
extern "C" uint64_t g_addr;
namespace ggh {
    namespace spf {
        uint32_t s_d = 0;
        void* g_ret = 0;
        void* g_gad = 0;

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
        void fr(void* m) {
            IMAGE_DOS_HEADER* d=(IMAGE_DOS_HEADER*)m;
            IMAGE_NT_HEADERS* n=(IMAGE_NT_HEADERS*)((uint8_t*)m+d->e_lfanew);
            IMAGE_SECTION_HEADER* s=IMAGE_FIRST_SECTION(n);
            for(int i=0;i<n->FileHeader.NumberOfSections;++i){
                if(memcmp(s[i].Name,".text",5)==0){
                    uint8_t* b=(uint8_t*)m+s[i].VirtualAddress;
                    for(size_t j=0;j<s[i].Misc.VirtualSize;++j)
                        if(b[j]==0xC3) { g_ret = b+j; return; }
                }
            }
        }
        void init() {
            HMODULE n = GetModuleHandleA("ntdll.dll");
            s_d = fs(GetProcAddress(n, "NtDelayExecution"));
            g_gad = fg(n);
            HMODULE k = GetModuleHandleA("kernel32.dll");
            fr(k);
        }
        void delay(LARGE_INTEGER* i) {
            g_ssn = s_d;
            g_addr = (uint64_t)g_gad;
            spf_invoke((void*)ggh_sys, g_ret, 0, (uint64_t)i);
        }
    }
}