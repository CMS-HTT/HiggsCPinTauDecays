/* class MVARefitVertexProducer
 * EDProducer
 * This producer is intended to take an existing track collection,
 * remove those tracks which are associated to tau decay products
 * and fit a new vertex
 */

#include "VertexRefit/TauRefit/plugins/MVARefitVertexProducer.h"

using namespace reco;
using namespace edm;
using namespace std;

MVARefitVertexProducer::MVARefitVertexProducer(const edm::ParameterSet& iConfig):
	CandsTag_(consumes<std::vector<pat::PackedCandidate> >(iConfig.getParameter<edm::InputTag>("CandsTag"))),
	TauPairTag_(consumes<std::vector<pat::Tau> >(iConfig.getParameter<edm::InputTag>("TauPairTag"))),
	PVTag_(consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("PVTag"))),
	beamSpotTag_(consumes<reco::BeamSpot>(iConfig.getParameter<edm::InputTag>("beamSpot"))),
	deltaRThreshold(iConfig.getParameter<double>("deltaRThreshold")),
	deltaPtThreshold(iConfig.getParameter<double>("deltaPtThreshold")),
	useBeamSpot_(iConfig.getParameter<bool>("useBeamSpot"))
{
	produces<VertexCollection>();
}

MVARefitVertexProducer::~MVARefitVertexProducer(){
}

void MVARefitVertexProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup){

	// Obtain collections
	edm::Handle<std::vector<pat::PackedCandidate> > PFCands;
	iEvent.getByToken(CandsTag_,PFCands);

	edm::Handle<std::vector<pat::Tau> > PFTau;
	iEvent.getByToken(TauPairTag_,PFTau);
	
	edm::Handle<reco::VertexCollection> PV;
	iEvent.getByToken(PVTag_,PV);

	edm::Handle<reco::BeamSpot> beamSpot;
	iEvent.getByToken(beamSpotTag_,beamSpot);
	
	edm::ESHandle<TransientTrackBuilder> transTrackBuilder;
	iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder",transTrackBuilder);
	
	reco::Vertex thePV = PV->front();

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

	// Create a new track collection by removing tracks from tau decay products
	//reco::Vertex thePV = PV->front();
	int vtxIdx=0; // AOD PV
	std::auto_ptr<reco::TrackCollection> newTrackCollection(new TrackCollection);
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

		newTrackCollection->push_back(*(Part->bestTrack()));
	} // loop over PFCands

	// Refit the vertex
	std::auto_ptr<VertexCollection> VertexCollection_out= std::auto_ptr<VertexCollection>(new VertexCollection);
	TransientVertex transVtx;
	std::vector<reco::TransientTrack> transTracks;
	for (std::vector<reco::Track>::const_iterator iter=newTrackCollection->begin(); iter!=newTrackCollection->end(); ++iter){
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
	if ( FitOk ) VertexCollection_out->push_back(transVtx);
	else VertexCollection_out->push_back(thePV);

	iEvent.put(VertexCollection_out);
}
DEFINE_FWK_MODULE(MVARefitVertexProducer);
