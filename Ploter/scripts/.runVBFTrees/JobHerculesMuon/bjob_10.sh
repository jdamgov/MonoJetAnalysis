#!/bin/sh
cd /afs/cern.ch/user/r/rgerosa/work/MyAnalysisVBF/VBFHWWlnuJ/scripts/runVBFTrees/
export SCRAM_ARCH=slc5_amd64_gcc462
cd /afs/cern.ch/work/r/rgerosa/CMSSW_5_3_3_patch3/src/ ; eval `scramv1 runtime -sh` ; cd -
source ../setup.sh
unbuffer .//afs/cern.ch/user/r/rgerosa/work/MyAnalysisVBF/VBFHWWlnuJ/bin/VBFNtupleFromRDTree.exe JobHerculesMuon/cfg_10.cfg >> /afs/cern.ch/user/r/rgerosa/work/MyAnalysisVBF/VBFHWWlnuJ/scripts/runVBFTrees//JobHerculesMuon/out_10.txt
