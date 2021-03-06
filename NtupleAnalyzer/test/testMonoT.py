############################# Monojet setting #############################
# Data / (Signal) MC (Fast/Full)
iRunData       = False  # True or False
iFullSim       = True  # No meaning if iRunData = True
iSignal        = False  # True or False, if True, PDF will be collected. No meaning if iRunData = True

# Choose the proper GT
# (No meaning if iRunData = False)
# 
iData          = "MMMMM" 

# Trigger
# MET, InvHiggs, SingleMuon, DoubleMuon, SingleElectron, DoubleElectron, Physics, NoTrig, Parked
# (If isSignal, trigger will be set to NoTrig automatically)
iTrigger       = "NoTrig"
iZmumu         = False
iZnunu         = False

# DebugMode
iDebugMode     = False

# Sample files
iFileData      = '/store/user/srimanob/invHiggs/22Jan13/0008F234-739C-E211-A836-002590832A48.root'

iFileMCFast    = '/store/group/phys_higgs/vbfHinv/DataSample/FastSim-stop-5314.root'

# running this right now
iFileMCFull    =  'file:testMiniAOD.root'

iMaxEvent      = -1

# NonPF collection
iIncludeNonPF  = False
iElectronTag   = "selectedPatElectrons"
iMuonTag       = "selectedPatMuons"
iTauTag        = "selectedPatTaus"
iJetTag        = "selectedPatJets"
iMETTag        = "patMETs"

# PF collection
iPFElectronTag = "slimmedElectrons"
iPFMuonTag     = "slimmedMuons"
iPFTauTag      = "slimmedTaus"
iPFJetTag      = "slimmedJets"
iPFJetAK8Tag      = "slimmedJetsAK8"
iPFMETTag      = "slimmedMETs"

usePFnoPU      = True
useTopProj     = True
METThreshold   = 0
############################# Monojet setting ############################


##-------------------------------------------------
##-- Load template
import FWCore.ParameterSet.Config as cms
process = cms.Process("Demo")

##-------------------------------------------------
##-- Data/MC sample, maxEvent
process.load("FWCore.MessageService.MessageLogger_cfi")
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(iMaxEvent) )
process.source = cms.Source("PoolSource", fileNames = cms.untracked.vstring('file:template.root'))
if iRunData == True:
    process.source.fileNames  = [iFileData]
    iFile = iFileData
else:
    if iFullSim == True:
        process.source.fileNames  = [iFileMCFull]
        iFile = iFileMCFull
    else:
        process.source.fileNames  = [iFileMCFast]
        iFile = iFileMCFast


##-------------------------------------------------
##-- Global Tag
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
if iRunData == True:
    if (iData.find("22Jan13")==0):
        process.GlobalTag = GlobalTag(process.GlobalTag, 'MCRUN2_74_V9A', '')
    else:
        process.GlobalTag = GlobalTag(process.GlobalTag, 'MCRUN2_74_V9A', '')
else:
    process.GlobalTag = GlobalTag(process.GlobalTag, 'MCRUN2_74_V9A', '')


##-------------------------------------------------
##-- Trigger


##-------------------------------------------------
##-- Physics object selection


##-------------------------------------------------
##-- NtupleAnalyzer
process.load("MonoJetAnalysis.NtupleAnalyzer.NtupleAnalyzer_cfi")
process.TFileService                          = cms.Service("TFileService", fileName = cms.string('ntuple.root'))
process.NtupleAnalyzer                        = process.NtupleAnalyzerTemplate.clone()
process.NtupleAnalyzer.debugMode              = iDebugMode
process.NtupleAnalyzer.includeNonPFCollection = iIncludeNonPF
process.NtupleAnalyzer.TriggerTag             = cms.untracked.InputTag("TriggerResults","","HLT")
process.NtupleAnalyzer.Tracks                 = cms.untracked.InputTag("generalTracks")
# PF collection
process.NtupleAnalyzer.PFElectronTag          = cms.untracked.InputTag(iPFElectronTag)
process.NtupleAnalyzer.PFMuonTag              = cms.untracked.InputTag(iPFMuonTag)
process.NtupleAnalyzer.PFTauTag               = cms.untracked.InputTag(iPFTauTag)
process.NtupleAnalyzer.PFJetTag               = cms.untracked.InputTag(iPFJetTag)
process.NtupleAnalyzer.PFJetAK8Tag            = cms.untracked.InputTag(iPFJetAK8Tag)
process.NtupleAnalyzer.PFMETTag               = cms.untracked.InputTag(iPFMETTag)
# NonPF collection
#process.NtupleAnalyzer.ElectronTag            = cms.untracked.InputTag(iElectronTag)
#process.NtupleAnalyzer.MuonTag                = cms.untracked.InputTag(iMuonTag)
#process.NtupleAnalyzer.TauTag                 = cms.untracked.InputTag(iTauTag)
#process.NtupleAnalyzer.JetTag                 = cms.untracked.InputTag(iJetTag)
#process.NtupleAnalyzer.METTag                 = cms.untracked.InputTag(iMETTag)
#
#process.NtupleAnalyzer.puJetIdTag             = cms.untracked.InputTag("puJetMva","full53xId")
#if usePFnoPU:
#     process.NtupleAnalyzer.puJetIdTag        = cms.untracked.InputTag("puJetMva","fullId")
#
process.NtupleAnalyzer.jetPtCut               = cms.double(0)
process.NtupleAnalyzer.jetEtaCut              = cms.double(5.5)
process.NtupleAnalyzer.weight                 = cms.double(1)
process.NtupleAnalyzer.METThreshold           = cms.double(METThreshold)

