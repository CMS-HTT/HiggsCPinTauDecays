import FWCore.ParameterSet.Config as cms

AdvancedRefitVertexBSProducer = cms.EDProducer(
	"AdvancedRefitVertexProducer",
	combineNLeptons = cms.int32(2),
	srcCands = cms.InputTag("packedPFCandidates"),
	srcLostTracks = cms.InputTag("lostTracks"),
	PVTag = cms.InputTag("offlineSlimmedPrimaryVertices"),
	deltaRThreshold = cms.double(0.001),
	deltaPtThreshold = cms.double(0.001),

	beamSpot = cms.InputTag("offlineBeamSpot"),
	useBeamSpot = cms.bool(True),
	useLostCands = cms.bool(True),

	srcElectrons = cms.InputTag("slimmedElectrons"),
	srcMuons = cms.InputTag("slimmedMuons"),
	srcTaus = cms.InputTag("slimmedTaus"),
	srcLeptons = cms.VInputTag(cms.InputTag("slimmedElectrons"), cms.InputTag("slimmedMuons"), cms.InputTag("slimmedTaus")),
)

AdvancedRefitVertexNoBSProducer = cms.EDProducer(
	"AdvancedRefitVertexProducer",
	combineNLeptons = cms.int32(2),
	srcCands = cms.InputTag("packedPFCandidates"),
	srcLostTracks = cms.InputTag("lostTracks"),
	PVTag = cms.InputTag("offlineSlimmedPrimaryVertices"),
	deltaRThreshold = cms.double(0.001),
	deltaPtThreshold = cms.double(0.001),

	beamSpot = cms.InputTag("offlineBeamSpot"),
	useBeamSpot = cms.bool(False),
	useLostCands = cms.bool(True),

	srcElectrons = cms.InputTag("slimmedElectrons"),
	srcMuons = cms.InputTag("slimmedMuons"),
	srcTaus = cms.InputTag("slimmedTaus"),
	srcLeptons = cms.VInputTag(cms.InputTag("slimmedElectrons"), cms.InputTag("slimmedMuons"), cms.InputTag("slimmedTaus")),
)

AdvancedRefitVertexBS = cms.Sequence(AdvancedRefitVertexBSProducer)
AdvancedRefitVertexNoBS = cms.Sequence(AdvancedRefitVertexNoBSProducer)
