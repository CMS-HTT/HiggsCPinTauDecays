import FWCore.ParameterSet.Config as cms

MVARefitVertexBSProducer = cms.EDProducer(
	"MVARefitVertexProducer",
	CandsTag = cms.InputTag("packedPFCandidates"),
	TauPairTag = cms.InputTag("slimmedTaus"),
	PVTag = cms.InputTag("offlineSlimmedPrimaryVertices"),
	deltaRThreshold = cms.double(0.001),
	deltaPtThreshold = cms.double(0.001),

	#TrackCollectionTag = cms.InputTag("NonTauPairTrackCollectionProducer"),
	#PVTag = cms.InputTag("offlineSlimmedPrimaryVertices"),
	beamSpot = cms.InputTag("offlineBeamSpot"),
	useBeamSpot = cms.bool(True)
)

MVARefitVertexNoBSProducer = cms.EDProducer(
	"MVARefitVertexProducer",
	CandsTag = cms.InputTag("packedPFCandidates"),
	TauPairTag = cms.InputTag("slimmedTaus"),
	PVTag = cms.InputTag("offlineSlimmedPrimaryVertices"),
	deltaRThreshold = cms.double(0.001),
	deltaPtThreshold = cms.double(0.001),

	#TrackCollectionTag = cms.InputTag("NonTauPairTrackCollectionProducer"),
	#PVTag = cms.InputTag("offlineSlimmedPrimaryVertices"),
	beamSpot = cms.InputTag("offlineBeamSpot"),
	useBeamSpot = cms.bool(False)
)

MVARefitVertexBS = cms.Sequence(MVARefitVertexBSProducer)
MVARefitVertexNoBS = cms.Sequence(MVARefitVertexNoBSProducer)
