# $Id: makeTreeFromPAT_cff.py,v 1.16 2013/01/24 15:42:53 mschrode Exp $
#

import FWCore.ParameterSet.Config as cms
import os

def makeTreeTreeFromMiniADO(process,
outFileName,
NJetsMin=2,
HTMin=350.,
MHTMin=0.,
reportEveryEvt=10,
testFileName="",
Global_Tag="",
MC=False,
debug = False,
QCD=False,
LostLepton=False,
numProcessedEvt=1000,
doAK8Reclustering=False,
doJECCorrection=False,
doPuppi=False,
leptonFilter=False):

    process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
    process.GlobalTag.globaltag = Global_Tag

    ## --- Log output ------------------------------------------------------
    process.load("FWCore.MessageService.MessageLogger_cfi")
    process.MessageLogger.cerr = cms.untracked.PSet(
        placeholder = cms.untracked.bool(True)
        )
    process.MessageLogger.cout = cms.untracked.PSet(
        INFO = cms.untracked.PSet(reportEvery = cms.untracked.int32(reportEveryEvt))
        )
    process.options = cms.untracked.PSet(
        wantSummary = cms.untracked.bool(True)
        ) 


    ## --- Files to process ------------------------------------------------
    process.maxEvents = cms.untracked.PSet(
        input = cms.untracked.int32(numProcessedEvt)
        )
    process.source = cms.Source("PoolSource",

        fileNames = cms.untracked.vstring(testFileName)
 #       fileNames = cms.untracked.vstring(
 #		'file:/nfs/dust/cms/user/csander/LHE/workdir/simulation_test/T1qqqqHV/output_66.root'
#		)
        )
        
    hltPath=['HLT_PFHT350_PFMET100_v*','HLT_PFNoPUHT350_PFMET100_v*']
    process.load('HLTrigger.HLTfilters.hltHighLevel_cfi')
    process.hltHighLevel.HLTPaths = cms.vstring(hltPath)
    process.hltHighLevel.andOr = cms.bool(True)
    process.hltHighLevel.throw = cms.bool(False)

    process.HLTSelection = cms.Sequence(
        process.hltHighLevel
        )
    if MC:
        print "Running on MC: removing HLT selection"
        process.HLTSelection.remove(process.hltHighLevel)
    elif not hltPath:
        print "Empty list of HLT paths: removing HLT selection"
        process.HLTSelection.remove(process.hltHighLevel)
    ## --- Output file -----------------------------------------------------
    process.TFileService = cms.Service(
        "TFileService",
        fileName = cms.string(outFileName+".root")
        )
	    
    ## --- Selection sequences ---------------------------------------------
    # leptons
    process.load("PhysicsTools.PatAlgos.selectionLayer1.muonCountFilter_cfi")
    process.load("PhysicsTools.PatAlgos.selectionLayer1.electronCountFilter_cfi")
    process.selectedIDIsoMuons = cms.EDFilter("CandPtrSelector", src = cms.InputTag("slimmedMuons"), cut = cms.string('''abs(eta)<2.5 && pt>10. &&
    (pfIsolationR04().sumChargedHadronPt+
    max(0.,pfIsolationR04().sumNeutralHadronEt+
    pfIsolationR04().sumPhotonEt-
    0.50*pfIsolationR04().sumPUPt))/pt < 0.20 &&
    (isPFMuon && (isGlobalMuon || isTrackerMuon) )'''))
    process.selectedIDMuons = cms.EDFilter("CandPtrSelector", src = cms.InputTag("slimmedMuons"), cut = cms.string('''abs(eta)<2.5 && pt>10. &&
    (isPFMuon && (isGlobalMuon || isTrackerMuon) )'''))
    process.selectedIDIsoElectrons = cms.EDFilter("CandPtrSelector", src = cms.InputTag("slimmedElectrons"), cut = cms.string('''abs(eta)<2.5 && pt>10. &&
    gsfTrack.isAvailable() &&
    gsfTrack.hitPattern().numberOfLostHits('MISSING_INNER_HITS')<2 &&
    (pfIsolationVariables().sumChargedHadronPt+
    max(0.,pfIsolationVariables().sumNeutralHadronEt+
    pfIsolationVariables().sumPhotonEt-
    0.5*pfIsolationVariables().sumPUPt))/pt < 0.20'''))
    process.selectedIDElectrons = cms.EDFilter("CandPtrSelector", src = cms.InputTag("slimmedElectrons"), cut = cms.string('''abs(eta)<2.5 && pt>10. &&
    gsfTrack.isAvailable() &&
    gsfTrack.hitPattern().numberOfLostHits('MISSING_INNER_HITS')<2'''))


###Lepton Filter
    process.filterSeq = cms.Sequence ()

