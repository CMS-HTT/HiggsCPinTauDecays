#include "HiggsCPinTauDecays/IpCorrection/interface/IpCorrection.h"

IpCorrection::IpCorrection(TString fileName) {

  TFile * file = new TFile(fileName);
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
      normData[i][j]   = histIpData[i][j]->GetSumOfWeights();
      normMC[i][j]   = histIpMC[i][j]->GetSumOfWeights();
    }
  }

}

IpCorrection::~IpCorrection() {

}

double IpCorrection::correctIp(int coor, double ip, double ipgen, double eta) {

  double ip_corr = ipgen + correctIp(coor,ip-ipgen,eta);
  return ip_corr;

}

double IpCorrection::correctIp(int coor, double ip, double eta) {

  double absEta = TMath::Abs(eta);
  int nEta = binNumber(absEta,EtaRanges);

  TH1D * histMC = histIpMC[coor][nEta];
  TH1D * histData = histIpData[coor][nEta];
  double xmin = histMC->GetXaxis()->GetBinLowEdge(1);
  double xmax = histMC->GetYaxis()->GetBinLowEdge(histMC->GetNbinsX()+1);

  if (ip<xmin||ip>xmax)
    return ip;

  int nBinMC = histMC->GetXaxis()->FindBin(ip);
  double int_lower = (histMC->Integral(0,nBinMC-1) - histMC->Integral(0,0))/normMC[coor][nEta];
  double int_upper = (histMC->Integral(0,nBinMC) - histMC->Integral(0,0))/normMC[coor][nEta];
  double int_diff = int_upper - int_lower;
  double xlow = histMC->GetXaxis()->GetBinLowEdge(nBinMC);
  double binWidth = histMC->GetXaxis()->GetBinWidth(nBinMC);
  double int_center = int_lower + int_diff*(ip-xlow)/binWidth;

  int nBinData = 1;
  double int_data_lower = 0;
  double int_data_higher = 0;
  for (int j=1; j<=histData->GetNbinsX(); ++j) {
    int_data_lower = (histData->Integral(0,j-1) - histData->Integral(0,0))/normData[coor][nEta];
    int_data_higher = (histData->Integral(0,j) - histData->Integral(0,0))/normData[coor][nEta];
    if (int_center>int_data_lower&&int_center<int_data_higher) {
      nBinData = j;
      break;
    }
  }
  
  double binWidthData = histData->GetXaxis()->GetBinWidth(nBinData);
  double xlowData = histData->GetXaxis()->GetBinLowEdge(nBinData);
  
  double int_data_diff = int_data_higher - int_data_lower;
  double ipcorr = xlowData + binWidthData*(int_center-int_data_lower)/int_data_diff;

  return ipcorr;

}
