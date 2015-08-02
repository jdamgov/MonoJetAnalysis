#
#  SUSY-PAT configuration file
#
#  PAT configuration for the SUSY group - 42X series
#  More information here:
#  https://twiki.cern.ch/twiki/bin/view/CMS/SusyPatLayer1DefV10
#

# Starting with a skeleton process which gets imported with the following line
from PhysicsTools.PatAlgos.patTemplate_cfg import *

#-- Meta data to be logged in DBS ---------------------------------------------
process.configurationMetadata = cms.untracked.PSet(
    version = cms.untracked.string('$Revision: 1.1 $'),
    name = cms.untracked.string('$Source: /cvs/CMSSW/UserCode/vergili/MonoJetAnalysis/NtupleAnalyzer/test/monojet_ntuple_cfg.py,v $'),
    annotation = cms.untracked.string('SUSY pattuple definition')
)

#-- Message Logger ------------------------------------------------------------
process.MessageLogger.categories.append('PATSummaryTables')
process.MessageLogger.cerr.PATSummaryTables = cms.untracked.PSet(
    limit = cms.untracked.int32(-1),
    reportEvery = cms.untracked.int32(1)
    )
process.MessageLogger.cerr.FwkReport.reportEvery = 100

#-- VarParsing ----------------------------------------------------------------
import FWCore.ParameterSet.VarParsing as VarParsing
options = VarParsing.VarParsing ('standard')

#options.output = "SUSYPAT.root"
		
options.maxEvents = 1000
#  for SusyPAT configuration
options.register('GlobalTag', "", VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.string, "GlobalTag to use (if empty default Pat GT is used)")
options.register('mcInfo', True, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.int, "process MonteCarlo data")
options.register('jetCorrections', 'L1FastJet', VarParsing.VarParsing.multiplicity.list, VarParsing.VarParsing.varType.string, "Level of jet corrections to use: Note the factors are read from DB via GlobalTag")
options.jetCorrections.append('L2Relative')
options.jetCorrections.append('L3Absolute')
options.register('hltName', 'HLT', VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.string, "HLT menu to use for trigger matching")
options.register('mcVersion', '', VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.string, "Currently not needed and supported")
options.register('jetTypes', '', VarParsing.VarParsing.multiplicity.list, VarParsing.VarParsing.varType.string, "Additional jet types that will be produced (AK5Calo and AK5PF, cross cleaned in PF2PAT, are included anyway)")
options.register('hltSelection', '', VarParsing.VarParsing.multiplicity.list, VarParsing.VarParsing.varType.string, "hlTriggers (OR) used to filter events")
options.register('doValidation', False, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.int, "Include the validation histograms from SusyDQM (needs extra tags)")
options.register('doExtensiveMatching', False, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.int, "Matching to simtracks (needs extra tags)")
options.register('doSusyTopProjection', False, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.int, "Apply Susy selection in PF2PAT to obtain lepton cleaned jets (needs validation)")
options.register('addKeep', '', VarParsing.VarParsing.multiplicity.list, VarParsing.VarParsing.varType.string, "Additional keep and drop statements to trim the event content")

#---parse user input
options.parseArguments()
options._tagOrder =[]

#-- Input Source --------------------------------------------------------------
if options.files:
    process.source.fileNames = cms.untracked.vstring (options.files)
process.source.fileNames = [
       "/store/data/Run2011A/MET/RECO/PromptReco-v6/000/172/822/C0A08434-8CC2-E011-ACEB-003048F11C5C.root"
    ]

process.maxEvents.input = options.maxEvents
# Due to problem in production of LM samples: same event number appears multiple times
process.source.duplicateCheckMode = cms.untracked.string('noDuplicateCheck')

#-- Calibration tag -----------------------------------------------------------
if options.GlobalTag:
    process.GlobalTag.globaltag = options.GlobalTag

