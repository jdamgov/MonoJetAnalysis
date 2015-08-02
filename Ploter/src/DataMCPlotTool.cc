#include "DataMCPlotTool.h"

// Plot only one stack without any reference to data point
void DrawStackError(THStack* hs, const std::string & Labels,  const std::map<int,double> & SystematicErrorMap,  const bool & isLog, const bool & isLabel, const double & syst){

  TObjArray* histos = hs->GetStack () ;
  TGaxis::SetMaxDigits(3);
  
  if (histos) {

    Int_t number = histos->GetEntries();
    TH1F* last = (TH1F*) histos->At(number-1) ;

    TString Title ;
    bool isgood    = false;
    bool isnumber  = false; 
    TString BinWidth = Form("%f",last->GetBinWidth(1));
    
    // Set the right approximation for density value in the Y axis Label
    for( int i = 0, j = 0 ; i < BinWidth.Length() ; i++) {

       if(j>=2) { BinWidth.Replace(i,BinWidth.Length()-i,""); break ; }
       if(BinWidth[i]=='.'){ isgood = true ;   continue ; }
       if(isgood && BinWidth[i]!='0'){ isnumber=true ; j++ ; continue ; }
       if(isgood && isnumber && BinWidth[i]=='0'){ BinWidth.Replace(i,BinWidth.Length()-i,""); break;}
    }   

    // Set the right Y axis Label
    if(TString(((TH1F*) histos->At(0))->GetXaxis()->GetTitle()).Contains("GeV")){
      if(!isnumber) Title = Form("Events / ( %d GeV )",atoi(BinWidth.Data()));
      else Title = Form("Events / ( %s GeV )",BinWidth.Data());
    }   
    else{
     if(atoi(BinWidth.Data()) == 1) Title = Form("Events");
     else if(!isnumber) Title = Form("Events / ( %d )",atoi(BinWidth.Data()));
     else Title = Form("Events/ ( %s )",BinWidth.Data());
    }

    // Set sone style for log and non-log plots
    last->GetYaxis()->SetTitle(Title.Data());
    last->GetYaxis()->SetTitleSize(0.055);
    last->GetYaxis()->SetLabelSize(0.055);
    last->GetYaxis()->SetTitleOffset(1.05);

    if(isLog){ last->SetMaximum(last->GetBinContent(last->GetMaximumBin())*5000000);
               last->SetMinimum(0.001) ;
    }
    else { last->SetMaximum(last->GetBinContent(last->GetMaximumBin())*1.8);
           last->SetMinimum(0) ;
    }

    if(isLabel){ last->GetXaxis()->SetTitle(Labels.c_str());
                 last->GetXaxis()->SetTitleSize(0.035);
                 last->GetXaxis()->SetTitleOffset(1.05);
                 last->GetXaxis()->SetLabelSize(0.035);

                 last->GetYaxis()->SetTitleSize(0.055);
                 last->GetYaxis()->SetTitleOffset(0.90);
                 last->GetYaxis()->SetLabelSize(0.035);
    }
    else {
                 last->GetXaxis()->SetTitleSize(0.);
                 last->GetXaxis()->SetLabelSize(0.);
                 last->GetYaxis()->SetTitleOffset(1.10);
    }

    last->SetFillStyle(1001);
    last->SetLineColor(kBlack);
    last->SetLineWidth(2);
    last->DrawClone ("hist") ;

    // Draw the correct error band on the MC taking into account stat+syst      

    std::vector <double> vErrSys (last->GetNbinsX(),0.) ;
    std::vector <double> vErrStat (last->GetNbinsX(),0.) ;

    for (int i = number-1 ; i >=0 ; --i) {
      TH1F * histo  = (TH1F*) histos->At (i) ;
      TH1F * histo2 = 0 ;

      if(i!=0) histo2 = (TH1F*) histos->At (i-1) ;

      histo->GetXaxis()->SetTitle(Labels.c_str());
      histo->SetFillStyle(1001);
      histo->SetLineColor(kBlack);
      histo->SetLineWidth(2);
      if(i != number-1) histo->Draw ("same hist") ;

      for(int iBin = 0 ; iBin < histo->GetNbinsX(); iBin++){
        if(histo2!=0)
  	  vErrSys.at(iBin) = vErrSys.at(iBin) + (histo->GetBinContent(iBin+1)-histo2->GetBinContent(iBin+1))*(histo->GetBinContent(iBin+1)-histo2->GetBinContent(iBin+1))*SystematicErrorMap.at(i)*SystematicErrorMap.at(i);
        else 
  	  vErrSys.at(iBin) = vErrSys.at(iBin) + histo->GetBinContent(iBin+1)*histo->GetBinContent(iBin+1)*SystematicErrorMap.at(i)*SystematicErrorMap.at(i);
        vErrStat.at(iBin) = vErrStat.at(iBin) + histo->GetBinError(iBin+1)*histo->GetBinError(iBin+1);  
      }
    }


    last->SetFillStyle(3013);
    last->SetFillColor(kBlack);
    last->SetLineWidth(3);
    last->SetMarkerSize(0);
    
    for (int iBin = 0 ; iBin < last->GetNbinsX(); iBin++) {
      double additionalError = sqrt(vErrSys.at(iBin) + last->GetBinContent(iBin+1) * last->GetBinContent(iBin+1) * syst * syst );
      last->SetBinError(iBin+1,sqrt(additionalError*additionalError + vErrStat.at(iBin)));
    }
    
    last->DrawClone ("sameE2") ;

  }
}

