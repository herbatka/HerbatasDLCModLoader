using System;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using Microsoft.Win32;

namespace WpfApp
{
    public partial class MainWindow : Window
    {
        private string configFilePath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "config.json");

        public MainWindow()
        {
            InitializeComponent();
            LoadConfig();
        }

        private void BtnSelectFolder_Click(object sender, RoutedEventArgs e)
        {
            var dialog = new Microsoft.Win32.OpenFileDialog
            {
                CheckFileExists = false,
                CheckPathExists = true,
                FileName = "Select Folder",
                Filter = "Folders|*."
            };

            if (dialog.ShowDialog() == true)
            {
                string? folderPath = Path.GetDirectoryName(dialog.FileName);
                if (string.IsNullOrEmpty(folderPath))
                {
                    MessageBox.Show("Invalid folder path selected.", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                    return;
                }

                TxtSelectedFolder.Text = folderPath;
                SaveConfig(folderPath);
            }
        }

        private void BtnCreateFile_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                string folderPath = TxtSelectedFolder.Text;
                string requiredPath = Path.Combine(folderPath, "Stalker2", "Content", "GameLite", "DLCGameData");

                if (!Directory.Exists(requiredPath))
                {
                    MessageBox.Show("The required folder structure 'Stalker2\\Content\\GameLite\\DLCGameData' does not exist.", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                    return;
                }

                string tempFolderPath = Path.Combine(Path.GetFullPath(Path.GetTempPath()), "HerbatasDLCModLoader");
                string repakFolderPath = Path.Combine(Path.GetFullPath(Path.GetTempPath()), "HerbatasDLCModLoader", "HerbatasDLCModLoader");
                string modsFolderPath = Path.Combine(folderPath, "Stalker2", "Content", "Paks", "~mods");
                string configFolderPath = Path.Combine(tempFolderPath, "HerbatasDLCModLoader", "Stalker2", "Plugins", "PlatformProviderController", "Config", "Steam");
                if (!Directory.Exists(configFolderPath))
                {
                    Directory.CreateDirectory(configFolderPath);
                }

                string exeSourcePath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "bin", "repak.exe");
                string exeDestinationPath = Path.Combine(tempFolderPath, "repak.exe");
                if (File.Exists(exeSourcePath))
                {
                    File.Copy(exeSourcePath, exeDestinationPath, true);
                }
                else
                {
                    MessageBox.Show("repak.exe not found in the bin folder.", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                    return;
                }

                var headerContent = new[]
                {
                    "[OnlineSubsystem]",
                    "DefaultPlatformService=Steam",
                    "",
                    "[OnlineSubsystemSteam]",
                    "bEnabled=true",
                    "SteamDevAppId=1643320",
                    "Achievement_0_Id=Sandwich",
                    "Achievement_1_Id=PerfectBarter",
                    "Achievement_2_Id=PurchaseUpgrade",
                    "Achievement_3_Id=HeadshotStreak",
                    "Achievement_4_Id=RoyalFlush",
                    "Achievement_5_Id=MutantHunter",
                    "Achievement_6_Id=BreakEquipment",
                    "Achievement_7_Id=Gunsmith",
                    "Achievement_8_Id=ArtiHoarder",
                    "Achievement_9_Id=ArchiHoarder",
                    "Achievement_10_Id=BlueHoarder",
                    "Achievement_11_Id=Discovery",
                    "Achievement_12_Id=LonerShooter",
                    "Achievement_13_Id=UseDifferentWeapons",
                    "Achievement_14_Id=DrunkMaster",
                    "Achievement_15_Id=CatchingUp",
                    "Achievement_16_Id=MerryGoRound",
                    "Achievement_17_Id=WipedOut",
                    "Achievement_18_Id=CanOpener",
                    "Achievement_19_Id=Bouncy",
                    "Achievement_20_Id=ChimeraRun",
                    "Achievement_21_Id=SneakyClearLair",
                    "Achievement_22_Id=FinishSquad",
                    "Achievement_23_Id=Lockpick",
                    "Achievement_24_Id=HelloZone",
                    "Achievement_25_Id=GoodbyeLesserZone",
                    "Achievement_26_Id=MeetShram",
                    "Achievement_27_Id=MainPrize",
                    "Achievement_28_Id=CrystalLock",
                    "Achievement_29_Id=KillFaust",
                    "Achievement_30_Id=OSoznanie",
                    "Achievement_31_Id=DoctorMystery",
                    "Achievement_32_Id=MeetStrelok",
                    "Achievement_33_Id=MeetDegtyarev",
                    "Achievement_34_Id=EndingKorshunov",
                    "Achievement_35_Id=EndingShram",
                    "Achievement_36_Id=EndingStrelok",
                    "Achievement_37_Id=EndingKaymanov",
                    "Achievement_38_Id=SaveChernozem",
                    "Achievement_39_Id=TheSmartest",
                    "Achievement_40_Id=SaveSkadovsk",
                    "Achievement_41_Id=ShootApples",
                    "Achievement_42_Id=SaveZalesie",
                    "Achievement_43_Id=Povodok",
                    "Achievement_44_Id=VerySpecialWeapon",
                    "Achievement_45_Id=SitNearBonfire",
                    "Achievement_46_Id=KillStrelok",
                    "Achievement_47_Id=KillKorshunov",
                    "Achievement_48_Id=KillShram",
                    "Achievement_49_Id=AcceptFaustHelp",
                    "Achievement_50_Id=DeclineFaustHelp",
                    "Achievement_51_Id=BetweenTheLines",
                    "Achievement_52_Id=FindAllScanners",
                    "",
                    "[SteamDLCs]",
                    "PreOrder=1661623",
                    "Deluxe=1661620",
                    "Ultimate=1661621"
                };

                var subfolders = Directory.GetDirectories(requiredPath)
                                         .Select(Path.GetFileName)
                                         .OrderBy(name => name)
                                         .Select(name => $"{name}=480")
                                         .ToList();

                string filePath = Path.Combine(configFolderPath, "SteamPlatformProviderEngine.ini");
                File.WriteAllLines(filePath, headerContent.Concat(subfolders));

                // Run repak.exe with the specified arguments
                var process = new Process
                {
                    StartInfo = new ProcessStartInfo
                    {
                        FileName = exeDestinationPath,
                        Arguments = "pack --version V11 \"" + repakFolderPath + "\"",
                        UseShellExecute = false,
                        RedirectStandardOutput = true,
                        RedirectStandardError = true,
                        CreateNoWindow = true
                    }
                };

                process.Start();
                string output = process.StandardOutput.ReadToEnd();
                string error = process.StandardError.ReadToEnd();
                process.WaitForExit();

                if (!string.IsNullOrWhiteSpace(error))
                {
                    MessageBox.Show($"Error running repak.exe: {error}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                }
                else
                {
                    MessageBox.Show($"repak.exe finished successfully. Output:\n{output}", "Success", MessageBoxButton.OK, MessageBoxImage.Information);
                }

                // Copy HerbatasDLCModLoader.pak to mods folder
                string sourcePakPath = Path.Combine(tempFolderPath, "HerbatasDLCModLoader.pak");
                if (!Directory.Exists(modsFolderPath))
                {
                    Directory.CreateDirectory(modsFolderPath);
                }

                string destinationPakPath = Path.Combine(modsFolderPath, "HerbatasDLCModLoader.pak");
                if (File.Exists(sourcePakPath))
                {
                    File.Copy(sourcePakPath, destinationPakPath, true);
                    MessageBox.Show($"HerbatasDLCModLoader.pak copied to: {destinationPakPath}", "Success", MessageBoxButton.OK, MessageBoxImage.Information);
                }
                else
                {
                    MessageBox.Show("HerbatasDLCModLoader.pak not found in temp folder.", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error creating file: {ex.Message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private void SaveConfig(string directory)
        {
            if (string.IsNullOrEmpty(directory))
            {
                MessageBox.Show("Configuration directory is invalid.", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                return;
            }

            try
            {
                File.WriteAllText(configFilePath, directory);
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error saving config: {ex.Message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private void LoadConfig()
        {
            try
            {
                if (File.Exists(configFilePath))
                {
                    TxtSelectedFolder.Text = File.ReadAllText(configFilePath);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error loading config: {ex.Message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private void GitHubImage_Click(object sender, RoutedEventArgs e)
        {
            Process.Start(new ProcessStartInfo
            {
                FileName = "https://github.com/herbatka/HerbatasDLCModLoader",
                UseShellExecute = true
            });
        }

        private void NexusModsImage_Click(object sender, RoutedEventArgs e)
        {
            Process.Start(new ProcessStartInfo
            {
                FileName = "https://www.nexusmods.com/stalker2heartofchornobyl/mods/664",
                UseShellExecute = true
            });
        }
    }
}
