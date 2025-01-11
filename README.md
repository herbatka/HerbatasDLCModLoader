# Herbata's Mod as DLC Loader

A WPF-based tool to load mods as DLCs for **Stalker 2**. This tool simplifies the process of configuring and packaging mods by generating the required folder structure, configuration files, and repackaging the data into a `.pak` file.

## Features

- Generates the required folder structure for Stalker 2 mods.
- Creates and configures the `GOGPlatformProviderEngine.ini` file.
- Runs `repak.exe` to package mods as `.pak` files.
- Automatically copies the generated `.pak` file to the Stalker 2 `~mods` folder.
- User-friendly GUI for folder selection and mod loading.
- Clickable links to GitHub and NexusMods for additional resources.

## Requirements

- **.NET 9**
- `repak.exe` in the `bin` folder of the application.
- Stalker 2 installed with the required folder structure.

## Usage

1. Launch the application.
2. Click "Select Folder" and navigate to the Stalker 2 installation path.
3. Click "Load mods as DLCs" to generate the necessary files and package the mods.
4. Verify that `HerbatasDLCModLoader.pak` is created and placed in the `~mods` folder.

## File Structure

The tool generates the following folder structure in the system's temp directory:
  ```bash
  HerbatasDLCModLoader/
    Stalker2/
      Plugins/
        PlatformProviderController/
          Config/
            GOG/
              GOGPlatformProviderEngine.ini
 ```
## Configuration

The `config.json` file stores the last selected folder path and is located in the same directory as the executable. This ensures a smoother user experience by remembering the previously selected path.

## Building a Standalone Application

To build a self-contained application:

1. Clone the repository:
   ```bash
   git clone https://github.com/herbatka/HerbatasDLCModLoader.git
   ```
2. Open the project in **Visual Studio**.
3. Build the solution in **Release** mode.

## Troubleshooting

- **Missing repak.exe**: Ensure `repak.exe` is located in the `bin` folder.
- **Invalid folder structure**: Verify the selected folder contains `Stalker2/Content/GameLite/DLCGameData`.
- **Configuration errors**: Delete `config.json` and restart the app.

## Links

- [GitHub Repository](https://github.com/herbatka/HerbatasDLCModLoader)
- [NexusMods Page](https://www.nexusmods.com/stalker2heartofchornobyl/mods/664)

---

Enjoy modding with **Herbata's Mod as DLC Loader**!
