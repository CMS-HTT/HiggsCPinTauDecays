#include "HiggsCPinTauDecays/IpCorrection/interface/IpCorrection.h"

IpCorrection::IpCorrection(TString fileName) {

  file = new TFile(fileName);
  if (file->IsZombie()) {
    std::cout << "IpCorrection : file " << fileName << " does not exist" << std::endl;
    exit(-1);
  }
  TH1D * etaBinsH = (TH1D*)file->Get("etaBinsH");

  if (etaBinsH==NULL) {
    std::cout << "IpCorrection : histogram etaBinsH does not exists " << std::endl;
    exit(-1);
  }
  nEtaBins = etaBinsH->GetNbinsX();

  for (int iB=1; iB<=nEtaBins; ++iB) {
    EtaNames.push_back(etaBinsH->GetXaxis()->GetBinLabel(iB));
    EtaRanges.push_back(etaBinsH->GetXaxis()->GetBinLowEdge(iB));
  }

  EtaRanges.push_back(etaBinsH->GetXaxis()->GetBinLowEdge(nEtaBins+1));

  for (unsigned int i=0; i<IpNames.size(); ++i) {
    for (unsigned int j=0; j<EtaNames.size(); ++j) {
      TString histNameData = IpNames[i] + EtaNames[j] + "_data";
      TString histNameMC   = IpNames[i] + EtaNames[j] + "_mc";
      histIpData[i][j] = (TH1D*)file->Get(histNameData);
      histIpMC[i][j]   = (TH1D*)file->Get(histNameMC);
      if (histIpData[i][j]==NULL) {
	std::cout << "ipCorrection: histogram " << histNameData << " does not exist" << std::endl;
	exit(-1);
      }
      if (histIpMC[i][j]==NULL) {
	std::cout << "ipCorrection: histogram " << histNameMC << " does not exist" << std::endl;
	exit(-1);
      }
    }
  }

  for (unsigned int i=0; i<IpErrNames.size(); ++i) {
    for (unsigned int j=0; j<EtaNames.size(); ++j) {
      TString histNameData = IpErrNames[i] + EtaNames[j] + "_data";
      TString histNameMC   = IpErrNames[i] + EtaNames[j] + "_mc";
      histErrData[i][j] = (TH1D*)file->Get(histNameData);
      histErrMC[i][j]   = (TH1D*)file->Get(histNameMC);
      if (histErrData[i][j]==NULL) {
	std::cout << "ipCorrection: histogram " << histNameData << " does not exist" << std::endl;
	exit(-1);
      }
      if (histErrMC[i][j]==NULL) {
	std::cout << "ipCorrection: histogram " << histNameMC << " does not exist" << std::endl;
	exit(-1);
      }
    }
  }

  for (unsigned int i=0; i<IpSigNames.size(); ++i) {
    for (unsigned int j=0; j<EtaNames.size(); ++j) {
      TString histNameData = IpSigNames[i] + EtaNames[j] + "_data";
      TString histNameMC   = IpSigNames[i] + EtaNames[j] + "_mc";
      histSigData[i][j] = (TH1D*)file->Get(histNameData);
      histSigMC[i][j]   = (TH1D*)file->Get(histNameMC);
      if (histErrData[i][j]==NULL) {
	std::cout << "ipCorrection: histogram " << histNameData << " does not exist" << std::endl;
	exit(-1);
      }
      if (histErrMC[i][j]==NULL) {
	std::cout << "ipCorrection: histogram " << histNameMC << " does not exist" << std::endl;
	exit(-1);
      }
    }
  }

}

IpCorrection::~IpCorrection() {
  file->Close();
}

double IpCorrection::correctIp(int coor, double ip, double ipgen, double eta) {

  double ip_corr = ipgen + correctIp(coor,ip-ipgen,eta);
  return ip_corr;

}

CovMatrix IpCorrection::correctIpCov(CovMatrix ipCovariance, double eta) {

  double absEta = TMath::Abs(eta);
  int nEta = binNumber(absEta,EtaRanges);
  /*
  std::cout << "absEta = " <<  absEta << "   nEta = " << nEta << std::endl;
  for (unsigned int iEta=0; iEta<=EtaRanges.size()-1; ++iEta) {
    std::cout << EtaRanges.at(iEta) << "  ";
  }
  std::cout << "    -> size : " << EtaRanges.size() << std::endl;
  */
  double err[3];
  for (int i=0; i<3; ++i) {
    TH1D * histMC = histErrMC[i][nEta];
    TH1D * histData = histErrData[i][nEta];
    err[i] = applyQuantileMapping(histMC,histData,TMath::Sqrt(ipCovariance(i,i)));
    err[i] /= TMath::Sqrt(ipCovariance(i,i));
  }
  ROOT::Math::SMatrix<float,3,3, ROOT::Math::MatRepStd< float, 3, 3 >> ipCovCorrected;
  for (int i=0; i<3; ++i) {
    for (int j=0; j<3; ++j) {
      ipCovCorrected(i,j) = err[i]*err[j]*ipCovariance(i,j);
    }
  }

  return ipCovCorrected;

}

