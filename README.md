Chromium-WebCL
==============

WebCL implementation for Chromium

Build Instruction:
----------------------

- Install Visual Studio 2010
- Install Chromium depot_tools and obtain Chromium source code, by following the steps at http://dev.chromium.org/developers/how-tos/get-the-code. IMPORTANT: 
Do this as a non-committer. Skip committer-specific steps.
Follow the instruction up to and including the “Actual Checkout” step, igore the rest
It will take a while, make sure your computer does not sleep. Sleep interrupts the checkout and cannot be resumed.
- Switch to branch_1700, i.e. current Chromium Beta, by “sh -x go_branch_1700.sh” in the src directory
- Delete all the .git directories by running, under the src directory, “find . -name .git | xargs rm -r”.
- Check out files from the AMD repository to add to or overwrite those from the Google repository. Execute the following commands in src’s parent directory:
	-- $ git init
	-- $ git remote add origin https://github.com/amd/Chromium-WebCL.git
	-- $ git fetch
	-- $ git reset --hard origin/master
- Go to src/chrome/build directory, load chrome.sln, and build

