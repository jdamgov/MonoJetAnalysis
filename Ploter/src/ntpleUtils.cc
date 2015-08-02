#include "ntpleUtils.h"


bool maggiore(double i, double j)
{
  return( i>j );
}






std::map<int, int> GetTotalEvents(const std::string& histoName, const std::string& inputFileList)
{
  std::ifstream inFile(inputFileList.c_str());
  std::string buffer;
  std::map<int, int> totalEvents;
  
  if(!inFile.is_open())
  {
    std::cerr << "** ERROR: Can't open '" << inputFileList << "' for input" << std::endl;
    return totalEvents;
  }
  
  while(1)
  {
    inFile >> buffer;
    if(!inFile.good()) break;

    TFile* f = TFile::Open(buffer.c_str());
    TH1F* histo = NULL;
    f -> GetObject(histoName.c_str(), histo);
    if(histo == NULL)
    {
      std::cout << ">>>ntpleUtils::Error in getting object " << histoName << std::endl;
      exit(-1);
    }
    
    
    for(int bin = 1; bin <= histo -> GetNbinsX(); ++bin)
      totalEvents[bin] += int(histo -> GetBinContent(bin));
    
    f -> Close();
    
    delete f;
  }

  return totalEvents;
}

//  ------------------------------------------------------------


bool FillChain(TChain& chain, const std::string& inputFileList)
{
  std::ifstream inFile(inputFileList.c_str());
  std::string buffer;

  if(!inFile.is_open())
  {
    std::cerr << "** ERROR: Can't open '" << inputFileList << "' for input" << std::endl;
    return false;
  }
  
  while(1)
  {
    inFile >> buffer;
    if(!inFile.good()) break;
    chain.Add(buffer.c_str());
    //std::cout << ">>> ntupleUtils::FillChain - treeName = " << chain.GetName() << " from file " << buffer << std::endl;
  }

  return true;
}

//  ------------------------------------------------------------

int parseConfigFile (const TString& config)
{
  std::cout << ">>> Parsing " << config << " file" << std::endl ;
  
  if (gConfigParser) return 1 ;
  gConfigParser = new ConfigParser();
  
  if( !(gConfigParser -> init(config)) )
  {
    std::cout << ">>> parseConfigFile::Could not open configuration file "
              << config << std::endl;
     return -1;
  }
  
  //gConfigParser -> print();
  
  return 0 ;
}

//  ------------------------------------------------------------






double deltaPhi(const double& phi1, const double& phi2)
{ 
  double deltaphi = fabs(phi1 - phi2);
  if (deltaphi > 6.283185308) deltaphi -= 6.283185308;
  if (deltaphi > 3.141592654) deltaphi = 6.283185308 - deltaphi;
  return deltaphi;
}

//  ------------------------------------------------------------

double deltaEta(const double& eta1, const double& eta2)
{ 
  double deltaeta = fabs(eta1 - eta2);
  return deltaeta;
}

//  ------------------------------------------------------------

double deltaR(const double& phi1, const double& phi2,
              const double& eta1, const double& eta2)
{ 
 double deltaphi = deltaPhi(phi1, phi2);
 double deltaeta = deltaEta(eta1, eta2);
 double deltar = sqrt(deltaphi*deltaphi + deltaeta*deltaeta);
 return deltar;
}

//  ------------------------------------------------------------