CovMatrix IpCorrection::correctIpCov(CovMatrix ipCovariance,
				     TVector3 ip,
				     TVector3 ipgen,
				     double eta) {

  double absEta = TMath::Abs(eta);
  int nEta = binNumber(absEta,EtaRanges);

  double ipdif[3]; 
  ipdif[0] = ip.X()-ipgen.X();
  ipdif[1] = ip.Y()-ipgen.Y();
  ipdif[2] = ip.Z()-ipgen.Z();

  double err[3];
  for (int i=0; i<3; ++i) { 
    TH1D * histMC = histIpMC[i][nEta];
    TH1D * histData = histIpData[i][nEta];
    double ipcorr = applyQuantileMapping(histMC,histData,ipdif[i]); 
    double ipsig = ipdif[i]/TMath::Sqrt(ipCovariance(i,i));
    histMC = histSigMC[i][nEta];
    histData = histSigData[i][nEta];
    double ipsigcorr = applyQuantileMapping(histMC,histData,ipsig);
    err[i] = ipcorr/ipsigcorr;
    err[i] /= TMath::Sqrt(ipCovariance(i,i));
  }
  ROOT::Math::SMatrix<float,3,3, ROOT::Math::MatRepStd< float, 3, 3 >> ipCovCorrected;
  for (int i=0; i<3; ++i) {
    for (int j=0; j<3; ++j) {
      ipCovCorrected(i,j) = err[i]*err[j]*ipCovariance(i,j);
    }
  }

  return ipCovCorrected;

}



double IpCorrection::correctIp(int coor, double ip, double eta) {

  double absEta = TMath::Abs(eta);
  int nEta = binNumber(absEta,EtaRanges);

  TH1D * histMC = histIpMC[coor][nEta];
  TH1D * histData = histIpData[coor][nEta];

  double ipcorr = applyQuantileMapping(histMC,histData,ip);

  return ipcorr;

}

TVector3 IpCorrection::correctIp(TVector3 ip, TVector3 ipgen, double eta) {

  TVector3 ipcorr;
  double IpX = ip.X();
  double IpGenX = ipgen.X();
  double IpXCorr = correctIp(IpCorrection::Coordinate::Ipx,IpX,IpGenX,eta);

  double IpY = ip.Y();
  double IpGenY = ipgen.Y();
  double IpYCorr = correctIp(IpCorrection::Coordinate::Ipy,IpY,IpGenY,eta);

  double IpZ = ip.Z();
  double IpGenZ = ipgen.Z();
  double IpZCorr = correctIp(IpCorrection::Coordinate::Ipz,IpZ,IpGenZ,eta);
  
  ipcorr.SetX(IpXCorr);
  ipcorr.SetY(IpYCorr);
  ipcorr.SetZ(IpZCorr);

  return ipcorr;

}

double IpCorrection::applyQuantileMapping(TH1D * histMC, TH1D * histData, double var) {

  double xmin = histMC->GetXaxis()->GetBinLowEdge(2);
  double xmax = histMC->GetXaxis()->GetBinLowEdge(histMC->GetNbinsX());

  double normMC = histMC->GetSumOfWeights();
  double normData = histData->GetSumOfWeights();

  if (var<xmin||var>xmax)
    return var;

  int nBinMC = histMC->GetXaxis()->FindBin(var);
  double int_lower = (histMC->Integral(0,nBinMC-1) - histMC->Integral(0,0))/normMC;
  double int_upper = (histMC->Integral(0,nBinMC) - histMC->Integral(0,0))/normMC;
  double int_diff = int_upper - int_lower;
  double xlow = histMC->GetXaxis()->GetBinLowEdge(nBinMC);
  double binWidth = histMC->GetXaxis()->GetBinWidth(nBinMC);
  double int_center = int_lower + int_diff*(var-xlow)/binWidth;

  int nBinData = 1;
  double int_data_lower = 0;
  double int_data_higher = 0;
  for (int j=1; j<=histData->GetNbinsX(); ++j) {
    int_data_lower = (histData->Integral(0,j-1) - histData->Integral(0,0))/normData;
    int_data_higher = (histData->Integral(0,j) - histData->Integral(0,0))/normData;
    if (int_center>int_data_lower&&int_center<int_data_higher) {
      nBinData = j;
      break;
    }
  }
  
  double binWidthData = histData->GetXaxis()->GetBinWidth(nBinData);
  double xlowData = histData->GetXaxis()->GetBinLowEdge(nBinData);
  
  double int_data_diff = int_data_higher - int_data_lower;
  double varcorr = xlowData + binWidthData*(int_center-int_data_lower)/int_data_diff;

  return varcorr;

}
