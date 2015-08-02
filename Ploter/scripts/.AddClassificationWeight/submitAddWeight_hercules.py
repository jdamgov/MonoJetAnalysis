import os
import glob
import math
import time

import ROOT
from ROOT import *
from ROOT import gROOT, gStyle, gSystem, TLatex
import subprocess

from subprocess import Popen
from optparse import OptionParser

# To Run : python ./scripts/AddClassificationWeight/submitAddWeight_hercules.py --batchMode --sampleToProcess all --channel mu --queque shortcms

############################################
#            Job steering                  #
############################################

parser = OptionParser()

parser.add_option('--batchMode', action='store_true', dest='batchMode', default=False,
                                    help='no X11 windows')

parser.add_option('--sampleToProcess',action="store",type="string",dest="sampleToProcess",default=None)

parser.add_option('--channel',action="store",type="string",dest="channel",default="mu")

parser.add_option('--queque',action="store",type="string",dest="queque",default="longcms")

(options, args) = parser.parse_args()

############################################################
############################################################


def submitBatchJob( command, fn ):

    currentDir = os.getcwd();


    # create a dummy bash/csh
    outScript=open(fn+".sh","w");

    outScript.write('#!/bin/bash');
    outScript.write("\n"+'cd /afs/cern.ch/work/r/rgerosa/CMSSW_5_3_3_patch3/src/');
    outScript.write("\n"+'eval `scram runtime -sh`');
    outScript.write("\n"+'cd '+currentDir);    
    outScript.write("\n"+command);
    outScript.close();

    Lunch = "lancia";
    outLunch=open("./scripts/AddClassificationWeight/"+Lunch+".sh","a");
    outLunch.write("qsub -V -d "+currentDir+"/scripts/AddClassificationWeight/ -q "+options.queque+" "+currentDir+"/"+fn+".sh \n") ;
    

if __name__ == '__main__':

    all = [ "ofile_BulkG_c0p2_M1000",
            "ofile_BulkG_c0p2_M1200",
            "ofile_BulkG_c0p2_M1500",
            "ofile_BulkG_c0p2_M1600",
            "ofile_BulkG_c0p2_M2000",
            "ofile_TTbar",
            "ofile_TTbar_Powheg",
            "ofile_TTbar_matchDn",
            "ofile_TTbar_matchUp",
            "ofile_TTbar_scaleDn",
            "ofile_TTbar_scaleUp",
            "ofile_WJets_Herwig",
            "ofile_WJets_Pythia",
            "ofile_WJets_Pythia180",
            "ofile_WW",
            "ofile_WZ",
            "ofile_ZJets",
            "ofile_ZZ",
            "ofile_data",
            "ofile_sch",
            "ofile_sch_bar",
            "ofile_tWch",
            "ofile_tWch_bar",
            "ofile_tch",
            "ofile_tch_bar"            
           ]


    os.system("rm scripts/AddClassificationWeight/lancia.sh");
    os.system("touch scripts/AddClassificationWeight/lancia.sh");
        
    if options.sampleToProcess == "all" and options.batchMode :
    
        for i in range(len(all)):


            
            if ( options.channel == 'electron' or options.channel == 'el' ) :            

             command = ' cat scripts/AddClassificationWeight/VBFApplyMVAWeight_BulkGraviton_TEMPLATE.cfg | sed \'s//data2/rgerosa/otrees_Higgs/trainingtrees_mu///data2/rgerosa/otrees_Higgs/trainingtrees_el//g\' > scripts/AddClassificationWeight/VBFApplyMVAWeight_BulkGraviton_TEMPLATE.cfg';
             os.system(command) ;

              
            command = ' cat scripts/AddClassificationWeight/VBFApplyMVAWeight_BulkGraviton_TEMPLATE.cfg | sed \'s/OFILENAME/'+all[i]+'/g\' > ./scripts/AddClassificationWeight/VBFApplyMVAWeight_BulkGraviton_'+all[i]+'.cfg';
            os.system(command);
            cmmd = './bin/VBFApplyMVAWeight.exe scripts/AddClassificationWeight/VBFApplyMVAWeight_BulkGraviton_'+all[i]+'.cfg'
            print cmmd
            fn = "scripts/AddClassificationWeight/createTreeScript_%s_%s"%(options.channel,all[i]);
            submitBatchJob( cmmd, fn );

    
    elif options.batchMode and not options.sampleToProcess == None:


        if ( options.channel == 'electron' or options.channel == 'el' ) :            

          command = ' cat scripts/AddClassificationWeight/VBFApplyMVAWeight_BulkGraviton_TEMPLATE.cfg | sed \'s//data2/rgerosa/otrees_Higgs/trainingtrees_mu///data2/rgerosa/otrees_Higgs/trainingtrees_el//g\' > scripts/AddClassificationWeight/VBFApplyMVAWeight_BulkGraviton_TEMPLATE.cfg';
          os.system(command) ;

             
        command = ' cat scripts/AddClassificationWeight/VBFApplyMVAWeight_BulkGraviton_TEMPLATE.cfg   | sed \'s/OFILENAME/'+options.sampleToProcess+'/g\' scripts/AddClassificationWeight/VBFApplyMVAWeight_BulkGraviton_'+options.sampleToProcess+'.cfg';
        os.system(command);
        cmmd = './bin/VBFApplyMVAWeight.exe scripts/AddClassificationWeight/VBFApplyMVAWeight_BulkGraviton_'+options.sampleToProcess+'.cfg'
        
        print cmmd
        fn = "scripts/AddClassificationWeight/createTreeScript_%s_%s"%(options.channel,options.sampleToProcess);
        submitBatchJob( cmmd, fn );

    else:
        print "do nothing"






