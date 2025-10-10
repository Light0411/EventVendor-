@echo off
TITLE EventVendor+ Fast Setup Script

REM This script will create the C:\vcpkg folder, then run the required setup commands.

cd /d C:\

REM --- Step 1: Create the C:\vcpkg folder by downloading and unzipping the files ---
if exist "C:\vcpkg" (
    echo --- Folder C:\vcpkg already exists. Skipping download. ---
) else (
    echo --- Downloading necessary files from the internet... ---
    powershell -Command "(New-Object System.Net.WebClient).DownloadFile('https://github.com/microsoft/vcpkg/archive/master.zip', 'C:\vcpkg.zip')"
    
    echo --- Unzipping files... ---
    powershell -Command "Expand-Archive -Path 'C:\vcpkg.zip' -DestinationPath 'C:\'"
    
    echo --- Renaming folder... ---
    ren C:\vcpkg-master vcpkg
    
    echo --- Cleaning up... ---
    del C:\vcpkg.zip
)

REM --- Step 2: Run the commands inside the newly created C:\vcpkg folder ---
cd C:\vcpkg

echo.
echo --- Bootstrapping vcpkg. This may take a few minutes (first time only). ---
call bootstrap-vcpkg.bat

echo.
echo --- Installing the PostgreSQL library (Release only, binary caching enabled). ---
.\vcpkg.exe install libpqxx:x64-windows --only-release --binarycaching

echo.
echo --- Integrating with Visual Studio. ---
.\vcpkg.exe integrate install

echo.
echo =================================
echo  SUCCESS! The script is finished.
echo =================================
pause