// Plot only two stack overlayed without any reference to data point
void DrawDoubleStackError(THStack* hs, THStack* hs_herwig, const std::string & Labels,  const std::map<int,double> & SystematicErrorMap, 
                           const std::map<int,double> & SystematicErrorMap_herwig,  const bool & isLog, const bool & isLabel, const bool & isttbar_controlplots, const double & syst){

 TObjArray* histos = hs->GetStack () ;
 TObjArray* histos_herwig = hs_herwig->GetStack () ;
 TGaxis::SetMaxDigits(3);  

 if (histos) {

    Int_t number = histos->GetEntries();
    TH1F* last = (TH1F*) histos->At (number-1) ;

    TString Title ;
    bool isgood = false;
    bool isnumber  = false; 
   
    // rigth y-axis density in the label
    TString BinWidth = Form("%f",last->GetBinWidth(1));
    for( Ssiz_t i = 0, j= 0 ; i < BinWidth.Length() ; i++) {
     
       if (j>=2) { BinWidth.Replace(i,BinWidth.Length()-i,""); break ; }
       if(BinWidth[i]=='.'){ isgood = true ;   continue; }
       if(isgood && BinWidth[i]!='0'){ isnumber=true ; j++ ; continue ; }
       if(isgood && isnumber && BinWidth[i]=='0'){ BinWidth.Replace(i,BinWidth.Length()-i,""); break;}
    }  

    // rigth y-axis label
    if(TString(((TH1F*) histos->At(0))->GetXaxis()->GetTitle()).Contains("GeV")){
      if(!isnumber) Title = Form("Events / ( %d GeV )",atoi(BinWidth.Data()));
      else Title = Form("Events / ( %s GeV )",BinWidth.Data());
    }   
    else{
     if(atoi(BinWidth.Data()) == 1) Title = Form("Events");
     else if(!isnumber) Title = Form("Events / ( %d )",atoi(BinWidth.Data()));
     else Title = Form("Events / ( %s )",BinWidth.Data());
    }
 
    // some style options
    last->GetYaxis()->SetTitle(Title.Data());
    last->GetYaxis()->SetTitleSize(0.055);
    last->GetYaxis()->SetLabelSize(0.055);
    last->GetYaxis()->SetTitleOffset(1.05);

    if(isLog){ last->SetMaximum(last->GetBinContent(last->GetMaximumBin())*5000000);
               last->SetMinimum(0.001) ;
    }
    else { last->SetMaximum(last->GetBinContent(last->GetMaximumBin())*2.6);
           last->SetMinimum(0) ;
    }

    if(isLabel){ last->GetXaxis()->SetTitle(Labels.c_str());
                 last->GetXaxis()->SetTitleSize(0.055);
                 last->GetXaxis()->SetTitleOffset(1.0);
                 last->GetXaxis()->SetLabelSize(0.035);

                 last->GetYaxis()->SetTitleSize(0.055);
                 last->GetYaxis()->SetLabelSize(0.035);
    }
    else {
                 last->GetXaxis()->SetTitleSize(0.);
                 last->GetXaxis()->SetLabelSize(0.);
                 last->GetYaxis()->SetTitleOffset(1.10);
    }

    last->SetFillStyle(1001);
    last->SetFillColor(0);

    if(!isttbar_controlplots) last->SetLineColor(kRed);
    else last->SetLineColor(210);

    last->SetLineWidth(2);
    last->DrawClone ("hist") ;
     
    // set error on the stack: sys + stat
    std::vector <double> vErrSys (last->GetNbinsX(),0.) ;
    std::vector <double> vErrStat (last->GetNbinsX(),0.) ;
    for (int i = number-1 ; i >=0 ; --i) {
      TH1F * histo = (TH1F*) histos->At (i) ;
      TH1F * histo2 = 0; 
      if(i!=0) histo2 = (TH1F*) histos->At (i-1) ;

      histo->GetXaxis()->SetTitle(Labels.c_str());
      histo->SetFillStyle(1001);
      histo->SetLineColor(kBlack);
      histo->SetLineWidth(2);
      if(i != number-1) histo->Draw ("same hist") ;

      for(int iBin = 0 ; iBin < histo->GetNbinsX(); iBin++){
        if(histo2!=0)
 	  vErrSys.at(iBin) = vErrSys.at(iBin) + (histo->GetBinContent(iBin+1)-histo2->GetBinContent(iBin+1))*(histo->GetBinContent(iBin+1)-histo2->GetBinContent(iBin+1))*SystematicErrorMap.at(i)*SystematicErrorMap.at(i);
        else 
  	  vErrSys.at(iBin) = vErrSys.at(iBin) + histo->GetBinContent(iBin+1)*histo->GetBinContent(iBin+1)*SystematicErrorMap.at(i)*SystematicErrorMap.at(i);
        vErrStat.at(iBin) = vErrStat.at(iBin) + histo->GetBinError(iBin+1)*histo->GetBinError(iBin+1);  
      }
    }

    last->SetFillStyle(3005);

    if(!isttbar_controlplots) last->SetFillColor(kRed);
    else last->SetFillColor(210);

    last->SetLineWidth(3);
    last->SetMarkerSize(0);
    
    for (int iBin = 0 ; iBin < last->GetNbinsX(); iBin++) {
      double additionalError = sqrt(vErrSys.at(iBin) + last->GetBinContent(iBin+1) * last->GetBinContent(iBin+1) * syst * syst );
      last->SetBinError(iBin+1,sqrt(additionalError*additionalError + vErrStat.at(iBin)));
    }
    
    last->DrawClone ("sameE2") ;

  }


  if (histos_herwig) {

    Int_t number = histos_herwig->GetEntries();
    TH1F* last = (TH1F*) histos_herwig->At (number-1) ;

    TString Title ;
    bool isgood = false;
    bool isnumber  = false; 

    // rigth y-axis density in the label
    TString BinWidth = Form("%f",last->GetBinWidth(1));
    for( Ssiz_t i = 0, j = 0 ; i < BinWidth.Length() ; i++) {

       if (j>=2) { BinWidth.Replace(i,BinWidth.Length()-i,""); break ; }
       if(BinWidth[i]=='.'){ isgood = true ;   continue; }
       if(isgood && BinWidth[i]!='0'){ isnumber=true ; j++ ; continue ; }
       if(isgood && isnumber && BinWidth[i]=='0'){ BinWidth.Replace(i,BinWidth.Length()-i,""); break;}
    }  


    // rigth y-axis label
    if(TString(((TH1F*) histos->At(0))->GetXaxis()->GetTitle()).Contains("GeV")){
      if(!isnumber) Title = Form("Events / ( %d GeV )",atoi(BinWidth.Data()));
      else Title = Form("Events / ( %s GeV )",BinWidth.Data());
    }   
    else{
     if(atoi(BinWidth.Data()) == 1) Title = Form("Events");
     else if(!isnumber) Title = Form("Events / ( %d )",atoi(BinWidth.Data()));
     else Title = Form("Events / ( %s )",BinWidth.Data());
    }
 
    last->GetYaxis()->SetTitle(Title.Data());
    last->GetYaxis()->SetTitleSize(0.055);
    last->GetYaxis()->SetLabelSize(0.05);
    last->GetYaxis()->SetTitleOffset(1.10);

    last->SetFillStyle(3005);

    if(!isttbar_controlplots) last->SetLineColor(kBlue);
    else last->SetLineColor(53);
    last->SetFillColor(0);
    last->SetLineWidth(2);
    last->DrawClone ("hist same") ;

    // set rigth stack uncertainty band : sys + stat
    std::vector <double> vErrSys (last->GetNbinsX(),0.) ;
    std::vector <double> vErrStat (last->GetNbinsX(),0.) ;

    for (int i = number-1 ; i >=0 ; --i) {

      TH1F * histo = (TH1F*) histos->At (i) ;
      TH1F * histo2 = 0; 
      if(i!=0) histo2 = (TH1F*) histos->At (i-1) ;

      histo->GetXaxis()->SetTitle(Labels.c_str());
      histo->SetFillStyle(1001);
      histo->SetLineColor(kBlack);
      histo->SetLineWidth(2);
      if(i != number-1) histo->Draw ("same hist") ;

      for(int iBin = 0 ; iBin < histo->GetNbinsX(); iBin++){
	if(histo2!=0)
         vErrSys.at(iBin) = vErrSys.at(iBin) + (histo->GetBinContent(iBin+1)-histo2->GetBinContent(iBin+1))*(histo->GetBinContent(iBin+1)-histo2->GetBinContent(iBin+1))*SystematicErrorMap.at(i)*SystematicErrorMap.at(i);
        else 
  	  vErrSys.at(iBin) = vErrSys.at(iBin) + histo->GetBinContent(iBin+1)*histo->GetBinContent(iBin+1)*SystematicErrorMap.at(i)*SystematicErrorMap.at(i);
        vErrStat.at(iBin) = vErrStat.at(iBin) + histo->GetBinError(iBin+1)*histo->GetBinError(iBin+1);  
      }
    }


    last->SetFillStyle(3004);

    if(!isttbar_controlplots) last->SetFillColor(kBlue);
    else last->SetFillColor(53);

    last->SetLineWidth(3);
    last->SetMarkerSize(0);

    for (int iBin = 0 ; iBin < last->GetNbinsX(); iBin++) {
      double additionalError = sqrt(vErrSys.at(iBin) + last->GetBinContent(iBin+1) * last->GetBinContent(iBin+1) * syst * syst );
      last->SetBinError(iBin+1,sqrt(additionalError*additionalError + vErrStat.at(iBin)));
    }

    last->DrawClone ("sameE2") ;
  }

}



