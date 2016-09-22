import FWCore.ParameterSet.Config as cms

AdvancedRefitVertexBSProducer = cms.EDProducer(
	"AdvancedRefitVertexProducer",
	combineNLeptons = cms.int32(2),
	srcCands = cms.InputTag("packedPFCandidates"),
	srcLostTracks = cms.InputTag("lostTracks"),
	#TauTag = cms.InputTag("slimmedTaus"),
	PVTag = cms.InputTag("offlineSlimmedPrimaryVertices"),
	deltaRThreshold = cms.double(0.001),
	deltaPtThreshold = cms.double(0.001),

	#TrackCollectionTag = cms.InputTag("NonTauPairTrackCollectionProducer"),
	#PVTag = cms.InputTag("offlineSlimmedPrimaryVertices"),
	beamSpot = cms.InputTag("offlineBeamSpot"),
	useBeamSpot = cms.bool(True),
	useLostCands = cms.bool(True),

	srcElectrons = cms.InputTag("slimmedElectrons"),
	#electronTypeID = cms.InputTag("Tight"),
	srcMuons = cms.InputTag("slimmedMuons"),
	#muonTypeID = cms.InputTag("Tight"),
	srcTaus = cms.InputTag("slimmedTaus"),
	#tauTypeID = cms.InputTag("Loose"),

	#srcLeptons = cms.VInputTag(srcElectrons+electronTypeID, srcMuons+muonTypeID, srcTaus+tauTypeID+"Cleaned"),
	srcLeptons = cms.VInputTag(cms.InputTag("slimmedElectrons"), cms.InputTag("slimmedMuons"), cms.InputTag("slimmedTaus")),
)

AdvancedRefitVertexNoBSProducer = cms.EDProducer(
	"AdvancedRefitVertexProducer",
	combineNLeptons = cms.int32(2),
	srcCands = cms.InputTag("packedPFCandidates"),
	srcLostTracks = cms.InputTag("lostTracks"),
	#TauTag = cms.InputTag("slimmedTaus"),
	PVTag = cms.InputTag("offlineSlimmedPrimaryVertices"),
	deltaRThreshold = cms.double(0.001),
	deltaPtThreshold = cms.double(0.001),

	#TrackCollectionTag = cms.InputTag("NonTauPairTrackCollectionProducer"),
	#PVTag = cms.InputTag("offlineSlimmedPrimaryVertices"),
	beamSpot = cms.InputTag("offlineBeamSpot"),
	useBeamSpot = cms.bool(False),
	useLostCands = cms.bool(True),

	srcElectrons = cms.InputTag("slimmedElectrons"),
	#electronTypeID = cms.InputTag("Tight"),
	srcMuons = cms.InputTag("slimmedMuons"),
	#muonTypeID = cms.InputTag("Tight"),
	srcTaus = cms.InputTag("slimmedTaus"),
	#tauTypeID = cms.InputTag("Loose"),
	#srcLeptons = cms.VInputTag(srcElectrons+electronTypeID, srcMuons+muonTypeID, srcTaus+tauTypeID+"Cleaned"),
	srcLeptons = cms.VInputTag(cms.InputTag("slimmedElectrons"), cms.InputTag("slimmedMuons"), cms.InputTag("slimmedTaus")),
)

AdvancedRefitVertexBS = cms.Sequence(AdvancedRefitVertexBSProducer)
AdvancedRefitVertexNoBS = cms.Sequence(AdvancedRefitVertexNoBSProducer)