############################# START SUSYPAT specifics ####################################
from PhysicsTools.Configuration.SUSY_pattuple_cff import addDefaultSUSYPAT, getSUSY_pattuple_outputCommands
#Apply SUSYPAT
addDefaultSUSYPAT(process,options.mcInfo,options.hltName,options.jetCorrections,options.mcVersion,options.jetTypes,options.doValidation,options.doExtensiveMatching,options.doSusyTopProjection)
SUSY_pattuple_outputCommands = getSUSY_pattuple_outputCommands( process )
############################## END SUSYPAT specifics ####################################

#-- HLT selection ------------------------------------------------------------
import HLTrigger.HLTfilters.hltHighLevel_cfi as hlt
if options.hltSelection:
    process.hltFilter = hlt.hltHighLevel.clone(
        HLTPaths = cms.vstring(options.hltSelection),
        TriggerResultsTag = cms.InputTag("TriggerResults","",options.hltName),
        throw = False
    )
    process.susyPatDefaultSequence.replace(process.eventCountProducer, process.eventCountProducer * process.hltFilter)

#-- Output module configuration -----------------------------------------------
process.out.fileName = options.output

# Custom settings
process.out.splitLevel = cms.untracked.int32(99)  # Turn on split level (smaller files???)
process.out.overrideInputFileSplitLevels = cms.untracked.bool(True)
process.out.dropMetaData = cms.untracked.string('DROPPED')   # Get rid of metadata related to dropped collections
process.out.outputCommands = cms.untracked.vstring('drop *', *SUSY_pattuple_outputCommands )
if options.hltSelection:
    process.out.SelectEvents = cms.untracked.PSet(SelectEvents = cms.vstring("p"))
if options.addKeep:
    process.out.outputCommands.extend(options.addKeep)



#---------------------------------------good Vertex filter------------------------------------------------------


process.goodVertices = cms.EDFilter("VertexSelector",
	filter = cms.bool(False),
	src = cms.InputTag("offlinePrimaryVertices"),
	cut = cms.string("!isFake && ndof > 4 && abs(z) <= 24 && position.rho < 2")
)

#---------------------------------------good collision filter------------------------------------------------------

process.load('L1TriggerConfig.L1GtConfigProducers.L1GtTriggerMaskAlgoTrigConfig_cff')
process.load('L1TriggerConfig.L1GtConfigProducers.L1GtTriggerMaskTechTrigConfig_cff')
process.load('HLTrigger/HLTfilters/hltLevel1GTSeed_cfi')
process.L1T1coll=process.hltLevel1GTSeed.clone()
process.L1T1coll.L1TechTriggerSeeding = cms.bool(True)
#process.L1T1coll.L1SeedsLogicalExpression = cms.string('0 AND (40 OR 41) AND NOT (36 OR 37 OR 38 OR 39) AND NOT ((42 AND NOT 43) OR (43 AND NOT 42))')

process.L1T1coll.L1SeedsLogicalExpression = cms.string('0 AND NOT (36 OR 37 OR 38 OR 39) ')



#---------------------------------------Physics Declared------------------------------------------------------------

process.load('HLTrigger.special.hltPhysicsDeclared_cfi')
process.hltPhysicsDeclared.L1GtReadoutRecordTag = 'gtDigis'



#-------------------------------------HLT Trigger Selection--------------------------------------------------------------

process.load('HLTrigger.HLTfilters.hltHighLevel_cfi')

process.hltHighLevel.HLTPaths = cms.vstring( "HLT_CentralJet80_MET80*" , "HLT_CentralJet80_MET95*")


process.hltHighLevel.andOr = True   # True = OR, False = AND
process.hltHighLevel.throw = False  # tolerate if triggers not available


#-------------------------------------Ntuple Producer Pluggin--------------------------------------------------------------


process.load("MonoJetAnalysis.NtupleAnalyzer.NtupleAnalyzer_cfi")

process.TFileService  = cms.Service("TFileService", fileName = cms.string('ntuple.root') )


