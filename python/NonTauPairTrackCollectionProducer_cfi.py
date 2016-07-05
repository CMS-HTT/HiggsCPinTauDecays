import FWCore.ParameterSet.Config as cms

NonTauPairTrackCollectionProducer = cms.EDProducer(
	"NonTauPairTrackCollectionProducer",
	src = cms.InputTag("packedPFCandidates"),
	lostSrc = cms.InputTag("lostTracks"),
	genSrc = cms.InputTag("packedGenParticles"),
	taus = cms.InputTag("slimmedTaus"),
	useLostCands = cms.bool(False),
	useTauTracks = cms.untracked.bool(False),
	deltaRMatch = cms.double(),
	deltaRThreshold = cms.double(),
)

