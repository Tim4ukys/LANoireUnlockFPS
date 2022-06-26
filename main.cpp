#include "main.h"

Config g_Config{"LANoireUnlockFPS.json"};

HANDLE findAndOpenProcess(const wchar_t* name) {
    auto hTH = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    if (!hTH)
        RTHROW("Fail run CreateToolhelp32Snapshot.");

    PROCESSENTRY32 proc{sizeof(PROCESSENTRY32)};
    if (Process32First(hTH, &proc)) {
        do {
            if (wcscmp(proc.szExeFile, name) == 0) {
                CloseHandle(hTH);
                return OpenProcess(/*PROCESS_VM_WRITE | PROCESS_VM_READ*/PROCESS_ALL_ACCESS,
                                   FALSE,
                                   proc.th32ProcessID);
            }
        } while (Process32Next(hTH, &proc));
    }
    CloseHandle(hTH);
    return NULL;
}

int main(int argc, char* argv[]) {
    HANDLE hProc{};
    try {
        while (!(hProc = findAndOpenProcess(L"LANoire.exe"))) {
            static int s_iPoints{};
            if (s_iPoints++ >= 3)
                s_iPoints = 1;
            
            Sleep(500);

            std::cout << "\rWait for start process ";
            for (size_t i{}; i < 3; ++i) {
                if (i < s_iPoints)
                    std::cout << '.';
                else
                    std::cout << ' ';
            }
            std::cout << std::flush;
        }
        std::cout << "\rWait for start process ... Done\n";

        HMODULE hBaseProcAddr{};
        {
            DWORD pad{};
            while (!EnumProcessModules(hProc, &hBaseProcAddr, sizeof(hBaseProcAddr), &pad)) {
                if (GetLastError() != 0x12b) {
                    RTHROW("Fail run EnumProcessModules.");
                }
                Sleep(2000);
            }
        }

        DWORD offsToStruct = g_Config["offsets"]["toStruct"].get<DWORD>();
        DWORD offsToFPSParam = g_Config["offsets"]["toFPSParam"].get<DWORD>();

        auto getFPSLockParam = [&]() -> float {
            DWORD pointer;
            ReadProcessMemory(hProc,
                              PVOID(DWORD(hBaseProcAddr) + offsToStruct),
                              &pointer,
                              sizeof(DWORD),
                              nullptr);
            float fps;
            ReadProcessMemory(hProc,
                              PVOID(pointer + offsToFPSParam),
                              &fps,
                              sizeof(float),
                              nullptr);
            //printf("fps_lock: %.3f\n", fps);
            return fps;
        };

        /*
        while (true) {
            if (GetAsyncKeyState(VK_DELETE)) {
                std::cout << "\nDestroy." << std::endl;
                break;
            }

            if (GetAsyncKeyState(VK_F5)) {
                getFPSLockParam();

                Sleep(1000);
            }
        }*/

        while (getFPSLockParam() != 59.94f){ Sleep(100); }
        {
            DWORD pointer{};
            ReadProcessMemory(hProc,
                              PVOID(DWORD(hBaseProcAddr) + offsToStruct),
                              &pointer,
                              sizeof(DWORD),
                              nullptr);
            
            const float fps_lock = g_Config["fps_param"].get<float>();
            WriteProcessMemory(hProc,
                               PVOID(pointer + offsToFPSParam),
                               &fps_lock,
                               sizeof(float),
                               nullptr);
        }
        std::cout << "Patch success! Destroy." << std::endl;
        system("pause");

        CloseHandle(hProc);
    }
    catch (const std::exception& ec) {
        std::cout << "ERROR: <0x" << std::hex << GetLastError() << "> "
                  << ec.what() << std::endl;
        system("pause");
        return EXIT_FAILURE;
    };

    return EXIT_SUCCESS;
}