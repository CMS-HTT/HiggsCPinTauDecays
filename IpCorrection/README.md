### The usage of the code

```cpp
#include "HiggsCPinTauDecays/IpCorrection/interface/IpCorrection.h"
#include "HiggsCPinTauDecays/ImpactParameter/interface/ImpactParameter.h"
#include "Math.h"
#include "TVector3.h"
#include "TMatrix.h"

string cmsswBase = (getenv ("CMSSW_BASE"));

// Instantiate class IpCorrection 
// IpCorrection ipCorrector( TString(cmsswBase)+"src/HiggsCPinTauDecays/IpCorrection/data/ip_2016.root"); // 2016 MC
// IpCorrection ipCorrector( TString(cmsswBase)+"src/HiggsCPinTauDecays/IpCorrection/data/ip_2017.root"); // 2017 MC
// IpCorrection ipCorrector( TString(cmsswBase)+"src/HiggsCPinTauDecays/IpCorrection/data/ip_2018.root"); // 2018 MC
// IpCorrection ipCorrector( TString(cmsswBase)+"src/HiggsCPinTauDecays/IpCorrection/data/ip_embed_2016.root"); // 2016 embedded
// IpCorrection ipCorrector( TString(cmsswBase)+"src/HiggsCPinTauDecays/IpCorrection/data/ip_embed_2017.root"); // 2017 embedded
IpCorrection ipCorrector( TString(cmsswBase)+"src/HiggsCPinTauDecays/IpCorrection/data/ip_embed_2018.root"); // 2018 embedded

// instantiate class ImpactParameter
ImpactParameter IP;

...
// Correct impact parameter of the track 
TVector3 ip_corrected = ipCorrector.correctIp(
                 ip, // uncorrected IP vector (TVector3)
                 ipgen, // generator level IP vector (TVector3)
                 eta // pseudorapidity of the track (double)
);

// Correct IP covariance matrix
CovMatrix cov_corrected = cov_corrected = ipCorrector.correctIpCov(
            cov, // uncorrected IP covariance matrix (CovMatrix)
            eta // pseudorapidity of the track (double)
);

// compute corrected IP significance
double ipsig_corrected = IP.CalculateIPSignificanceHelical(ip_corrected,cov_corrected);

```
