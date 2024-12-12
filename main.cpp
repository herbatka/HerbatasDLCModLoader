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
    std::string modLoaderPathGOG = herbataPath + "\\HerbatasDLCModLoader\\Stalker2\\Plugins\\PlatformProviderController\\Config\\GOG";
    std::string modLoaderPathEpic = herbataPath + "\\HerbatasDLCModLoader\\Stalker2\\Plugins\\PlatformProviderController\\Config\\EGS";
    fs::create_directories(modLoaderPath);
    fs::create_directories(modLoaderPathGOG);
    fs::create_directories(modLoaderPathEpic);

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

    // Create GOGPlatformProviderEngine.ini
    std::ofstream iniFileGOG(modLoaderPathGOG + "\\GOGPlatformProviderEngine.ini");
    iniFileGOG << R"([OnlineSubsystem]
DefaultPlatformService=GOG

[OnlineSubsystemGOG]
bEnabled=true
ClientID=56678921250274561
ClientSecret=eec835b13ae4567392c02370bd5dfbbb474a7fe3132d5b8150fc883df8bd5da8
bRequireBackendAuthorization=false
+Achievements=DoHeadshots
+Achievements=DoStealthKills
+Achievements=Sandwich
+Achievements=PerfectBarter
+Achievements=PurchaseUpgrade
+Achievements=HeadshotStreak
+Achievements=RoyalFlush
+Achievements=MutantHunter
+Achievements=BreakEquipment
+Achievements=Demoman
+Achievements=Gunsmith
+Achievements=ArtiHoarder
+Achievements=ArchiHoarder
+Achievements=BlueHoarder
+Achievements=Discovery
+Achievements=LonerShooter
+Achievements=UseDifferentWeapons
+Achievements=NoObstacleTooBig
+Achievements=DrunkMaster
+Achievements=CatchingUp
+Achievements=MerryGoRound
+Achievements=WipedOut
+Achievements=CanOpener
+Achievements=CouponsToBurn
+Achievements=Bouncy
+Achievements=ChimeraRun
+Achievements=SneakyClearLair
+Achievements=FinishSquad
+Achievements=Lockpick
+Achievements=HelloZone
+Achievements=GoodbyeLesserZone
+Achievements=MeetShram
+Achievements=MainPrize
+Achievements=CrystalLock
+Achievements=KillFaust
+Achievements=OSoznanie
+Achievements=DoctorMystery
+Achievements=MeetStrelok
+Achievements=MeetDegtyarev
+Achievements=EndingKorshunov
+Achievements=EndingShram
+Achievements=EndingStrelok
+Achievements=EndingKaymanov
+Achievements=SaveChernozem
+Achievements=TheSmartest
+Achievements=SaveSkadovsk
+Achievements=ShootApples
+Achievements=SaveZalesie
+Achievements=Povodok
+Achievements=VerySpecialWeapon
+Achievements=SitNearBonfire
+Achievements=KillStrelok
+Achievements=KillKorshunov
+Achievements=KillShram
+Achievements=AcceptFaustHelp
+Achievements=DeclineFaustHelp
+Achievements=BetweenTheLines
+Achievements=FindAllScanners

[GOGDLCs]
PreOrder=1536886246
Deluxe=1891718289
Ultimate=1109022028
)"; // Add fixed content for achievements here

    // Add dynamic folder lines
    for (size_t x = 0; x < folderNames.size(); ++x) {
        iniFileGOG << folderNames[x] << "=" << 1529799785 << "\n";
    }

    iniFileGOG.close();

    // Create EGSPlatformProviderEngine.ini
    std::ofstream iniFileEpic(modLoaderPathEpic + "\\EGSPlatformProviderEngine.ini");
    iniFileEpic << R"([OnlineSubsystem]
DefaultPlatformService=EOS

[OnlineSubsystemEOS]
bEnabled=true