void DrawStackError(THStack* hs, const std::string & Labels, const TH1F* dataHist, const std::map<int,double> & SystematicErrorMap, const bool & isLog, 
                    const bool & isLabel, const double & syst){

  TObjArray* histos = hs->GetStack () ;
  TGaxis::SetMaxDigits(3);

  if (histos) {

    Int_t number = histos->GetEntries();
    TH1F* last = (TH1F*) histos->At (number-1) ;

    TString Title ;
    bool isgood = false;
    bool isnumber  = false; 

    // Set the right approximation for density value in the Y axis Label
    TString BinWidth = Form("%f",last->GetBinWidth(1));
    for( Ssiz_t i = 0, j = 0 ; i < BinWidth.Length() ; i++) {

       if (j>=2) { BinWidth.Replace(i,BinWidth.Length()-i,""); break ; }
       if(BinWidth[i]=='.'){ isgood = true ;   continue; }
       if(isgood && BinWidth[i]!='0'){ isnumber=true ; j++ ; continue ; }
       if(isgood && isnumber && BinWidth[i]=='0'){ BinWidth.Replace(i,BinWidth.Length()-i,""); break;}
    }  

    // Set the right Y axis Label
    if(TString(((TH1F*) histos->At(0))->GetXaxis()->GetTitle()).Contains("GeV")){
      if(!isnumber) Title = Form("Events / ( %d GeV )",atoi(BinWidth.Data()));
      else Title = Form("Events / ( %s GeV )",BinWidth.Data());
    }   
    else{
     if(atoi(BinWidth.Data()) == 1) Title = Form("Events");
     else if(!isnumber) Title = Form("Events / ( %d )",atoi(BinWidth.Data()));
     else Title = Form("Events / ( %s )",BinWidth.Data());
    }
     
    // some plot style
    last->GetYaxis()->SetTitle(Title.Data());
    last->GetYaxis()->SetTitleSize(0.055);
    last->GetYaxis()->SetLabelSize(0.05);
    last->GetYaxis()->SetTitleOffset(1.05);

    if(isLog){ last->SetMaximum(std::max(last->GetBinContent(last->GetMaximumBin())*15000,dataHist->GetBinContent(dataHist->GetMaximumBin())*5000000));
               last->SetMinimum(0.001) ;
    }
    else { last->SetMaximum(std::max(last->GetBinContent(last->GetMaximumBin())*1.6,dataHist->GetBinContent(dataHist->GetMaximumBin())*1.6));
           last->SetMinimum(0.) ;
    }

    if(isLabel){ last->GetXaxis()->SetTitle(Labels.c_str());
                 last->GetXaxis()->SetTitleSize(0.055);
                 last->GetXaxis()->SetTitleOffset(1.0);
                 last->GetXaxis()->SetLabelSize(0.035);

                 last->GetYaxis()->SetTitleSize(0.055);
                 last->GetYaxis()->SetLabelSize(0.035);
    }
    else {
                 last->GetXaxis()->SetTitleSize(0.);
                 last->GetXaxis()->SetLabelSize(0.);
                 last->GetYaxis()->SetTitleOffset(1.10);
    }
   
    last->SetFillStyle(1001);
    last->SetLineColor(kBlack);
    last->SetLineWidth(2);
    last->DrawClone ("hist") ;

    // set uncertainty on each bin : stat + sys
    std::vector <double> vErrSys (last->GetNbinsX(),0.) ;
    std::vector <double> vErrStat (last->GetNbinsX(),0.) ;

    for (int i = number-1 ; i >=0 ; --i) {

      TH1F * histo = (TH1F*) histos->At (i) ;
      TH1F * histo2 = 0; 
      if(i!=0) histo2 = (TH1F*) histos->At (i-1) ;

      histo->GetXaxis()->SetTitle(Labels.c_str());
      histo->SetFillStyle(1001);
      histo->SetLineColor(kBlack);
      histo->SetLineWidth(2);
      if(i != number-1) histo->Draw ("same hist") ;

      for(int iBin = 0 ; iBin < histo->GetNbinsX(); iBin++){
	if(histo2!=0)
	vErrSys.at(iBin) = vErrSys.at(iBin) + (histo->GetBinContent(iBin+1)-histo2->GetBinContent(iBin+1))*(histo->GetBinContent(iBin+1)-histo2->GetBinContent(iBin+1))*SystematicErrorMap.at(i)*SystematicErrorMap.at(i);
        else 
  	  vErrSys.at(iBin) = vErrSys.at(iBin) + histo->GetBinContent(iBin+1)*histo->GetBinContent(iBin+1)*SystematicErrorMap.at(i)*SystematicErrorMap.at(i);
        vErrStat.at(iBin) = vErrStat.at(iBin) + histo->GetBinError(iBin+1)*histo->GetBinError(iBin+1);  
      }
    }

    last->SetFillStyle(3013);
    last->SetLineWidth(3);
    last->SetFillColor(kBlack);
    last->SetMarkerSize(0);
    
    for (int iBin = 0 ; iBin < last->GetNbinsX(); iBin++) {
      double additionalError = sqrt(vErrSys.at(iBin) + last->GetBinContent(iBin+1) * last->GetBinContent(iBin+1) * syst * syst );
      last->SetBinError(iBin+1,sqrt(additionalError*additionalError + vErrStat.at(iBin)));
    }
    
    last->DrawClone("sameE2") ;
  }
}

