/*
 * CrossFactionResFix.dll — OctoWoW WoW 1.12.1 in-memory patch
 * Byte offset in the input PE file: 0x2067DE.
 * In the verified WoW.exe, .text raw offset equals RVA, so RVA = 0x2067DE.
 *
 * Uses VanillaFixes dlls.txt loading. Leaves WoW.exe on disk untouched.
 * Does nothing when the surrounding instruction bytes differ.
 * Compile for 32-bit Windows. No C runtime required.
 */

#define WINAPI __stdcall
#define DLL_PROCESS_ATTACH 1u
#define PAGE_EXECUTE_READWRITE 0x40u

typedef unsigned long DWORD;
typedef int BOOL;
typedef void *HANDLE;
typedef void *HMODULE;
typedef void *LPVOID;

__declspec(dllimport) HMODULE WINAPI GetModuleHandleA(const char *);
__declspec(dllimport) BOOL WINAPI VirtualProtect(LPVOID, DWORD, DWORD, DWORD *);
__declspec(dllimport) BOOL WINAPI FlushInstructionCache(HANDLE, const void *, DWORD);
__declspec(dllimport) HANDLE WINAPI GetCurrentProcess(void);
__declspec(dllimport) void WINAPI OutputDebugStringA(const char *);

static void patch_crossfaction_res(void) {
    const unsigned int rva = 0x2067DEu;
    const unsigned char expected_prefix[] = {0x83, 0xF8};
    const unsigned char expected_suffix[] = {0x0F, 0x9D, 0xC0, 0x5D, 0xC2, 0x04, 0x00};
    HMODULE main_exe = GetModuleHandleA((const char *)0);
    unsigned char *at;
    DWORD old_protect = 0, unused_protect = 0;
    unsigned int i;

    if (!main_exe) return;
    at = (unsigned char *)main_exe + rva;
    for (i = 0; i < sizeof(expected_prefix); ++i) {
        if (at[(int)i - 2] != expected_prefix[i]) {
            OutputDebugStringA("CrossFactionResFix: opcode mismatch (prefix), no patch\n");
            return;
        }
    }
    for (i = 0; i < sizeof(expected_suffix); ++i) {
        if (at[i + 1] != expected_suffix[i]) {
            OutputDebugStringA("CrossFactionResFix: opcode mismatch (suffix), no patch\n");
            return;
        }
    }
    if (*at == 0x00u) {
        OutputDebugStringA("CrossFactionResFix: already applied\n");
        return;
    }
    if (*at != 0x04u) {
        OutputDebugStringA("CrossFactionResFix: unexpected target byte, no patch\n");
        return;
    }
    if (!VirtualProtect(at, 1u, PAGE_EXECUTE_READWRITE, &old_protect)) {
        OutputDebugStringA("CrossFactionResFix: VirtualProtect failed\n");
        return;
    }
    *at = 0x00u;
    FlushInstructionCache(GetCurrentProcess(), at, 1u);
    VirtualProtect(at, 1u, old_protect, &unused_protect);
    OutputDebugStringA("CrossFactionResFix: patch applied in memory\n");
}

BOOL WINAPI DllMain(HMODULE module, DWORD reason, LPVOID reserved) {
    (void)module;
    (void)reserved;
    if (reason == DLL_PROCESS_ATTACH) patch_crossfaction_res();
    return 1;
}
