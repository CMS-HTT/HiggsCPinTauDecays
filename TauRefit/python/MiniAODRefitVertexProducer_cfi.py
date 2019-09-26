import FWCore.ParameterSet.Config as cms

MiniAODRefitVertexBSProducer = cms.EDProducer(
	"MiniAODRefitVertexProducer",
	srcCands = cms.InputTag("packedPFCandidates"),
	srcLostTracks = cms.InputTag("lostTracks"),
	PVTag = cms.InputTag("offlineSlimmedPrimaryVertices"),
	beamSpot = cms.InputTag("offlineBeamSpot"),
	useBeamSpot = cms.bool(True),
	useLostCands = cms.bool(True)
)

MiniAODRefitVertexNoBSProducer = cms.EDProducer(
	"MiniAODRefitVertexProducer",
	srcCands = cms.InputTag("packedPFCandidates"),
	srcLostTracks = cms.InputTag("lostTracks"),
	PVTag = cms.InputTag("offlineSlimmedPrimaryVertices"),
	beamSpot = cms.InputTag("offlineBeamSpot"),
	useBeamSpot = cms.bool(False),
	useLostCands = cms.bool(True)
)

MiniAODRefitVertexBS = cms.Sequence(MiniAODRefitVertexBSProducer)
MiniAODRefitVertexNoBS = cms.Sequence(MiniAODRefitVertexNoBSProducer)
