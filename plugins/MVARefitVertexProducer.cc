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
	srcCands_(consumes<std::vector<pat::PackedCandidate> >(iConfig.getParameter<edm::InputTag>("srcCands"))),
	//TauTag_(consumes<std::vector<pat::Tau> >(iConfig.getParameter<edm::InputTag>("TauTag"))),
	srcElectrons_(consumes<std::vector<pat::Electron> >(iConfig.getParameter<edm::InputTag>("srcElectrons"))),
	srcMuons_(consumes<std::vector<pat::Muon> >(iConfig.getParameter<edm::InputTag>("srcMuons"))),
	srcTaus_(consumes<std::vector<pat::Tau> >(iConfig.getParameter<edm::InputTag>("srcTaus"))),

	PVTag_(consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("PVTag"))),
	beamSpotTag_(consumes<reco::BeamSpot>(iConfig.getParameter<edm::InputTag>("beamSpot"))),
	deltaRThreshold(iConfig.getParameter<double>("deltaRThreshold")),
	deltaPtThreshold(iConfig.getParameter<double>("deltaPtThreshold")),
	useBeamSpot_(iConfig.getParameter<bool>("useBeamSpot"))


{
	vInputTag srcLeptonsTags = iConfig.getParameter<vInputTag>("srcLeptons");
	for (vInputTag::const_iterator it = srcLeptonsTags.begin(); it != srcLeptonsTags.end(); ++it) {
		srcLeptons_.push_back(consumes<reco::CandidateView>(*it));
	}
	
	combineNLeptons_ = iConfig.getParameter<int>("combineNLeptons");

	produces<VertexCollection>();
}

MVARefitVertexProducer::~MVARefitVertexProducer(){
}

