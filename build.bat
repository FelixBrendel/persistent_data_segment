@echo off
pushd %~dp0
mkdir bin 2>nul
mkdir bin\obj 2>nul

:: --------------
::  config start
:: --------------
set cleanup_after=true
set app_name=persistency
:: --------------
::   config end
:: --------------

set app_name_pre_patch=%app_name%_pre_patch.exe
set app_name_post_patch=%app_name%.exe

taskkill /im %app_name%.exe > NUL 2> NUL

pushd bin

del *.exe       > NUL 2> NUL
del *.pdb       > NUL 2> NUL
del /Q obj\*.*  > NUL 2> NUL

:: compiling the program
cl /nologo /Z7 /O1 /std:c++latest ^
   /Fo: obj\ /Fe: %app_name_pre_patch% ^
   ..\src\persistent_data\persistent_main.cpp ^
   ..\src\*.cpp /GS- /MP ^
   /D_CRT_SECURE_NO_WARNINGS ^
   /link /NODEFAULTLIB /subsystem:windows /INCREMENTAL:NO /entry:persistent_main ^
   -PDB:%app_name%.pdb user32.lib kernel32.lib || exit /B 1

:: compiling the patcher
cl /nologo /Z7 /MD /std:c++latest ^
   /Fo: obj\ /Fe: patcher.exe ^
   /D_CRT_SECURE_NO_WARNINGS ^
   /Dexe_name_pre_patch=\"%app_name_pre_patch%\" ^
   /Dexe_name_post_patch=\"%app_name_post_patch%\" ^
   ../src/persistent_data/patcher.cpp /link /INCREMENTAL:NO || exit 1

:: running the patcher
echo patching...
patcher || exit /b 1
echo.

if %cleanup_after% == true (
   echo cleaning up %app_name_pre_patch%
   del %app_name_pre_patch%
   echo cleaning up patcher.pdb
   del patcher.pdb
   echo cleaning up patcher.exe
   del patcher.exe
)

popd

popd
