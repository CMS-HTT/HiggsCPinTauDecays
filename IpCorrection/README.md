The usage of the code

#include "HiggsCPinTauDecays/IpCorrection/interface/IpCorrection.h"

// pass to the constructor the name of the RooT file with IP resolution lineshapes derived from Z->mumu events

string cmsswBase = (getenv ("CMSSW_BASE"));
```
// IpCorrection ipCorrector( TString(cmsswBase)+"src/HiggsCPinTauDecays/IpCorrection/data/ip_2016.root"); // 2016 data
```
// IpCorrection ipCorrector( TString(cmsswBase)+"src/HiggsCPinTauDecays/IpCorrection/data/ip_2017.root"); // 2017 data
```
IpCorrection ipCorrector( TString(cmsswBase)+"src/HiggsCPinTauDecays/IpCorrection/data/ip_2018.root"); // 2018 data

// correct ip within event loop

double ipx_corr = ipCorrector.correctIp(
```
IpCorrection::Coordinate::Ipx, // (int) coordinate index
```
ipx, // (double) (un)corrected ip (x coordinate)
```
ipx_gen, // (double) generator level ip (x coordinate)
```
eta // (double) particle pseudorapidity
```
);

double ipy_corr = ipCorrector.correctIp(
```
IpCorrection::Coordinate::Ipy, // (int) coordinate index
```
ipy, // (double) (un)corrected ip (y coordinate)
```
ipy_gen, // (double) generator level ip (y coordinate)
```
eta // (double) particle pseudorapidity
```
);

double ipz_corr = ipCorrector.correctIp(
```
IpCorrection::Coordinate::Ipz, // (int) coordinate index
```
ipz, // (double) (un)corrected ip (z coordinate)
```
ipz_gen, // (double) generator level ip (z coordinate)
```
eta // (double) particle pseudorapidity
```
);
