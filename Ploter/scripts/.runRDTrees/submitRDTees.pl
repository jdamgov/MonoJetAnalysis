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
$LISTOFSamples    = $User_Preferences{"LISTOFSamples"} ;
$JOBDir           = $User_Preferences{"JOBDir"};
$RootFileName     = $User_Preferences{"RootFileName"};
$BatchSystem      = $User_Preferences{"BatchSystem"};

print "\n";
print "BASEDir = "          .$BASEDir."\n" ;
print "LISTOFSamples = "    .$LISTOFSamples."\n" ;
print "JOBDir        = "    .$JOBDir."\n" ;
print "RootFileName  = "    .$RootFileName."\n" ;
print "BatchSystem   = "    .$BatchSystem."\n" ;
print "\n";


$sampleJobListFile = "./lancia.sh";
open(SAMPLEJOBLISTFILE, ">", $sampleJobListFile);


$command = "if [ -e ".$BASEDir."/".$JOBDir." ] ; then rm -r ".$BASEDir."/".$JOBDir." ; fi"; 
system ($command);

$jobIt =0 ;

open (LISTOFSamples,$LISTOFSamples) ;
while (<LISTOFSamples>)
{
    system("cd ".$BASEDir."\n");
    
    chomp($_);
    
    ($SampleFlag,$isQCD,$RunFlag) = split(" ") ;

    $subsample = substr($SampleFlag,0,1);

    if($subsample eq "#"){ next ; }

    $command = "if [ ! -e ".$JOBDir." ] ; then mkdir ".$JOBDir." ; fi"; 
    system($command);

    $tempBjob = $JOBDir."/bjob_".$RootFileName."_".$jobIt.".sh" ;
    $command = "touch ".$tempBjob ;
    system($command);

    open (SAMPLEJOBFILE, ">", $tempBjob) or die "Can't open file ".$tempBjob;

    $command = "#!/bin/sh" ;
    print SAMPLEJOBFILE $command."\n";

    $command = "cd ".$BASEDir ;
    print SAMPLEJOBFILE $command."\n";

    $command = "export SCRAM_ARCH=slc5_amd64_gcc462" ;
    print SAMPLEJOBFILE $command."\n";
    
    $command = "eval `scramv1 runtime -sh`" ;
    print SAMPLEJOBFILE $command."\n";
    
    $command = "unbuffer root -l -b -q ".$RootFileName.".C\\(".$SampleFlag.",".$isQCD.",".$RunFlag."\\) >> ".$BASEDir."/".$JOBDir."/out_".$jobIt.".txt";
    print SAMPLEJOBFILE $command."\n";
    
    ############
    # submit job
    ############
    
    if($BatchSystem eq 'hercules') { $command = " qsub -V -d ".$BASEDir."/".$JOBDir." -q shortcms ".$BASEDir."/".$tempBjob ;  }      
    if($BatchSystem eq 'lxbatch')  { $command = " bsub -cwd ".$BASEDir."/".$JOBDir." -q cmscaf1nh ".$BASEDir."/".$tempBjob ;  }
 
    print SAMPLEJOBLISTFILE $command."\n";
  
    ++$jobIt ;
}