void DrawDoubleStackError(THStack* hs, THStack* hs_herwig, const std::string & Labels, const TH1F* dataHist, const std::map<int,double> & SystematicErrorMap, 
                          const std::map<int,double> & SystematicErrorMap_herwig, const bool & isLog, const bool & isLabel, const bool & isttbar_controlplots, const double & syst){

  TObjArray* histos = hs->GetStack () ;
  TObjArray* histos_herwig = hs_herwig->GetStack () ;
  TGaxis::SetMaxDigits(3);

  if (histos) {

    Int_t number = histos->GetEntries();
    TH1F* last = (TH1F*) histos->At (number-1) ;

    TString Title ;
    bool isgood = false;
    bool isnumber  = false; 

    // Set the right approximation for density value in the Y axis Label
    TString BinWidth = Form("%f",last->GetBinWidth(1));
    for( Ssiz_t i = 0, j = 0 ; i < BinWidth.Length() ; i++) {

       if (j>=2) { BinWidth.Replace(i,BinWidth.Length()-i,""); break ; }
       if(BinWidth[i]=='.'){ isgood = true ;   continue; }
       if(isgood && BinWidth[i]!='0'){ isnumber=true ; j++ ; continue ; }
       if(isgood && isnumber && BinWidth[i]=='0'){ BinWidth.Replace(i,BinWidth.Length()-i,""); break;}
    }  
 

    // Set the right Y axis Label
    if(TString(((TH1F*) histos->At(0))->GetXaxis()->GetTitle()).Contains("GeV")){
      if(!isnumber) Title = Form("Events / ( %d GeV )",atoi(BinWidth.Data()));
      else Title = Form("Events / ( %s GeV )",BinWidth.Data());
    }   
    else{
     if(atoi(BinWidth.Data()) == 1) Title = Form("Events");
     else if(!isnumber) Title = Form("Events / ( %d )",atoi(BinWidth.Data()));
     else Title = Form("Events / ( %s )",BinWidth.Data());
    }

    // some plot style
    last->GetYaxis()->SetTitle(Title.Data());
    last->GetYaxis()->SetTitleSize(0.055);
    last->GetYaxis()->SetLabelSize(0.05);
    last->GetYaxis()->SetTitleOffset(1.05);

    if(isLog){ last->SetMaximum(std::max(last->GetBinContent(last->GetMaximumBin())*15000,dataHist->GetBinContent(dataHist->GetMaximumBin())*5000000));
               last->SetMinimum(0.001) ;
    }
    else { last->SetMaximum(std::max(last->GetBinContent(last->GetMaximumBin())*2.6,dataHist->GetBinContent(dataHist->GetMaximumBin())*2.6));
           last->SetMinimum(0.) ;
    }

    if(isLabel){ last->GetXaxis()->SetTitle(Labels.c_str());
                 last->GetXaxis()->SetTitleSize(0.05);
                 last->GetXaxis()->SetTitleOffset(1.0);
                 last->GetXaxis()->SetLabelSize(0.035);

                 last->GetYaxis()->SetTitleSize(0.05);
                 last->GetYaxis()->SetLabelSize(0.035);
    }
    else {
                 last->GetXaxis()->SetTitleSize(0.);
                 last->GetXaxis()->SetLabelSize(0.);
                 last->GetYaxis()->SetTitleOffset(1.10);
    }
   
    last->SetFillStyle(1001);
    last->SetFillColor(0); 
    if(!isttbar_controlplots) last->SetLineColor(kRed);
    else last->SetLineColor(210);
    last->SetLineWidth(2);
    last->DrawClone ("hist") ;

    // set stack bin errors : stat +sys
    std::vector <double> vErrSys (last->GetNbinsX(),0.) ;
    std::vector <double> vErrStat (last->GetNbinsX(),0.) ;

    for (int i = number-1 ; i >=0 ; --i) {

      TH1F * histo = (TH1F*) histos->At (i) ;
      TH1F * histo2 = 0; 
      if(i!=0) histo2 = (TH1F*) histos->At (i-1) ;

      histo->GetXaxis()->SetTitle(Labels.c_str());
      histo->SetFillStyle(1001);
      histo->SetLineColor(kBlack);
      histo->SetLineWidth(2);
      if(i != number-1) histo->Draw ("same hist") ;

      for(int iBin = 0 ; iBin < histo->GetNbinsX(); iBin++){
	if(histo2!=0)
 	  vErrSys.at(iBin) = vErrSys.at(iBin) + (histo->GetBinContent(iBin+1)-histo2->GetBinContent(iBin+1))*(histo->GetBinContent(iBin+1)-histo2->GetBinContent(iBin+1))*SystematicErrorMap.at(i)*SystematicErrorMap.at(i);
        else 
  	  vErrSys.at(iBin) = vErrSys.at(iBin) + histo->GetBinContent(iBin+1)*histo->GetBinContent(iBin+1)*SystematicErrorMap.at(i)*SystematicErrorMap.at(i);
        vErrStat.at(iBin) = vErrStat.at(iBin) + histo->GetBinError(iBin+1)*histo->GetBinError(iBin+1);  
      }
    }

    last->SetFillStyle(3004);
    last->SetLineWidth(3);
    if(!isttbar_controlplots)    last->SetFillColor(kRed);
    else last->SetFillColor(210);
    last->SetMarkerSize(0);
    
    for (int iBin = 0 ; iBin < last->GetNbinsX(); iBin++) {
      double additionalError = sqrt(vErrSys.at(iBin) + last->GetBinContent(iBin+1) * last->GetBinContent(iBin+1) * syst * syst );
      last->SetBinError(iBin+1,sqrt(additionalError*additionalError + vErrStat.at(iBin)));
    }
    
    last->DrawClone ("sameE2") ;
  }

  if (histos_herwig) {

    Int_t number = histos_herwig->GetEntries();
    TH1F* last = (TH1F*) histos_herwig->At (number-1) ;

    TString Title ;
    bool isgood = false;
    bool isnumber  = false; 

    TString BinWidth = Form("%f",last->GetBinWidth(1));
    for( Ssiz_t i = 0, j = 0 ; i < BinWidth.Length() ; i++) {
     
       if (j>=2) { BinWidth.Replace(i,BinWidth.Length()-i,""); break ; }
       if(BinWidth[i]=='.'){ isgood = true ;   continue; }
       if(isgood && BinWidth[i]!='0'){ isnumber=true ; j++ ; continue ; }
       if(isgood && isnumber && BinWidth[i]=='0'){ BinWidth.Replace(i,BinWidth.Length()-i,""); break;}
    }  


    if(TString(((TH1F*) histos->At(0))->GetXaxis()->GetTitle()).Contains("GeV")){
      if(!isnumber) Title = Form("Events / ( %d GeV )",atoi(BinWidth.Data()));
      else Title = Form("Events / ( %s GeV )",BinWidth.Data());
    }   
    else{
     if(atoi(BinWidth.Data()) == 1) Title = Form("Events");
     else if(!isnumber) Title = Form("Events / ( %d )",atoi(BinWidth.Data()));
     else Title = Form("Events / ( %s )",BinWidth.Data());
    }


    last->GetYaxis()->SetTitle(Title.Data());
    last->GetYaxis()->SetTitleSize(0.055);
    last->GetYaxis()->SetLabelSize(0.05);
    last->GetYaxis()->SetTitleOffset(1.10);

    last->SetFillStyle(3005);
    last->SetFillColor(0);
    if(!isttbar_controlplots) last->SetLineColor(kBlue);
    else last->SetLineColor(53);
    last->SetLineWidth(2);
    last->DrawClone ("hist same") ;

    std::vector <double> vErrSys (last->GetNbinsX(),0.) ;
    std::vector <double> vErrStat (last->GetNbinsX(),0.) ;

    for (int i = number-1 ; i >=0 ; --i) {

      TH1F * histo = (TH1F*) histos->At (i) ;
      TH1F * histo2 = 0 ; 
      if(i!=0) histo2 = (TH1F*) histos->At (i-1) ;

      histo->GetXaxis()->SetTitle(Labels.c_str());
      histo->SetFillStyle(1001);
      histo->SetLineColor(kBlack);
      histo->SetLineWidth(2);
      if(i != number-1) histo->Draw ("same hist") ;

      for(int iBin = 0 ; iBin < histo->GetNbinsX(); iBin++){
	if(histo2!=0)
	 vErrSys.at(iBin) = vErrSys.at(iBin) + (histo->GetBinContent(iBin+1)-histo2->GetBinContent(iBin+1))*(histo->GetBinContent(iBin+1)-histo2->GetBinContent(iBin+1))*SystematicErrorMap.at(i)*SystematicErrorMap.at(i);
        else 
  	  vErrSys.at(iBin) = vErrSys.at(iBin) + histo->GetBinContent(iBin+1)*histo->GetBinContent(iBin+1)*SystematicErrorMap.at(i)*SystematicErrorMap.at(i);
        vErrStat.at(iBin) = vErrStat.at(iBin) + histo->GetBinError(iBin+1)*histo->GetBinError(iBin+1);  
      }
    }

    last->SetFillStyle(3005);
    last->SetLineWidth(3);
    if(!isttbar_controlplots)    last->SetFillColor(kBlue);
    else last->SetFillColor(53);
    last->SetMarkerSize(0);
    
    for (int iBin = 0 ; iBin < last->GetNbinsX(); iBin++) {
      double additionalError = sqrt(vErrSys.at(iBin) + last->GetBinContent(iBin+1) * last->GetBinContent(iBin+1) * syst * syst );
      last->SetBinError(iBin+1,sqrt(additionalError*additionalError + vErrStat.at(iBin)));
    }
    
    last->DrawClone ("sameE2") ;
  }


}


