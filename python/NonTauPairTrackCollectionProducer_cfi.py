import FWCore.ParameterSet.Config as cms

NonTauPairTrackCollectionProducer = cms.EDProducer(
	"NonTauPairTrackCollectionProducer",
	GenParticlesTag = cms.InputTag("prunedGenParticles"),
	TauPairTag = cms.InputTag("slimmedTaus"),
	PVTag = cms.InputTag("offlineSlimmedPrimaryVertices"),
	deltaRThreshold = cms.double(0.5)

	#GenParticles = cms.InputTag("genParticles"),
	#TauPairTag = cms.InputTag("hpsPFTauProducer"),
	#PVTag = cms.InputTag("offlineSlimmedPrimaryVertices"),
	#deltaRThreshold = cms.double(0.5)
)