#    from AllHadronicSUSY.Utils.leptonfilter_cfi import leptonFilter
    process.load('AllHadronicSUSY.Utils.leptonfilter_cfi')

    process.leptonFilter.electronsInputTag = cms.InputTag("slimmedElectrons")
    process.leptonFilter.muonsInputTag = cms.InputTag("slimmedMuons")
    process.leptonFilter.eleFilterPtCut = cms.double(20.0)
    process.leptonFilter.muFilterPtCut = cms.double(20.0)
    
    if (leptonFilter):
        process.filterSeq = cms.Sequence (process.leptonFilter)
    
       ## --- Setup of TreeMaker ----------------------------------------------
    FilterNames = cms.VInputTag()
    FilterNames.append(cms.InputTag("HBHENoiseFilterRA2","HBHENoiseFilterResult","PAT"))
    FilterNames.append(cms.InputTag("beamHaloFilter"))
    FilterNames.append(cms.InputTag("eeNoiseFilter"))
    FilterNames.append(cms.InputTag("trackingFailureFilter"))
    FilterNames.append(cms.InputTag("inconsistentMuons"))
    FilterNames.append(cms.InputTag("greedyMuons"))
    FilterNames.append(cms.InputTag("ra2EcalTPFilter"))
    FilterNames.append(cms.InputTag("ra2EcalBEFilter"))
    FilterNames.append(cms.InputTag("hcalLaserEventFilter"))
    FilterNames.append(cms.InputTag("ecalLaserCorrFilter"))
    FilterNames.append(cms.InputTag("eeBadScFilter"))
    FilterNames.append(cms.InputTag("PBNRFilter"))
    FilterNames.append(cms.InputTag("HCALLaserEvtFilterList2012"))
    FilterNames.append(cms.InputTag("manystripclus53X"))
    FilterNames.append(cms.InputTag("toomanystripclus53X"))
    FilterNames.append(cms.InputTag("logErrorTooManyClusters"))
    FilterNames.append(cms.InputTag("RA2HONoiseFilter"))

    
    ## --- Setup WeightProducer -------------------------------------------
    from AllHadronicSUSY.WeightProducer.getWeightProducer_cff import getWeightProducer
    process.WeightProducer = getWeightProducer(testFileName)
    process.WeightProducer.Lumi                       = cms.double(5000)
    process.WeightProducer.PU                         = cms.int32(0) # PU S10 3 for S10 2 for S7
    process.WeightProducer.FileNamePUDataDistribution = cms.string("NONE")
    print process.WeightProducer.PU

    from RecoBTag.Configuration.RecoBTag_cff import *
    from RecoJets.JetAssociationProducers.j2tParametersVX_cfi import *
    process.slimmedJetsPFJetTracksAssociatorAtVertex = cms.EDProducer("JetTracksAssociatorAtVertex",
      j2tParametersVX,
      jets = cms.InputTag("iterativeCone5PFJets")
    )
    process.slimmedJetsPFJetTracksAssociatorAtVertex.jets = "slimmedJets"
    process.slimmedJetsPFJetTracksAssociatorAtVertex.tracks = "generalTracks"
    
    process.slimmedJetsPFImpactParameterTagInfos = impactParameterTagInfos.clone()
    process.slimmedJetsPFImpactParameterTagInfos.jetTracks = "slimmedJetsPFJetTracksAssociatorAtVertex"
    process.slimmedJetsPFSecondaryVertexTagInfos = secondaryVertexTagInfos.clone()
    process.slimmedJetsPFSecondaryVertexTagInfos.trackIPTagInfos = "slimmedJetsPFImpactParameterTagInfos"
    #slimmedJetsPFSimpleSecondaryVertexBJetTags = simpleSecondaryVertexBJetTags.clone()
    #slimmedJetsPFSimpleSecondaryVertexBJetTags.tagInfos = cms.VInputTag( cms.InputTag("slimmedJetsPFSecondaryVertexTagInfos") )
    process.slimmedJetsPFCombinedSecondaryVertexBJetTags = combinedInclusiveSecondaryVertexV2BJetTags.clone()
 #   process.slimmedJetsPFStandardCombinedSecondaryVertex = combinedSecondaryVertex.clone()
 #   process.slimmedJetsPFCombinedSecondaryVertexBJetTags.jetTagComputer = cms.string('slimmedJetsPFStandardCombinedSecondaryVertex')
 #   process. slimmedJetsPFCombinedSecondaryVertexBJetTags.tagInfos = cms.VInputTag( cms.InputTag("slimmedJetsPFImpactParameterTagInfos"), cms.InputTag("slimmedJetsPFSecondaryVertexTagInfos") )
    
    process.slimmedJetsPFJetBtaggingSV = cms.Sequence(
    	process.slimmedJetsPFImpactParameterTagInfos *
    process.slimmedJetsPFSecondaryVertexTagInfos 
    # slimmedJetsPFStandardCombinedSecondaryVertex *
#    process.slimmedJetsPFCombinedSecondaryVertexBJetTags
    )
    process.slimmedJetsPFJetsBtag = cms.Sequence(
    process.slimmedJetsPFJetTracksAssociatorAtVertex *
    process.slimmedJetsPFJetBtaggingSV
    )
    
    ## isotrack producer
    from AllHadronicSUSY.Utils.trackIsolationMaker_cfi import trackIsolationFilter
    from AllHadronicSUSY.Utils.trackIsolationMaker_cfi import trackIsolationCounter
    ## default
    process.IsolatedTracks = trackIsolationFilter.clone(
      doTrkIsoVeto= False,
      vertexInputTag = cms.InputTag("offlineSlimmedPrimaryVertices"),
      pfCandidatesTag = cms.InputTag("packedPFCandidates"),
      dR_ConeSize         = cms.double(0.3),
      dz_CutValue         = cms.double(0.05),
      minPt_PFCandidate   = cms.double(15.0),
      isoCut              = cms.double(0.1),
      )
    #study
    process.IsolatedTracksPT10 = trackIsolationFilter.clone(
      doTrkIsoVeto= False,
      vertexInputTag = cms.InputTag("offlineSlimmedPrimaryVertices"),
      pfCandidatesTag = cms.InputTag("packedPFCandidates"),
      dR_ConeSize         = cms.double(0.3),
      dz_CutValue         = cms.double(0.05),
      minPt_PFCandidate   = cms.double(10.0),
      isoCut              = cms.double(0.1),
      )
    process.IsolatedTracksPT10IsoCut08 = trackIsolationFilter.clone(
      doTrkIsoVeto= False,
      vertexInputTag = cms.InputTag("offlineSlimmedPrimaryVertices"),
      pfCandidatesTag = cms.InputTag("packedPFCandidates"),
      dR_ConeSize         = cms.double(0.3),
      dz_CutValue         = cms.double(0.05),
      minPt_PFCandidate   = cms.double(10.0),
      isoCut              = cms.double(0.08),
      )
    process.IsolatedTracksPT10IsoCut12 = trackIsolationFilter.clone(
      doTrkIsoVeto= False,
      vertexInputTag = cms.InputTag("offlineSlimmedPrimaryVertices"),
      pfCandidatesTag = cms.InputTag("packedPFCandidates"),
      dR_ConeSize         = cms.double(0.3),
      dz_CutValue         = cms.double(0.05),
      minPt_PFCandidate   = cms.double(10.0),
      isoCut              = cms.double(0.12),
      )
    process.CountIsoTracks = trackIsolationCounter.clone(
      src = cms.InputTag("IsolatedTracks"),
      minNumber = 1,
      )

    jecLevelsAK8 = [
        "PHYS14_25_V2_All_L1FastJet_AK8PFchs.txt",
        "PHYS14_25_V2_All_L2Relative_AK8PFchs.txt",
        "PHYS14_25_V2_All_L3Absolute_AK8PFchs.txt"
        ]
    jecLevelsAK4 = [
#        "AAA/src/AllHadronicSUSY/PHYS14_25_V2_All_L1FastJet_AK4PFchs.txt",
        "PHYS14_25_V2_All_L1FastJet_AK4PFchs.txt",
        "PHYS14_25_V2_All_L2Relative_AK4PFchs.txt",
        "PHYS14_25_V2_All_L3Absolute_AK4PFchs.txt"
        ]
