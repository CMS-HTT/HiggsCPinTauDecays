import FWCore.ParameterSet.Config as cms
from HiggsCPinTauDecays.TauRefit.NonTauPairTrackCollectionProducer_cfi import *

RefitVertexBSProducer = cms.EDProducer(
	"RefitVertexProducer",
	TrackCollectionTag = cms.InputTag("NonTauPairTrackCollectionProducer"),
	PVTag = cms.InputTag("offlineSlimmedPrimaryVertices"),
	beamSpot = cms.InputTag("offlineBeamSpot"),
	useBeamSpot = cms.bool(True)
)

RefitVertexNoBSProducer = cms.EDProducer(
	"RefitVertexProducer",
	TrackCollectionTag = cms.InputTag("NonTauPairTrackCollectionProducer"),
	PVTag = cms.InputTag("offlineSlimmedPrimaryVertices"),
	beamSpot = cms.InputTag("offlineBeamSpot"),
	useBeamSpot = cms.bool(False)
)

RefitVertexBS = cms.Sequence(NonTauPairTrackCollectionProducer*RefitVertexBSProducer)
RefitVertexNoBS = cms.Sequence(NonTauPairTrackCollectionProducer*RefitVertexNoBSProducer)
