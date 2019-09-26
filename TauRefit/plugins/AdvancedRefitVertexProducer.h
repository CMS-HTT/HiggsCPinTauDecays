#ifndef AdvancedRefitVertexProducer_h
#define AdvancedRefitVertexProducer_h

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
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrackFwd.h"
#include "DataFormats/Common/interface/RefToBase.h"

#include "DataFormats/EgammaCandidates/interface/Electron.h"
#include "DataFormats/EgammaCandidates/interface/ElectronFwd.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/TauReco/interface/PFTau.h"
#include "DataFormats/TauReco/interface/PFTauFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Tau.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/PatCandidates/interface/PackedGenParticle.h"

#include "DataFormats/Common/interface/RefProd.h"

#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/TauReco/interface/PFTauDiscriminator.h"
#include "CommonTools/Utils/interface/StringCutObjectSelector.h"

#include "HiggsCPinTauDecays/TauRefit/interface/RefitVertex.h"

#include <memory>
#include <boost/foreach.hpp>
#include <TFormula.h>


using namespace reco;
using namespace edm;
using namespace std;




class AdvancedRefitVertexProducer : public EDProducer {
	public:
		enum Alg{useInputPV=0, useFontPV};

		explicit AdvancedRefitVertexProducer(const edm::ParameterSet& iConfig);
		~AdvancedRefitVertexProducer();

		virtual void produce(edm::Event&,const edm::EventSetup&);

		typedef std::vector<edm::InputTag> vInputTag;



	private:
		void doCombinations(int offset, int k);

		edm::EDGetTokenT<std::vector<pat::PackedCandidate> > srcCands_;
		edm::EDGetTokenT<std::vector<pat::PackedCandidate> > srcLostTracks_;
		//edm::EDGetTokenT<std::vector<pat::Tau> > TauTag_;
		edm::EDGetTokenT<std::vector<pat::Electron> > srcElectrons_;
		edm::EDGetTokenT<std::vector<pat::Muon> > srcMuons_;
		edm::EDGetTokenT<std::vector<pat::Tau> > srcTaus_;
		edm::EDGetTokenT<reco::VertexCollection > PVTag_;
		edm::EDGetTokenT<reco::BeamSpot> beamSpotTag_;
		double deltaRThreshold;
		double deltaPtThreshold;
		bool useBeamSpot_;
		bool useLostCands_;
		bool excludeFullyLeptonic_;

		std::vector<edm::EDGetTokenT<reco::CandidateView> > srcLeptons_;
		std::vector<edm::Ptr<reco::Candidate> > allLeptons_;
		std::vector<std::vector<edm::Ptr<reco::Candidate> > > combinations_;
		std::vector<edm::Ptr<reco::Candidate> > combination_;

		size_t combineNLeptons_;

};


#endif