[/Script/OnlineSubsystemEOS.EOSSettings]
CacheDir=CacheDir
DefaultArtifactName=Stalker2Live
TickBudgetInMilliseconds=0
bEnableOverlay=True
bEnableSocialOverlay=True
bEnableEditorOverlay=True
bShouldEnforceBeingLaunchedByEGS=False
TitleStorageReadChunkLength=0
+Artifacts=(ArtifactName="Stalker2Live",ClientId="xyza7891Os1dzhb5gxuO9g7Y65Njy0I9",ClientSecret="2x/ovfSOVVnVIxIIUTFn9gH2nqDIgku5y5jhg1NxHRQ",ProductId="b96b390642dd4ae79c0378ec4fd52a3d",SandboxId="602a0ef0aceb46cca62445439661d712",DeploymentId="04bc20720282478899e40b126fc0c4c4",EncryptionKey="1234567890123456789012345678901234567890123456789012345678901234")
+Artifacts=(ArtifactName="fc2498779fc6461bbbc0eb25ef46d0ac",ClientId="xyza7891Os1dzhb5gxuO9g7Y65Njy0I9",ClientSecret="2x/ovfSOVVnVIxIIUTFn9gH2nqDIgku5y5jhg1NxHRQ",ProductId="b96b390642dd4ae79c0378ec4fd52a3d",SandboxId="p-n8hbcw6yadnpkhug6lk24s6gy7b8c9",DeploymentId="4185dc94702d43aab88a8fdb0c471358",EncryptionKey="1234567890123456789012345678901234567890123456789012345678901234")
+Artifacts=(ArtifactName="84921bc96ad64f7d81a2a1a9d550ee05",ClientId="xyza7891Os1dzhb5gxuO9g7Y65Njy0I9",ClientSecret="2x/ovfSOVVnVIxIIUTFn9gH2nqDIgku5y5jhg1NxHRQ",ProductId="b96b390642dd4ae79c0378ec4fd52a3d",SandboxId="p-n8hbcw6yadnpkhug6lk24s6gy7b8c9",DeploymentId="4185dc94702d43aab88a8fdb0c471358",EncryptionKey="1234567890123456789012345678901234567890123456789012345678901234")
+Artifacts=(ArtifactName="Stalker2Dev",ClientId="xyza7891Os1dzhb5gxuO9g7Y65Njy0I9",ClientSecret="2x/ovfSOVVnVIxIIUTFn9gH2nqDIgku5y5jhg1NxHRQ",ProductId="b96b390642dd4ae79c0378ec4fd52a3d",SandboxId="p-n8hbcw6yadnpkhug6lk24s6gy7b8c9",DeploymentId="4185dc94702d43aab88a8fdb0c471358",EncryptionKey="1234567890123456789012345678901234567890123456789012345678901234")
+Artifacts=(ArtifactName="e7dd7df6a0f8421baa650c44e42ae722",ClientId="xyza7891Os1dzhb5gxuO9g7Y65Njy0I9",ClientSecret="2x/ovfSOVVnVIxIIUTFn9gH2nqDIgku5y5jhg1NxHRQ",ProductId="b96b390642dd4ae79c0378ec4fd52a3d",SandboxId="p-qbsgba95nkf56l8wdg8qsqtfrcpfh5",DeploymentId="a262dea3aabc44cfaa5cc7a2b38147a1",EncryptionKey="1234567890123456789012345678901234567890123456789012345678901234")
+Artifacts=(ArtifactName="Stalker2Stage",ClientId="xyza7891Os1dzhb5gxuO9g7Y65Njy0I9",ClientSecret="2x/ovfSOVVnVIxIIUTFn9gH2nqDIgku5y5jhg1NxHRQ",ProductId="b96b390642dd4ae79c0378ec4fd52a3d",SandboxId="p-qbsgba95nkf56l8wdg8qsqtfrcpfh5",DeploymentId="a262dea3aabc44cfaa5cc7a2b38147a1",EncryptionKey="1234567890123456789012345678901234567890123456789012345678901234")
bUseEAS=True
bUseEOSConnect=False
bMirrorStatsToEOS=False
bMirrorAchievementsToEOS=False
bUseEOSSessions=False
bMirrorPresenceToEAS=False

[PreOrder_EOSDLCs]
PreOrder=aaff224c9d47468b8a747573703748a6
Ultimate=0fcd6577bf5848d8983526a4e8d584e5
Deluxe=9aded54a65ba4552b1a21306c5f6faad
)"; // Add fixed content for achievements here

    // Add dynamic folder lines
    for (size_t z = 0; z < folderNames.size(); ++z) {
        iniFileEpic << folderNames[z] << "=" << "3854f76514eb4aadb8d1426563c7b279" << "\n";
    }

    iniFileEpic.close();
    std::ofstream iniFileEpic2(modLoaderPathEpic + "\\EGSPlatformProviderEngine.ini", std::ios::app);
    iniFileEpic2 << R"(
[Launch_EOSDLCs]
Ultimate=4f73634217a54f8d9ecdff2b5ffe9144
Deluxe=dbb6d9098e634e0da58ef2e6700f2bbd
)";
    for (size_t y = 0; y < folderNames.size(); ++y) {
        iniFileEpic2 << folderNames[y] << "=" << "3854f76514eb4aadb8d1426563c7b279" << "\n";
    }
    iniFileEpic2.close();
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