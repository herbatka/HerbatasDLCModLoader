#include <windows.h>
#include <shlobj.h>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <direct.h> // For _mkdir
#include "resource.h"

namespace fs = std::filesystem;

// Global declarations
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
std::string selectedDirectory;
const char* iniFilePath = "config.ini";

// Function to save the directory path to an .ini file
void SaveDirectoryToIni(const std::string& directory) {
    std::ofstream iniFile(iniFilePath);
    if (iniFile.is_open()) {
        iniFile << "[Settings]\n";
        iniFile << "Directory=" << directory << "\n";
        iniFile.close();
    }
}

// Function to load the directory path from an .ini file
bool LoadDirectoryFromIni(std::string& directory) {
    std::ifstream iniFile(iniFilePath);
    if (iniFile.is_open()) {
        std::string line;
        while (std::getline(iniFile, line)) {
            if (line.find("Directory=") != std::string::npos) {
                directory = line.substr(line.find('=') + 1);
                iniFile.close();
                return true;
            }
        }
        iniFile.close();
    }
    return false;
}

// Helper function to open folder selection dialog
std::string BrowseForFolder(HWND hwnd) {
    BROWSEINFO bi = {0};
    bi.lpszTitle = "Select Stalker 2 Installation Directory";
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

    if (pidl != nullptr) {
        char path[MAX_PATH];
        if (SHGetPathFromIDList(pidl, path)) {
            CoTaskMemFree(pidl);
            return std::string(path);
        }
        CoTaskMemFree(pidl);
    }
    return "";
}

// Function to delete a directory and its contents
void DeleteDirectory(const std::string& path) {
    if (fs::exists(path)) {
        fs::remove_all(path);
    }
}

// Function to delete HerbatasDLCModLoader.pak if it exists in the ~mods folder
void DeleteExistingPak(const std::string& modsPath) {
    std::string pakFile = modsPath + "\\HerbatasDLCModLoader.pak";
    if (fs::exists(pakFile)) {
        fs::remove(pakFile);
    }
}

