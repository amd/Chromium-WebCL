REM Setup (1)

REM Switch to branch 1700 (i.e. chromium beta)

REM   This part should only need to be done once, but it won't hurt to repeat it.  The first
REM   time might take a while because it fetches an extra 1/2 GB or so of branch commits. 
gclient sync --with_branch_heads
git fetch

set BRANCH=1700
git config --replace-all svn-remote.svn_%BRANCH%.url svn://svn.chromium.org/chrome
git config --replace-all svn-remote.svn_%BRANCH%.fetch branches/%BRANCH%/src:refs/remotes/origin/%BRANCH%

REM   This "initializes" the %BRANCH% refspec, to prevent 'git svn fetch' from scanning
REM   the whole svn repo history for the branch commits we already synced above.
REM "ref: refs/remotes/branch-heads/%BRANCH%" > .git/refs/remotes/origin/%BRANCH%

REM   This should finish quickly, just doing a bit of git-svn housekeeping.
REM   It might fetch a few recent revisions, if the git mirror is slightly out-of-date,
REM   but it shouldn't be behind by much, and most of the time it shouldn't need to
REM   fetch anything. If it's taking a long time or looks like it's fetching a bunch of
REM   stuff from svn, something is probably wrong.
git svn fetch svn_%BRANCH%

REM   Checkout the branch 'src' tree.
git checkout -b branch_%BRANCH% origin/%BRANCH%
REM   Make git-cl happy.
git config branch.branch_%BRANCH%.merge refs/heads/%BRANCH%
git config branch.branch_%BRANCH%.remote branch-heads
REM   Checkout all the submodules at their branch DEPS revisions.
gclient sync --jobs 16
gclient runhooks --force

REM Setup (2)

REM remove all .git and .gitmodules directories that came with the chromium clone
RMDIR /S /Q .\.git\*.* .\breakpad\src\.git\*.* .\chrome\test\data\extensions\api_test\permissions\nacl_enabled\bin\.git\*.* .\chrome\test\data\perf\canvas_bench\.git\*.* .\chrome\test\data\perf\frame_rate\content\.git\*.* .\chrome\test\data\perf\third_party\octane\.git\*.* .\chrome\tools\test\reference_build\chrome_win\.git\*.* .\chrome_frame\tools\test\reference_build\chrome_win\.git\*.* .\media\cdm\ppapi\api\.git\*.* .\native_client\.git\*.* .\sdch\open-vcdiff\.git\*.* .\testing\gmock\.git\*.* .\testing\gtest\.git\*.* .\third_party\angle\.git\*.* .\third_party\angle_dx11\.git\*.* .\third_party\bidichecker\.git\*.* .\third_party\bison\.git\*.* .\third_party\brotli\src\.git\*.* .\third_party\cacheinvalidation\src\.git\*.* .\third_party\cld_2\src\.git\*.* .\third_party\cygwin\.git\*.* .\third_party\ffmpeg\.git\*.* .\third_party\flac\.git\*.* .\third_party\gnu_binutils\.git\*.* .\third_party\gperf\.git\*.* .\third_party\html_office\.git\*.* .\third_party\hunspell\.git\*.* .\third_party\hunspell_dictionaries\.git\*.* .\third_party\icu\.git\*.* .\third_party\jsoncpp\source\include\.git\*.* .\third_party\jsoncpp\source\src\lib_json\.git\*.* .\third_party\leveldatabase\src\.git\*.* .\third_party\libaddressinput\src\.git\*.* .\third_party\libc++\trunk\.git\*.* .\third_party\libc++abi\trunk\.git\*.* .\third_party\libexif\sources\.git\*.* .\third_party\libjingle\source\talk\.git\*.* .\third_party\libjpeg_turbo\.git\*.* .\third_party\libphonenumber\src\phonenumbers\.git\*.* .\third_party\libphonenumber\src\resources\.git\*.* .\third_party\libphonenumber\src\test\.git\*.* .\third_party\libsrtp\.git\*.* .\third_party\libvpx\.git\*.* .\third_party\libyuv\.git\*.* .\third_party\lighttpd\.git\*.* .\third_party\mesa\src\.git\*.* .\third_party\mingw-w64\mingw\bin\.git\*.* .\third_party\nacl_sdk_binaries\.git\*.* .\third_party\nss\.git\*.* .\third_party\openmax_dl\.git\*.* .\third_party\opus\src\.git\*.* .\third_party\ots\.git\*.* .\third_party\pefile\.git\*.* .\third_party\perl\.git\*.* .\third_party\psyco_win32\.git\*.* .\third_party\pyftpdlib\src\.git\*.* .\third_party\python_26\.git\*.* .\third_party\pywebsocket\src\.git\*.* .\third_party\safe_browsing\testing\.git\*.* .\third_party\scons-2.0.1\.git\*.* .\third_party\sfntly\cpp\src\.git\*.* .\third_party\skia\gyp\.git\*.* .\third_party\skia\include\.git\*.* .\third_party\skia\src\.git\*.* .\third_party\smhasher\src\.git\*.* .\third_party\snappy\src\.git\*.* .\third_party\speex\.git\*.* .\third_party\swig\Lib\.git\*.* .\third_party\swig\win\.git\*.* .\third_party\syzygy\binaries\.git\*.* .\third_party\trace-viewer\.git\*.* .\third_party\usrsctp\usrsctplib\.git\*.* .\third_party\webdriver\pylib\.git\*.* .\third_party\webgl\src\.git\*.* .\third_party\webgl_conformance\.git\*.* .\third_party\WebKit\.git\*.* .\third_party\WebKit\LayoutTests\w3c\csswg-test\.git\*.* .\third_party\WebKit\LayoutTests\w3c\web-platform-tests\.git\*.* .\third_party\webpagereplay\.git\*.* .\third_party\webrtc\.git\*.* .\third_party\xulrunner-sdk\.git\*.* .\third_party\yasm\binaries\.git\*.* .\third_party\yasm\source\patched-yasm\.git\*.* .\tools\deps2git\.git\*.* .\tools\grit\.git\*.* .\tools\gyp\.git\*.* .\tools\page_cycler\acid3\.git\*.* .\tools\swarming_client\.git\*.* .\v8\.git\*.* .\src\.gitmodules\*.* .\src\third_party\trace-viewer\third_party\Promises\.gitmodules\*.* .\src\third_party\WebKit\LayoutTests\w3c\web-platform-tests\.gitmodules\*.*

REM Step (3) init AMD repository and pull
git init
git remote rm origin 2> nul
git remote add origin https://github.com/amd/Chromium-WebCL.git
git fetch
git reset --hard origin/master