double SelectJets(std::vector<int>& it, std::vector<ROOT::Math::XYZTVector>& jets,
                  const std::string& method,
                  const double& etMin,
                  const std::vector<int>* blacklist)
{
  // initialize vector with result
  it.clear();
  it.push_back(-1);
  it.push_back(-1);
  
  
  
  // initialize the selection variable
  double maxDeta = -999999.;
  double tempDeta = 0.;
  
  double maxMJJ = -999999.;
  double tempMJJ = 0.;
  
  double maxPt = -999999.;
  double tempPt = 0.;
  
  double maxSumPt = -999999.;
  double tempSumPt = 0.;
  
  
  
  // loop over 1st jet
  for(unsigned int i = 0; i < jets.size(); ++i)
  {
    if(jets.at(i).Et() < etMin) continue;
    
    bool skipJet1 = false;
    if(blacklist)
      for(unsigned int kk = 0; kk < blacklist -> size(); ++kk)
        if(blacklist -> at(kk) == static_cast<int>(i)) skipJet1 = true;
    if(skipJet1) continue;
    
    
    
    // loop over 2nd jet
    for(unsigned int j = i+1; j < jets.size(); ++j)
    {
      if(jets.at(j).Et() < etMin) continue;
      
      bool skipJet2 = false;
      if(blacklist)
        for(unsigned int kk = 0; kk < blacklist -> size(); ++kk)
          if(blacklist -> at(kk) == static_cast<int>(j)) skipJet2 = true;
      if(skipJet2) continue;
      
      
      
      // -------------------------------------
      // select jets with different techniques
      // -------------------------------------
      
      if(method == "maxDeta")
      {
        tempDeta = deltaEta(jets.at(i).Eta(), jets.at(j).Eta());
        if(tempDeta > maxDeta)
        {
          maxDeta = tempDeta;
          
    	    it.at(0) = i;
	        it.at(1) = j;
        }
      }
      
      // -------------------------------------
      
      if(method == "maxMJJ")
      {
        tempMJJ = (jets.at(i) + jets.at(j)).mass();
        if(tempMJJ > maxMJJ)
        {
          maxMJJ = tempMJJ;
          
    	    it.at(0) = i;
	        it.at(1) = j;
        }
      }
      
      // -------------------------------------
      else if(method == "maxPt")
      {
        tempPt = sqrt( (jets.at(i) + jets.at(j)).perp2() );
        if(tempPt > maxPt)
        {
          maxPt = tempPt;
          
	        it.at(0) = i;
	        it.at(1) = j;
        }
      }
      
      // -------------------------------------
      
      else if(method == "maxSumPt")
      {
        tempSumPt = sqrt(jets.at(i).perp2()) + sqrt(jets.at(j).perp2());
        if(tempSumPt > maxSumPt)
        {
          maxSumPt = tempSumPt;
          
   	      it.at(0) = i;
	        it.at(1) = j;
        }
      }
      
      // -------------------------------------
      
      
      
    } // loop over 2nd jet
  } // loop over 1st jet
  
  
  
  if(method == "maxMJJ")
    return maxMJJ;
  
  else if(method == "maxPt")
    return maxPt;
  
  else if(method == "maxSumPt")
    return maxSumPt;
  
  else return -1.;
}

//  ------------------------------------------------------------

double SelectJets(std::vector<int>& it, std::vector<ROOT::Math::XYZTVector>& jets,
                  const std::string& method,
                  const double& etMin,
                  const double& DetaMin,
                  const double& DetaMax,
                  const std::vector<int>* blacklist)
{
  // initialize vector with result
  it.clear();
  it.push_back(-1);
  it.push_back(-1);
  
  
  
  // initialize the selection variable
  double maxDeta = -999999.;
  double tempDeta = 0.;
  
  double maxMJJ = -999999.;
  double tempMJJ = 0.;
  
  double maxPt = -999999.;
  double tempPt = 0.;
  
  double maxSumPt = -999999.;
  double tempSumPt = 0.;
  
  
  
  // loop over 1st jet
  for(unsigned int i = 0; i < jets.size(); ++i)
  {
    if(jets.at(i).Et() < etMin) continue;
    
    bool skipJet1 = false;
    if(blacklist)
      for(unsigned int kk = 0; kk < blacklist -> size(); ++kk)
        if(blacklist -> at(kk) == static_cast<int>(i)) skipJet1 = true;
    if(skipJet1) continue;
    
    
    
    // loop over 2nd jet
    for(unsigned int j = i+1; j < jets.size(); ++j)
    {
      if(jets.at(j).Et() < etMin) continue;
      
      bool skipJet2 = false;
      if(blacklist)
        for(unsigned int kk = 0; kk < blacklist -> size(); ++kk)
          if(blacklist -> at(kk) == static_cast<int>(j)) skipJet2 = true;
      if(skipJet2) continue;
      
      
      
      // -------------------------------------
      // select jets with different techniques
      // -------------------------------------
      
      if(method == "maxDeta")
      {
        tempDeta = deltaEta(jets.at(i).Eta(), jets.at(j).Eta());
        if( (tempDeta > maxDeta) && 
            (tempDeta > DetaMin) &&
            (tempDeta < DetaMax) )
        {
          maxDeta = tempDeta;
          
    	    it.at(0) = i;
	        it.at(1) = j;
        }
      }
      
      // -------------------------------------
      
      if(method == "maxMJJ")
      {
        tempMJJ = (jets.at(i) + jets.at(j)).mass();
        tempDeta = deltaEta(jets.at(i).Eta(), jets.at(j).Eta());
        
        if( (tempMJJ > maxMJJ) &&
            (tempDeta > DetaMin) &&
            (tempDeta < DetaMax) )
        {
          maxMJJ = tempMJJ;
          
    	    it.at(0) = i;
	        it.at(1) = j;
        }
      }
      
      // -------------------------------------
      else if(method == "maxPt")
      {
        tempPt = sqrt( (jets.at(i) + jets.at(j)).perp2() );
        tempDeta = deltaEta(jets.at(i).Eta(), jets.at(j).Eta());
        if( (tempPt > maxPt) &&
            (tempDeta > DetaMin) &&
            (tempDeta < DetaMax) )
        {
          maxPt = tempPt;
          
	        it.at(0) = i;
	        it.at(1) = j;
        }
      }
      
      // -------------------------------------
      
      else if(method == "maxSumPt")
      {
        tempSumPt = sqrt(jets.at(i).perp2()) + sqrt(jets.at(j).perp2());
        tempDeta = deltaEta(jets.at(i).Eta(), jets.at(j).Eta());
        if( (tempSumPt > maxSumPt) &&
            (tempDeta > DetaMin) &&
            (tempDeta < DetaMax) )
        {
          maxSumPt = tempSumPt;
          
   	      it.at(0) = i;
	        it.at(1) = j;
        }
      }
      
      // -------------------------------------
      
      
      
    } // loop over 2nd jet
  } // loop over 1st jet
  
  
  
  if(method == "maxMJJ")
    return maxMJJ;
  
  else if(method == "maxPt")
    return maxPt;
  
  else if(method == "maxSumPt")
    return maxSumPt;
  
  else return -1.;
}

