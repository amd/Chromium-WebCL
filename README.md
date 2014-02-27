Chromium-WebCL
==============

WebCL implementation for Chromium

Build Instruction:
----------------------

1)      Install windows 8.x SDK http://msdn.microsoft.com/en-us/windows/hardware/hh852363.aspx

2)      Install DirectX SDK june 2010 http://www.microsoft.com/en-us/download/details.aspx?id=6812
Including .net libraries
(restart)

2.5) Google "AMD APP SDK" and install it. To verify the installation, check that you have the AMDAPPSDKROOT environment variable created for you. The tested version of the SDK for this project is AMD-APP-SDK-v2.9-Windows-64.exe

3)      Install visual studio 2010, make sure x64 compiler and tools are installed. Install SP1. The tested environment looks like (Visual Studio -> Help -> About) Version 1.0.40219 SP1Rel.

4)      Install VS2010 SP1 https://www.microsoft.com/en-us/download/details.aspx?id=23691
(restart)

5)      Download depot_tools https://src.chromium.org/svn/trunk/tools/depot_tools.zip

6)      Install depot_tools.zip in c:\depot_tools

7)      Add c:\depots_tools in your path

8)      open cmd tool

9)      cd c:\depot_tools

10)   git config –global user.name “your name”

11)   git comfig –global user.email “your email”

12)   git config –global core.autocrlf false

13)   git config –global core.filemode false

14)   run ‘gclient’
[In case of gclient giving an error fetching python, get python_bin.zip as per the message says, copy all the content of the zip directly inside c:\depot_tools. Edit update_depot_tools.bat and remove the ‘force’ option (see comment in the file). git commit update_depot_tools.bat. Run gclient again]

15)   Go to  location where you want to store chromium code.
Best is to use a separate drive than the system drive. Turn off windows disk index for this folder and subfolder

16)   cd /the/location/for/chromium

17)   fetch chromium –nosvn=True 
This will create a ‘src’ folder at the location you run fetch
This will take a while, and require a good internet connection
Remove sleep timer on your pc, as If the pc goes to sleep it will interrupt the fetch, and you will have to start again from scratch.
[if fetch does not complete, try gclient sync. But it fails, the only solution seems to delete ‘src’ and do a fetch again
https://code.google.com/p/chromium/issues/detail?id=230691]
 
For more information consult http://dev.chromium.org/developers/how-tos/get-the-code

18) Get and run setup.bat. Get setup.bat from https://github.com/amd/Chromium-WebCL/raw/master/src/setup.bat, and save it in the *src* directory. Then, cd into *src* and run setup.bat. NOTE: setup.bat MUST BE EXECUTED IN THE *src* DIRECTORY! What this script does is the following: (a) switch to branch_1700, i.e. current Chromium Beta (b) Delete all the .git directories that came with your chromium clone (c) point at and sync with AMD repository (WebCL related changes to Chromium)

19) In Visual Studio, load chrome.sln from src/chrome. (a) Set "chrome" project as Startup Project by right-clicking on it and selecting "Set as Startup Project". (b) Now we add AMD APP SDK paths globally to Visual Studio. To do this, click on project "chrome" to select it. Then in the View menu, select Property Manager, and Property Manager will show up as a tab beside Solution Explorer, Class View, etc. In Property Manager, expand the "chrome" project, then expand "Debug|Win32". Right click on Microsoft.Cpp.Win32.user, and select "Properties". Then, in Common Properties, click on "VC++ Directories". Add "$(AMDAPPSDKROOT)\include" to "Include Directories"; add $(AMDAPPSDKROOT)\lib\x86 to "Library Directories". Then, expand "Release|Win32", and repeat.

19.5) Select Debug or Release, and start building the chrome project. This can take hours.

20) Go to src/build/Debug or src/build/Release, depending on your build, and execute chrome.exe

21) Run Superconductor demos. Point your web server at the *superconductor* directory, which is at the same level of *src*; use a port number of your choice, say 8888. Then, in the browser, load http://localhost:8888/examples/linegraph-3d or http://localhost:8888/examples/treemap to load the main Superconductor demos. To get a sense of how the demos should look, watch the screen-capture video at http://youtu.be/dGD9NpipcrE