#    print cms.string(os.environ['CMSSW_BASE']+"/src/AllHadronicSUSY/PHYS14_25_V2_All_L1FastJet_AK8PFchs.txt")

    process.substructureSequence = cms.Sequence()
    process.softdrop_onMiniAOD = cms.Sequence()
    process.pruning_onMiniAOD = cms.Sequence()
    process.redoPatJets = cms.Sequence()
    process.puppi_onMiniAOD = cms.Sequence()
    process.redoPuppiJets = cms.Sequence()

    if (doAK8Reclustering):
        from RecoJets.Configuration.RecoPFJets_cff import ak4PFJetsCHS, ak8PFJetsCHS, ak8PFJetsCHSPruned, ak8PFJetsCHSSoftDrop, ak8PFJetsCHSPrunedMass, ak8PFJetsCHSSoftDropMass

        process.chs = cms.EDFilter("CandPtrSelector",
                               src = cms.InputTag('packedPFCandidates'),
                               cut = cms.string('fromPV')
                               )

        process.NjettinessAK8 = cms.EDProducer("NjettinessAdder",
                            src=cms.InputTag("ak8PFJetsCHS"),
                            Njets=cms.vuint32(1,2,3,4),          # compute 1-, 2-, 3-, 4- subjettiness
                            # variables for measure definition : 
                            measureDefinition = cms.uint32( 0 ), # CMS default is normalized measure
                            beta = cms.double(1.0),              # CMS default is 1
                            R0 = cms.double( 0.8 ),              # CMS default is jet cone size
                            Rcutoff = cms.double( -999.0),       # not used by default
                            # variables for axes definition :
                            axesDefinition = cms.uint32( 6 ),    # CMS default is 1-pass KT axes
                            nPass = cms.int32(-999),             # not used by default
                            akAxesR0 = cms.double(-999.0)        # not used by default
                            )

        process.ak4PFJetsCHS = ak4PFJetsCHS.clone(src = 'chs')
        process.ak8PFJetsCHS = ak8PFJetsCHS.clone( src = 'chs', jetPtMin = 100.0 )

        process.ak8PFJetsCHSPruned = ak8PFJetsCHSPruned.clone( src = 'chs', jetPtMin = 100.0 )
        process.ak8PFJetsCHSPrunedMass = ak8PFJetsCHSPrunedMass.clone()
        process.ak8PFJetsCHSSoftDrop = ak8PFJetsCHSSoftDrop.clone( src = 'chs', jetPtMin = 100.0 )
        process.ak8PFJetsCHSSoftDropMass = ak8PFJetsCHSSoftDropMass.clone()

        process.substructureSequence+=process.chs
        process.substructureSequence+=process.ak8PFJetsCHS
        process.substructureSequence+=process.NjettinessAK8

        process.softdrop_onMiniAOD += process.ak8PFJetsCHSSoftDrop + process.ak8PFJetsCHSSoftDropMass
        process.pruning_onMiniAOD += process.ak8PFJetsCHSPruned + process.ak8PFJetsCHSPrunedMass

        ####### Redo pat jets sequence ##########
        from ExoDiBosonResonances.EDBRJets.redoPatJets_cff import patJetCorrFactorsAK8, patJetsAK8, selectedPatJetsAK8

        # Redo pat jets from ak8PFJetsCHS

        process.patJetCorrFactorsAK8 = patJetCorrFactorsAK8.clone( src = 'ak8PFJetsCHS' )
        process.patJetsAK8 = patJetsAK8.clone( jetSource = 'ak8PFJetsCHS' )
        process.patJetsAK8.userData.userFloats.src = [ cms.InputTag("ak8PFJetsCHSPrunedMass"), cms.InputTag("ak8PFJetsCHSSoftDropMass"), cms.InputTag("NjettinessAK8:tau1"), cms.InputTag("NjettinessAK8:tau2"), cms.InputTag("NjettinessAK8:tau3")]
        process.patJetsAK8.jetCorrFactorsSource = cms.VInputTag( cms.InputTag("patJetCorrFactorsAK8") )
        process.selectedPatJetsAK8 = selectedPatJetsAK8.clone( cut = cms.string('pt > 20') )

        process.redoPatJets+=process.patJetCorrFactorsAK8
        process.redoPatJets+=process.patJetsAK8
        process.redoPatJets+=process.selectedPatJetsAK8

    if (doPuppi):

        from CommonTools.PileupAlgos.Puppi_cff import puppi
        from RecoJets.JetProducers.ak4PFJetsPuppi_cfi import ak4PFJetsPuppi

        process.ak8PFJetsPuppi = ak4PFJetsPuppi.clone( rParam = 0.8 )
        process.puppi = puppi.clone( candName = cms.InputTag('packedPFCandidates'),
                             vertexName = cms.InputTag('offlineSlimmedPrimaryVertices'))

        process.puppi_onMiniAOD = cms.Sequence(process.puppi + process.ak8PFJetsPuppi)

        from RecoJets.Configuration.RecoPFJets_cff import ak4PFJetsCHS, ak8PFJetsCHS, ak8PFJetsCHSPruned, ak8PFJetsCHSSoftDrop, ak8PFJetsCHSPrunedMass, ak8PFJetsCHSSoftDropMass

        process.NjettinessAK8 = cms.EDProducer("NjettinessAdder",
                            src=cms.InputTag("ak8PFJetsPuppi"),
                            Njets=cms.vuint32(1,2,3,4),          # compute 1-, 2-, 3-, 4- subjettiness
                            # variables for measure definition : 
                            measureDefinition = cms.uint32( 0 ), # CMS default is normalized measure
                            beta = cms.double(1.0),              # CMS default is 1
                            R0 = cms.double( 0.8 ),              # CMS default is jet cone size
                            Rcutoff = cms.double( -999.0),       # not used by default
                            # variables for axes definition :
                            axesDefinition = cms.uint32( 6 ),    # CMS default is 1-pass KT axes
                            nPass = cms.int32(-999),             # not used by default
                            akAxesR0 = cms.double(-999.0)        # not used by default
                            )

        process.ak4PFJetsPuppi = ak4PFJetsPuppi.clone(src = 'puppi')
        process.ak8PFJetsPuppi = process.ak8PFJetsPuppi.clone( src = 'puppi', jetPtMin = 100.0 )

        process.ak8PFJetsCHSPruned = ak8PFJetsCHSPruned.clone( src = 'puppi', jetPtMin = 100.0 )
        process.ak8PFJetsCHSPrunedMass = ak8PFJetsCHSPrunedMass.clone(    
            matched = cms.InputTag("ak8PFJetsCHSPruned"),
            src = cms.InputTag("ak8PFJetsPuppi")
            )
        process.ak8PFJetsCHSSoftDrop = ak8PFJetsCHSSoftDrop.clone( src = 'puppi', jetPtMin = 100.0 )
        process.ak8PFJetsCHSSoftDropMass = ak8PFJetsCHSSoftDropMass.clone(
            matched = cms.InputTag("ak8PFJetsCHSSoftDrop"),
            src = cms.InputTag("ak8PFJetsPuppi")
            )

        process.substructureSequence+=process.puppi
        process.substructureSequence+=process.ak8PFJetsPuppi
        process.substructureSequence+=process.NjettinessAK8

        process.softdrop_onMiniAOD += process.ak8PFJetsCHSSoftDrop + process.ak8PFJetsCHSSoftDropMass
        process.pruning_onMiniAOD += process.ak8PFJetsCHSPruned + process.ak8PFJetsCHSPrunedMass

        ####### Redo pat jets sequence ##########
        from ExoDiBosonResonances.EDBRJets.redoPatJets_cff import patJetCorrFactorsAK8, patJetsAK8, selectedPatJetsAK8

        # Redo pat jets from puppi AK8

        process.puppiJetCorrFactorsAK8 = patJetCorrFactorsAK8.clone( src = 'ak8PFJetsPuppi' )
        process.puppiJetsAK8 = patJetsAK8.clone( jetSource = 'ak8PFJetsPuppi' )
        process.puppiJetsAK8.userData.userFloats.src = [ cms.InputTag("ak8PFJetsCHSPrunedMass"), cms.InputTag("ak8PFJetsCHSSoftDropMass"), cms.InputTag("NjettinessAK8:tau1"), cms.InputTag("NjettinessAK8:tau2"), cms.InputTag("NjettinessAK8:tau3")]
        process.puppiJetsAK8.jetCorrFactorsSource = cms.VInputTag( cms.InputTag("puppiJetCorrFactorsAK8") )
        process.selectedPuppiJetsAK8 = selectedPatJetsAK8.clone( src = 'puppiJetsAK8', cut = cms.string('pt > 20') )

        process.redoPuppiJets+=process.puppiJetCorrFactorsAK8
        process.redoPuppiJets+=process.puppiJetsAK8
        process.redoPuppiJets+=process.selectedPuppiJetsAK8



