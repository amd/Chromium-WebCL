REM Setup (1)

REM Switch to branch 1700 (i.e. chromium beta)

REM   This part should only need to be done once, but it won't hurt to repeat it.  The first
REM   time might take a while because it fetches an extra 1/2 GB or so of branch commits. 
call gclient sync --with_branch_heads
call git fetch

set BRANCH=1700
git config --replace-all svn-remote.svn_$BRANCH.url http://src.chromium.org/chrome
REM only for committers call git config --replace-all svn-remote.svn_%BRANCH%.url svn://svn.chromium.org/chrome
call git config --replace-all svn-remote.svn_%BRANCH%.fetch branches/%BRANCH%/src:refs/remotes/origin/%BRANCH%
REM NEW for 2014: You also need to convince git-svn that the commits on http://src.chromium.org/chrome are the same as those on svn://svn.chromium.org/chrome
git config --replace-all svn-remote.svn_$BRANCH.rewriteRoot "svn://svn.chromium.org/chrome"
git config --replace-all svn-remote.svn_$BRANCH.rewriteUUID "0039d316-1c4b-4281-b951-d872f2087c98"

REM   This "initializes" the %BRANCH% refspec, to prevent 'git svn fetch' from scanning
REM   the whole svn repo history for the branch commits we already synced above.
echo ref: refs/remotes/branch-heads/%BRANCH% > .git/refs/remotes/origin/%BRANCH%

REM   This should finish quickly, just doing a bit of git-svn housekeeping.
REM   It might fetch a few recent revisions, if the git mirror is slightly out-of-date,
REM   but it shouldn't be behind by much, and most of the time it shouldn't need to
REM   fetch anything. If it's taking a long time or looks like it's fetching a bunch of
REM   stuff from svn, something is probably wrong.
call git svn fetch svn_%BRANCH%

REM   Checkout the branch 'src' tree.
call git checkout -b branch_%BRANCH% origin/%BRANCH%
REM   Make git-cl happy.
call git config branch.branch_%BRANCH%.merge refs/heads/%BRANCH%
call git config branch.branch_%BRANCH%.remote branch-heads
REM   Checkout all the submodules at their branch DEPS revisions.
call gclient sync
call gclient runhooks --force

REM Setup (2)
REM remove all .git and .gitmodules directories that came with the chromium clone

FOR /F "tokens=*" %%G IN ('DIR /B /AD /S ".git"') DO RMDIR /Q /S "%%G"

REM Step (3) init AMD repository and pull

cd ..

call git init
call git remote rm origin 2> nul
call git remote add origin https://github.com/amd/Chromium-WebCL.git
call git fetch
call git reset --hard origin/master
call git branch --set-upstream-to=origin/master master

cd src