void MVARefitVertexProducer::doCombinations(int offset, int k){
	
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

void MVARefitVertexProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup){
	std::cout << "++++++++++++++++++++++++++" << std::endl;
	// Obtain collections
	edm::Handle<std::vector<pat::PackedCandidate> > PFCands;
	iEvent.getByToken(srcCands_,PFCands);

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

	std::cout << "allLep size " << allLeptons_.size() << std::endl;
	for (size_t i=0; i<allLeptons_.size(); ++i){
		std::cout << " " << allLeptons_[i]->pdgId() << std::endl;
	}

	std::cout << "comb size " << combinations_.size() << std::endl;
	for (size_t j=0; j<combinations_.size(); ++j){
		std::cout << " comb[" << j << "]" << std::endl;
		for (size_t jj=0; jj<combinations_[j].size(); ++jj){
			std::cout << "  " << combinations_[j].at(jj)->pdgId() << std::endl;
		}
	}


//	// Select tau-pair to consider
//	std::vector<pat::Tau> PFTauPair;
//	int i=0;
//	for (std::vector<pat::Tau>::const_iterator Part = PatTaus->begin();  Part != PatTaus->end(); ++Part)
//	{
//		if ( i<2 ){
//			PFTauPair.push_back(*Part);
//			++i;
//		}
//	}

	// Create a new track collection by removing tracks from tau decay products
	//reco::Vertex thePV = PV->front();
	//int vtxIdx=0; // AOD PV
	//std::auto_ptr<reco::TrackCollection> newTrackCollection(new TrackCollection);

	std::auto_ptr<VertexCollection> VertexCollection_out= std::auto_ptr<VertexCollection>(new VertexCollection);
	//TransientVertex transVtx;
	//std::vector<reco::TransientTrack> transTracks;

	std::cout << " " << std::endl;
	for (std::vector<std::vector<edm::Ptr<reco::Candidate>>>::const_iterator pair = combinations_.begin(); pair != combinations_.end(); ++pair) {
		std::cout << "pair pdgId " << (*pair)[0]->pdgId() << " " << (*pair)[1]->pdgId() << std::endl;
		std::cout << "pair bestTrack " << (bool)pair->at(0)->bestTrack() << " " << (bool)pair->at(1)->bestTrack() << std::endl;
		
		// loop over the PFCandidates
		for (std::vector<pat::PackedCandidate>::const_iterator cand = PFCands->begin(); cand != PFCands->end(); ++cand) {

			if(std::abs(cand->pdgId())==11 || std::abs(cand->pdgId())==13){
				std::cout << "  cand pdgId " << cand->pdgId() << std::endl;
			}
			if (cand->charge()==0 || cand->vertexRef().isNull() ) continue;
			if ( !(cand->bestTrack()) ) continue;
			bool skipTrack = false;

			// if cand is an electron, compare it with pair==electron
			if (std::abs(cand->pdgId())==11){
				for (size_t i=0; i<pair->size(); ++i){
					if (std::abs(pair->at(i)->pdgId())==11){
						if (reco::deltaR(pair->at(i)->p4(), cand->p4())<deltaRThreshold
							&& std::abs(pair->at(i)->pt()/cand->pt() -1)<deltaPtThreshold){
							skipTrack = true;
						}
					} // if pair[i]==electron
				} // loop over pair components
			} // if cand==electron

			// if cand is an muon, compare it with pair==muon
			else if (std::abs(cand->pdgId())==13){
				for (size_t i=0; i<pair->size(); ++i){
					if (std::abs(pair->at(i)->pdgId())==13){
						if (reco::deltaR(pair->at(i)->p4(), cand->p4())<deltaRThreshold
							&& std::abs(pair->at(i)->pt()/cand->pt() -1)<deltaPtThreshold){
							skipTrack = true;
						}
					} // if pair[i]==muon
				} // loop over pair components
			} // if cand==muon

//		if (std::abs(pair->at(0)->pdgId())==15){
//			edm::Ptr<pat::Tau> copy = (edm::Ptr<pat::Tau>)pair->at(0);
//			std::cout << "copy " << copy->signalChargedHadrCands().size() << std::endl;
//		}

			// if cand is neither electron or muon, compare it with the hadrons from tau decay
			else if (std::abs(pair->at(0)->pdgId())==15){
				edm::Ptr<pat::Tau> pair0 = (edm::Ptr<pat::Tau>)pair->at(0);
				for (size_t i=0; i< pair0->signalChargedHadrCands().size(); ++i){
					if (reco::deltaR(pair0->signalChargedHadrCands()[i]->p4(), cand->p4())<deltaRThreshold
							&& std::abs(pair0->signalChargedHadrCands()[i]->pt()/cand->pt() -1)<deltaPtThreshold){
							skipTrack = true;
					}
				} // loop over charged hadrons from tau
			} // if pair[0]==tau

			else if (std::abs(pair->at(1)->pdgId())==15){
				edm::Ptr<pat::Tau> pair1 = (edm::Ptr<pat::Tau>)pair->at(1);
				for (size_t i=0; i<pair1->signalChargedHadrCands().size(); ++i){
					if (reco::deltaR(pair1->signalChargedHadrCands()[i]->p4(), cand->p4())<deltaRThreshold
							&& std::abs(pair1->signalChargedHadrCands()[i]->pt()/cand->pt() -1)<deltaPtThreshold){
							skipTrack = true;
					}
				} // loop over charged hadrons from tau
			} // if pair[1]==tau

			if (skipTrack){
				std::cout << "skipTrack " << skipTrack << std::endl;
			}
		} // loop over the PFCandidates
	} // loop over the pair combinations



//	for (std::vector<pat::PackedCandidate>::const_iterator Part = PFCands->begin(); Part != PFCands->end(); ++Part){
//		if (Part->charge() ==0 || Part->vertexRef().isNull()) continue;
//		if ( !(Part->bestTrack()) ) continue;
//		bool skipTrack = false;
//		for (std::vector<pat::Tau>::const_iterator tauPart = PFTauPair.begin();  tauPart != PFTauPair.end(); ++tauPart){
//			for (size_t i=0; i<tauPart->signalChargedHadrCands().size(); ++i){
//				if ( reco::deltaR(tauPart->signalChargedHadrCands()[i]->p4(), Part->p4())<deltaRThreshold
//					&& std::abs( tauPart->signalChargedHadrCands()[i]->pt()/Part->pt() -1)<deltaPtThreshold ){
//					skipTrack = true;
//				}
//			} // loop over hadrCands
//		} // loop over PFTauPair
//
//		if (skipTrack) continue;
//		
//		int key = Part->vertexRef().key();
//		int quality = Part->pvAssociationQuality();
//		if (key != vtxIdx ||
//			(quality != pat::PackedCandidate::UsedInFitTight &&
//			quality != pat::PackedCandidate::UsedInFitLoose) ) continue;
//
//		newTrackCollection->push_back(*(Part->bestTrack()));
//	} // loop over PFCands
//
//	// Refit the vertex
//	std::auto_ptr<VertexCollection> VertexCollection_out= std::auto_ptr<VertexCollection>(new VertexCollection);
//	TransientVertex transVtx;
//	std::vector<reco::TransientTrack> transTracks;
//	for (std::vector<reco::Track>::const_iterator iter=newTrackCollection->begin(); iter!=newTrackCollection->end(); ++iter){
//		transTracks.push_back(transTrackBuilder->build(*iter));
//	}
//	bool FitOk(true);
//	if ( transTracks.size() >= 3 ) {
//		AdaptiveVertexFitter avf;
//		avf.setWeightThreshold(0.1); //weight per track. allow almost every fit, else --> exception
//		try {
//			if ( !useBeamSpot_ ){
//				transVtx = avf.vertex(transTracks);
//			} else {
//				transVtx = avf.vertex(transTracks, *beamSpot);
//			}
//		} catch (...) {
//			FitOk = false;
//		}
//	} else FitOk = false;
//	if ( FitOk ) VertexCollection_out->push_back(transVtx);
//	else VertexCollection_out->push_back(thePV);

	iEvent.put(VertexCollection_out);
}
DEFINE_FWK_MODULE(MVARefitVertexProducer);
