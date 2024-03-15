if "%1"=="" (
    exit /b
)

set MinGW_DIR="C:\Qt\Tools\mingw1310_64\bin"
set QT_DIR="C:\Qt\6.8.0\mingw_64"
set Path=%MinGW_DIR%;%Path%
set VERSION=%1
set SCRIPT_DIR=%~dp0
set BUILD_DIR="..\Raptor-Legend-Never-Die"
set DIST_DIR="%SCRIPT_DIR%\..\Dist"
set OUTPUT_NAME="Raptor-%VERSION%-Windows-x64.zip"
if exist %BUILD_DIR% (
    rmdir /s /q %BUILD_DIR%
)

cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=%QT_DIR% -G "MinGW Makefiles" -S .. -B %BUILD_DIR%
cmake --build %BUILD_DIR%
for %%i in ("%SCRIPT_DIR%\..\Dist") do set ABSOLUTE_PATH=%%~fi
if not exist %ABSOLUTE_PATH% (
    mkdir /p %ABSOLUTE_PATH%
)

for %%i in ("%SCRIPT_DIR%\..\Target\Release") do set ABSOLUTE_PATH=%%~fi
if exist "%ABSOLUTE_PATH%\Raptor.log" (
    del "%ABSOLUTE_PATH%\Raptor.log"
)

copy "%SCRIPT_DIR%\..\3rd-Party\Curl\bin\Curl.dll" %ABSOLUTE_PATH%
copy "%SCRIPT_DIR%\..\3rd-Party\OpenSSL\bin\Crypto.dll" %ABSOLUTE_PATH%
copy "%SCRIPT_DIR%\..\3rd-Party\OpenSSL\bin\SSL.dll" %ABSOLUTE_PATH%
copy "%SCRIPT_DIR%\..\3rd-Party\Sass\bin\Sass.dll" %ABSOLUTE_PATH%
copy "%SCRIPT_DIR%\..\3rd-Party\SECP256K1\bin\SECP256K1.dll" %ABSOLUTE_PATH%
copy "%SCRIPT_DIR%\..\3rd-Party\UV\bin\UV.dll" %ABSOLUTE_PATH%
copy "%SCRIPT_DIR%\..\3rd-Party\Yaml\bin\Yaml.dll" %ABSOLUTE_PATH%
copy "%SCRIPT_DIR%\..\3rd-Party\ZInt\bin\ZInt.dll" %ABSOLUTE_PATH%
set QT_DIR=%QT_DIR:~1,-1%
"%QT_DIR%\bin\WinDeployQt.exe" -no-translations "%ABSOLUTE_PATH%\Raptor.exe"
if exist %DIST_DIR%\%OUTPUT_NAME% (
    del %DIST_DIR%\%OUTPUT_NAME%
)

"%SCRIPT_DIR%\Tool\7za.exe" a -tzip "%DIST_DIR%\%OUTPUT_NAME%" "%ABSOLUTE_PATH%\*"