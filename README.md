To checkout this repository use
```
git clone --recurse-submodules git@github.com:CMS-HTT/HiggsCPinTauDecays.git
```
or if you want to checkout via https use
```
git clone --recurse-submodules https://github.com/CMS-HTT/HiggsCPinTauDecays.git
```

Submodules are checkout out in a detached HEAD state. You can enter the directory of the submodule (e.g. TauRefit) and checkout the desired branch such as master.
```
git checkout master
```
To pull updates for the submodule one can simply run
```
git submodule update --remote
```