######### A4PF-nonCHS jets ###########

    from RecoJets.JetProducers.ak4PFJets_cfi import ak4PFJets

    process.ak4PFJets = ak4PFJets.clone(src = "packedPFCandidates")

    from AllHadronicSUSY.Utils.ak4pfjets_cfi import patJetCorrFactorsAK4, patJetsAK4

    process.patJetCorrFactorsAK4 = patJetCorrFactorsAK4.clone( src = 'ak4PFJets' )
    process.patJetsAK4 = patJetsAK4.clone( jetSource = 'ak4PFJets' )

    #
    # Set up electron ID (VID framework)
    #
    from PhysicsTools.SelectorUtils.tools.vid_id_tools import *
    # turn on VID producer, indicate data format to be
    # DataFormat.AOD or DataFormat.MiniAOD, as appropriate
#    if useAOD == True :
#        dataFormat = DataFormat.AOD
#    else :
#        dataFormat = DataFormat.MiniAOD 
    dataFormat=DataFormat.MiniAOD
    switchOnVIDElectronIdProducer(process,dataFormat)

    process.egmGsfElectronIDSequence = cms.Sequence(process.egmGsfElectronIDs)

    # define which IDs we want to produce
    my_id_modules = ['RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_PHYS14_PU20bx25_V2_cff',
                     'RecoEgamma.ElectronIdentification.Identification.heepElectronID_HEEPV51_cff']
    #add them to the VID producer
    for idmod in my_id_modules:
        setupAllVIDIdsInModule(process,idmod,setupVIDElectronSelection)

    # Producers
    from AllHadronicSUSY.Utils.electron_cfi import electron
    process.Electrons = electron.clone(
        VertexTag = cms.InputTag("offlineSlimmedPrimaryVertices"),
        EleTag = cms.InputTag("slimmedElectrons"),
        MinPt = cms.double(-1),
        RhoTag = cms.InputTag("fixedGridRhoFastjetAll"),
        eleVetoIdMap = cms.InputTag("egmGsfElectronIDs:cutBasedElectronID-PHYS14-PU20bx25-V2-standalone-veto"),
        eleLooseIdMap = cms.InputTag("egmGsfElectronIDs:cutBasedElectronID-PHYS14-PU20bx25-V2-standalone-loose"),
        eleMediumIdMap = cms.InputTag("egmGsfElectronIDs:cutBasedElectronID-PHYS14-PU20bx25-V2-standalone-medium"),
        eleTightIdMap = cms.InputTag("egmGsfElectronIDs:cutBasedElectronID-PHYS14-PU20bx25-V2-standalone-tight"),
        eleHEEPIdMap = cms.InputTag("egmGsfElectronIDs:heepElectronID-HEEPV51")
    )
    from AllHadronicSUSY.Utils.muon_cfi import muon
    process.Muons = muon.clone(
        VertexTag = cms.InputTag("offlineSlimmedPrimaryVertices"),
        MuTag = cms.InputTag("slimmedMuons"),
        MinPt = cms.double(-1),
        RhoTag = cms.InputTag("fixedGridRhoFastjetAll")
    )
    from AllHadronicSUSY.Utils.subJetSelection_cfi import SubJetSelection
    process.HTJets = SubJetSelection.clone(
    JetTag  = cms.InputTag('slimmedJets'),
    MinPt								  = cms.double(50),
    MaxEta								  = cms.double(2.5),
    )
    from AllHadronicSUSY.Utils.htdouble_cfi import htdouble
    process.HT = htdouble.clone(
    JetTag  = cms.InputTag('HTJets'),
    )
    from AllHadronicSUSY.Utils.njetint_cfi import njetint
    process.NJets = njetint.clone(
    JetTag  = cms.InputTag('HTJets'),
    )
    from AllHadronicSUSY.Utils.btagint_cfi import btagint
    process.BTags = btagint.clone(
    JetTag  = cms.InputTag('HTJets'),
    BTagInputTag	        = cms.string('combinedInclusiveSecondaryVertexV2BJetTags'),
    BTagCutValue					= cms.double(0.679)
    )
    from AllHadronicSUSY.Utils.subJetSelection_cfi import SubJetSelection
    process.MHTJets = SubJetSelection.clone(
    JetTag  = cms.InputTag('slimmedJets'),
    MinPt								  = cms.double(30),
    MaxEta								  = cms.double(5.0),
    )
    process.MHTJetsAK8 = SubJetSelection.clone(
    JetTag  = cms.InputTag('slimmedJetsAK8'),
    MinPt								  = cms.double(30),
    MaxEta								  = cms.double(5.0),
    )
    from AllHadronicSUSY.Utils.jetproperties_cfi import jetproperties
    process.MHTJetsProperties = jetproperties.clone(
    JetTag  = cms.InputTag('MHTJets'),
    doJEC  = cms.bool(doJECCorrection),
    L1File = cms.string("PHYS14_25_V2_All_L1FastJet_AK4PFchs.txt"),
    L2File = cms.string("PHYS14_25_V2_All_L2Relative_AK4PFchs.txt"),
    L3File = cms.string("PHYS14_25_V2_All_L3Absolute_AK4PFchs.txt"),
#    jecPayloadNames      = cms.vstring(jecLevelsAK4),
#    BTagInputTag	        = cms.string('combinedInclusiveSecondaryVertexV2BJetTags'),
    )
    from AllHadronicSUSY.Utils.jetpropertiesAK8_cfi import jetpropertiesAK8
    process.MHTJetsPropertiesAK8 = jetpropertiesAK8.clone(
    JetTag  = cms.InputTag('MHTJetsAK8'),
    puppiJetTag = cms.InputTag('selectedPuppiJetsAK8'),
    doJEC  = cms.bool(doJECCorrection),
    L1File = cms.string("PHYS14_25_V2_All_L1FastJet_AK8PFchs.txt"),
    L2File = cms.string("PHYS14_25_V2_All_L2Relative_AK8PFchs.txt"),
    L3File = cms.string("PHYS14_25_V2_All_L3Absolute_AK8PFchs.txt"),
#    jecPayloadNames      = cms.vstring(jecLevelsAK8),
#    BTagInputTag	        = cms.string('combinedInclusiveSecondaryVertexV2BJetTags'),
    )
    from AllHadronicSUSY.Utils.jetproperties_cfi import jetproperties
    process.JetsProperties = jetproperties.clone(
    JetTag  = cms.InputTag('slimmedJets'),
    MinPt = cms.double(-1),
    doJEC  = cms.bool(doJECCorrection),
    L1File = cms.string("PHYS14_25_V2_All_L1FastJet_AK4PFchs.txt"),
    L2File = cms.string("PHYS14_25_V2_All_L2Relative_AK4PFchs.txt"),
    L3File = cms.string("PHYS14_25_V2_All_L3Absolute_AK4PFchs.txt"),
#    jecPayloadNames      = cms.vstring(jecLevelsAK4),
#    BTagInputTag	        = cms.string('combinedInclusiveSecondaryVertexV2BJetTags'),
    )
    from AllHadronicSUSY.Utils.jetpropertiesAK8_cfi import jetpropertiesAK8
    process.JetsPropertiesAK8 = jetpropertiesAK8.clone(
    JetTag  = cms.InputTag('slimmedJetsAK8'),
#    puppiJetTag = cms.InputTag('selectedPuppiJetsAK8'),
    MinPt = cms.double(-1),
    doJEC  = cms.bool(doJECCorrection),
    L1File = cms.string("PHYS14_25_V2_All_L1FastJet_AK8PFchs.txt"),
    L2File = cms.string("PHYS14_25_V2_All_L2Relative_AK8PFchs.txt"),
    L3File = cms.string("PHYS14_25_V2_All_L3Absolute_AK8PFchs.txt"),
#    jecPayloadNames      = cms.vstring(jecLevelsAK8),
#    BTagInputTag	        = cms.string('combinedInclusiveSecondaryVertexV2BJetTags'),
    )
    if doAK8Reclustering:
        process.JetsPropertiesAK8.JetTag = cms.InputTag('selectedPatJetsAK8')
    if doPuppi:
        process.JetsPropertiesAK8.JetTag = cms.InputTag('selectedPuppiJetsAK8')
    from AllHadronicSUSY.Utils.mhtdouble_cfi import mhtdouble
    process.MHT = mhtdouble.clone(
    JetTag  = cms.InputTag('MHTJets'),
    )
    from AllHadronicSUSY.Utils.deltaphidouble_cfi import deltaphidouble
    process.DeltaPhi = deltaphidouble.clone(
    DeltaPhiJets  = cms.InputTag('HTJets'),
    MHTJets  = cms.InputTag("MHTJets"),
    )
    from AllHadronicSUSY.Utils.metdouble_cfi import metdouble
    process.MET = metdouble.clone(
    METTag  = cms.InputTag("slimmedMETs"),
    JetTag  = cms.InputTag('slimmedJets'),
    doJEC  = cms.bool(doJECCorrection),
    L1File = cms.string("PHYS14_25_V2_All_L1FastJet_AK4PFchs.txt"),
    L2File = cms.string("PHYS14_25_V2_All_L2Relative_AK4PFchs.txt"),
    L3File = cms.string("PHYS14_25_V2_All_L3Absolute_AK4PFchs.txt"),
#    jecPayloadNames      = cms.vstring(jecLevelsAK4),
    MuTag = cms.InputTag("slimmedMuons"),
    RhoTag = cms.InputTag("fixedGridRhoFastjetAll"),
    corrMet = cms.bool(True),
    )
    from AllHadronicSUSY.Utils.leptonint_cfi import leptonint
    process.Leptons = leptonint.clone(
    LeptonTag = cms.VInputTag(cms.InputTag('selectedIDIsoMuons'),cms.InputTag('selectedIDIsoElectrons')),
    )
    from AllHadronicSUSY.Utils.primaryverticies_cfi import primaryverticies
    process.NVtx = primaryverticies.clone(
    VertexCollection  = cms.InputTag('offlineSlimmedPrimaryVertices'),
    )
    from AllHadronicSUSY.Utils.genLeptonRecoCand_cfi import genLeptonRecoCand
    process.GenLeptons = genLeptonRecoCand.clone(
    PrunedGenParticleTag  = cms.InputTag("prunedGenParticles"),
    )
    if not MC:
        process.GenLeptons = cms.Sequence()
    RecoCandVector = cms.vstring()
    RecoCandVector.extend(['IsolatedTracks']) # basic muons electrons and isoalted tracks
