### The usage of the code

The recommended way of getting the ip, its covariance and the ip significance using the helical approach is to use the following functions.
```cpp
#include "HiggsCPinTauDecays/ImpactParameter/interface/ImpactParameter.h"

typedef ROOT::Math::SMatrix<double, 3, 3, ROOT::Math::MatRepSym<double, 3> > SMatrixSym3D;
typedef ROOT::Math::PositionVector3D<ROOT::Math::Cartesian3D<float> > RMPoint;

ImpactParameter ip;

std::pair <TVector3, ROOT::Math::SMatrix<float,3,3, ROOT::Math::MatRepStd< float, 3, 3 >>> ipAndCov;
ipAndCov = ip.CalculateIPandCovariance(
	magneticField, // (double)
	helixParameters, // (std::vector<float>)
	referencePoint, // (RMPoint)
	primaryVertex, // (RMPoint)
	helixParameterCovariance, // (ROOT::Math::SMatrix<float,5,5, ROOT::Math::MatRepSym<float,5>>)
	primaryVertexCovariance // (SMatrixSym3D)
);

TVector3 ip = ipAndCov.first;
ROOT::Math::SMatrix<float,3,3, ROOT::Math::MatRepStd< float, 3, 3 >> ipCovariance = ipAndCov.second;

double ipSignificance = ip.CalculateIPSignificanceHelical(ip, ipCovariance);
```

Alternatively you can call the function CalculatePCA and CalculatePCACovariance by themselves.
However it is important to call CalculatePCACovariance right after calling CalculatePCA, since the time value which minimises the distance between the position on the track and the primary vertex is stored as a member of the class and needed to calculate the Covariance.

```cpp
#include "HiggsCPinTauDecays/ImpactParameter/interface/ImpactParameter.h"

ImpactParameter ip;

TVector3 ip =  ip.CalculatePCA(magneticField, helixParameters, referencePoint, primaryVertex)
ROOT::Math::SMatrix<float,3,3, ROOT::Math::MatRepStd< float, 3, 3 >> ipCovariance = ip.CalculatePCACovariance(helixParameterCovariance, primaryVertexCovariance)

double ipSignificance = ip.CalculateIPSignificanceHelical(ip, ipCovariance);
```
