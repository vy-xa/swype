#pragma once
#include <Windows.h>
#include <cstdint>

extern "C" uint32_t g_ssn;
extern "C" uint64_t g_addr;
extern "C" void ggh_sys();

namespace ggh {
    namespace sys {
        typedef struct _UNICODE_STRING {
            USHORT Length;
            USHORT MaximumLength;
            PWSTR  Buffer;
        } UNICODE_STRING;

        typedef struct _OBJECT_ATTRIBUTES {
            ULONG           Length;
            HANDLE          RootDirectory;
            UNICODE_STRING* ObjectName;
            ULONG           Attributes;
            PVOID           SecurityDescriptor;
            PVOID           SecurityQualityOfService;
        } OBJECT_ATTRIBUTES;

        typedef struct _CLIENT_ID {
            HANDLE UniqueProcess;
            HANDLE UniqueThread;
        } CLIENT_ID;

        typedef struct _PROCESS_INSTRUMENTATION_CALLBACK_INFORMATION {
            ULONG Version;
            ULONG Reserved;
            PVOID Callback;
        } PROCESS_INSTRUMENTATION_CALLBACK_INFORMATION, *PPROCESS_INSTRUMENTATION_CALLBACK_INFORMATION;

        typedef struct _SYSTEM_HANDLE_TABLE_ENTRY_INFO {
            USHORT UniqueProcessId;
            USHORT CreatorBackTraceIndex;
            UCHAR ObjectTypeIndex;
            UCHAR HandleAttributes;
            USHORT HandleValue;
            PVOID Object;
            ULONG GrantedAccess;
        } SYSTEM_HANDLE_TABLE_ENTRY_INFO, *PSYSTEM_HANDLE_TABLE_ENTRY_INFO;

        typedef struct _SYSTEM_HANDLE_INFORMATION {
            ULONG NumberOfHandles;
            SYSTEM_HANDLE_TABLE_ENTRY_INFO Handles[1];
        } SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;

        void init();
        void set(uint32_t s);
        void r(HANDLE h, uint64_t a, void* b, size_t s);
        void w(HANDLE h, uint64_t a, void* b, size_t s);
        void p(HANDLE h, void** a, SIZE_T* s, ULONG n, ULONG* o);
        void d(LARGE_INTEGER* i);
        void s(HANDLE h, PROCESS_INSTRUMENTATION_CALLBACK_INFORMATION* i);
        void ft(void* m, void** a, SIZE_T* z);
        void o(PHANDLE h, ACCESS_MASK a, OBJECT_ATTRIBUTES* b, CLIENT_ID* c);
        void q(HANDLE h, void* a, int c, void* b, size_t s, size_t* r);
        void qsi(int c, void* b, ULONG l, ULONG* r);
        void dup(HANDLE s, HANDLE sh, HANDLE t, PHANDLE th, ACCESS_MASK a, ULONG attr, ULONG o);
        void qsi(int c, void* b, ULONG l, ULONG* r);
        void dup(HANDLE s, HANDLE sh, HANDLE t, PHANDLE th, ACCESS_MASK a, ULONG attr, ULONG o);
    }
}