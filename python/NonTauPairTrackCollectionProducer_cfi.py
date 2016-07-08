import FWCore.ParameterSet.Config as cms

NonTauPairTrackCollectionProducer = cms.EDProducer(
	"NonTauPairTrackCollectionProducer",
	GenParticles = cms.InputTag("genParticles"),
	TauPairTag = cms.InputTag("hpsPFTauProducer"),
	PVTag = cms.InputTag("offlinePrimaryVertices"),
	deltaRThreshold = cms.double(0.5)
)