void LatexCMS (const double & lumi, const std::string & LeptonType, const bool & isLabel){

  TLatex latex;
  latex.SetNDC();
  latex.SetTextAlign(21); // align right                                                                                                                        

  if(isLabel) latex.SetTextSize(0.036);
  else latex.SetTextSize(0.043);

  if(LeptonType == "muon" || LeptonType == "mu") latex.DrawLatex(0.56,0.96,Form("CMS         L = %.1f fb^{-1} at #sqrt{s} = 13 TeV, W+jets",(float)lumi/1000));
  else if(LeptonType == "electron" || LeptonType == "el") latex.DrawLatex(0.52,0.95,Form("CMS,   L = %.1f fb^{-1} at #sqrt{s} = 13 TeV, W+jets",(float)lumi/1000));
  else latex.DrawLatex(0.56,0.962,Form("CMS Preliminary, %.1f fb^{-1} at #sqrt{s} = 13 TeV, W+jets",(float)lumi/1000));

}


void SetTotalSystematicVector( std::vector<double> & SysError, THStack* hs, const std::map<int,double> & SystematicErrorMap, const double & syst){

  TObjArray* histos = hs->GetStack () ;

  if (histos) {

    Int_t number = histos->GetEntries();
    TH1F* last = (TH1F*) histos->At (number-1) ;

    SysError.assign(last->GetNbinsX(),0.);

    for (int i = number-1 ; i >=0 ; --i) {
      
      TH1F * histo  = (TH1F*) histos->At (i) ;
      TH1F * histo2 = 0; 
      if(i!=0 && histo2 == 0) histo2 = (TH1F*) histos->At (i-1) ;

      for(int iBin = 0 ; iBin < histo->GetNbinsX(); iBin++){
        if(histo2!=0) 
	  SysError.at(iBin) = SysError.at(iBin) + (histo->GetBinContent(iBin+1)-histo2->GetBinContent(iBin+1))*(histo->GetBinContent(iBin+1)-histo2->GetBinContent(iBin+1))*SystematicErrorMap.at(i)*SystematicErrorMap.at(i)*SystematicErrorMap.at(i) + (histo->GetBinContent(iBin+1)-histo2->GetBinContent(iBin+1))*(histo->GetBinContent(iBin+1)-histo2->GetBinContent(iBin+1))*syst*syst;
        else SysError.at(iBin) = SysError.at(iBin) + histo->GetBinContent(iBin+1)*histo->GetBinContent(iBin+1)*SystematicErrorMap.at(i)*SystematicErrorMap.at(i)+histo->GetBinContent(iBin+1)*histo->GetBinContent(iBin+1)*syst*syst;

      }
    }
  }
}
