import FWCore.ParameterSet.Config as cms
from Tau.TauRefit.NonTauPairTrackCollectionProducer_cfi import *

RefitVertexProducer = cms.EDProducer("RefitVertexProducer",
									 TrackCollectionTag = cms.InputTag("NonTauPairTrackCollectionProducer"),
									 #TrackCollectionTag = cms.InputTag("globalMuons"),
									 beamSpot = cms.InputTag("offlineBeamSpot"),
									 useBeamSpot = cms.bool(False)
									 )

RefitVertex = cms.Sequence(NonTauPairTrackCollectionProducer*RefitVertexProducer)
