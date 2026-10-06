#include <windows.h>
#include <cstdio>

struct IDirect3D8;
using Fn_t = void(*)();

HMODULE hPayloadModule = NULL;
const char* MAIN_PATH = "main.dll";
const char* COPY_PATH = "main_copy.dll";

FILETIME GetFileWriteTime(const char* name) {
    FILETIME ft = { 0 };
    HANDLE f = CreateFileA(name, GENERIC_READ,
                           FILE_SHARE_READ | FILE_SHARE_WRITE,
                           NULL, OPEN_EXISTING,
                           FILE_ATTRIBUTE_NORMAL, NULL);

    if (f == INVALID_HANDLE_VALUE) return ft;

    GetFileTime(f, NULL, NULL, &ft);
    CloseHandle(f);
    return ft;
}

void LoadPayload() {
    if (hPayloadModule) {
        Fn_t fn = reinterpret_cast<Fn_t>
            (GetProcAddress(hPayloadModule, "Unload"));
        if (fn) fn();

        FreeLibrary(hPayloadModule);
        hPayloadModule = NULL;
    }

    Sleep(100);

    CopyFileA(MAIN_PATH, COPY_PATH, FALSE);

    hPayloadModule = LoadLibrary(COPY_PATH);

    if (hPayloadModule) {
        Fn_t fn = reinterpret_cast<Fn_t>
            (GetProcAddress(hPayloadModule, "Load"));
        if (fn) fn();
    }
}

DWORD WINAPI FileWatcherThread(LPVOID lpParam) {
    FILETIME last = GetFileWriteTime(MAIN_PATH);
    LoadPayload();

    while (true) {
        Sleep(500);

        FILETIME current = GetFileWriteTime(MAIN_PATH);

        if ((current.dwLowDateTime != last.dwLowDateTime
                    || current.dwHighDateTime != last.dwHighDateTime)
                && current.dwLowDateTime != 0) {
            last = current;
            LoadPayload();
        }
    }

    return 0;
}

void StartHotReloadWatcher() {
    CreateThread(NULL, 0, FileWatcherThread, NULL, 0, NULL);
}

extern "C" IDirect3D8* WINAPI Direct3DCreate8(UINT SDKVersion) {
    using Direct3DCreate8_t = IDirect3D8*(WINAPI*)(UINT);
    static Direct3DCreate8_t original = nullptr;

    if (!original) {
        char path[MAX_PATH]{};
        UINT len = GetSystemDirectoryA(path, MAX_PATH);
        if (len == 0 || len >= MAX_PATH) return nullptr;

        lstrcatA(path, "\\d3d8.dll");
        static HMODULE module = LoadLibraryA(path);
        if (!module) return nullptr;

        original = reinterpret_cast<Direct3DCreate8_t>
            (GetProcAddress(module, "Direct3DCreate8"));
        if (!original) return nullptr;
    }

    static bool console = false;
    if (!console) {
        AllocConsole();
        FILE* f;
        freopen_s(&f, "CONOUT$", "w", stdout);
        console = true;
    }
    printf("Hello World!\n");

    static bool payloaded = false;
    if (!payloaded) {
        LoadPayload();
        payloaded = true;
    }

    return original(SDKVersion);
}

BOOL APIENTRY DllMain(HMODULE, DWORD, LPVOID) {
    return TRUE;
}