# Data/MC-Bkg/MC-Signal
if iRunData == True:
    process.NtupleAnalyzer.isMC     = cms.bool(False)
    process.NtupleAnalyzer.isSignal = cms.double(0)
else:
    process.NtupleAnalyzer.isMC     = cms.bool(True)
    process.NtupleAnalyzer.chooseOnlyGenZmumu = cms.bool(iZmumu)
    process.NtupleAnalyzer.chooseOnlyGenZnunu = cms.bool(iZnunu)
    if iSignal == True:
        process.NtupleAnalyzer.isSignal = cms.double(1)
    else:
        process.NtupleAnalyzer.isSignal = cms.double(0)
#

# Singal run with NoTrig
if iSignal == True and iRunData == False:
    process.NtupleAnalyzer.triggerUsed = cms.double(99)


##-------------------------------------------------
##-- PDFWeight
process.pdfWeights1 = cms.EDProducer(
    "PdfWeightProducer",
    # Fix POWHEG if buggy (this PDF set will also appear on output,
    # so only two more PDF sets can be added in PdfSetNames if not "")
    #FixPOWHEG = cms.untracked.string("cteq66.LHgrid"),
    #GenTag = cms.untracked.InputTag("genParticles"),
    PdfInfoTag = cms.untracked.InputTag("generator"),
    PdfSetNames = cms.untracked.vstring(
    "cteq66.LHgrid"
    )
)
process.pdfWeights2 = cms.EDProducer(
    "PdfWeightProducer",
    # Fix POWHEG if buggy (this PDF set will also appear on output,
    # so only two more PDF sets can be added in PdfSetNames if not "")
    #FixPOWHEG = cms.untracked.string("cteq66.LHgrid"),
    #GenTag = cms.untracked.InputTag("genParticles"),
    PdfInfoTag = cms.untracked.InputTag("generator"),
    PdfSetNames = cms.untracked.vstring( 
    "MSTW2008nlo68cl.LHgrid"
    )
)
process.pdfWeights3 = cms.EDProducer(
    "PdfWeightProducer",
    # Fix POWHEG if buggy (this PDF set will also appear on output,
    # so only two more PDF sets can be added in PdfSetNames if not "")
    #FixPOWHEG = cms.untracked.string("cteq66.LHgrid"),
    #GenTag = cms.untracked.InputTag("genParticles"),
    PdfInfoTag = cms.untracked.InputTag("generator"),
    PdfSetNames = cms.untracked.vstring(
    "NNPDF20_100.LHgrid"
    )
)


##-------------------------------------------------
##-- Let it run
if iRunData == True:
    iFullSim = True



process.p   = cms.Path(
#    process.primaryVertexFilter
#    + process.goodVertices
#    + process.type0PFMEtCorrection
#    + process.producePFMETCorrections
#    + process.recoTauClassicHPSSequence
#    + getattr(process,"patPF2PATSequence"+postfix)
#    + process.patDefaultSequence
#    + process.PhysicsObjectSequence
#    + process.puJetId + process.puJetMva
)
process.ntp = cms.EndPath(process.NtupleAnalyzer)

if iRunData == False and iSignal == True:
    process.ntp.replace(process.NtupleAnalyzer,process.pdfWeights1*process.NtupleAnalyzer)


##-------------------------------------------------
##-- Output


##-------------------------------------------------
##-- Options
#process.options.wantSummary = True
#process.MessageLogger.categories.append('PATSummaryTables')
#process.MessageLogger.cerr.PATSummaryTables = cms.untracked.PSet(
#    limit = cms.untracked.int32(-1),
#    reportEvery = cms.untracked.int32(1)
#    )
#process.MessageLogger.cerr.FwkReport.reportEvery = 1000


##-------------------------------------------------
##-- Dump config
iFileName = "Monojet_"
iFileName += iTrigger
if iRunData == True:
    iFileName += "_DATA_"
    iFileName += iData
    
else:
    if iSignal == True:
        iFileName += "_MCSignal"
    else:
        iFileName += "_MC_"
    
    if iFullSim == True:
        iFileName += "Full"
    else:
        iFileName += "Fast"

if usePFnoPU:
    iFileName += "_useCHS"
else:
    iFileName += "_noCHS"

if useTopProj:
    iFileName += "_useTopP"
else:
    iFileName += "_noTopP"

if iZmumu:
    iFileName += "_GenZmumu"
if iZnunu:
    iFileName += "_GenZnunu"
iFileName += "_cfg.py"
file = open(iFileName,'w')
file.write(str(process.dumpPython()))
file.close()
