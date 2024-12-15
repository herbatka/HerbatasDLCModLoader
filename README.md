# Herbata's Mod-as-DLC Loader

Herbata's Mod-as-DLC Loader is a Windows application designed to streamline the process of preparing and managing mods as DLC for the game **S.T.A.L.K.E.R. 2: Heart of Chornobyl**.
This program automates the creation of a compatible mod loader, ensuring an easy setup for users to enjoy community-driven content.

---

## Features
- **INI Configuration**: Automatically generates a configuration file for DLC compatibility.
- **Mod Preparation**: Builds the required directory structure and processes the mod files.

---

## Requirements
- **Operating System**: Windows
- **Dependencies**: 
  - `repak.exe` (included in the `bin` directory)
  - `S.T.A.L.K.E.R. 2: Heart of Chornobyl (STEAM VERSION)` installed on your system.

---

## How to Use
1. Launch the program.
2. Select your **Stalker 2 installation directory** when prompted (C:\Program Files (x86)\Steam\steamapps\common\S.T.A.L.K.E.R. 2 Heart of Chornobyl).
3. Click **Prepare DLC Mod Loader** to start the process.
   - The program will:
     - List existing DLC folders.
     - Create necessary directory structures.
     - Generate a `.pak` file for mod loading.
4. Enjoy your custom mods as part of your Stalker 2 experience!

---

## How to make a mod to use this?
Follow this [guide.](https://docs.google.com/document/d/1dlH18CWkIuBjdzZJbeuH0pFcji3JNdv336uHq1KsDR0/edit?tab=t.0)

If you want to see it in action, head over to my other repo [here.](https://github.com/herbatka/ZubrCaliberChange/tree/dev)

---

## Technical Details
- The application:
  - Reads and saves the installation directory to a `config.ini` file.
  - Utilizes the Windows `IFileOpenDialog` API for directory selection.
  - Automatically manages existing mod files to avoid conflicts.
  - Creates the needed configuration file to configure loading the mods as DLC.

---

## Developer Notes
- The main application window is implemented using the Windows API.
- Directory operations are handled using the C++17 `std::filesystem` library.
- Ensure the `repak.exe` file is present in the `bin` folder for proper operation.

---

## Contributing
Contributions are welcome! Please visit our [GitHub Repository](https://github.com/herbatka/HerbatasDLCModLoader) to open issues, submit pull requests, or get involved in the development process.

---

## Support
For issues and troubleshooting, feel free to:
- Visit our [GitHub Repository](https://github.com/herbatka/HerbatasDLCModLoader).
- Submit a bug report or feature request.
- Send me a direct message on Discord: @herbata
