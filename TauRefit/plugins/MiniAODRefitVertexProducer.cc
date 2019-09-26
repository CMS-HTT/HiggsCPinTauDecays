/* class MiniAODRefitVertexProducer
 * EDProducer
 * This producer is intended to take an existing track collection,
 * and refit the vertex applying BeamSpot constraint
 */

#include "HiggsCPinTauDecays/TauRefit/plugins/MiniAODRefitVertexProducer.h"

using namespace reco;
using namespace edm;
using namespace std;

MiniAODRefitVertexProducer::MiniAODRefitVertexProducer(const edm::ParameterSet& iConfig):
	srcCands_(consumes<std::vector<pat::PackedCandidate> >(iConfig.getParameter<edm::InputTag>("srcCands"))),
	srcLostTracks_(consumes<std::vector<pat::PackedCandidate> >(iConfig.getParameter<edm::InputTag>("srcLostTracks"))),
	PVTag_(consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("PVTag"))),
	beamSpotTag_(consumes<reco::BeamSpot>(iConfig.getParameter<edm::InputTag>("beamSpot"))),
	useBeamSpot_(iConfig.getParameter<bool>("useBeamSpot")),
	useLostCands_(iConfig.getParameter<bool>("useLostCands"))


{
  produces<std::vector<RefitVertex> >();
}

MiniAODRefitVertexProducer::~MiniAODRefitVertexProducer(){
}

void MiniAODRefitVertexProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup){

	// Obtain collections
	edm::Handle<std::vector<pat::PackedCandidate> > PFCands;
	iEvent.getByToken(srcCands_,PFCands);

	edm::Handle<std::vector<pat::PackedCandidate> > PFLostTracks;
	iEvent.getByToken(srcLostTracks_,PFLostTracks);

	edm::Handle<reco::VertexCollection> PV;
	iEvent.getByToken(PVTag_,PV);

	edm::Handle<reco::BeamSpot> beamSpot;
	iEvent.getByToken(beamSpotTag_,beamSpot);
	
	edm::ESHandle<TransientTrackBuilder> transTrackBuilder;
	iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder",transTrackBuilder);
	
	reco::Vertex thePV = PV->front();

	// Create a new track collection
	int vtxIdx=0; // AOD PV
	
#if CMSSW_MAJOR_VERSION < 8
	std::auto_ptr<RefitVertexCollection> VertexCollection_out = std::auto_ptr<RefitVertexCollection>(new RefitVertexCollection);
#else	
	std::unique_ptr<RefitVertexCollection> VertexCollection_out = std::auto_ptr<RefitVertexCollection>(new RefitVertexCollection);
#endif
	
	// create the TrackCollection for the current pair
	//reco::TrackCollection* newTrackCollection(new TrackCollection);
	std::auto_ptr<reco::TrackCollection> newTrackCollection = std::auto_ptr<reco::TrackCollection>(new TrackCollection);
	std::vector<reco::TransientTrack> transTracks;
	
	TransientVertex transVtx;
	RefitVertex newPV(thePV); // initialized to the PV

	// loop over the PFCandidates
	for (std::vector<pat::PackedCandidate>::const_iterator cand = PFCands->begin(); cand != PFCands->end(); ++cand) {
	  
	  if (cand->charge()==0 || cand->vertexRef().isNull() ) continue;
	  if ( !(cand->bestTrack()) ) continue;

	  int key = cand->vertexRef().key();
	  int quality = cand->pvAssociationQuality();
	  if (key != vtxIdx ||
	      (quality != pat::PackedCandidate::UsedInFitTight &&
	       quality != pat::PackedCandidate::UsedInFitLoose) ) continue;
	
	  newTrackCollection->push_back(*(cand->bestTrack()));
	  
	} // loop over the PFCandidates


	// loop over the PFLostTracks
	if(useLostCands_){
	  for (std::vector<pat::PackedCandidate>::const_iterator cand = PFLostTracks->begin(); cand != PFLostTracks->end(); ++cand) {
	    
	    if (cand->charge()==0 || cand->vertexRef().isNull() ) continue;
	    if ( !(cand->bestTrack()) ) continue;
	
	    int key = cand->vertexRef().key();
	    int quality = cand->pvAssociationQuality();
	    if (key != vtxIdx ||
		(quality != pat::PackedCandidate::UsedInFitTight &&
		 quality != pat::PackedCandidate::UsedInFitLoose) ) continue;
	    
	    newTrackCollection->push_back(*(cand->bestTrack()));
	    
	  } // loop over the PFLostTracks
	} // if useLostCands==true


	// Refit the vertex
	for (std::vector<reco::Track>::const_iterator iter = newTrackCollection->begin(); iter != newTrackCollection->end(); ++iter){
	  transTracks.push_back(transTrackBuilder->build(*iter));
	}
	bool FitOk(true);
	if ( transTracks.size() >= 3 ) {
	  AdaptiveVertexFitter avf;
	  avf.setWeightThreshold(0.1); //weight per track. allow almost every fit, else --> exception
	  try {
	    if ( !useBeamSpot_ ){
	      transVtx = avf.vertex(transTracks);
	    } else {
	      transVtx = avf.vertex(transTracks, *beamSpot);
	    }
	  } catch (...) {
	    FitOk = false;
	  }
	} else FitOk = false;
	if ( FitOk ){
	  newPV = (RefitVertex)transVtx;
	  VertexCollection_out->push_back(newPV);
	} // if FitOk == true

	//if (combinations_.size()==0) VertexCollection_out->push_back((RefitVertex)thePV);
#if CMSSW_MAJOR_VERSION < 8
	iEvent.put(VertexCollection_out);
#else
	iEvent.put(std::move(VertexCollection_out));
#endif
	
}
DEFINE_FWK_MODULE(MiniAODRefitVertexProducer);
