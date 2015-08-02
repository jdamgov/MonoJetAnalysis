from WMCore.Configuration import Configuration

config = Configuration()
config.section_('General')
config.General.transferLogs = True
config.General.transferOutputs = True
config.General.requestName = 'data_Commissioning_Run2015A-PromptReco-v1'
config.General.workArea = 'data_Commissioning_Run2015A-PromptReco-v1'

config.section_('JobType')
config.JobType.psetName = 'miniAOD-prod_PAT.py'
config.JobType.pluginName = 'Analysis'
#config.JobType.pyCfgParams = ['global_tag=MCRUN2_74_V9::All', 'MC=True']
config.JobType.allowUndistributedCMSSW = True
#config.JobType.inputFiles = ['PHYS14_25_V2_All_L1FastJet_AK8PFchs.txt','PHYS14_25_V2_All_L2Relative_AK8PFchs.txt','PHYS14_25_V2_All_L3Absolute_AK8PFchs.txt','PHYS14_25_V2_All_L1FastJet_AK4PFchs.txt','PHYS14_25_V2_All_L2Relative_AK4PFchs.txt','PHYS14_25_V2_All_L3Absolute_AK4PFchs.txt' ]

config.section_('Data')
config.Data.inputDataset = '/Commissioning/Run2015A-PromptReco-v1/AOD'
config.Data.unitsPerJob = 50
config.Data.publication = True
config.Data.inputDBS = 'global' #'http://cmsdbsprod.cern.ch/cms_dbs_prod_global/servlet/DBSServlet'
config.Data.splitting = 'LumiBased'
config.Data.outLFNDirBase = '/store/group/dpg_ecal/alca_ecalcalib/ecalMIBI/lbrianza/miniAOD/data_Commissioning_Run2015A-PromptReco-v1/'

config.section_('User')

config.section_('Site')
config.Site.storageSite = 'T2_CH_CERN'
