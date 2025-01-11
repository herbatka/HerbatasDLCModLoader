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

                string tempFolderPath = Path.Combine(Path.GetTempPath(), "HerbatasDLCModLoader");
                string repakFolderPath = Path.Combine(Path.GetTempPath(), "HerbatasDLCModLoader", "HerbatasDLCModLoader");
                string modsFolderPath = Path.Combine(folderPath, "Stalker2", "Content", "Paks", "~mods");
                string configFolderPath = Path.Combine(tempFolderPath, "HerbatasDLCModLoader", "Stalker2", "Plugins", "PlatformProviderController", "Config", "GOG");
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
                    "DefaultPlatformService=GOG",
                    "",
                    "[OnlineSubsystemGOG]",
                    "bEnabled=true",
                    "ClientID=56678921250274561",
                    "ClientSecret=eec835b13ae4567392c02370bd5dfbbb474a7fe3132d5b8150fc883df8bd5da8",
                    "bRequireBackendAuthorization=false",
                    "+Achievements=DoHeadshots",
                    "+Achievements=DoStealthKills",
                    "+Achievements=Sandwich",
                    "+Achievements=PerfectBarter",
                    "+Achievements=PurchaseUpgrade",
                    "+Achievements=HeadshotStreak",
                    "+Achievements=RoyalFlush",
                    "+Achievements=MutantHunter",
                    "+Achievements=BreakEquipment",
                    "+Achievements=Demoman",
                    "+Achievements=Gunsmith",
                    "+Achievements=ArtiHoarder",
                    "+Achievements=ArchiHoarder",
                    "+Achievements=BlueHoarder",
                    "+Achievements=Discovery",
                    "+Achievements=LonerShooter",
                    "+Achievements=UseDifferentWeapons",
                    "+Achievements=NoObstacleTooBig",
                    "+Achievements=DrunkMaster",
                    "+Achievements=CatchingUp",
                    "+Achievements=MerryGoRound",
                    "+Achievements=WipedOut",
                    "+Achievements=CanOpener",
                    "+Achievements=CouponsToBurn",
                    "+Achievements=Bouncy",
                    "+Achievements=ChimeraRun",
                    "+Achievements=SneakyClearLair",
                    "+Achievements=FinishSquad",
                    "+Achievements=Lockpick",
                    "+Achievements=HelloZone",
                    "+Achievements=GoodbyeLesserZone",
                    "+Achievements=MeetShram",
                    "+Achievements=MainPrize",
                    "+Achievements=CrystalLock",
                    "+Achievements=KillFaust",
                    "+Achievements=OSoznanie",
                    "+Achievements=DoctorMystery",
                    "+Achievements=MeetStrelok",
                    "+Achievements=MeetDegtyarev",
                    "+Achievements=EndingKorshunov",
                    "+Achievements=EndingShram",
                    "+Achievements=EndingStrelok",
                    "+Achievements=EndingKaymanov",
                    "+Achievements=SaveChernozem",
                    "+Achievements=TheSmartest",
                    "+Achievements=SaveSkadovsk",
                    "+Achievements=ShootApples",
                    "+Achievements=SaveZalesie",
                    "+Achievements=Povodok",
                    "+Achievements=VerySpecialWeapon",
                    "+Achievements=SitNearBonfire",
                    "+Achievements=KillStrelok",
                    "+Achievements=KillKorshunov",
                    "+Achievements=KillShram",
                    "+Achievements=AcceptFaustHelp",
                    "+Achievements=DeclineFaustHelp",
                    "+Achievements=BetweenTheLines",
                    "+Achievements=FindAllScanners",
                    "",
                    "[GOGDLCs]",
                    "PreOrder=1536886246",
                    "Deluxe=1891718289",
                    "Ultimate=1109022028"
                };

                var subfolders = Directory.GetDirectories(requiredPath)
                                         .Select(Path.GetFileName)
                                         .OrderBy(name => name)
                                         .Select(name => $"{name}=1891718289")
                                         .ToList();

                string filePath = Path.Combine(configFolderPath, "GOGPlatformProviderEngine.ini");
                File.WriteAllLines(filePath, headerContent.Concat(subfolders));

                // Run repak.exe with the specified arguments
                var process = new Process
                {
                    StartInfo = new ProcessStartInfo
                    {
                        FileName = exeDestinationPath,
                        Arguments = "pack --version V11 " + repakFolderPath,
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
