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
    version = cms.untracked.string('$Revision: 1.38 $'),
    name = cms.untracked.string('$Source: /afs/cern.ch/project/cvs/reps/CMSSW/CMSSW/PhysicsTools/Configuration/test/SUSY_pattuple_cfg.py,v $'),
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
options.maxEvents = 100
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
     # "/store/mc/Summer11/ADDmonoJet_MD-2_d-4_7TeV_Tune4C-pythia8/AODSIM/PU_S4_START42_V11-v1/0000/8855A814-2EAF-E011-B388-0017A4770828.root"
    "/store/user/vergili/DM_AVd_mass300GeV_M40TeV_madgraph/DM_AVd_mass300GeV_M40TeV_madgraph/526d90c8425ce4e0f47563e547c8ceb5/darkmatter_AODSIM_183_1_RbT.root"  
     #"file:/uscms_data/d1/lpcjm/DATA/skim/QCD_300_470_MonoJet_remaining_1Event.root"
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

process.hltHighLevel.HLTPaths = cms.vstring( "HLT_CentralJet80_MET80*" ,
                                             "HLT_CentralJet80_MET95*" 
)


process.hltHighLevel.andOr = True   # True = OR, False = AND
process.hltHighLevel.throw = False  # tolerate if triggers not available




#-------------------------------------Ntuple Producer Pluggin--------------------------------------------------------------


process.load("MonoJetAnalysis.NtupleAnalyzer.NtupleAnalyzer_cfi")

process.TFileService  = cms.Service("TFileService", fileName = cms.string('ntuple.root') )


#-----------------------------Execution path ------------------------------------------------------------


process.pdfWeights1 = cms.EDProducer("PdfWeightProducer",
					# Fix POWHEG if buggy (this PDF set will also appear on output,
					# so only two more PDF sets can be added in PdfSetNames if not "")
					#FixPOWHEG = cms.untracked.string("cteq66.LHgrid"),
					#GenTag = cms.untracked.InputTag("genParticles"),
					PdfInfoTag = cms.untracked.InputTag("generator"),
					PdfSetNames = cms.untracked.vstring(
							   "cteq66.LHgrid" 
						)
)

process.pdfWeights2 = cms.EDProducer("PdfWeightProducer",
					# Fix POWHEG if buggy (this PDF set will also appear on output,
					# so only two more PDF sets can be added in PdfSetNames if not "")
					#FixPOWHEG = cms.untracked.string("cteq66.LHgrid"),
					#GenTag = cms.untracked.InputTag("genParticles"),
					PdfInfoTag = cms.untracked.InputTag("generator"),
					PdfSetNames = cms.untracked.vstring( 
							 "MSTW2008nlo68cl.LHgrid"
						)
)

process.pdfWeights3 = cms.EDProducer("PdfWeightProducer",
					# Fix POWHEG if buggy (this PDF set will also appear on output,
					# so only two more PDF sets can be added in PdfSetNames if not "")
					#FixPOWHEG = cms.untracked.string("cteq66.LHgrid"),
					#GenTag = cms.untracked.InputTag("genParticles"),
					PdfInfoTag = cms.untracked.InputTag("generator"),
					PdfSetNames = cms.untracked.vstring(
						       "NNPDF20_100.LHgrid"
						)
)


#-------------------------------------Pythia-MadGraph Fix--------------------------------------------------------------

process.totalKinematicsFilter = cms.EDFilter('TotalKinematicsFilter',
  src             = cms.InputTag("genParticles"),
  tolerance       = cms.double(0.5),
  verbose         = cms.untracked.bool(False)                                   
)



process.p = cms.Path(
		#process.hltPhysicsDeclared *
		#process.L1T1coll *
		#process.hltHighLevel *
		#process.totalKinematicsFilter *
		process.susyPatDefaultSequence*
		
		process.goodVertices *
		#process.noscraping *	
		#process.HBHENoiseFilter * 
		#process.eeNoiseFilter *
		#process.trackingFailureFilter *
		#process.beamHaloFilter *
		#process.ecalDeadCellTPfilter*
		
		process.pdfWeights1*
		#process.pdfWeights2*
		#process.pdfWeights3*


		process.NtupleAnalyzer

)



#-- Dump config ------------------------------------------------------------
file = open('MonoJetPAT_cfg.py','w')
file.write(str(process.dumpPython()))
file.close()

