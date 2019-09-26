import FWCore.ParameterSet.Config as cms

NonTauPairTrackCollectionProducer = cms.EDProducer(
	"NonTauPairTrackCollectionProducer",
	CandsTag = cms.InputTag("packedPFCandidates"),
	TauPairTag = cms.InputTag("slimmedTaus"),
	PVTag = cms.InputTag("offlineSlimmedPrimaryVertices"),
	deltaRThreshold = cms.double(0.001),
	deltaPtThreshold = cms.double(0.001)
)

