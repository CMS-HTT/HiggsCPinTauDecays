/* class PFTauPrimaryVertexProducer
 * EDProducer of the 
 * authors: R. Caspart <rcaspart@cern.ch>
 * This producer is intended to select a from a collection of Tracks originating from a
 * primary vertex those which are not associated to a tau-lepton or its decay products
 * Thanks goes to Andrew Gilbert and Roger Wolf for their help and suggestions.
 */


#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "DataFormats/TauReco/interface/PFTau.h"
#include "DataFormats/TauReco/interface/PFTauFwd.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h" 
#include "DataFormats/GsfTrackReco/interface/GsfTrackFwd.h" 
#include "DataFormats/Common/interface/RefToBase.h"
#include "DataFormats/EgammaCandidates/interface/Electron.h"
#include "DataFormats/EgammaCandidates/interface/ElectronFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include "DataFormats/Common/interface/RefProd.h"

#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/TauReco/interface/PFTauDiscriminator.h"
#include "CommonTools/Utils/interface/StringCutObjectSelector.h"
#include <memory>
#include <boost/foreach.hpp>
#include <TFormula.h>

#include <memory>

using namespace reco;
using namespace edm;
using namespace std;

class NonTauPairTrackCollectionProducer : public EDProducer {
 public:
  explicit NonTauPairTrackCollectionProducer(const edm::ParameterSet& iConfig);
  ~NonTauPairTrackCollectionProducer();
  virtual void produce(edm::Event&,const edm::EventSetup&);
 private:
  edm::EDGetTokenT<std::vector<reco::GenParticle> > GenParticleTag_;
  edm::EDGetTokenT<std::vector<reco::PFTau> > TauPairTag_;
  edm::EDGetTokenT<reco::VertexCollection > PVTag_;
  double deltaRThreshold;
  bool MCSamples_;
};

NonTauPairTrackCollectionProducer::NonTauPairTrackCollectionProducer(const edm::ParameterSet& iConfig):
  //GenParticleTag_(iConfig.getParameter<edm::InputTag>("GenParticles")),
  //TauPairTag_(iConfig.getParameter<edm::InputTag>("TauPairTag")),
  //PVTag_(iConfig.getParameter<edm::InputTag>("PVTag")),
  GenParticleTag_(consumes<std::vector<reco::GenParticle> >(iConfig.getParameter<edm::InputTag>("GenParticles"))),
  TauPairTag_(consumes<std::vector<reco::PFTau> >(iConfig.getParameter<edm::InputTag>("TauPairTag"))),
  PVTag_(consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("PVTag"))),
  deltaRThreshold(iConfig.getParameter<double>("deltaRThreshold"))
{
  produces<std::vector<reco::Track>>(); 
  produces<int>();
  //produces<TrackCollection>("NonTauPairTrackCollection"); 
}

NonTauPairTrackCollectionProducer::~NonTauPairTrackCollectionProducer(){
}

void NonTauPairTrackCollectionProducer::produce(edm::Event& iEvent,const edm::EventSetup& iSetup){
  // Obtain Collections

  //edm::Handle<std::vector<reco::Candidate> > GenParticles;
  edm::Handle<std::vector<reco::GenParticle> > GenParticles;
  iEvent.getByToken(GenParticleTag_,GenParticles);

  edm::Handle<std::vector<reco::PFTau> > PFTau;
  iEvent.getByToken(TauPairTag_,PFTau);

  edm::Handle<reco::VertexCollection> PV;
  iEvent.getByToken(PVTag_,PV);


  // Select tau-pair to consider
  std::auto_ptr<int> nonMatchingTaus(new int(0));
  std::vector<reco::GenParticle> TauPair;
  reco::Vertex thePV = PV->front();
  double Threshold = 0.1;
  //std::cout << thePV.position() << std::endl;
  for (std::vector<reco::GenParticle>::const_iterator Part = GenParticles->begin();  Part != GenParticles->end(); Part++)
  {
    if(abs(Part->pdgId()) == 15 && (Part->mother()->pdgId()==25 || Part->mother()->pdgId()==23 || Part->mother()->pdgId()==22 || Part->mother()->pdgId()==36))
	{
	  TauPair.push_back(*Part);
	  if(abs(Part->vx() - thePV.x()) > Threshold || abs(Part->vy() - thePV.y()) > Threshold || abs(Part->vz() - thePV.z()) > Threshold) (*nonMatchingTaus) += 1;
	  //std::cout << (*Part).vertex() << std::endl;
	  //std::cout << *Part.vx() << " " << *Part.vy() << " " << *Part.vz() <<std::endl;
	}
  }
  //std::cout << std::endl;
  //assert(TauPair.size()>2);
  std::vector<reco::PFTau> PFTauPair;
  for (std::vector<reco::PFTau>::const_iterator Part = PFTau->begin();  Part != PFTau->end(); Part++)
  {
  	for (std::vector<reco::GenParticle>::const_iterator iPart = TauPair.begin();  iPart != TauPair.end(); iPart++)
  	{
	  if(reco::deltaR(*Part,*iPart) < deltaRThreshold)
	  {
	    PFTauPair.push_back(*Part);
	  }
	}
  }
  
  std::vector<reco::TrackBaseRef> TauPairTracks;
    for(std::vector<reco::PFTau>::const_iterator Part = PFTauPair.begin(); Part != PFTauPair.end(); Part++) {
      //reco::Vertex thePV = PV->front();
	  ///////////////////////////////////////////////////////////////////////////////////////////////
  	  // Get tracks from PFTau daugthers
//#if CMSSW_MAJOR_VERSION >= 7
	  const std::vector<edm::Ptr<reco::PFCandidate> > cands = (*Part).signalPFChargedHadrCands();
	  for (std::vector<edm::Ptr<reco::PFCandidate> >::const_iterator iter = cands.begin(); iter!=cands.end(); iter++){
//#else
//	  const edm::RefVector<std::vector<reco::PFCandidate> > cands = (*Part).signalPFChargedHadrCands();
//	  for(edm::RefVector<std::vector<reco::PFCandidate> >::const_iterator iter = cands.begin(); iter!=cands.end(); iter++){
//#endif
	    if(iter->get()->trackRef().isNonnull()) TauPairTracks.push_back(reco::TrackBaseRef(iter->get()->trackRef()));
	    else if(iter->get()->gsfTrackRef().isNonnull()){TauPairTracks.push_back(reco::TrackBaseRef(((iter)->get()->gsfTrackRef())));}
	  }
	}
	  ///////////////////////////////////////////////////////////////////////////////////////////////
	  // Get Non-Tau tracks
      std::auto_ptr<std::vector<reco::Track>>  nonTauTracks(new TrackCollection);
	  for(std::vector<reco::TrackBaseRef>::const_iterator vtxTrkRef=thePV.tracks_begin();vtxTrkRef<thePV.tracks_end();vtxTrkRef++){
	    bool nonTauTrackFlag = true;
	    //if(TauPairTracks.size() == 0) nonTauTracks->push_back(**vtxTrkRef);
	    for (unsigned int sigTrk = 0; sigTrk < TauPairTracks.size(); sigTrk++) {
	      if((*vtxTrkRef)==TauPairTracks[sigTrk] ){
	        nonTauTrackFlag = false;
			//nonTauTracks->push_back(**vtxTrkRef);
	      }
        }
		if(nonTauTrackFlag) nonTauTracks->push_back(**vtxTrkRef);
	  }
    iEvent.put(nonTauTracks);
	iEvent.put(nonMatchingTaus);
}
DEFINE_FWK_MODULE(NonTauPairTrackCollectionProducer);
