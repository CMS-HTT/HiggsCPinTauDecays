import FWCore.ParameterSet.Config as cms

filteredMuons = cms.EDFilter("PATMuonSelector",
                             src = cms.InputTag("slimmedMuons"),
                             cut = cms.string("pt > 20 && " + "abs(eta) < 2.4")
                             )
filteredElectrons = cms.EDFilter("PATElectronSelector",
                                 src = cms.InputTag("slimmedElectrons"),
                                 cut = cms.string("pt > 20 && " + "abs(eta) < 2.4")
                                 )
filteredTaus = cms.EDFilter("PATTauSelector",
                            src = cms.InputTag("NewTauIDsEmbedded"),
                            cut = cms.string("tauID('decayModeFindingNewDMs') > 0.5" +
                                             " && (tauID('byVVLooseIsolationMVArun2017v2DBoldDMwLT2017') > 0.5 || tauID('byVVVLooseDeepTau2017v2p1VSjet') > 0.5)"
                                             )
                            )

from HiggsCPinTauDecays.TauRefit.AdvancedRefitVertexProducer_cfi import *
AdvancedRefitVertexBSProducer.srcLeptons = cms.VInputTag(cms.InputTag("filteredElectrons"), cms.InputTag("filteredMuons"), cms.InputTag("filteredTaus"))
AdvancedRefitVertexBSProducer.excludeFullyLeptonic = cms.untracked.bool(False)
AdvancedRefitVertexBSSequence = cms.Sequence(filteredMuons*filteredElectrons*filteredTaus*AdvancedRefitVertexBSProducer)
AdvancedRefitVertexNoBSProducer.srcLeptons = cms.VInputTag(cms.InputTag("filteredElectrons"), cms.InputTag("filteredMuons"), cms.InputTag("filteredTaus"))
AdvancedRefitVertexNoBSProducer.excludeFullyLeptonic = cms.untracked.bool(False)
AdvancedRefitVertexNoBSBSSequence = cms.Sequence(filteredMuons*filteredElectrons*filteredTaus*AdvancedRefitVertexNoBSProducer)