#    RecoCandVector.extend(['selectedIDIsoMuons','selectedIDIsoElectrons','IsolatedTracks']) # basic muons electrons and isoalted tracks
#    RecoCandVector.extend(['selectedIDMuons','selectedIDElectrons']) # mu and e no isolation cuts
    RecoCandVector.extend(['GenLeptons:Boson(GenBoson)|GenLeptons:BosonPDGId(I_GenBosonPDGId)','GenLeptons:Muon(GenMu)|GenLeptons:MuonTauDecay(I_GenMuFromTau)' ,'GenLeptons:Electron(GenElec)|GenLeptons:ElectronTauDecay(I_GenElecFromTau)','GenLeptons:Tau(GenTau)|GenLeptons:TauHadronic(I_GenTauHad)','GenLeptons:Neutrino(GenNu)'] ) # gen information on leptons
    RecoCandVector.extend(['JetsProperties(Jets)|JetsProperties:bDiscriminatorCSV(F_bDiscriminatorCSV)|JetsProperties:bDiscriminatorICSV(F_bDiscriminatorICSV)|JetsProperties:chargedEmEnergyFraction(F_chargedEmEnergyFraction)|JetsProperties:chargedHadronEnergyFraction(F_chargedHadronEnergyFraction)|JetsProperties:chargedHadronMultiplicity(I_chargedHadronMultiplicity)|JetsProperties:electronMultiplicity(I_electronMultiplicity)|JetsProperties:jetArea(F_jetArea)|JetsProperties:muonEnergyFraction(F_muonEnergyFraction)|JetsProperties:muonMultiplicity(I_muonMultiplicity)|JetsProperties:neutralEmEnergyFraction(F_neutralEmEnergyFraction)|JetsProperties:neutralHadronMultiplicity(I_neutralHadronMultiplicity)|JetsProperties:photonEnergyFraction(F_photonEnergyFraction)|JetsProperties:photonMultiplicity(I_photonMultiplicity)|JetsProperties:isLooseJetId(b_isLooseJetId)|JetsProperties:PtCorr(F_PtCorr)|JetsProperties:EtaCorr(F_EtaCorr)|JetsProperties:PhiCorr(F_PhiCorr)|JetsProperties:ECorr(F_ECorr)'] ) # jet information on various variables
    RecoCandVector.extend(['JetsPropertiesAK8(AK8Jets)|JetsPropertiesAK8:AK8bDiscriminatorCSV(F_bDiscriminatorCSV)|JetsPropertiesAK8:AK8bDiscriminatorICSV(F_bDiscriminatorICSV)|JetsPropertiesAK8:AK8chargedEmEnergyFraction(F_chargedEmEnergyFraction)|JetsPropertiesAK8:AK8chargedHadronEnergyFraction(F_chargedHadronEnergyFraction)|JetsPropertiesAK8:AK8chargedHadronMultiplicity(I_chargedHadronMultiplicity)|JetsPropertiesAK8:AK8electronMultiplicity(I_electronMultiplicity)|JetsPropertiesAK8:AK8jetArea(F_jetArea)|JetsPropertiesAK8:AK8muonEnergyFraction(F_muonEnergyFraction)|JetsPropertiesAK8:AK8muonMultiplicity(I_muonMultiplicity)|JetsPropertiesAK8:AK8neutralEmEnergyFraction(F_neutralEmEnergyFraction)|JetsPropertiesAK8:AK8neutralHadronMultiplicity(I_neutralHadronMultiplicity)|JetsPropertiesAK8:AK8photonEnergyFraction(F_photonEnergyFraction)|JetsPropertiesAK8:AK8photonMultiplicity(I_photonMultiplicity)|JetsPropertiesAK8:AK8prunedMass(F_prunedMass)|JetsPropertiesAK8:AK8softDropMass(F_softDropMass)|JetsPropertiesAK8:AK8trimmedMass(F_trimmedMass)|JetsPropertiesAK8:AK8filteredMass(F_filteredMass)|JetsPropertiesAK8:AK8tau1(F_tau1)|JetsPropertiesAK8:AK8tau2(F_tau2)|JetsPropertiesAK8:AK8tau3(F_tau3)|JetsPropertiesAK8:AK8isLooseJetId(b_AK8isLooseJetId)|JetsPropertiesAK8:PtCorr(F_PtCorr)|JetsPropertiesAK8:EtaCorr(F_EtaCorr)|JetsPropertiesAK8:PhiCorr(F_PhiCorr)|JetsPropertiesAK8:ECorr(F_ECorr)'] ) # AK8 jet information on various variables
