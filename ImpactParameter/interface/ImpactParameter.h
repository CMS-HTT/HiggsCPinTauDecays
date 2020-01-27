
#pragma once

#include <TMath.h>
#include "TVector3.h"
#include "TMatrix.h"

#include <Math/PtEtaPhiM4D.h>
#include <Math/LorentzVector.h>
#include <Math/Point3D.h>
#include <Math/MatrixRepresentationsStatic.h>
#include <Math/SMatrix.h>

typedef ROOT::Math::PtEtaPhiM4D<float> RMFLV_Store;
typedef ROOT::Math::SMatrix<double, 3, 3, ROOT::Math::MatRepSym<double, 3> > SMatrixSym3D;

typedef ROOT::Math::LorentzVector<RMFLV_Store> RMFLV;
typedef ROOT::Math::PositionVector3D<ROOT::Math::Cartesian3D<float> > RMPoint;

/*
	A class to collect all approaches for calculating the Impact Parameter
	The tangential approach is far simpler to calculate but the uncertainty
	propagation is incomplete (cmssw does not give errors on the track or momentum)
	The helical appraoch is more complicated but allows for full error propagation,
	since all used variables depend on either the helix parameters or the
 	primary Vertex and the given covariance matrices allow for error propagation
*/

class ImpactParameter
{
public:
	double CalculatePCADifferece(SMatrixSym3D cov, TVector3 DeltaPCA);
	TVector3 CalculatePCA(double B, std::vector<float> h_param, RMPoint ref, RMPoint PrV);
	ROOT::Math::SMatrix<float,3,3, ROOT::Math::MatRepStd< float, 3, 3 >> CalculatePCACovariance(ROOT::Math::SMatrix<float,5,5, ROOT::Math::MatRepSym<float,5>> helixCov, SMatrixSym3D SigmaPrV);
	ROOT::Math::SMatrix<float,3,3, ROOT::Math::MatRepStd< float, 3, 3 >> CalculatePCACovarianceTrack(ROOT::Math::SMatrix<float,5,5, ROOT::Math::MatRepSym<float,5>> helixCov);
	ROOT::Math::SMatrix<float,3,3, ROOT::Math::MatRepStd< float, 3, 3 >> CalculatePCACovariancePV(SMatrixSym3D SigmaPrV);
	//TVector3 CalculateShortestDistance(KGenParticle* genParticle, RMPoint* pv);
	TVector3 CalculateShortestDistance(RMFLV p4, RMPoint vertex, RMPoint* pv);
	TVector3 CalculateShortestDistance(RMFLV p4, RMPoint ref, RMPoint pv);
	//std::vector<double> CalculateIPErrors(RMFLV p4, RMPoint ref, KVertex* pv, TVector3* ipvec);
	double CalculateIPSignificanceHelical(TVector3 IP, ROOT::Math::SMatrix<float,3,3, ROOT::Math::MatRepStd< float, 3, 3 >> IPCovariance);
	double CalculateIPSignificanceTangential(TVector3 IP, SMatrixSym3D PVCovariance);

	std::pair <TVector3, ROOT::Math::SMatrix<float,3,3, ROOT::Math::MatRepStd< float, 3, 3 >>> CalculateIPandCovariance(double B, std::vector<float> h_param, RMPoint ref, RMPoint PrV, ROOT::Math::SMatrix<float,5,5, ROOT::Math::MatRepSym<float,5>> helixCov, SMatrixSym3D SigmaPrV);

	// set functions for variables used in the helical approach
	inline double GetRecoDxy(){ return recoDxy; }
	inline double GetRecoDsz(){ return recoDsz; }
	inline double GetXBest(){ return xBest; }
	inline double GetPCAIsCalculated(){ return pcaIsCalculated; }
private:
	double xBest;
	double recoDxy;
	double recoDsz;
	bool pcaIsCalculated = false;

	// set functions for variables used in the helical approach
	inline void SetXBest(double XBest){ xBest = XBest; }
	inline void SetRecoDxy(double Dxy){ recoDxy = Dxy; }
	inline void SetRecoDsz(double Dsz){ recoDsz = Dsz; }
	inline void SetPCAIsCalculated(bool isCalculated){ pcaIsCalculated = isCalculated; }
};

