#!/usr/bin/perl

# ----------------------------------------------------------------------------
#      MAIN PROGRAM
# ----------------------------------------------------------------------------

use Env;

print "reading ".$ARGV[0]."\n" ;

open (USERCONFIG,$ARGV[0]) ;

while (<USERCONFIG>)
{
    chomp; 
    s/#.*//;                # no comments
    s/^\s+//;               # no leading white
    s/\s+$//;               # no trailing white
#    next unless length;     # anything left?
    my ($var, $value) = split(/\s*=\s*/, $_, 2);
    $User_Preferences{$var} = $value;
}

$BASEDir          = $User_Preferences{"BASEDir"};
$InputDirectory   = $User_Preferences{"InputDirectory"};
$TreeName         = $User_Preferences{"TreeName"};
$LeptonType       = $User_Preferences{"LeptonType"};

$JetPtWboostedMin = $User_Preferences{"JetPtWboostedMin"};
$JetPtCutMin      = $User_Preferences{"JetPtCutMin"};
$JetEtaCutMax     = $User_Preferences{"JetEtaCutMax"};
$CleaningTreshold = $User_Preferences{"CleaningTreshold"};
$JetCollectionDimension = $User_Preferences{"JetCollectionDimension"};
$NumJetMin        = $User_Preferences{"NumJetMin"};

$OutputRootDirectory = $User_Preferences{"OutputRootDirectory"};

$CMSSWPATH       = $User_Preferences{"CMSSWPATH"};
$JOBDir           = $User_Preferences{"JOBDir"};
$BatchSystem      = $User_Preferences{"BatchSystem"};
$EXEFile          = $User_Preferences{"EXEFile"};

print "\n";
print "BASEDir = "      .$BASEDir."\n" ;
print "InputDirectory = ".$InputDirectory."\n";
print "TreeName       = ".$TreeName."\n";
print "LeptonType     = ".$LeptonType."\n";

print "JetPtWboostedMin = ".$JetPtWboostedMin."\n";
print "JetPtCutMin      = ".$JetPtCutMin."\n";
print "JetEtaCutMax     = ".$JetEtaCutMax."\n";
print "CleaningTreshold = ".$CleaningTreshold."\n";
print "JetCollectionDimension = ".$JetCollectionDimension."\n";
print "NumJetMin        = ".$NumJetMin."\n";

print "OutputRootDirectory = ".$OutputRootDirectory."\n";

print "JOBDir  =".$JOBDir."\n";
print "BatchSystem   = ".$BatchSystem."\n";
print "EXEFile       = ".$EXEFile."\n";
print "\n";


$sampleJobListFile = "./lancia.sh";
open(SAMPLEJOBLISTFILE, ">", $sampleJobListFile);

$command = "if [ -e ".$BASEDir."/".$JOBDir." ] ; then rm -r ".$BASEDir."/".$JOBDir." ; fi";

system ($command);


$LISTOFSample = $BASEDir."/LISTOFSample.txt";

$command = "if [ -f ".$LISTOFSample." ] ; then rm ".$LISTOFSample." ; fi";

system($command);



if($LeptonType eq "Muon")    { $command = "ls ".$InputDirectory." | grep mu | grep root >> ".$LISTOFSample ;

                               print "LISTOFSample ".$command."\n";

                               system ($command);}

if($LeptonType eq "Electron"){ $command = "ls ".$InputDirectory." | grep el | grep root >> ".$LISTOFSample ;

                               print "LISTOFSample ".$command."\n";

                               system ($command);

                               $command = "ls ".$InputDirectory." | grep Wenu | grep -v QCD |  grep root >> ".$LISTOFSample ;

                               print "LISTOFSample ".$command."\n";

                               system ($command);

			   }


$jobIt =0 ;