//  ------------------------------------------------------------

int SelectLepton(std::vector<ROOT::Math::XYZTVector>& leptons,
                 const std::string& method,
                 const double& ptMin,
                 const std::vector<int>* blacklist)
{
  // initialize variable with result
  int it = -1;
  
  
  
  // initialize the selection variable
  double maxPt = -999999.;
  double tempPt = 0.;
  
  
  
  // loop over leptons
  for(unsigned int i = 0; i < leptons.size(); ++i)
  {
    if( sqrt(leptons.at(i).Perp2()) < ptMin ) continue;
    
    bool skipLep = false;
    if(blacklist)
      for(unsigned int kk = 0; kk < blacklist -> size(); ++kk)
        if(blacklist -> at(kk) == static_cast<int>(i)) skipLep = true;
    if(skipLep) continue;
    
    
    // -------------------------------------
    // select jets with different techniques
    // -------------------------------------
    
    if(method == "maxPt")
    {
      tempPt = sqrt(leptons.at(i).perp2());
      if(tempPt > maxPt)
      {
        maxPt = tempPt;
        
        it = i;
      }
    }
    
    // -------------------------------------
    
    
    
  } // loop over leptons
  
  
  
  if(method == "maxPt")
    return it;
  
  else return -1;
}

//  ------------------------------------------------------------






double SelectTagJets(std::vector<int>& it, std::vector<ROOT::Math::XYZTVector>& jets,
                     const double& etMin,
                     const double& DetaMIN,
                     const double& mjjMIN,
                     const std::vector<int>* blacklist)
{
  // initialize vector with result
  it.clear();
  it.push_back(-1);
  it.push_back(-1);
  
  
  
  // initialize the selection variable
  double maxMjj = -999999.;
  double tempMjj = 0.;
  double tempDeta = 0.;
  double prodEta = 0.;
  
  
  
  // loop over 1st jet
  for(unsigned int i = 0; i < jets.size(); ++i)
  {
    if(jets.at(i).Et() < etMin) continue;
    
    bool skipJet1 = false;
    if(blacklist)
      for(unsigned int kk = 0; kk < blacklist -> size(); ++kk)
        if(blacklist -> at(kk) == static_cast<int>(i)) skipJet1 = true;
    if(skipJet1) continue;
    
    
    
    // loop over 2nd jet
    for(unsigned int j = i+1; j < jets.size(); ++j)
    {
      if(jets.at(j).Et() < etMin) continue;
      
      bool skipJet2 = false;
      if(blacklist)
        for(unsigned int kk = 0; kk < blacklist -> size(); ++kk)
          if(blacklist -> at(kk) == static_cast<int>(j)) skipJet2 = true;
      if(skipJet2) continue;
      
      
      
      // -------------------------------------
      // select jets with different techniques
      // -------------------------------------
      
      tempMjj = (jets.at(i) + jets.at(j)).mass();
      tempDeta = deltaEta(jets.at(i).eta(), jets.at(j).eta());
      prodEta = jets.at(i).eta() * jets.at(j).eta(); 
      
      if( (tempMjj > maxMjj) && 
          (tempDeta > DetaMIN) &&
          (tempMjj > mjjMIN) && 
          (prodEta < 0.) )
      {
        maxMjj = tempMjj;
        
        it.at(0) = i;
        it.at(1) = j;
      }
      
      
      
    } // loop over 2nd jet
  } // loop over 1st jet
  
  
  
  return maxMjj;
}






