/* class AdvancedRefitVertexProducer
 * EDProducer
 * This producer is intended to take an existing track collection,
 * remove those tracks which are associated to tau decay products
 * and fit a new vertex
 */

#include "VertexRefit/TauRefit/plugins/AdvancedRefitVertexProducer.h"

using namespace reco;
using namespace edm;
using namespace std;

AdvancedRefitVertexProducer::AdvancedRefitVertexProducer(const edm::ParameterSet& iConfig):
	srcCands_(consumes<std::vector<pat::PackedCandidate> >(iConfig.getParameter<edm::InputTag>("srcCands"))),
	srcLostTracks_(consumes<std::vector<pat::PackedCandidate> >(iConfig.getParameter<edm::InputTag>("srcLostTracks"))),
	srcElectrons_(consumes<std::vector<pat::Electron> >(iConfig.getParameter<edm::InputTag>("srcElectrons"))),
	srcMuons_(consumes<std::vector<pat::Muon> >(iConfig.getParameter<edm::InputTag>("srcMuons"))),
	srcTaus_(consumes<std::vector<pat::Tau> >(iConfig.getParameter<edm::InputTag>("srcTaus"))),

	PVTag_(consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("PVTag"))),
	beamSpotTag_(consumes<reco::BeamSpot>(iConfig.getParameter<edm::InputTag>("beamSpot"))),
	deltaRThreshold(iConfig.getParameter<double>("deltaRThreshold")),
	deltaPtThreshold(iConfig.getParameter<double>("deltaPtThreshold")),
	useBeamSpot_(iConfig.getParameter<bool>("useBeamSpot")),
	useLostCands_(iConfig.getParameter<bool>("useLostCands"))


{
	vInputTag srcLeptonsTags = iConfig.getParameter<vInputTag>("srcLeptons");
	for (vInputTag::const_iterator it = srcLeptonsTags.begin(); it != srcLeptonsTags.end(); ++it) {
		srcLeptons_.push_back(consumes<reco::CandidateView>(*it));
	}
	
	combineNLeptons_ = iConfig.getParameter<int>("combineNLeptons");

	//produces<RefitVertexCollection>();
	//produces<std::vector<RefitVertex> >();
	produces<VertexCollection>();
}

AdvancedRefitVertexProducer::~AdvancedRefitVertexProducer(){
}

void AdvancedRefitVertexProducer::doCombinations(int offset, int k){
	
	if (k==0){
		combinations_.push_back(combination_);
		combination_.pop_back();
		return;
	}
	for (size_t i = offset; i <= allLeptons_.size() - k; ++i){
		combination_.push_back(allLeptons_[i]);
		doCombinations(i+1, k-1);
	}
	combination_.clear();
}

void AdvancedRefitVertexProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup){

	//std::cout << "+++++++++++++++++++++++++++" << std::endl;
	// Obtain collections
	edm::Handle<std::vector<pat::PackedCandidate> > PFCands;
	iEvent.getByToken(srcCands_,PFCands);

	edm::Handle<std::vector<pat::PackedCandidate> > PFLostTracks;
	iEvent.getByToken(srcLostTracks_,PFLostTracks);

	edm::Handle<std::vector<pat::Electron> > PatElectrons;
	iEvent.getByToken(srcElectrons_,PatElectrons);
	
	edm::Handle<std::vector<pat::Muon> > PatMuons;
	iEvent.getByToken(srcMuons_,PatMuons);
	
	edm::Handle<std::vector<pat::Tau> > PatTaus;
	iEvent.getByToken(srcTaus_,PatTaus);
	
	edm::Handle<reco::VertexCollection> PV;
	iEvent.getByToken(PVTag_,PV);

	edm::Handle<reco::BeamSpot> beamSpot;
	iEvent.getByToken(beamSpotTag_,beamSpot);
	
	edm::ESHandle<TransientTrackBuilder> transTrackBuilder;
	iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder",transTrackBuilder);
	
	reco::Vertex thePV = PV->front();

	allLeptons_.clear();
	combinations_.clear();
	for (std::vector<edm::EDGetTokenT<reco::CandidateView>>::const_iterator srcLeptonsPart = srcLeptons_.begin(); srcLeptonsPart != srcLeptons_.end(); ++srcLeptonsPart) {
		edm::Handle<reco::CandidateView> leptons;
		iEvent.getByToken(*srcLeptonsPart, leptons);
		for (size_t i=0; i<leptons->size(); ++i){
			allLeptons_.push_back(leptons->ptrAt(i));
		}
	}

	if (allLeptons_.size() >= combineNLeptons_){
		doCombinations(0, combineNLeptons_);
	}


	// Create a new track collection by removing tracks from tau decay products
	//reco::Vertex thePV = PV->front();
	int vtxIdx=0; // AOD PV
	std::auto_ptr<VertexCollection> VertexCollection_out = std::auto_ptr<VertexCollection>(new VertexCollection);
	//std::auto_ptr<RefitVertexCollection> VertexCollection_out;
	//std::auto_ptr<std::vector<RefitVertex> > VertexCollection_out2;
	//VertexCollection_out2->clear(); // 

	// loop over the pairs in combinations_
	for (std::vector<std::vector<edm::Ptr<reco::Candidate>>>::const_iterator pair = combinations_.begin(); pair != combinations_.end(); ++pair) {

		// create the TrackCollection for the current pair
		reco::TrackCollection* newTrackCollection(new TrackCollection);
		std::vector<reco::TransientTrack> transTracks;

		TransientVertex transVtx;
		reco::Vertex newPV = PV->front(); // initialized to the PV
		//RefitVertex newPV2(PV->front()); // initialized to the PV

		// loop over the PFCandidates
		for (std::vector<pat::PackedCandidate>::const_iterator cand = PFCands->begin(); cand != PFCands->end(); ++cand) {

			if (cand->charge()==0 || cand->vertexRef().isNull() ) continue;
			if ( !(cand->bestTrack()) ) continue;
			bool skipTrack = false;

			// loop over the pair components
			for (size_t i=0; i<pair->size(); ++i){

				// if pair[i]==electron || muon
				if (std::abs(pair->at(i)->pdgId())==11 || std::abs(pair->at(i)->pdgId())==13){
					if (reco::deltaR(pair->at(i)->p4(), cand->p4())<deltaRThreshold
						&& std::abs(pair->at(i)->pt()/cand->pt() -1)<deltaPtThreshold){
						skipTrack = true;
					}
				} // if pair[i]==electron || muon

				else{
					edm::Ptr<pat::Tau> pair_i = (edm::Ptr<pat::Tau>)pair->at(i);
					for (size_t j=0; j< pair_i->signalChargedHadrCands().size(); ++j){
						if (reco::deltaR(pair_i->signalChargedHadrCands()[j]->p4(), cand->p4())<deltaRThreshold
								&& std::abs(pair_i->signalChargedHadrCands()[j]->pt()/cand->pt() -1)<deltaPtThreshold){
								skipTrack = true;
						}
					} // loop over charged hadrons from tau

				} // if pair[i]==tau

			} // loop over pair components

			if (skipTrack) continue;
			
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
				bool skipTrack = false;
	
				// loop over the pair components
				for (size_t i=0; i<pair->size(); ++i){
	
					// if pair[i]==electron || muon
					if (std::abs(pair->at(i)->pdgId())==11 || std::abs(pair->at(i)->pdgId())==13){
						if (reco::deltaR(pair->at(i)->p4(), cand->p4())<deltaRThreshold
							&& std::abs(pair->at(i)->pt()/cand->pt() -1)<deltaPtThreshold){
							skipTrack = true;
						}
					} // if pair[i]==electron || muon
	
					else{
						edm::Ptr<pat::Tau> pair_i = (edm::Ptr<pat::Tau>)pair->at(i);
						for (size_t j=0; j< pair_i->signalChargedHadrCands().size(); ++j){
							if (reco::deltaR(pair_i->signalChargedHadrCands()[j]->p4(), cand->p4())<deltaRThreshold
									&& std::abs(pair_i->signalChargedHadrCands()[j]->pt()/cand->pt() -1)<deltaPtThreshold){
									skipTrack = true;
							}
						} // loop over charged hadrons from tau
	
					} // if pair[i]==tau
	
				} // loop over pair components
	
				if (skipTrack) continue;
				
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
		//if ( FitOk ) VertexCollection_out->push_back(transVtx);
		//else VertexCollection_out->push_back(thePV);
		if ( FitOk ) newPV = (reco::Vertex)transVtx;
		//if ( FitOk ) newPV2 = (RefitVertex)transVtx;

		// Creating reference to the given pair to create the hash-code
		//size_t iCount = 0;
		//for (size_t i=0; i<pair->size(); ++i){
		//	newPV.addUserVtx("lepton"+std::to_string(iCount++), pair->at(i))
		//}

		VertexCollection_out->push_back(newPV);
		//VertexCollection_out2->push_back(newPV2);



	} // loop over the pair combinations

	if (combinations_.size()==0) VertexCollection_out->push_back(thePV);

	iEvent.put(VertexCollection_out);
}
DEFINE_FWK_MODULE(AdvancedRefitVertexProducer);
