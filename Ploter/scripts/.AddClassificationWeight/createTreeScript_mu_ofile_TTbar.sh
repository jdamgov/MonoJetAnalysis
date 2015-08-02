#!/bin/bash
cd /afs/cern.ch/work/r/rgerosa/CMSSW_5_3_3_patch3/src/
eval `scram runtime -sh`
cd /afs/cern.ch/work/r/rgerosa/MyAnalysisVBF/VBFHWWlnuJ
./bin/VBFApplyMVAWeight.exe scripts/AddClassificationWeight/VBFApplyMVAWeight_BulkGraviton_ofile_TTbar.cfg