void ListDLCFolders(const std::string& basePath, std::vector<std::string>& folderNames) {
    std::string dlcPath = basePath + "\\Stalker2\\Content\\GameLite\\DLCGameData";
    if (!fs::exists(dlcPath)) {
        MessageBox(NULL, "DLC GameData path does not exist!", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    for (const auto& entry : fs::directory_iterator(dlcPath)) {
        if (entry.is_directory()) {
            folderNames.push_back(entry.path().filename().string());
        }
    }
}

void CreateDirectoryStructureAndIni(const std::string& baseDir, const std::vector<std::string>& folderNames) {
    std::string appDataPath = std::getenv("APPDATA");
    std::string herbataPath = appDataPath + "\\HerbatasDLCLoader";

    // Check and delete existing HerbatasDLCLoader directory
    DeleteDirectory(herbataPath);

    // Create HerbatasDLCLoader folder
    _mkdir(herbataPath.c_str());

    // Copy repak.exe
    std::string repakSrc = fs::current_path().string() + "\\bin\\repak.exe";
    std::string repakDst = herbataPath + "\\repak.exe";
    fs::copy(repakSrc, repakDst, fs::copy_options::overwrite_existing);

    // Create folder structure
    std::string modLoaderPath = herbataPath + "\\HerbatasDLCModLoader\\Stalker2\\Plugins\\PlatformProviderController\\Config\\Steam";
    fs::create_directories(modLoaderPath);

    // Create SteamPlatformProviderEngine.ini
    std::ofstream iniFile(modLoaderPath + "\\SteamPlatformProviderEngine.ini");
    iniFile << R"([OnlineSubsystem]
DefaultPlatformService=Steam

[OnlineSubsystemSteam]
bEnabled=true
SteamDevAppId=1643320
Achievement_0_Id=Sandwich
Achievement_1_Id=PerfectBarter
Achievement_2_Id=PurchaseUpgrade
Achievement_3_Id=HeadshotStreak
Achievement_4_Id=RoyalFlush
Achievement_5_Id=MutantHunter
Achievement_6_Id=BreakEquipment
Achievement_7_Id=Gunsmith
Achievement_8_Id=ArtiHoarder
Achievement_9_Id=ArchiHoarder
Achievement_10_Id=BlueHoarder
Achievement_11_Id=Discovery
Achievement_12_Id=LonerShooter
Achievement_13_Id=UseDifferentWeapons
Achievement_14_Id=DrunkMaster
Achievement_15_Id=CatchingUp
Achievement_16_Id=MerryGoRound
Achievement_17_Id=WipedOut
Achievement_18_Id=CanOpener
Achievement_19_Id=Bouncy
Achievement_20_Id=ChimeraRun
Achievement_21_Id=SneakyClearLair
Achievement_22_Id=FinishSquad
Achievement_23_Id=Lockpick
Achievement_24_Id=HelloZone
Achievement_25_Id=GoodbyeLesserZone
Achievement_26_Id=MeetShram
Achievement_27_Id=MainPrize
Achievement_28_Id=CrystalLock
Achievement_29_Id=KillFaust
Achievement_30_Id=OSoznanie
Achievement_31_Id=DoctorMystery
Achievement_32_Id=MeetStrelok
Achievement_33_Id=MeetDegtyarev
Achievement_34_Id=EndingKorshunov
Achievement_35_Id=EndingShram
Achievement_36_Id=EndingStrelok
Achievement_37_Id=EndingKaymanov
Achievement_38_Id=SaveChernozem
Achievement_39_Id=TheSmartest
Achievement_40_Id=SaveSkadovsk
Achievement_41_Id=ShootApples
Achievement_42_Id=SaveZalesie
Achievement_43_Id=Povodok
Achievement_44_Id=VerySpecialWeapon
Achievement_45_Id=SitNearBonfire
Achievement_46_Id=KillStrelok
Achievement_47_Id=KillKorshunov
Achievement_48_Id=KillShram
Achievement_49_Id=AcceptFaustHelp
Achievement_50_Id=DeclineFaustHelp
Achievement_51_Id=BetweenTheLines
Achievement_52_Id=FindAllScanners



[SteamDLCs]
PreOrder=1661623
Deluxe=1661620
Ultimate=1661621
)"; // Add fixed content for achievements here

    // Add dynamic folder lines
    for (size_t i = 0; i < folderNames.size(); ++i) {
        iniFile << folderNames[i] << "=" << 480 << "\n";
    }

    iniFile.close();
}

void RunRepakAndCopyPak(const std::string& selectedDir) {
    std::string appDataPath = std::getenv("APPDATA");
    std::string herbataPath = appDataPath + "\\HerbatasDLCLoader";
    std::string modLoaderPath = herbataPath + "\\HerbatasDLCModLoader";

    // Run repak.exe
    std::string repakCommand = herbataPath + "\\repak.exe pack --version V11 " + modLoaderPath;
    system(repakCommand.c_str());

    // Copy the resulting .pak file
    std::string pakFile = herbataPath + "\\HerbatasDLCModLoader.pak";
    std::string modsPath = selectedDir + "\\Stalker2\\Content\\Paks\\~mods";

    // Check and delete existing pak file in ~mods
    DeleteExistingPak(modsPath);

    fs::create_directories(modsPath);
    fs::copy(pakFile, modsPath + "\\HerbatasDLCModLoader.pak", fs::copy_options::overwrite_existing);
}

// Main entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const char CLASS_NAME[] = "Stalker2ModLoader";

    WNDCLASS wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = CreateSolidBrush(RGB(192, 192, 192)); // Set grey background
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0, CLASS_NAME, "Herbata's Mod-as-DLC Loader", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 255, 200, // Adjusted window size
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

// Window Procedure
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HWND label, button1, button2, link;
    static HICON hIcon = NULL;
    static bool directoryLoaded = false;

    switch (uMsg) {
        case WM_CREATE:
            //Load the custom icon
            hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON));
            if (hIcon) {
                // Set the large icon (used in the title bar)
                SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);

                // Set the small icon (used in the Alt+Tab switcher and taskbar)
                SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
            }
            // Create UI elements
            label = CreateWindow("STATIC", "Herbata's Mod-as-DLC Loader", WS_VISIBLE | WS_CHILD,
                                 20, 10, 200, 20, hwnd, NULL, (HINSTANCE) GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);

            button1 = CreateWindow("BUTTON", "Select Stalker 2 Directory", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                                    20, 40, 200, 30, hwnd, (HMENU) 1, (HINSTANCE) GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);

            button2 = CreateWindow("BUTTON", "Prepare DLC Mod Loader", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                                    20, 80, 200, 30, hwnd, (HMENU) 2, (HINSTANCE) GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);

            link = CreateWindow("BUTTON", "Visit GitHub Repository", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                                20, 120, 200, 30, hwnd, (HMENU) 3, (HINSTANCE) GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
            
            if (LoadDirectoryFromIni(selectedDirectory)) {
                MessageBox(hwnd, "Previous Stalker 2 installation path successfully loaded!", "Success", MB_OK | MB_ICONINFORMATION);
                SendMessage(button1, WM_SETTEXT, 0, (LPARAM)"Directory Loaded");
                SendMessage(button1, WM_CTLCOLOREDIT, (WPARAM)CreateSolidBrush(RGB(144, 238, 144)), 0); // Light green
                MessageBox(hwnd, ("Loaded Directory: " + selectedDirectory).c_str(), "Loaded Directory", MB_OK);
                directoryLoaded = true;
            }
            break;

        case WM_COMMAND:
            if (LOWORD(wParam) == 1) {
                selectedDirectory = BrowseForFolder(hwnd);
                if (!selectedDirectory.empty()) {
                    MessageBox(hwnd, ("Selected Directory: " + selectedDirectory).c_str(), "Directory Selected", MB_OK);
                    SaveDirectoryToIni(selectedDirectory);
                    if (!directoryLoaded) {
                        SendMessage(button1, WM_SETTEXT, 0, (LPARAM)"Directory Saved");
                        directoryLoaded = true;
                    }
                } else {
                    MessageBox(hwnd, "No directory selected.", "Warning", MB_OK);
                }
            } else if (LOWORD(wParam) == 2) {
                if (selectedDirectory.empty()) {
                    MessageBox(hwnd, "Please select a directory first.", "Error", MB_OK | MB_ICONERROR);
                } else {
                    std::vector<std::string> folderNames;
                    ListDLCFolders(selectedDirectory, folderNames);
                    CreateDirectoryStructureAndIni(selectedDirectory, folderNames);
                    RunRepakAndCopyPak(selectedDirectory);
                    MessageBox(hwnd, "DLC Mod Loader preparation complete!", "Success", MB_OK);
                }
            } else if (LOWORD(wParam) == 3) {
                ShellExecute(NULL, "open", "https://github.com/herbatka/HerbatasDLCModLoader", NULL, NULL, SW_SHOWNORMAL);
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}