double SelectTagJet(std::vector<int>& it, std::vector<ROOT::Math::XYZTVector>& jets,
                    const double& etMin,
                    const double& etaMin,
                    const std::vector<int>* blacklist)
{
  // initialize vector with result
  it.clear();
  it.push_back(-1);
  it.push_back(-1);
  
  
  
  // initialize the selection variable
  double maxAbsEta = -999999.;
  double tempAbsEta = 0.;
  
  
  // loop over jets
  for(unsigned int i = 0; i < jets.size(); ++i)
  {
    if(jets.at(i).Et() < etMin) continue;
    if(fabs(jets.at(i).eta()) < etaMin) continue;
    
    bool skipJet1 = false;
    if(blacklist)
      for(unsigned int kk = 0; kk < blacklist -> size(); ++kk)
        if(blacklist -> at(kk) == static_cast<int>(i)) skipJet1 = true;
    if(skipJet1) continue;
    
    
    
    // -------------------------------------
    // select jets with different techniques
    // -------------------------------------
    
    tempAbsEta = fabs(jets.at(i).eta());
    if( tempAbsEta > maxAbsEta )
    {
      maxAbsEta = tempAbsEta;
      
      it.at(0) = i;
    }
      
      
      
  } // loop over jets
  
  
  
  return maxAbsEta;
}






double Select4Jets(std::vector<int>& it_W, std::vector<int>& it_tag, 
                   std::vector<ROOT::Math::XYZTVector>& jets,
                   const std::string& method,
                   const double& etMin,
                   const double& etaMAX,
                   const double& DetaMAX,
                   const double& mjjMAX)
{
  // initialize vector with result
  it_W.clear();
  it_tag.clear();
  it_W.push_back(-1);
  it_W.push_back(-1);
  it_tag.push_back(-1);
  it_tag.push_back(-1);
  
  ROOT::Math::XYZTVector jet1_W;
  ROOT::Math::XYZTVector jet2_W;
  ROOT::Math::XYZTVector jet1_tag;
  ROOT::Math::XYZTVector jet2_tag;
  
  
  
  // initialize the selection variable
  double tempDeta = 0.;
  double minDeta = 999999999999.; 
  double tempDeta_W = 0.; 
  double tempMjj_W = 0.; 
  double tempDeta_tag = 0.; 
  double tempMjj_tag = 0.; 
  
  
  
  // loop over 1st jet
  for(unsigned int i = 0; i < jets.size(); ++i)
  {
    if(jets.at(i).Et() < etMin) continue;
    if(fabs(jets.at(i).eta()) > etaMAX) continue;
    
    
    // loop over 2nd jet
    for(unsigned int j = i+1; j < jets.size(); ++j)
    {
      if(jets.at(j).Et() < etMin) continue;
      if(fabs(jets.at(j).eta()) > etaMAX) continue;      
      
      
      jet1_W = jets.at(i);
      jet2_W = jets.at(j);
      tempDeta_W = deltaEta(jet1_W.eta(), jet2_W.eta());
      tempMjj_W = (jet1_W + jet2_W).mass();
      
      tempDeta = (tempDeta_W / 2.5);
      
      //std::cout << "1st jet: " << i << "   pt = " << jet1_W.pt() << "   eta = " << jet1_W.eta() << std::endl;
      //std::cout << "2nd jet: " << j << "   pt = " << jet2_W.pt() << "   eta = " << jet2_W.eta() << std::endl;
      //std::cout << "mJJ = " << (jet1_W+jet2_W).mass() << "   Deta = " << deltaEta(jet1_W.eta(),jet2_W.eta()) << std::endl; 
      bool isTagJetFound = false;
      
      
      // loop over 3rd jet
      for(unsigned int k = 0; k < jets.size(); ++k)
      {
        if(k == i) continue;
        if(k == j) continue;
        if(jets.at(k).Et() < etMin) continue;
        
        
        // loop over 4th jet
        for(unsigned int l = k+1; l < jets.size(); ++l)
        {
          if(l == i) continue;
          if(l == j) continue;          
          if(jets.at(l).Et() < etMin) continue;
          
          
          jet1_tag = jets.at(k);
          jet2_tag = jets.at(l);
          tempDeta_tag = deltaEta(jet1_tag.eta(), jet2_tag.eta());          
          tempMjj_tag = (jet1_tag + jet2_tag).mass();
          
          tempDeta += 1. / (tempMjj_tag / 80.399);
	  
          //std::cout << "3rd jet: " << k << "   pt = " << jet1_tag.pt() << "   eta = " << jet1_tag.eta() << std::endl;
          //std::cout << "4th jet: " << l << "   pt = " << jet2_tag.pt() << "   eta = " << jet2_tag.eta() << std::endl;
          //std::cout << "mJJ = " << (jet1_tag+jet2_tag).mass() << "   Deta = " << deltaEta(jet1_tag.eta(),jet2_tag.eta()) << std::endl;           

          if(method == "minDeta")
          {
            if( (tempDeta < minDeta) &&
                (tempDeta_W < DetaMAX) &&
                (tempMjj_W < mjjMAX) )
            {
              minDeta = tempDeta;
	      //std::cout << "trovati W e tag con " << minDeta << std::endl; 
              it_W.at(0) = i;
              it_W.at(1) = j;
              it_tag.at(0) = k;
              it_tag.at(1) = l;
              
              isTagJetFound = true;
            }
          }
          
        }
      }
      
      
      
      if( (method == "minDeta") && (isTagJetFound == false ) )
      {
        if( (tempDeta < minDeta) &&
            (tempDeta_W < DetaMAX) &&
            (tempMjj_W < mjjMAX) )
        {
          minDeta = tempDeta;
          //std::cout << "trovato W con " << minDeta << std::endl; 
          it_W.at(0) = i;
          it_W.at(1) = j;
          it_tag.at(0) = -1;
          it_tag.at(1) = -1;
        }
      }
      
    } // loop over 2nd jet
  } // loop over 1st jet
  
  
  
  return minDeta;
}