# 
    RecoCandVector.extend(['Electrons(Electrons)|Electrons:charge(I_charge)|Electrons:isHEEP(b_isHEEP)|Electrons:type(I_type)|Electrons:mass(F_mass)|Electrons:pfDeltaCorrRelIso(F_pfDeltaCorrRelIso)|Electrons:pfRhoCorrRelIso04(F_pfRhoCorrRelIso04)|Electrons:pfRhoCorrRelIso03(F_pfRhoCorrRelIso03)|Electrons:pfRelIso(F_pfRelIso)|Electrons:photonIso(F_photonIso)|Electrons:neutralHadIso(F_neutralHadIso)|Electrons:chargedHadIso(F_chargedHadIso)|Electrons:trackIso(F_trackIso)|Electrons:isLoose(b_isLoose)'] ) # electron information on various variables
    RecoCandVector.extend(['Muons(Muons)|Muons:charge(I_charge)|Muons:isHighPt(b_isHighPt)|Muons:type(I_type)|Muons:mass(F_mass)|Muons:pfDeltaCorrRelIso(F_pfDeltaCorrRelIso)|Muons:pfRelIso(F_pfRelIso)|Muons:photonIso(F_photonIso)|Muons:neutralHadIso(F_neutralHadIso)|Muons:chargedHadIso(F_chargedHadIso)|Muons:trackIso(F_trackIso)|Muons:isLoose(b_isLoose)|Muons:isPFMuon(b_isPFMuon)|'] ) # muon information on various variables
    
    from AllHadronicSUSY.TreeMaker.treeMaker import TreeMaker
    process.TreeMaker2 = TreeMaker.clone(
    	TreeName          = cms.string("PreSelection"),
    	VarsRecoCand = RecoCandVector,
    	#VarsRecoCand = cms.vstring('selectedIDIsoMuons','selectedIDIsoElectrons','IsolatedTracks','HTJets'),
    	VarsDouble  	  = cms.vstring('WeightProducer:weight(Weight)','MHT','MET:Pt(METPt)','MET:Phi(METPhi)','MET:PtRaw(METPtRaw)','MET:PhiRaw(METPhiRaw)','HT','DeltaPhi:DeltaPhi1(DeltaPhi1)','DeltaPhi:DeltaPhi2(DeltaPhi2)','DeltaPhi:DeltaPhi3(DeltaPhi3)'),
    	VarsInt = cms.vstring('NJets','BTags','NVtx'),#,'Leptons'),
    #	VarsDoubleNamesInTree = cms.vstring('WeightProducer'),
    debug = debug,
    	)

    ## --- Final paths ----------------------------------------------------
    process.out = cms.OutputModule("PoolOutputModule",
                                   fileName = cms.untracked.string("output.root"),
                                   )

    process.dump = cms.EDAnalyzer("EventContentAnalyzer")
    process.WriteTree = cms.Path(
        process.filterSeq *
        process.Muons *
        process.egmGsfElectronIDSequence*
        process.Electrons *
#    	process.selectedIDIsoMuons *
#    	process.selectedIDMuons *
#    	process.selectedIDIsoElectrons *
#    	process.selectedIDElectrons *
    	process.WeightProducer *
    	process.IsolatedTracks *
 #   	process.IsolatedTracksPT10 *
 #   	process.IsolatedTracksPT10IsoCut08 *
 #   	process.IsolatedTracksPT10IsoCut12 *
  #  	process.slimmedJetsPFCombinedSecondaryVertexBJetTags *
        process.puppi_onMiniAOD *
        process.substructureSequence *
        process.softdrop_onMiniAOD *
        process.pruning_onMiniAOD *
        process.redoPatJets*
        process.redoPuppiJets*
      process.HTJets *
      process.HT *
      process.NJets *
      process.BTags *
#      process.ak4PFJets *
#      process.patJetCorrFactorsAK4 *
#      process.patJetsAK4 *
      process.MHTJets *
      process.MHTJetsProperties *
      process.JetsProperties *
      process.MHTJetsAK8 *
      process.MHTJetsPropertiesAK8 *
      process.JetsPropertiesAK8 *
      process.MHT *
#      process.Leptons *
      process.MET *
      process.DeltaPhi *
      process.NVtx *
      process.GenLeptons *
    	#process.dump *
 #   	process.CountIsoTracks *
 #   	process.PrintDecay *
    	process.TreeMaker2

        )

#    process.outPath = cms.EndPath(process.out)
