rem -- doWinDeployQt.bat
rem -- Qt requires a bunch of DLLs along with qtpovray.exe
rem -- It provides a tool to determine which DLLs are needed and bundle them.
rem -- This script is nominally called from stageQtPovray.sh
rem
call \qt\5.15.2\msvc2015_64\bin\qtenv2.bat
call "\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat"
cd \Users\dick\staged\qtpovray
\qt\5.15.2\msvc2015_64\bin\windeployqt.exe qtpovray.exe