double SelectWJets(std::vector<int>& it, std::vector<ROOT::Math::XYZTVector>& jets,
                   const std::string& method,
                   const double& etMin,
                   const double& etaMAX,
                   const double& DetaMAX,
                   const double& mjjMAX,
                   const std::vector<int>* blacklist)
{
  // initialize vector with result
  it.clear();
  it.push_back(-1);
  it.push_back(-1);
  
  
  
  // initialize the selection variable
  double minDeta = 999999.;
  double maxSumPt = -999999.;
  double minDMjj = 999999.;
  double tempSumPt = 0.;
  double tempMjj = 0.;
  double tempDMjj = 0.;
  double tempDeta = 0.; 
  
  
  
  // loop over 1st jet
  for(unsigned int i = 0; i < jets.size(); ++i)
  {
    if(jets.at(i).Et() < etMin) continue;
    if(fabs(jets.at(i).eta()) > etaMAX) continue;
    
    bool skipJet1 = false;
    if(blacklist)
      for(unsigned int kk = 0; kk < blacklist -> size(); ++kk)
        if(blacklist -> at(kk) == static_cast<int>(i)) skipJet1 = true;
    if(skipJet1) continue;
    
    
    
    // loop over 2nd jet
    for(unsigned int j = i+1; j < jets.size(); ++j)
    {
      if(jets.at(j).Et() < etMin) continue;
      if(fabs(jets.at(j).eta()) > etaMAX) continue;      

      bool skipJet2 = false;
      if(blacklist)
        for(unsigned int kk = 0; kk < blacklist -> size(); ++kk)
          if(blacklist -> at(kk) == static_cast<int>(j)) skipJet2 = true;
      if(skipJet2) continue;
      
      
      
      // -------------------------------------
      // select jets with different techniques
      // -------------------------------------
      tempSumPt = jets.at(i).pt() + jets.at(j).pt();
      tempDeta = deltaEta(jets.at(i).Eta(), jets.at(j).Eta());
      tempMjj = (jets.at(i) + jets.at(j)).mass();
      tempDMjj = fabs((jets.at(i) + jets.at(j)).mass() - 80.399);
      
      if(method == "maxSumPt")
      {
        if( (tempSumPt > maxSumPt) &&
            (tempDeta < DetaMAX) &&
            (tempMjj < mjjMAX) )
        {
          maxSumPt = tempSumPt;
          
          it.at(0) = i;
          it.at(1) = j;
        }
      }
      
      if(method == "minDeta")
      {  
        if( (tempDeta < minDeta) &&
            (tempDeta < DetaMAX) &&
            (tempMjj < mjjMAX) )
        {
          minDeta = tempDeta;
          
          it.at(0) = i;
          it.at(1) = j;
        }
      }
      
      if(method == "minDMjj")
      {  
        if( (tempDMjj < minDMjj) &&
            (tempDeta < DetaMAX) &&
            (tempMjj < mjjMAX) )
        {
          minDMjj = tempDMjj;
          
          it.at(0) = i;
          it.at(1) = j;
        }
      }
      
      
      
    } // loop over 2nd jet
  } // loop over 1st jet
  
  
  
  return maxSumPt;
}











