import FWCore.ParameterSet.Config as cms

filteredMuons = cms.EDFilter("PATMuonSelector",
                             src = cms.InputTag("slimmedMuons"),
                             cut = cms.string("pt > 20 && " + "abs(eta) < 2.4" +
                                              " && isGlobalMuon"+
                                              " && (pfIsolationR04.sumChargedHadronPt+max(0.,(pfIsolationR04.sumNeutralHadronEt+pfIsolationR04.sumPhotonEt-0.5*pfIsolationR04.sumPUPt)))/pt < 0.3"
                                              )
                             )
filteredElectrons = cms.EDFilter("PATElectronSelector",
                                 src = cms.InputTag("slimmedElectrons"),
                                 cut = cms.string("pt > 20 && " + "abs(eta) < 2.4"+
                                                  " && (pfIsolationVariables.sumChargedHadronPt+max(0.,(pfIsolationVariables.sumNeutralHadronEt+pfIsolationVariables.sumPhotonEt-0.5*pfIsolationVariables.sumPUPt)))/pt < 0.3"
                                                  )
                                 )
filteredTaus = cms.EDFilter("PATTauSelector",
                            src = cms.InputTag("NewTauIDsEmbedded"),
                            cut = cms.string("tauID('decayModeFindingNewDMs') > 0.5" +
                                             " && tauID('byVLooseIsolationMVArun2017v2DBnewDMwLT2017') > 0.5"
                                             )
                            )

