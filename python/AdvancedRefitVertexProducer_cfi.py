import FWCore.ParameterSet.Config as cms

MVARefitVertexBSProducer = cms.EDProducer(
	"MVARefitVertexProducer",
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

MVARefitVertexNoBSProducer = cms.EDProducer(
	"MVARefitVertexProducer",
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

MVARefitVertexBS = cms.Sequence(MVARefitVertexBSProducer)
MVARefitVertexNoBS = cms.Sequence(MVARefitVertexNoBSProducer)
