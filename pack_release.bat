rd /S /Q release
mkdir release
mkdir release\profile
copy %windir%\SYSWOW64\msvcr120.dll release\msvcr120.dll
copy %windir%\SYSWOW64\msvcp120.dll release\msvcp120.dll
copy ic_tg_t17m.txt release\profile\ic_tg_t17m.txt
copy irecore.exe release\irecore.exe
copy kernel.c release\kernel.c