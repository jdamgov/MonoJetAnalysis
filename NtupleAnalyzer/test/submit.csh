#!/bin/tcsh -f


echo $0  ' executing'
echo $#
set nparm = $#
if (! ($nparm == 2) ) then
  echo 'you did not enter the right number of arguments'
  echo ' this script requires 2 arguments:'
  echo ' 1. DataSet Name'
  echo ' 2. Resilient Directory '
  exit 1
endif



####################################################################################################################
cat > crab.cfg << +EOF
[CRAB]
jobtype                = cmssw
scheduler              = condor

[CMSSW]

datasetpath            = $1

dbs_url                = https://cmsdbsprod.cern.ch:8443/cms_dbs_ph_analysis_01_writer/servlet/DBSServlet
pset                   = MonoJetPAT_cfg.py


total_number_of_events = -1
number_of_jobs         = 200

output_file            = ntuple.root

[USER]
return_data            = 0
copy_data              = 1

storage_element        = cmssrm.fnal.gov
storage_path           = /srm/managerv2?SFN=/resilient/vergili
user_remote_dir        = $2
ui_working_dir         = $2



+EOF
####################################################################################################################


crab -create -submit 


echo
echo  ------------------------------------------------------
echo  $0 finished on `date`
echo  ------------------------------------------------------
echo


