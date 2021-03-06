////////////////////////////////////////////////////////////////////////////
//            nTuple Analysis Macro                                       //
////////////////////////////////////////////////////////////////////////////

#include "EventData.h"
#include "Operation.h"
#include "Histogram01.h"
#include "Histogram02.h"
#include "Constants.h"


// ROOT includes
#include <TStyle.h>
#include <TROOT.h>

// std includes
#include <fstream>
#include <string>

using namespace Operation;
using namespace Histogram01;
using namespace Histogram02;
using namespace Constants;
using namespace std;

// Define the output directory

int main(int argc, char ** argv) 
{
  if (argc < 6){
    cerr << "Program need more than this parameter " << endl;
    cerr << "Example:   Analysis  sampleName(ex:wjets or met or ...)   maxEvents   isMC(0 or 1)   cuts(jet,met,jetmet)   folder(./results/...)" << endl;
    return 1;
  }
  
  //cout<<"argc = "<<argc<<" | argv[0] = "<<argv[0]<<", argv[4] = "<<argv[4]<<endl;
  
  string anaout = Constants::outputDir; 
  anaout += "results/";
  anaout += argv[5];
  
  int varycutIndex = 0;
  string varycut = "";
  varycut += argv[4];
  if(varycut.find("met") != std::string::npos || varycut.find("Met") != std::string::npos || varycut.find("MET") != std::string::npos)
    varycutIndex += 1;
  if(varycut.find("jet") != std::string::npos || varycut.find("Jet") != std::string::npos || varycut.find("JET") != std::string::npos)
    varycutIndex += 2;
  if(varycutIndex<=0){
    cerr << "Check the defined cuts. It should be [jet,met,jetmet]" << endl;
    return 1;
  }
  
  int  isMC;
  sscanf(argv[3], "%d", &isMC);
  
  int nev;
  if ( argc >= 3 ) sscanf ( argv[2], "%d", &nev );
  else nev = 100000000;
  
  std::string logFileName = anaout + "/";
  logFileName += argv[1];
  logFileName += ".log";
  
  
  Manager manager(logFileName);
  
  string histFile = anaout + "/";
  histFile += argv[1];
  
  cout << "Starting to Run ........" << endl;
  cout << "Vary cuts by ";
  if(varycutIndex==1) cout<<"MET"; 
  if(varycutIndex==2) cout<<"Jet";
  if(varycutIndex==3) cout<<"JetMET";
  cout<<endl;


  //---------------------------------------CUTS & Fill Hist--------------------------------------------------------------
  
  
  vector<int> abrun;
  vector<int> abevt;

  
  CutHLT    CHLT0(0);    //Primay vertex
  CutHLT    CHLT1(1);   // HLT
  CutAbnormalEvents  CAbnormalEvents(abrun , abevt); 
  manager.Add(&CHLT0);   
  manager.Add(&CHLT1);   
  manager.Add(&CAbnormalEvents);	
  hDataMcMatching    DataMcMatching0(histFile+"_AnaZ_0.root"); 
  manager.Add(&DataMcMatching0);


  GenParPt  CGenZPt1(23 , 250);  
  manager.Add(&CGenZPt1); 
  hDataMcMatching    DataMcMatching1(histFile+"_AnaZ_1.root"); 
  manager.Add(&DataMcMatching1);
  
  
  CutNoiseClean CNoiseClean(0.95 , 0.98, 1,  0.01, 0.99);
  manager.Add(&CNoiseClean);
  CutHLT CHLT2(2);   // No scraping
  CutHLT CHLT3(3);   // HBHE
  CutHLT CHLT4(4);   // CSC Tight Halo
  CutHLT CHLT5(5);   // CSC Loose Halo [Don't use]
  CutHLT CHLT6(6);   // HCAL Laser
  CutHLT CHLT7(7);   // ECal Dead Cell
  CutHLT CHLT8(8);   // Tracking failure
  CutHLT CHLT9(9);   // eeBadSc
  CutHLT CHLT10(10); // Muon greedy [Don't use]
  CutHLT CHLT11(11); // ECal Laser
  CutHLT CHLT12(12); // Trk POG
  CutHLT CHLT13(13); // HCal Laser 2012
  /*
    manager.Add(&CHLT2);    //No scraping
    manager.Add(&CHLT3);    //HBHE
    manager.Add(&CHLT4);    //CSC Tight Halo
    //manager.Add(&CHLT5);  //CSC Loose Halo [Don't use]
    manager.Add(&CHLT6);    //HCAL Laser
    manager.Add(&CHLT7);    //ECal Dead Cell
    manager.Add(&CHLT8);    //Tracking failure
    manager.Add(&CHLT9);    //eeBadSc
    //manager.Add(&CHLT10); //Muon greedy [Don't use]
    manager.Add(&CHLT11);   //ECal Laser
    manager.Add(&CHLT12);   //Trk POG
    manager.Add(&CHLT13);   //HCal Laser 2012
  */
  hDataMcMatching DataMcMatching2(histFile+"_AnaZ_2.root");    
  manager.Add(&DataMcMatching2);
  hWZAnalysis WZAnalysis0(histFile+"_ZAnalysis_0.root");
  manager.Add(&WZAnalysis0); 
   
  
  CutJet1 CJet1(110 , 2.4,  0.02, 0.98);
  manager.Add(&CJet1);
  //CutJet2 CJet20(30,4.7);
  //manager.Add(&CJet20);
  hDataMcMatching DataMcMatching3(histFile+"_AnaZ_3.root");    
  manager.Add(&DataMcMatching3);
    
  
  CutNJet CNJet(3);
  manager.Add(&CNJet);
  hDataMcMatching DataMcMatching4(histFile+"_AnaZ_4.root");
  manager.Add(&DataMcMatching4);
 
   
  CutDeltaPhi3 CDeltaPhi3(2.5);
  manager.Add(&CDeltaPhi3); 
  hWSourceAnalysis WSourceAnalysis(histFile+"_hWSourceAnalysis.root");
  manager.Add(&WSourceAnalysis);
  hDataMcMatching DataMcMatching5(histFile+"_AnaZ_5.root");
  manager.Add(&DataMcMatching5);
  hWZAnalysis WZAnalysis1(histFile+"_ZAnalysis_1.root");
  manager.Add(&WZAnalysis1);
 

  //  NoPFMuon           CNoPFMuon(10., 66.);  
  //  manager.Add(&CNoPFMuon); // no cut on eta
  hDataMcMatching    DataMcMatching6(histFile+"_AnaZ_6.root"); 
  manager.Add(&DataMcMatching6);

  
  //  NoPFElec           CNoPFElec(10., 66.);  
  //  manager.Add(&CNoPFElec); // no cut on eta
  hDataMcMatching    DataMcMatching7(histFile+"_AnaZ_7.root"); 
  manager.Add(&DataMcMatching7);


  //PFLepIso CPFLepIso(20.);
  //manager.Add( &CPFLepIso );	
  //WsignSelection CWsignSelection(1.0);
  //manager.Add( & CWsignSelection );
  CutTau CTau(1,1);
  manager.Add(&CTau);
  hDataMcMatching DataMcMatching8(histFile+"_AnaZ_8.root");
  manager.Add(&DataMcMatching8);
  hWZAnalysis WZAnalysis2(histFile+"_ZAnalysis_2.root");
  manager.Add(&WZAnalysis2); 
 


  ////// end of standard selection	


  //-----------
  GenParPt           CGenZPt2(23 , 250); 
  CutJet1            CJet2(250 , 2.4,  0.02, 0.98); 
  if(varycutIndex==1 || varycutIndex==3){
    manager.Add(&CGenZPt2);
  }
  if(varycutIndex==2 || varycutIndex==3){
    manager.Add(&CJet2);
  }
  hDataMcMatching    DataMcMatching9(histFile+"_AnaZ_9.root");   
  manager.Add(&DataMcMatching9);
  hWZAnalysis        WZAnalysis3(histFile+"_ZAnalysis_3.root");   
  manager.Add(&WZAnalysis3);
  
  
  //-------------	
  GenParPt           CGenZPt3(23 , 300); 
  CutJet1            CJet3(300 , 2.4,  0.02, 0.98); 
  if(varycutIndex==1 || varycutIndex==3){
    manager.Add(&CGenZPt3);
  }
  if(varycutIndex==2 || varycutIndex==3){
    manager.Add(&CJet3);
  }
  hDataMcMatching    DataMcMatching10(histFile+"_AnaZ_10.root");   
  manager.Add(&DataMcMatching10);
  hWZAnalysis        WZAnalysis4(histFile+"_ZAnalysis_4.root");   
  manager.Add(&WZAnalysis4);
  
  
  //-------------	
  GenParPt           CGenZPt4(23 , 350); 
  CutJet1            CJet4(350 , 2.4,  0.02, 0.98); 
  if(varycutIndex==1 || varycutIndex==3){
    manager.Add(&CGenZPt4);
  }
  if(varycutIndex==2 || varycutIndex==3){
    manager.Add(&CJet4);
  }
  hDataMcMatching    DataMcMatching11(histFile+"_AnaZ_11.root");   
  manager.Add(&DataMcMatching11);
  hWZAnalysis        WZAnalysis5(histFile+"_ZAnalysis_5.root");   
  manager.Add(&WZAnalysis5);
  
  
  //-------------	
  CutJet1            CJet5(400 , 2.4,  0.02, 0.98); 
  GenParPt           CGenZPt5(23 , 400); 
  if(varycutIndex==1 || varycutIndex==3){
    manager.Add(&CGenZPt5);
  }
  if(varycutIndex==2 || varycutIndex==3){
    manager.Add(&CJet5);
  }
  hDataMcMatching    DataMcMatching12(histFile+"_AnaZ_12.root");   
  manager.Add(&DataMcMatching12);
  hWZAnalysis        WZAnalysis6(histFile+"_ZAnalysis_6.root"); 
  manager.Add(&WZAnalysis6);
  
  
  //-------------
  CutJet1            CJet6(450 , 2.4,  0.02, 0.98); 
  GenParPt           CGenZPt6(23 , 450); 
  if(varycutIndex==1 || varycutIndex==3){
    manager.Add(&CGenZPt6);
  }
  if(varycutIndex==2 || varycutIndex==3){
    manager.Add(&CJet6);
  } 
  hDataMcMatching    DataMcMatching13(histFile+"_AnaZ_13.root");   
  manager.Add(&DataMcMatching13);
  hWZAnalysis        WZAnalysis7(histFile+"_ZAnalysis_7.root"); 
  manager.Add(&WZAnalysis7);
  
  
  //-------------		
  CutJet1            CJet7(500 , 2.4,  0.02, 0.98); 
  GenParPt           CGenZPt7(23 , 500); 
  if(varycutIndex==1 || varycutIndex==3){
    manager.Add(&CGenZPt7);
  }
  if(varycutIndex==2 || varycutIndex==3){
    manager.Add(&CJet7);
  } 
  hDataMcMatching    DataMcMatching14(histFile+"_AnaZ_14.root");   
  manager.Add(&DataMcMatching14);
  hWZAnalysis        WZAnalysis8(histFile+"_ZAnalysis_8.root"); 
  manager.Add(&WZAnalysis8);
  
  
  //-------------	
  CutJet1            CJet8(550 , 2.4,  0.02, 0.98);
  GenParPt           CGenZPt8(23 , 550); 
  if(varycutIndex==1 || varycutIndex==3){
    manager.Add(&CGenZPt8);
  }
  if(varycutIndex==2 || varycutIndex==3){
    manager.Add(&CJet8);
  } 
  hDataMcMatching    DataMcMatching15(histFile+"_AnaZ_15.root");   
  manager.Add(&DataMcMatching15);
  hWZAnalysis        WZAnalysis9(histFile+"_ZAnalysis_9.root"); 
  manager.Add(&WZAnalysis9);
  
  //-------------
  
  //-------------------------------------------------------------------------------------------------------------------------
  cout << "Running over sample " << argv[1] << endl;
  EventData eventData(argv[1], nev, isMC); 
  
  // Loop over events
  manager.Run(eventData); 
}