#-----------------------------Execution path ------------------------------------------------------------


#--------------------------------------------------------------------------------
# parametrization of MET x/y shift vs. sumEt
process.pfMEtSysShiftCorrParameters_2011runAvsSumEt_data = cms.PSet(
    px = cms.string("-3.365e-1 + 4.801e-3*sumEt"),
    py = cms.string("+2.578e-1 - 6.124e-3*sumEt")
)

process.pfMEtSysShiftCorrParameters_2011runAvsSumEt_mc = cms.PSet(
    px = cms.string("-9.389e-2 + 1.815e-4*sumEt"),
    py = cms.string("+1.571e-1 - 3.710e-3*sumEt")
)

process.pfMEtSysShiftCorrParameters_2011runBvsSumEt_data = cms.PSet(
    px = cms.string("-3.265e-1 + 5.162e-3*sumEt"),
    py = cms.string("-1.956e-2 - 6.299e-3*sumEt")
)

process.pfMEtSysShiftCorrParameters_2011runBvsSumEt_mc = cms.PSet(
    px = cms.string("-1.070e-1 + 9.587e-5*sumEt"),
    py = cms.string("-1.517e-2 - 3.357e-3*sumEt")
)
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
# parametrization of MET x/y shift vs. Nvtx
process.pfMEtSysShiftCorrParameters_2011runAvsNvtx_data = cms.PSet(
    px = cms.string("+3.87339e-1 + 2.58294e-1*Nvtx"),
    py = cms.string("-7.83502e-1 - 2.88899e-1*Nvtx")
)

process.pfMEtSysShiftCorrParameters_2011runAvsNvtx_mc = cms.PSet(
    px = cms.string("-1.94451e-2 - 4.38986e-3*Nvtx"),
    py = cms.string("-4.31368e-1 - 1.90753e-1*Nvtx")
)

process.pfMEtSysShiftCorrParameters_2011runBvsNvtx_data = cms.PSet(
    px = cms.string("+6.64470e-1 + 2.71292e-1*Nvtx"),
    py = cms.string("-1.23999e0 - 3.18661e-1*Nvtx")
)

process.pfMEtSysShiftCorrParameters_2011runBvsNvtx_mc = cms.PSet(
    px = cms.string("-9.89706e-2 + 6.64796e-3*Nvtx"),
    py = cms.string("-5.32495e-1 - 1.82195e-1*Nvtx")
)

process.selectedVerticesForMEtCorr = cms.EDFilter("VertexSelector",
    src = cms.InputTag('offlinePrimaryVertices'),
    cut = cms.string("isValid & ndof >= 4 & chi2 > 0 & tracksSize > 0 & abs(z) < 24 & abs(position.Rho) < 2."),
    filter = cms.bool(False)                                          
)
#--------------------------------------------------------------------------------

process.pfMEtSysShiftCorr = cms.EDProducer("SysShiftMETcorrInputProducer",
    src = cms.InputTag('pfMet'), # "raw"/uncorrected PFMEt, needed to access sumEt
    srcVertices = cms.InputTag('selectedVerticesForMEtCorr'),                                   
    parameter = pfMEtSysShiftCorrParameters_2011runBvsNvtx_data
)                                     



process.p = cms.Path(
		process.hltPhysicsDeclared *
		process.L1T1coll *
		process.hltHighLevel *
		process.susyPatDefaultSequence*
		
		process.goodVertices *
		#process.noscraping *	
		#process.HBHENoiseFilter * 
		#process.eeNoiseFilter *
		#process.trackingFailureFilter *
		#process.beamHaloFilter *
		#process.ecalDeadCellTPfilter*

		selectedVerticesForMEtCorr * 
		pfMEtSysShiftCorr *
		process.NtupleAnalyzer

)




#-- Dump config ------------------------------------------------------------
file = open('MonoJetPAT_cfg.py','w')
file.write(str(process.dumpPython()))
file.close()

