/* class PFTauPrimaryVertexProducer
 * EDProducer of the 
 * authors: R. Caspart <rcaspart@cern.ch>
 * This producer is intended to select from a collection of Tracks originating from a
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
  edm::InputTag cands_;
  edm::InputTag lostCands_;
  edm::InputTag genCands_;
  edm::InputTag taus_;
  bool useTauTracks_;
  bool useLostCands_;
  double deltaRMatch;
  double deltaRThreshold;
};

NonTauPairTrackCollectionProducer::NonTauPairTrackCollectionProducer(const edm::ParameterSet& iConfig):

	cands_(consumes<edm::View<pat::PackedCandidate> >(iConfig.getParameter<edm::InputTag>("src"))),
	lostCands_(consumes<edm::View<pat::PackedCandidate> >(iConfig.getParameter<edm::InputTag>("lostSrc"))),
	genCands_(consumes<edm::View<pat::PackedGenParticle> >(iConfig.getParameter<edm::InputTag>("genSrc"))),
	taus_(consumes<std::vector<pat::Tau> >(iConfig.getParameter<edm::InputTag>("taus"))),
	useLostCands_(iConfig.getParameter<bool>("useLostCands")),
	useTauTracks_(iConfig.getUntrackedParameter<bool>("useTauTracks",false)),
	deltaRMatch(iConfig.getParameter<double>("deltaRMatch")),
	deltaRThreshold(iConfig.getParameter<double>("deltaRThreshold")),

{
  //produces<std::vector<reco::Track>>(); 
  produces<reco::TrackCollection>("NonTauPairTrackCollection");
}

NonTauPairTrackCollectionProducer::~NonTauPairTrackCollectionProducer(){
}


void NonTauPairTrackCollectionProducer::produce(edm::Event& iEvent,const edm::EventSetup& iSetup){

	// get collections
	edm::Handle<edm::View<pat::PackedCandidate> > cands;
	iEvent.getByToken(cands_, cands);
	edm::Handle<edm::View<pat::PackedCandidate> > lostCands;
	iEvent.getByToken(lostCands_, lostCands);
	edm::Handle<edm::View<pat::PackedGenParticle> > genCands;
	iEvent.getByToken(genCands_, genCands);
	edm::Handle<wdm::View<pat::Tau> > taus;
	iEvent.getByToken(taus_, taus);

	int vtxIdx = 0;  // AOD PV
	reco::TrackCollection nonTauTracks;
	edm::View<pat::PackedGenParticle> GenTauPair;

	// select the tau pair to consider:
	// find the gen tau pair
	// and find the PF tau pair that matches the gen tau pair
	edm::View<pat::PackedGenParticle> GenTauPair;
	edm::View<pat::Tau> PFTauPair;

	// select gen tau pair
	for (edm::View<pat::PackedGenParticle>::const_iterator Part=genCands->begin(); Part!=genCands->end(); ++Part){
		if (Part->charge()==0) continue;
		if (abs(Part->pdgId())==15 && 
			(Part->mother()->pdgId()==25 || Part->mother()->pdgId()==23 || Part->mother()->pdgId()==22 Part->mother()->pdgId()==36)){
			GenTauPair.push_back(*Part);
		} // if pdgId and mother-pdgId
	} // for Part=genCands

	assert(GenTauPair.push_back(*Part);

	// select PF tau pair that matches the gen tau pair within a threshold deltaRMatch
	for (edm::View<pat::Tau>::const_iterator Part=taus->begin(); Part!=taus->end(); ++Part){
		for (edm::View<pat::PackedGenParticle>::const_iterator iPart=GenTauPair->begin(); iPart!=GenTauPair->end(); ++iPart){
			if (deltaR(*Part, *iPart)<deltaRMatch){
				PFTauPair.push_back(*Part);
			} // if deltaRMatch
		} // for iPart=GenTauPair
	} // for Part=taus

	// skip tau pair tracks
	// loop over cands
	for (edm::View<pat::PackedCandidate>::const_iterator Part=cands->begin(); Part!=cands->end(); ++Part){
		if (Part->charge()==0 || Part->vertexRef()->isNull()) continue;
		if (!(Part->bestTrack())) continue;

		bool skipTrack = false;

		if(!useTauTracks_){
			if (PFTauPair->at(0) && PFTauPair->at(1)){
				// loop over PF tau pair
				for (edm::View<pat::Tau>::const_iterator tauPart=PFTauPair->begin(); tauPart!=PFTauPair->end(); ++tauPart){
					std::vector<edm::View<pat::PackedCandidate> > hadrCands = Part->signalChargedHadrCands();

					//loop over hadrons daughters
					for (std::vector<edm::View<pat::PackedCandidate> > hadrPart=){
						if (deltaR(*hadrPart, *Part)<deltaRThreshold){
							skipTrack = true;
						} // if deltaRThreshold
					} // for hadrPart=hadrCands
				} // for tauPart=PFTauPair
			} // if PFTauPair
		} // if !useTauTracks

	if (skipTrack) continue;

	int key = Part->vertexRef().key();
	int quality = Part->pvAssociationQuality();

	if (key!=vtxIdx || (quality!=pat::PackedCandidate::UsedInFitTight && quality!=pat::PackedCandidate::UsedInFitLoose)) continue;

	nonTauTracks.push_back(*(Part->bestTrack()));
	} // for Part=cands

	// loop over lostCands
	if (useLostCands_){
		for (edm::View<pat::PackedCandidate>::const_iterator Part=lostCands->begin(); Part!=lostCands->end(); ++Part){
			if (Part->charge()==0 || Part->vertexRef()->isNull()) continue;
			if (!(Part->bestTrack())) continue;
	
			bool skipTrack = false;
	
			if(!useTauTracks_){
				if (PFTauPair->at(0) && PFTauPair->at(1)){
					// loop over PF tau pair
					for (edm::View<pat::Tau>::const_iterator tauPart=PFTauPair->begin(); tauPart!=PFTauPair->end(); ++tauPart){
						std::vector<edm::View<pat::PackedCandidate> > hadrCands = Part->signalChargedHadrCands();
	
						//loop over hadrons daughters
						for (std::vector<edm::View<pat::PackedCandidate> > hadrPart=){
							if (deltaR(*hadrPart, *Part)<deltaRThreshold){
								skipTrack = true;
							} // if deltaRThreshold
						} // for hadrPart=hadrCands
					} // for tauPart=PFTauPair
				} // if PFTauPair
			} // if !useTauTracks
	
		if (skipTrack) continue;
	
		int key = Part->vertexRef().key();
		int quality = Part->pvAssociationQuality();
	
		if (key!=vtxIdx || (quality!=pat::PackedCandidate::UsedInFitTight && quality!=pat::PackedCandidate::UsedInFitLoose)) continue;
	
		nonTauTracks.push_back(*(Part->bestTrack()));
		} // for Part=lostCands
	} // if useLostCands

	iEvent.put(nonTauTracks);
}
DEFINE_FWK_MODULE(NonTauPairTrackCollectionProducer);
