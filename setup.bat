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
