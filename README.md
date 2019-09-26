The Repository can be checked out via
```
git clone git@github.com:CMS-HTT/HiggsCPinTauDecays.git
```
or if you want to checkout via https use
```
git clone https://github.com/CMS-HTT/HiggsCPinTauDecays.git
```
However, you might want to only checkout certain subdirectories because you may have a different setup for skimming or for doing the analysis with the ntuple. One can perform a sparse checkout to only include the directories that are needed like this:
```
git clone git@github.com:CMS-HTT/HiggsCPinTauDecays.git
cd HiggsCPinTauDecays
echo subdirectory/ >> .git/info/sparse-checkout
git config core.sparsecheckout true
git read-tree -mu HEAD
cd ..
```
By exchanging the placeholder ```subdirectory``` with e.g. ```TauRefit``` you can check out only the desired subdirectories which is convenient for saving space.
