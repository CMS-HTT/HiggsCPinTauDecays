/* class RefitVertexProducer
 * EDProducer of the 
 * authors: R. Caspart <rcaspart@cern.ch>
 * This producer is intended to take an existing Track collection and perform a vertex fitting for it.
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

#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "RecoVertex/VertexPrimitives/interface/TransientVertex.h"
#include "RecoVertex/AdaptiveVertexFit/interface/AdaptiveVertexFitter.h"

#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

using namespace reco;
using namespace edm;
using namespace std;

class RefitVertexProducer : public EDProducer {
	public:
		enum Alg{useInputPV=0, useFontPV};

		explicit RefitVertexProducer(const edm::ParameterSet& iConfig);
		~RefitVertexProducer();
		virtual void produce(edm::Event&,const edm::EventSetup&);

	private:
		edm::EDGetTokenT<reco::BeamSpot> beamSpotTag_;
		edm::EDGetTokenT<reco::TrackCollection> TrackCollectionTag_;
		edm::EDGetTokenT<reco::VertexCollection > PVTag_;
		bool useBeamSpot_;
};

RefitVertexProducer::RefitVertexProducer(const edm::ParameterSet& iConfig):
	//beamSpotTag_(iConfig.getParameter<edm::InputTag>("beamSpot")),
	//TrackCollectionTag_(iConfig.getParameter<edm::InputTag>("TrackCollectionTag")),
	beamSpotTag_(consumes<reco::BeamSpot>(iConfig.getParameter<edm::InputTag>("beamSpot"))),
	TrackCollectionTag_(consumes<reco::TrackCollection>(iConfig.getParameter<edm::InputTag>("TrackCollectionTag"))),
	PVTag_(consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("PVTag"))),
	useBeamSpot_(iConfig.getParameter<bool>("useBeamSpot"))
{
	produces<VertexCollection>();
}

RefitVertexProducer::~RefitVertexProducer(){

}

void RefitVertexProducer::produce(edm::Event& iEvent,const edm::EventSetup& iSetup){

	// Obtain Collections
	edm::ESHandle<TransientTrackBuilder> transTrackBuilder;
	iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder",transTrackBuilder);
	
	edm::Handle<reco::BeamSpot> beamSpot;
	iEvent.getByToken(beamSpotTag_,beamSpot);
	
	edm::Handle<reco::TrackCollection> trackCollection;
	iEvent.getByToken(TrackCollectionTag_,trackCollection);
	
	edm::Handle<reco::VertexCollection> PV;
	iEvent.getByToken(PVTag_,PV);

	reco::Vertex thePV = PV->front();

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Refit the vertex
	std::auto_ptr<VertexCollection> VertexCollection_out= std::auto_ptr<VertexCollection>(new VertexCollection);
	TransientVertex transVtx;
	std::vector<reco::TransientTrack> transTracks;
	for (std::vector<reco::Track>::const_iterator iter=trackCollection->begin(); iter!=trackCollection->end(); ++iter){
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
DEFINE_FWK_MODULE(RefitVertexProducer);
