#ifndef IpCorrection_h
#define IpCorrection_h

#include <TFile.h>
#include <TH1.h>
#include <TF1.h>
#include <TString.h>
#include <TRandom.h>
#include <TMath.h>
#include <assert.h>

class IpCorrection {
 public:
  IpCorrection(TString fileName);
  ~IpCorrection();
  float correctIp(int coordinate, float ip, float eta);
  float correctIp(int coordinate, float ip, float ipgen, float eta);
  enum Coordinate{Ipx=0, Ipy=1, Ipz=2};

 private:
  int binNumber(float x, const std::vector<float> bins) const
  {
    for (size_t iB=0; iB<bins.size(); ++iB)
      if (x>=bins[iB]&&x<bins[iB+1])
	return iB;
    return 0;
  }

  int binNumber(float x, int nbins, const float * bins) {
    int binN = 0;
    for (int iB=0; iB<nbins; ++iB) {
      if (x>=bins[iB]&&x<bins[iB+1]) {
	binN = iB;
	break;
      }
    }    
    return binN;
  }

  TH1D * histIpData[3][4];
  TH1D * histIpMC[3][4];
  float normData[3][4];
  float normMC[3][4];

  std::vector<TString> IpNames = {"ipx","ipy","ipz"};
  std::vector<TString> EtaNames;
  int nEtaBins;
  std::vector<float>  EtaRanges;



};

#endif
