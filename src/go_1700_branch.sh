# Make sure you are in 'src'.
# This part should only need to be done once, but it won't hurt to repeat it.  The first
# time might take a while because it fetches an extra 1/2 GB or so of branch commits. 
gclient sync --with_branch_heads
git fetch

BRANCH=1700  ## Put your branch number here.
git config --replace-all svn-remote.svn_$BRANCH.url svn://svn.chromium.org/chrome
git config --replace-all svn-remote.svn_$BRANCH.fetch branches/$BRANCH/src:refs/remotes/origin/$BRANCH

# This "initializes" the $BRANCH refspec, to prevent 'git svn fetch' from scanning
# the whole svn repo history for the branch commits we already synced above.
echo "ref: refs/remotes/branch-heads/$BRANCH" > .git/refs/remotes/origin/$BRANCH

# This should finish quickly, just doing a bit of git-svn housekeeping.
# It might fetch a few recent revisions, if the git mirror is slightly out-of-date,
# but it shouldn't be behind by much, and most of the time it shouldn't need to
# fetch anything. If it's taking a long time or looks like it's fetching a bunch of
# stuff from svn, something is probably wrong.
git svn fetch svn_$BRANCH

# Checkout the branch 'src' tree.
git checkout -b branch_$BRANCH origin/$BRANCH
# Make git-cl happy.
git config branch.branch_$BRANCH.merge refs/heads/$BRANCH
git config branch.branch_$BRANCH.remote branch-heads
# Checkout all the submodules at their branch DEPS revisions.
gclient sync --jobs 16
