:: Update the qt_dir as necessary
set qt_dir=E:\Qt\5.4\msvc2013_opengl

rd /S /Q release
mkdir release
mkdir release\profile
mkdir release\platforms
:: copy %windir%\SYSWOW64\msvcr120.dll release\msvcr120.dll
:: copy %windir%\SYSWOW64\msvcp120.dll release\msvcp120.dll
copy ic_tg_t17m.txt release\profile\ic_tg_t17m.txt
copy irecore.exe release\irecore.exe
copy kernel.c release\kernel.c
copy gic_zh.qm release\gic_zh.qm
copy LICENSE release\LICENSE
copy %qt_dir%\bin\Qt5Core.dll release\Qt5Core.dll
copy %qt_dir%\bin\Qt5GUI.dll release\Qt5GUI.dll
copy %qt_dir%\bin\Qt5Widgets.dll release\Qt5Widgets.dll
copy %qt_dir%\bin\Qt5Network.dll release\Qt5Network.dll
copy %qt_dir%\plugins\platforms\qwindows.dll release\platforms\qwindows.dll
copy %qt_dir%\bin\icuin53.dll release\icuin53.dll
copy %qt_dir%\bin\icuuc53.dll release\icuuc53.dll
copy icudt53_min.dll release\icudt53.dll