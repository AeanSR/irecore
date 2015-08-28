# IreCore
[Project Home](http://sim.aean.net)

IreCore is a World of Warcraft simulator for the purpose of boosting large-scale theorycrafting, for fury warriors. IreCore is aimed at making the speedup as high as possible. 

IreCore is derived from the [LuaCL project](https://github.com/llxibo/LuaCL) as an initial practice. IreCore Kernel is being developed in OpenCL C for heterogeneous computing.

# How to Build
IreCore is built with [the Qt Toolkit](http://qt.io), but still have some dependencies on Windows platform.

To build IreCore, you should first have all of the listed toolkits installed:

* Visual Studio 2013 Community

* [Qt 5.5](http://www.qt.io/download-open-source/), the component `msvc2013` `qtcreator` is required at minimum. If you want to have Chinese Simplified localization, `qtlinguist` is required.

* [Visual Studio Add-in 1.2.4 for Qt5](http://www.qt.io/download-open-source/#section-7)

* A OpenCL SDK from your device vendor, it could be one of the listed:

    * Not Recommended: [Intel INDE](https://software.intel.com/en-us/intel-inde)

    * Recommended: [nVidia CUDA Toolkit](https://developer.nvidia.com/cuda-downloads)

    * Recommended: [AMD APP SDK](http://developer.amd.com/tools-and-sdks/opencl-zone/amd-accelerated-parallel-processing-app-sdk/)

    * If you selected the Intel INDE, you may need manually configure lib & include directories in VS project.

Open the VS project `gic.vcxproj` with a text editor. Search for `<QTDIR>`. There are two lines beginning with `<QTDIR>`:

`<QTDIR>E:\Qt\5.5\msvc2013</QTDIR>`

Edit the directory to your Qt path. The Qt path should directly contain `bin` `lib` and `include` subfolders.

`<QTDIR>C:\Qt\Qt5.5\msvc2013</QTDIR>`

Open the batch file `pack_release.bat` with a text editor. Edit the directory in the second line to your Qt path.

`set qt_dir=E:\Qt\5.5\msvc2013`

`set qt_dir=C:\Qt\Qt5.5\msvc2013`

Open the VS solution `irecore.sln` and build the solution. A release package will be copied into a `release` folder.

If you have the Qt Linguist installed, you could first release the localization `gic_zh.ts` to `gic_zh.qm` for Chinese Simplified localization.