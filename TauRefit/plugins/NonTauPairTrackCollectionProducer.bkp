/* class PFTauPrimaryVertexProducer
 * EDProducer of the 
 * authors: R. Caspart <rcaspart@cern.ch>
 * This producer is intended to select from a collection of Tracks originating from a
 * primary vertex those tracks which are not associated to a tau-lepton or its decay products
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

#include "DataFormats/PatCandidates/interface/Tau.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/PatCandidates/interface/PackedGenParticle.h"

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
		edm::EDGetTokenT<std::vector<pat::PackedCandidate> > CandsTag_;
		edm::EDGetTokenT<std::vector<pat::Tau> > TauPairTag_;
		edm::EDGetTokenT<reco::VertexCollection > PVTag_;
		double deltaRThreshold;
		double deltaPtThreshold;
};

NonTauPairTrackCollectionProducer::NonTauPairTrackCollectionProducer(const edm::ParameterSet& iConfig):
	CandsTag_(consumes<std::vector<pat::PackedCandidate> >(iConfig.getParameter<edm::InputTag>("CandsTag"))),
	TauPairTag_(consumes<std::vector<pat::Tau> >(iConfig.getParameter<edm::InputTag>("TauPairTag"))),
	PVTag_(consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("PVTag"))),
	deltaRThreshold(iConfig.getParameter<double>("deltaRThreshold")),
	deltaPtThreshold(iConfig.getParameter<double>("deltaPtThreshold"))
{
	produces<std::vector<reco::Track>>();
	produces<int>();
}

NonTauPairTrackCollectionProducer::~NonTauPairTrackCollectionProducer(){
}

void NonTauPairTrackCollectionProducer::produce(edm::Event& iEvent,const edm::EventSetup& iSetup){

	// Obtain Collections
	edm::Handle<std::vector<pat::PackedCandidate> > PFCands;
	iEvent.getByToken(CandsTag_,PFCands);

	edm::Handle<std::vector<pat::Tau> > PFTau;
	iEvent.getByToken(TauPairTag_,PFTau);
	
	edm::Handle<reco::VertexCollection> PV;
	iEvent.getByToken(PVTag_,PV);
	
	// Select tau-pair to consider
	std::vector<pat::Tau> PFTauPair;
	int i=0;
	for (std::vector<pat::Tau>::const_iterator Part = PFTau->begin();  Part != PFTau->end(); ++Part)
	{
		if ( i<2 ){
			PFTauPair.push_back(*Part);
			++i;
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Get Non-Tau tracks
	//reco::Vertex thePV = PV->front();
	int vtxIdx=0; // AOD PV
	std::auto_ptr<reco::TrackCollection> nonTauTracks(new TrackCollection);
	for (std::vector<pat::PackedCandidate>::const_iterator Part = PFCands->begin(); Part != PFCands->end(); ++Part){
		if (Part->charge() ==0 || Part->vertexRef().isNull()) continue;
		if ( !(Part->bestTrack()) ) continue;
		bool skipTrack = false;
		for (std::vector<pat::Tau>::const_iterator tauPart = PFTauPair.begin();  tauPart != PFTauPair.end(); ++tauPart){
			for (size_t i=0; i<tauPart->signalChargedHadrCands().size(); ++i){
				if ( reco::deltaR(tauPart->signalChargedHadrCands()[i]->p4(), Part->p4())<deltaRThreshold
					&& std::abs( tauPart->signalChargedHadrCands()[i]->pt()/Part->pt() -1)<deltaPtThreshold ){
					skipTrack = true;
				}
			} // loop over hadrCands
		} // loop over PFTauPair

		if (skipTrack) continue;
		
		int key = Part->vertexRef().key();
		int quality = Part->pvAssociationQuality();
		if (key != vtxIdx ||
			(quality != pat::PackedCandidate::UsedInFitTight &&
			quality != pat::PackedCandidate::UsedInFitLoose) ) continue;

		nonTauTracks->push_back(*(Part->bestTrack()));
	} // loop over PFCands

	iEvent.put(nonTauTracks);
}
DEFINE_FWK_MODULE(NonTauPairTrackCollectionProducer);