print "\n";
open (LISTOFSample,$LISTOFSample) ;
while (<LISTOFSample>)
{
    system("cd ".$BASEDir."\n");
    
    chomp($_);
    
    ($Sample) = split(" ") ;

    $subsample = substr($Sample,0,1);

    if($subsample eq "#"){ next ; }

    print "Sample = ".$Sample."\n";

    $command = "if [ ! -e ".$JOBDir." ] ; then mkdir ".$JOBDir." ; fi"; 
    system($command);

    $tempBjob = $JOBDir."/bjob_".$jobIt.".sh" ;
    $command = "touch ".$tempBjob ;
    system($command);

#    print "Job sh : ".$command."\n";

    $tempCfg = $JOBDir."/cfg_".$jobIt.".cfg" ;
    $command = "touch ".$tempCfg ;
    system($command);

#    print "Job cfg : ".$command."\n";

    open (SAMPLECFGFILE, ">", $tempCfg) or die "Can't open file ".$tempCfg;

    $command = "[Input]" ;
    print SAMPLECFGFILE $command."\n";
    print SAMPLECFGFILE "\n";

    $command = "InputDirectory = ".$InputDirectory ;
    print SAMPLECFGFILE $command."\n";
    print SAMPLECFGFILE "\n";

    $command = "InputRootFile = ".$Sample ;
    print SAMPLECFGFILE $command."\n";
    print SAMPLECFGFILE "\n";

    $command = "TreeName = ".$TreeName ;
    print SAMPLECFGFILE $command."\n";
    print SAMPLECFGFILE "\n";

    $command = "LeptonType = ".$LeptonType ;
    print SAMPLECFGFILE $command."\n";
    print SAMPLECFGFILE "\n";

    $command = "JetPtWboostedMin = ".$JetPtWboostedMin ;
    print SAMPLECFGFILE $command."\n";
    print SAMPLECFGFILE "\n";

    $command = "JetPtCutMin = ".$JetPtCutMin ;
    print SAMPLECFGFILE $command."\n";
    print SAMPLECFGFILE "\n";

    $command = "JetEtaCutMax = ".$JetEtaCutMax ;
    print SAMPLECFGFILE $command."\n";
    print SAMPLECFGFILE "\n";

    $command = "CleaningTreshold = ".$CleaningTreshold ;
    print SAMPLECFGFILE $command."\n";
    print SAMPLECFGFILE "\n";

    $command = "JetCollectionDimension = ".$JetCollectionDimension ;
    print SAMPLECFGFILE $command."\n";
    print SAMPLECFGFILE "\n";

    $command = "NumJetMin = ".$NumJetMin ;
    print SAMPLECFGFILE $command."\n";
    print SAMPLECFGFILE "\n";

    $command = "[Output]" ;
    print SAMPLECFGFILE $command."\n";
    print SAMPLECFGFILE "\n";

    $command = "OutputRootDirectory = ".$OutputRootDirectory ;
    print SAMPLECFGFILE $command."\n";
    print SAMPLECFGFILE "\n";

    $command = "OutputRootFile = ".$Sample ;
    $command =~  s/RD_/VBF_/g ;
    print SAMPLECFGFILE $command."\n";
    print SAMPLECFGFILE "\n";

  

    open (SAMPLEJOBFILE, ">", $tempBjob) or die "Can't open file ".$tempBjob;

    $command = "#!/bin/sh" ;
    print SAMPLEJOBFILE $command."\n";

    $command = "cd ".$BASEDir ;
    print SAMPLEJOBFILE $command."\n";

    $command = "export SCRAM_ARCH=slc5_amd64_gcc462" ;
    print SAMPLEJOBFILE $command."\n";
    
    $command = "cd ".$CMSSWPATH." ; eval `scramv1 runtime -sh` ; cd -" ;
    print SAMPLEJOBFILE $command."\n";

    $command = "source ../setup.sh" ;
    print SAMPLEJOBFILE $command."\n";
    
    $command = "unbuffer ./".$EXEFile." ".$tempCfg." >> ".$BASEDir."/".$JOBDir."/out_".$jobIt.".txt";
    print SAMPLEJOBFILE $command."\n";
    
    ############
    # submit job
    ############
    
    if($BatchSystem eq 'hercules') { $command = " qsub -V -d ".$BASEDir."/".$JOBDir." -q longcms ".$BASEDir."/".$tempBjob ;  }      
    if($BatchSystem eq 'lxbatch')  { $command = " bsub -cwd ".$BASEDir."/".$JOBDir." -q cmscaf1nh ".$BASEDir."/".$tempBjob ;  }
 
    print SAMPLEJOBLISTFILE $command."\n";
  
    ++$jobIt ;
}
