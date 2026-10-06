
extern "C" __declspec(dllexport) void Load() { printf("load\n"); }

extern "C" __declspec(dllexport) void Unoad() { printf("unload\n"); }

BOOL APIENTRY DllMain(DWORD, LPVOID) {
	
}