int Build4JetCombinations(std::vector<std::vector<int> >& combinations, const int& nJets)
{
  combinations.clear();
  
  std::vector<int> vi;
  for(int i = 0; i < nJets; ++i)
 	  vi.push_back(i);
  
  std::vector<int> buffer;
  buffer.push_back(0);
  buffer.push_back(1);
  buffer.push_back(2);
  buffer.push_back(3);
  
  combinations.push_back(buffer);
  

  std::vector<int> oldCombination = buffer;
  while( next_permutation(vi.begin(), vi.end()) )      
  {
    if( (vi.at(0) < vi.at(1)) && (vi.at(2) < vi.at(3)) )
    {
      buffer.at(0) = vi.at(0);
      buffer.at(1) = vi.at(1);
      buffer.at(2) = vi.at(2);
      buffer.at(3) = vi.at(3);                  
      
      if(buffer == oldCombination) continue;
      
      combinations.push_back(buffer);
      oldCombination = buffer;
    }  
  }
  
  return combinations.size();
}

//  ------------------------------------------------------------

void Print4JetCombination(const std::vector<int>& combination)
{
  std::cout << "(" << combination.at(0) << "," << combination.at(1) << ")";
  std::cout << "   ---   ";
  std::cout << "(" << combination.at(2) << "," << combination.at(3) << ")";  
  std::cout << std::endl;
}

//  ------------------------------------------------------------







void Print4V(const ROOT::Math::XYZTVector& p)
{
  std::cout << std::fixed << std::setprecision(2)
            << "(E,px,py,pz) = " 
            << "(" << std::setw(8) << p.energy() 
            << "," << std::setw(8) << p.px()
            << "," << std::setw(8) << p.py()
            << "," << std::setw(8) << p.pz()
            << ")" 
            << "\tpt = " 
            << std::setw(7) << p.pt()
            << "\teta = " 
            << std::setw(6) << p.eta()
            << "\tphi = " 
            << std::setw(5) << p.phi()
            << std::endl;
}

//  ------------------------------------------------------------






bool GetElectronFlag(const std::string& flag)
{
  //std::cout << "flag = " << flag << std::endl;
  std::stringstream ss(flag);
  
  if( flag == "0" ) return true;
  else return false; 
}

bool GetElectronSeverityLevel(const std::string& severityLevel)
{
  //std::cout << "severityLevel = " << severityLevel << std::endl;
  
  if( severityLevel == "0" ) return true;
  else return false; 
}

// ----------- conversion function

float eta_to_theta (float eta){
  return 2 * atan (exp (-1 * eta)) ;
}
  
float theta_to_eta (float theta){
  return -1 * log (tan (0.5 * theta)) ;
}  
  
float polar_to_cart_r (float x, float y, float z){
  return sqrt (x*x + y*y + z*z) ;
}

float polar_to_cart_theta (float x, float y, float z){
  return acos (z/polar_to_cart_r (x,y,z)) ;
}

float polar_to_cart_phi (float x, float y, float z){
  return atan2 (y, x) ;
}

float cart_to_polar_x (float r, float theta, float phi){
  return r * sin (theta) * cos (phi) ;
}

float cart_to_polar_y (float r, float theta, float phi){
  return r * sin (theta) * sin (phi) ;
}

float cart_to_polar_z (float r, float theta, float phi){
  return r * cos (theta) ;
}

float hep_to_polar_x (float t, float eta, float phi){
  return t * cos (phi) ;
}

float hep_to_polar_y (float t, float eta, float phi){
  return t * sin (phi) ;
}

float hep_to_polar_z (float t, float eta, float phi){
  float theta = eta_to_theta (eta) ;
  if (theta == 0) return 0. ;
  else return t / tan (theta) ;
}


