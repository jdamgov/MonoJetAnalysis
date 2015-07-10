// Package: NtupleAnalyzer                                                    
// Class: NtupleAnalyzer
// Purpose: MonoJet Analysis / ntuple producer
//
// Created: 30 May 2010 (Mehmet Vergili)
// Ver06F (Paper): 10 Feb 2013 (Phat Srimanobhas)
// Ver07A (Parked, B+MET): 1 Nov 2014 (Phat Srimanobhas)
// Ver07B (MiniAOD in 72X): 16 Dec 2014 (Phat Srimanobhas) 

#include "NtupleAnalyzer.h"

#define TIVMAX     10000
#define MAXGENPAR  1000
#define MAXTAU     50
#define MAXMUON    30
#define MAXELEC    30
#define MAXJET     30
#define MAXMET     40
#define MAXPHOT    30
#define MAXVTX     200

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Tau.h"
#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"

#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/GenRunInfoProduct.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"     
//#include "Cintex/Cintex.h"

using namespace edm;
using namespace reco;
using namespace std;

class NtupleAnalyzer : public edm::EDAnalyzer 
{
public:
  explicit NtupleAnalyzer(const edm::ParameterSet&);
  ~NtupleAnalyzer();
  
private:
  virtual void beginJob();
  virtual void beginRun(const edm::Run&, const edm::EventSetup&);
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void endJob();
  void FirstValue();
  float deltaPhi(float v1, float v2);
  float deltaR(float eta1, float phi1, float eta2, float phi2);
  float checkPtMatch(float v1, float v2);
  
  ///------------------------------------------------
  /// member data
  ///------------------------------------------------
  Service<TFileService> fs;
  TTree *mtree; 
  map<string,TH1D*> histo1D;
  map<string,TH2D*> histo2D;
  
  ///------------------------------------------------
  /// Input Collections
  ///------------------------------------------------
  edm::InputTag TriggerTag_;
  double triggerUsed; 
  std::string hltPath1Name_;
  std::string hltPath2Name_;  
  //  
  edm::InputTag ElectronTag_; 
  edm::InputTag MuonTag_; 
  edm::InputTag TauTag_;
  edm::InputTag JetTag_;  
  edm::InputTag METTag_;  
  //
  edm::InputTag PFElectronTag_; 
  edm::InputTag PFMuonTag_; 
  edm::InputTag PFTauTag_;
  edm::InputTag PFJetTag_;  
  edm::InputTag PFJetAK8Tag_;  
  edm::InputTag PFMETTag_;
  //
  edm::InputTag puJetIdTag_;
  //
  double JetPtCutTag;
  double JetEtaCutTag;
  double METThresholdTag;
  double WeightTag;
  bool   isMCTag;
  double isSignalTag;
  bool   chooseOnlyGenZmumu;
  bool   chooseOnlyGenZnunu;

  //
  edm::InputTag Tracks_; 
  //
  int    debugMode;
  //
  bool   includeNonPFCollection;
  ///------------------------------------------------
  /// HLT  L1, L1Tech
  ///------------------------------------------------
  HLTConfigProvider hltConfig_;
  unsigned hltBit1_;
  unsigned hltBit2_;
  TriggerNames trgnm_;
  TriggerNames trgnm2_; 
  char    trgnm[15000];
  int     nHLT;
  int     nL1;
  int     nL1T;
  int     nNoiseFlag; 
  int     HLTArray[500];
  int     L1Array[128];
  int     L1TArray[64];
  int     HLTArray2[100];
  int     HLTPreScale[500];
  int     HLTPreScale2[100];
  int     nhlt; 
  
  ///------------------------------------------------
  /// Noise Flags
  ///------------------------------------------------
  int     NoiseFlag[25];
  int     flg_hnoise;
  int     flg_hfbadhit;
  int     flg_ecalspike;
  int     mbeamHaloLoose;
  int     mbeamHaloTight;
  double  mSumTrackPt;
  
  ///------------------------------------------------  
  /// Rho
  ///------------------------------------------------
  double  mfastJetRho;

  ///-------------------------------------------------------------------------- 
  /// PileUP Summury
  ///--------------------------------------------------------------------------
  int     mnpvm1;
  int     mnpv0;
  int     mnpvp1;
  
  ///------------------------------------------------
  /// Primary vertices
  ///------------------------------------------------
  double  mPVx[MAXVTX];
  double  mPVy[MAXVTX];
  double  mPVz[MAXVTX];
  double  mPVchi2[MAXVTX];
  double  mPVndof[MAXVTX]; 
  int	  mPVntracks[MAXVTX];
  int     mNPV;
  
  ///------------------------------------------------
  /// Run event Lumi BX
  ///------------------------------------------------
  int     mRun;
  int     mLumi;
  int     mBX;
  long    mEvent;
  
  ///------------------------------------------------
  /// Generator Information
  ///------------------------------------------------
  int     mNGenPar;
  int     mGenParIndex[MAXGENPAR]; 
  int     mGenParId[MAXGENPAR]; 
  int     mGenParStatus[MAXGENPAR]; 
  int     mGenParMother[MAXGENPAR]; 
  int     mGenParCharge[MAXGENPAR];
  int     mGenParDoughterOf[MAXGENPAR]; 
  int     mGenParMother1[MAXGENPAR]; 
  int     mGenParMother2[MAXGENPAR];
  //
  double  mGenParE[MAXGENPAR];
  double  mGenParPt[MAXGENPAR]; 
  double  mGenParPx[MAXGENPAR]; 
  double  mGenParPy[MAXGENPAR]; 
  double  mGenParPz[MAXGENPAR]; 
  double  mGenParEta[MAXGENPAR]; 
  double  mGenParPhi[MAXGENPAR]; 
  double  mGenParMass[MAXGENPAR];

  ///------------------------------------------------
  /// Generator Information: PDF
  ///------------------------------------------------
  double  mGenScale;
  double  mPdfscale;
  double  mPdfx1; 
  double  mPdfx2; 
  double  mPdfpdf1;
  double  mPdfpdf2; 
  double  mPdfWeights[150];
  double  mPdfWeights1[150];
  //
  int     mPdff1; 
  int     mPdff2;	
  int     mNPdfWeights;
  int     mNPdfWeights1;
  
  ///------------------------------------------------
  /// Generator Information: Tau decays
  ///------------------------------------------------
  int     mWTauDecayMode;
  int     mWTauN;
  int     mWTauDecayId[MAXGENPAR];  
  int     mWTauDecayStatus[MAXGENPAR]; 
  double  mWTauDecayPt[MAXGENPAR]; 
  double  mWTauDecayEta[MAXGENPAR];
  double  mWTauDecayPhi[MAXGENPAR];
  double  mWTauDecayMass[MAXGENPAR];
  
  ///------------------------------------------------
  /// Calo AK4 Jets
  ///------------------------------------------------
  double  mCaloAK4JetE[MAXJET];
  double  mCaloAK4JetPt[MAXJET];
  double  mCaloAK4JetPx[MAXJET];
  double  mCaloAK4JetPy[MAXJET];
  double  mCaloAK4JetPz[MAXJET]; 
  double  mCaloAK4JetEta[MAXJET];
  double  mCaloAK4JetPhi[MAXJET];
  double  mCaloAK4JetEmf[MAXJET];
  double  mCaloAK4JetfHPD[MAXJET];
  double  mCaloAK4JetfRBX[MAXJET];
  double  mCaloAK4JetN90Hits[MAXJET];
  int     mNCaloAK4Jets;  
  int     mCaloAK4JetN90[MAXJET]; 
  int     mCaloAK4JetNtrkCalo[MAXJET]; 
  int     mCaloAK4JetNtrkVtx[MAXJET];
  double  mCaloAK4JetSigEta[MAXJET];
  double  mCaloAK4JetSigPhi[MAXJET];
  double  mCaloAK4JetIDEmf[MAXJET];
  double  mCaloAK4JetECor[MAXJET];
  double  mCaloAK4JetPtCor[MAXJET];
  double  mCaloAK4JetPxCor[MAXJET];
  double  mCaloAK4JetPyCor[MAXJET];
  double  mCaloAK4JetPzCor[MAXJET]; 
  double  mCaloAK4TrackPt[MAXJET];	
  double  mCaloAK4JetBtagTkCountHighEff[MAXJET]; 
  double  mCaloAK4JetBTagCSVv2[MAXJET];
  double  mCaloAK4JetBTagCombSecVtx[MAXJET]; 
  int     mCaloAK4JetIDLOOSE[MAXJET]; 
  int     mCaloAK4JetIDTIGHT[MAXJET];

  ///------------------------------------------------
  /// Gen AK4 Jets (for Calo)
  ///------------------------------------------------
  double  mGenAK4JetE[MAXJET];
  double  mGenAK4JetPt[MAXJET];
  double  mGenAK4JetPx[MAXJET];
  double  mGenAK4JetPy[MAXJET];
  double  mGenAK4JetPz[MAXJET];
  double  mGenAK4JetEta[MAXJET];
  double  mGenAK4JetPhi[MAXJET];
  double  mGenAK4JetEmEnergy[MAXJET];
  double  mGenAK4JetHadEnergy[MAXJET];		
  //int     mNGenAK4Jets; 
  

  ///------------------------------------------------
  /// Gen AK8 Jets (for Calo)
  ///------------------------------------------------
  double  mGenAK8JetE[MAXJET];
  double  mGenAK8JetPt[MAXJET];
  double  mGenAK8JetPx[MAXJET];
  double  mGenAK8JetPy[MAXJET];
  double  mGenAK8JetPz[MAXJET];
  double  mGenAK8JetEta[MAXJET];
  double  mGenAK8JetPhi[MAXJET];
  double  mGenAK8JetEmEnergy[MAXJET];
  double  mGenAK8JetHadEnergy[MAXJET];		
  //int     mNGenAK8Jets; 
  
  ///------------------------------------------------
  /// PF AK4Jets
  ///------------------------------------------------
  int     mNPFAK4Jets;
  int     mPFAK4JetN90[MAXJET];
  int     mPFAK4JetNtrkPF[MAXJET];
  int     mPFAK4JetNtrkVtx[MAXJET];
  double  mPFAK4JetE[MAXJET];
  double  mPFAK4JetPt[MAXJET];
  double  mPFAK4JetPx[MAXJET];
  double  mPFAK4JetPy[MAXJET];
  double  mPFAK4JetPz[MAXJET];
  double  mPFAK4JetEta[MAXJET];
  double  mPFAK4JetPhi[MAXJET];
  double  mPFAK4JetfHPD[MAXJET];
  double  mPFAK4JetfRBX[MAXJET];
  double  mPFAK4JetN90Hits[MAXJET];
  double  mPFAK4JetSigEta[MAXJET];
  double  mPFAK4JetSigPhi[MAXJET];
  double  mPFAK4JetIDEmf[MAXJET];
  double  mPFAK4JetECor[MAXJET];
  double  mPFAK4JetPtCor[MAXJET];
  double  mPFAK4JetPxCor[MAXJET];
  double  mPFAK4JetPyCor[MAXJET];
  double  mPFAK4JetPzCor[MAXJET];
  //
  double  mPFAK4JetBtagTkCountHighEff[MAXJET];
  double  mPFAK4JetBtagTkCountHighPur[MAXJET];
  double  mPFAK4JetBtagJetProbability[MAXJET];
  double  mPFAK4JetBtagJetBProbability[MAXJET];
  double  mPFAK4JetBTagCSVv2[MAXJET];
  double  mPFAK4JetBTagCombSecVtx[MAXJET];
  double  mPFAK4JetBTagCombSecVtxMVA[MAXJET];
  //
  int     mPFAK4JetFlavour[MAXJET]; // algorithmic jet flavour definition
  int     mPFAK4JetGenPartonFlavour[MAXJET]; // flavour of the parton matched to jet (0 if no match)   
  //
  double  mPFAK4JetNeuEmEngFrac[MAXJET];
  double  mPFAK4JetChaEmEngFrac[MAXJET];
  double  mPFAK4JetChaHadEngFrac[MAXJET];
  double  mPFAK4JetNeuHadEngFrac[MAXJET];
  //
  double  mPFAK4JetChaEmEng[MAXJET];
  double  mPFAK4JetNeuEmEng[MAXJET];
  double  mPFAK4JetChaHadEng[MAXJET];
  double  mPFAK4JetNeuHadEng[MAXJET];
  double  mPFAK4JetPhotEng[MAXJET];
  double  mPFAK4JetElecEng[MAXJET];
  double  mPFAK4JetMuonEng[MAXJET];
  double  mPFAK4JetHFHadEng[MAXJET];
  double  mPFAK4JetHFEmEng[MAXJET];		
  double  mPFAK4JetChaMuEng[MAXJET];
  //
  int     mPFAK4JetNumOfChaMu[MAXJET];
  int     mPFAK4JetNumOfMuon[MAXJET];
  int     mPFAK4JetNumOfPhot[MAXJET];
  int     mPFAK4JetNumOfElec[MAXJET];		
  int     mPFAK4JetNumOfNeu[MAXJET];
  int     mPFAK4JetNumOfCha[MAXJET];
  int     mPFAK4JetNumOfNeuHad[MAXJET];
  int     mPFAK4JetNumOfChaHad[MAXJET];
  int     mPFAK4JetNumOfDaughters[MAXJET];
  int     mPFAK4JetIDLOOSE[MAXJET];
  int     mPFAK4JetIDTIGHT[MAXJET];
  double     mPFAK4JetPUFullJetId[MAXJET];
  int     mPFAK4JetNumOfHFHad[MAXJET];
  int     mPFAK4JetNumOfHFEM[MAXJET];
  //
  double  mPFAK4uncer[MAXJET];

  ///------------------------------------------------
  /// PF AK8Jets
  ///------------------------------------------------
  int     mNPFAK8Jets;
  int     mPFAK8JetN90[MAXJET];
  int     mPFAK8JetNtrkPF[MAXJET];
  int     mPFAK8JetNtrkVtx[MAXJET];
  double  mPFAK8JetE[MAXJET];
  double  mPFAK8JetPt[MAXJET];
  double  mPFAK8JetPx[MAXJET];
  double  mPFAK8JetPy[MAXJET];
  double  mPFAK8JetPz[MAXJET];
  double  mPFAK8JetEta[MAXJET];
  double  mPFAK8JetPhi[MAXJET];
  double  mPFAK8JetfHPD[MAXJET];
  double  mPFAK8JetfRBX[MAXJET];
  double  mPFAK8JetN90Hits[MAXJET];
  double  mPFAK8JetSigEta[MAXJET];
  double  mPFAK8JetSigPhi[MAXJET];
  double  mPFAK8JetIDEmf[MAXJET];
  double  mPFAK8JetECor[MAXJET];
  double  mPFAK8JetPtCor[MAXJET];
  double  mPFAK8JetPxCor[MAXJET];
  double  mPFAK8JetPyCor[MAXJET];
  double  mPFAK8JetPzCor[MAXJET];
  //
  double  mPFAK8JetBtagTkCountHighEff[MAXJET];
  double  mPFAK8JetBtagTkCountHighPur[MAXJET];
  double  mPFAK8JetBtagJetProbability[MAXJET];
  double  mPFAK8JetBtagJetBProbability[MAXJET];
  double  mPFAK8JetBTagCSVv2[MAXJET];
  double  mPFAK8JetBTagCombSecVtx[MAXJET];
  double  mPFAK8JetBTagCombSecVtxMVA[MAXJET];
  //
  int     mPFAK8JetFlavour[MAXJET]; // algorithmic jet flavour definition
  int     mPFAK8JetGenPartonFlavour[MAXJET]; // flavour of the parton matched to jet (0 if no match)   
  //
  double  mPFAK8JetNeuEmEngFrac[MAXJET];
  double  mPFAK8JetChaEmEngFrac[MAXJET];
  double  mPFAK8JetChaHadEngFrac[MAXJET];
  double  mPFAK8JetNeuHadEngFrac[MAXJET];
  //
  double  mPFAK8JetChaEmEng[MAXJET];
  double  mPFAK8JetNeuEmEng[MAXJET];
  double  mPFAK8JetChaHadEng[MAXJET];
  double  mPFAK8JetNeuHadEng[MAXJET];
  double  mPFAK8JetPhotEng[MAXJET];
  double  mPFAK8JetElecEng[MAXJET];
  double  mPFAK8JetMuonEng[MAXJET];
  double  mPFAK8JetHFHadEng[MAXJET];
  double  mPFAK8JetHFEmEng[MAXJET];		
  double  mPFAK8JetChaMuEng[MAXJET];
  //
  int     mPFAK8JetNumOfChaMu[MAXJET];
  int     mPFAK8JetNumOfMuon[MAXJET];
  int     mPFAK8JetNumOfPhot[MAXJET];
  int     mPFAK8JetNumOfElec[MAXJET];		
  int     mPFAK8JetNumOfNeu[MAXJET];
  int     mPFAK8JetNumOfCha[MAXJET];
  int     mPFAK8JetNumOfNeuHad[MAXJET];
  int     mPFAK8JetNumOfChaHad[MAXJET];
  int     mPFAK8JetNumOfDaughters[MAXJET];
  int     mPFAK8JetIDLOOSE[MAXJET];
  int     mPFAK8JetIDTIGHT[MAXJET];
  double     mPFAK8JetPUFullJetId[MAXJET];
  int     mPFAK8JetNumOfHFHad[MAXJET];
  int     mPFAK8JetNumOfHFEM[MAXJET];
  //
  double  mPFAK8uncer[MAXJET];
  
  ///------------------------------------------------
  /// Gen AK4 Jets (for PF)
  ///------------------------------------------------
  double  mGenPFAK4JetE[MAXJET];
  double  mGenPFAK4JetPt[MAXJET];
  double  mGenPFAK4JetPx[MAXJET];
  double  mGenPFAK4JetPy[MAXJET];
  double  mGenPFAK4JetPz[MAXJET];
  double  mGenPFAK4JetEta[MAXJET];
  double  mGenPFAK4JetPhi[MAXJET];
  double  mGenPFAK4JetEmEnergy[MAXJET];
  double  mGenPFAK4JetHadEnergy[MAXJET];
  //int     mNGenPFAK4Jets; 
  //
  double  mPFAK4JERCentral[MAXJET];
  double  mPFAK4JERUp[MAXJET];
  double  mPFAK4JERDown[MAXJET];
  
  ///------------------------------------------------
  /// Gen AK8 Jets (for PF)
  ///------------------------------------------------
  double  mGenPFAK8JetE[MAXJET];
  double  mGenPFAK8JetPt[MAXJET];
  double  mGenPFAK8JetPx[MAXJET];
  double  mGenPFAK8JetPy[MAXJET];
  double  mGenPFAK8JetPz[MAXJET];
  double  mGenPFAK8JetEta[MAXJET];
  double  mGenPFAK8JetPhi[MAXJET];
  double  mGenPFAK8JetEmEnergy[MAXJET];
  double  mGenPFAK8JetHadEnergy[MAXJET];
  //int     mNGenPFAK8Jets; 
  //
  double  mPFAK8JERCentral[MAXJET];
  double  mPFAK8JERUp[MAXJET];
  double  mPFAK8JERDown[MAXJET];
  
  ///------------------------------------------------
  /// MET
  ///------------------------------------------------
  int     mNMet;
  double  mMetPt[MAXMET];
  double  mMetPhi[MAXMET];
  double  mMetSumEt[MAXMET];
  double  mMetPx[MAXMET];
  double  mMetPy[MAXMET];
  double  mMetSign[MAXMET];
  
  ///------------------------------------------------
  /// MHT
  ///------------------------------------------------
  double  mMHTPt;
  double  mMHTPhi;
  double  mMHTPx;
  double  mMHTPy;
  double  mMHTSign;
  
  ///------------------------------------------------
  /// Muons
  ///------------------------------------------------
  int     mNMuon;
  double  mMuonPt[MAXMUON];
  double  mMuonEt[MAXMUON];
  double  mMuonE[MAXMUON];
  double  mMuonPx[MAXMUON];
  double  mMuonPy[MAXMUON];
  double  mMuonPz[MAXMUON];
  double  mMuonEta[MAXMUON];
  double  mMuonPhi[MAXMUON];
  double  mMuonCharge[MAXMUON];  			  
  int	  mMuonIsGlobal[MAXMUON];
  int	  mMuonIsStandAlone[MAXMUON];
  int	  mMuonIsTracker[MAXMUON];			  
  double  mMuonSumPtDR03[MAXMUON];
  double  mMuonSumPtDR05[MAXMUON];
  double  mMuonEmEtDR03[MAXMUON];
  double  mMuonHadEtDR03[MAXMUON];
  int     mMuonNumOfMatches[MAXMUON];
  int     mMuonIsTight[MAXMUON];  
  int     mMuonIsLoose[MAXMUON];
  // comb muon	
  double  mMuonCombChi2Norm[MAXMUON];  
  int	  mMuonCombValidHits[MAXMUON];  
  int	  mMuonCombLostHits[MAXMUON];
  double  mMuonCombPt[MAXMUON]; 
  double  mMuonCombPz[MAXMUON]; 
  double  mMuonCombP[MAXMUON]; 
  double  mMuonCombEta[MAXMUON]; 
  double  mMuonCombPhi[MAXMUON]; 
  double  mMuonCombChi2[MAXMUON]; 
  double  mMuonCombCharge[MAXMUON]; 
  double  mMuonCombQOverPError[MAXMUON]; 
  double  mMuonCombNdof[MAXMUON]; 
  double  mMuonCombVx[MAXMUON]; 
  double  mMuonCombVy[MAXMUON]; 
  double  mMuonCombVz[MAXMUON]; 
  double  mMuonCombD0[MAXMUON]; 
  double  mMuonCombDz[MAXMUON]; 
  // standalone muon	
  double  mMuonStandChi2Norm[MAXMUON];
  int	  mMuonStandValidHits[MAXMUON];
  int	  mMuonStandLostHits[MAXMUON];
  double  mMuonStandPt[MAXMUON];
  double  mMuonStandPz[MAXMUON];
  double  mMuonStandP[MAXMUON];
  double  mMuonStandEta[MAXMUON];
  double  mMuonStandPhi[MAXMUON];
  double  mMuonStandChi2[MAXMUON];
  double  mMuonStandCharge[MAXMUON];
  double  mMuonStandQOverPError[MAXMUON];
  // track muon	
  double  mMuonTrkChi2Norm[MAXMUON]; 
  int	  mMuonTrkValidHits[MAXMUON];
  int	  mMuonTrkLostHits[MAXMUON];
  double  mMuonTrkPt[MAXMUON]; 
  double  mMuonTrkPz[MAXMUON]; 
  double  mMuonTrkP[MAXMUON]; 
  double  mMuonTrkEta[MAXMUON]; 
  double  mMuonTrkPhi[MAXMUON]; 
  double  mMuonTrkChi2[MAXMUON]; 
  double  mMuonTrkCharge[MAXMUON]; 
  double  mMuonTrkQOverPError[MAXMUON]; 
  double  mMuonTrkDxy[MAXMUON]; 
  int     mMuonTrkNumOfValidPixHits[MAXMUON];
  
  ///------------------------------------------------
  /// PFMuons
  ///------------------------------------------------
  int     mNPFMuon;
  double  mPFMuonPt[MAXMUON];               
  double  mPFMuonEt[MAXMUON];               
  double  mPFMuonE[MAXMUON];                
  double  mPFMuonPx[MAXMUON];               
  double  mPFMuonPy[MAXMUON];               
  double  mPFMuonPz[MAXMUON];               
  double  mPFMuonEta[MAXMUON];              
  double  mPFMuonPhi[MAXMUON];              
  int     mPFMuonCharge[MAXMUON];           
  //
  int     mPFMuonIsGlobal[MAXMUON];         
  int     mPFMuonIsStandAlone[MAXMUON];     
  int     mPFMuonIsTracker[MAXMUON];  
  int     mPFMuonIsPF[MAXMUON];
  int     mPFMuonIsTight[MAXMUON];  
  int     mPFMuonIsLoose[MAXMUON];   
  //
  double  mPFMuonChargedHadronIso[MAXMUON]; 
  double  mPFMuonPhotonIso[MAXMUON];        
  double  mPFMuonNeutralHadronIso[MAXMUON]; 
  int     mPFMuonisGMPT[MAXMUON];           
  double  mPFMuonR04ChargedHadronPt[MAXMUON];
  double  mPFMuonR04NeutralHadronEt[MAXMUON];
  double  mPFMuonR04PhotonEt[MAXMUON];
  double  mPFMuonR04PUPt[MAXMUON];
  //
  int     mPFMuonNumOfMatches[MAXMUON];
  int     mPFMuonNumOfMatchesStation[MAXMUON];
  //
  double  mPFMuonGlobChi2Norm[MAXMUON];
  int     mPFMuonGlobNumOfValidMuonHits[MAXMUON];
  //
  double  mPFMuonBestTrkdxy[MAXMUON];
  double  mPFMuonBestTrkdz[MAXMUON];
  //
  double  mPFMuoninnertrackPt[MAXMUON];     
  int     mPFMuonnValidHits[MAXMUON];       
  int     mPFMuonnValidPixelHits[MAXMUON];  
  double  mPFMuondxy[MAXMUON];
  double  mPFMuondz[MAXMUON];     
  //
  double  mPFMuonCombChi2Norm[MAXMUON];
  int     mPFMuonCombValidHits[MAXMUON];
  int     mPFMuonCombLostHits[MAXMUON];    
  double  mPFMuonCombPt[MAXMUON];          
  double  mPFMuonCombPz[MAXMUON];          
  double  mPFMuonCombP[MAXMUON];           
  double  mPFMuonCombEta[MAXMUON];         
  double  mPFMuonCombPhi[MAXMUON];         
  double  mPFMuonCombChi2[MAXMUON];        
  double  mPFMuonCombCharge[MAXMUON];      
  double  mPFMuonCombQOverPError[MAXMUON]; 	
  double  mPFMuonCombNdof[MAXMUON];        
  double  mPFMuonCombVx[MAXMUON];          
  double  mPFMuonCombVy[MAXMUON];          
  double  mPFMuonCombVz[MAXMUON];          
  double  mPFMuonCombD0[MAXMUON];          
  double  mPFMuonCombDz[MAXMUON];          
  //
  double  mPFMuonStandChi2Norm[MAXMUON];   
  int     mPFMuonStandValidHits[MAXMUON];  
  int     mPFMuonStandLostHits[MAXMUON];   
  double  mPFMuonStandPt[MAXMUON];         
  double  mPFMuonStandPz[MAXMUON];         
  double  mPFMuonStandP[MAXMUON];          
  double  mPFMuonStandEta[MAXMUON];        
  double  mPFMuonStandPhi[MAXMUON];        
  double  mPFMuonStandChi2[MAXMUON];       
  double  mPFMuonStandCharge[MAXMUON];     
  double  mPFMuonStandQOverPError[MAXMUON];
  //
  double  mPFMuonTrkChi2Norm[MAXMUON];    
  int     mPFMuonTrkValidHits[MAXMUON];   
  int     mPFMuonTrkLostHits[MAXMUON];    
  double  mPFMuonTrkPt[MAXMUON];          
  double  mPFMuonTrkPz[MAXMUON];          
  double  mPFMuonTrkP[MAXMUON];           
  double  mPFMuonTrkEta[MAXMUON];         
  double  mPFMuonTrkPhi[MAXMUON];         
  double  mPFMuonTrkChi2[MAXMUON];        
  double  mPFMuonTrkCharge[MAXMUON];      
  double  mPFMuonTrkQOverPError[MAXMUON]; 
  double  mPFMuonTrkDxy[MAXMUON];         
  double  mPFMuonTrkNumOfValidPixHits[MAXMUON];
  int     mPFMuonTrkLayerWithMeasurement[MAXMUON];
  
  ///------------------------------------------------
  /// Electrons
  ///------------------------------------------------
  int     mElecEcalDrivenSeed[MAXELEC];        
  int     mElecID80[MAXELEC];            
  int     mElecID95[MAXELEC];   
  int     mElecdB[MAXELEC];              
  int     mElecIsEB[MAXELEC]; 
  int     mElecCharge[MAXELEC];   
  int     mElecNumOfValidHits[MAXELEC];  
  int     mElecInnerNumOfHits[MAXELEC];  
  int     mNElec;
  //
  int     mElecIsTight[MAXELEC];
  int     mElecIsMedium[MAXELEC];
  int     mElecIsLoose[MAXELEC];
  int     mElecIsVeto[MAXELEC];
  //
  double  mElecE[MAXELEC];               
  double  mElecPt[MAXELEC];              
  double  mElecPx[MAXELEC];              
  double  mElecPy[MAXELEC];              
  double  mElecPz[MAXELEC];              
  double  mElecEta[MAXELEC];             
  double  mElecPhi[MAXELEC];             
  double  mElecdr03HcalTowerSumEt[MAXELEC]; 
  double  mElecdr03EcalRecHitSumEt[MAXELEC]; 
  double  mElecdr03TkSumPt[MAXELEC];     
  double  mElecdist[MAXELEC];            
  double  mElecdcot[MAXELEC];            
  double  mElecNormChi2[MAXELEC];        
  double  mElecdxy[MAXELEC];             
  double  mElecdz[MAXELEC];                         
  double  mElecfbrem[MAXELEC];           
  double  mElecDEtaSuTrAtVtx[MAXELEC];   
  double  mElecDPhiSuTrAtVtx[MAXELEC];   
  double  mElecHadronicOverEm[MAXELEC];
  double  mElecHcalOverEcal[MAXELEC];    
  double  mElecSuperClusOverP[MAXELEC];  
  double  mElecetaeta[MAXELEC];          
  double  mElecietaieta[MAXELEC];  
  
  ///------------------------------------------------
  /// PFElectrons
  ///------------------------------------------------
  int     mNPFElec;
  int     mPFElecCharge[MAXELEC]; 
  int     mPFElecIsEB[MAXELEC];  
  //
  int     mPFElecIsTight[MAXELEC];
  int     mPFElecIsMedium[MAXELEC];
  int     mPFElecIsLoose[MAXELEC];
  int     mPFElecIsVeto[MAXELEC];
  //
  double  mPFElecE[MAXELEC];           
  double  mPFElecPt[MAXELEC];          
  double  mPFElecPx[MAXELEC];          
  double  mPFElecPy[MAXELEC];          
  double  mPFElecPz[MAXELEC];          
  double  mPFElecEta[MAXELEC];         
  double  mPFElecPhi[MAXELEC];         
  double  mPFElecCharHadIso[MAXELEC];  
  double  mPFElecPhoIso[MAXELEC];      
  double  mPFElecNeuHadIso[MAXELEC];   
  double  mPFElecMva[MAXELEC]; 
  double  mPFElecEffArea[MAXELEC];
  //
  double  mPFElecdxy[MAXELEC];         
  double  mPFElecdz[MAXELEC];          
  double  mPFElecHadOverEm[MAXELEC];   
  double  mPFElecHcalOverEm[MAXELEC];  
  double  mPFElecSupClusOverP[MAXELEC];
  double  mPFElecInnerHits[MAXELEC];   
  double  mPFElecConvDist[MAXELEC];    
  double  mPFElecConvDcot[MAXELEC];
  //
  int     mPFElecEcalDrivenSeed[MAXELEC]; 
  int     mPFElecdB[MAXELEC];  
  int     mPFElecNumOfValidHits[MAXELEC];  
  double  mPFElecdr03HcalTowerSumEt[MAXELEC]; 
  double  mPFElecdr03EcalRecHitSumEt[MAXELEC]; 
  double  mPFElecdr03TkSumPt[MAXELEC]; 
  double  mPFElecNormChi2[MAXELEC];    
  double  mPFElecfbrem[MAXELEC];           
  double  mPFElecDEtaSuTrAtVtx[MAXELEC];   
  double  mPFElecDPhiSuTrAtVtx[MAXELEC];   
  double  mPFElecHadronicOverEm[MAXELEC];
  double  mPFElecHcalOverEcal[MAXELEC];    
  double  mPFElecSuperClusOverP[MAXELEC];  
  double  mPFElecetaeta[MAXELEC];          
  double  mPFElecietaieta[MAXELEC];  
  //
  int     mPFeidLoose[MAXELEC];              
  int     mPFeidRobustHighEnergy[MAXELEC];   
  int     mPFeidRobustLoose[MAXELEC];        
  int     mPFeidRobustTight[MAXELEC];        
  int     mPFeidTight[MAXELEC];              
  int     mPFevspi[MAXELEC];                 
  int     mPFevsmu[MAXELEC];                 
  
  ///------------------------------------------------
  /// Taus
  ///------------------------------------------------
  int     mNTau;
  int     mTauCharge[MAXTAU];
  double  mTauE[MAXTAU]; 
  double  mTauPt[MAXTAU]; 
  double  mTauPx[MAXTAU]; 
  double  mTauPy[MAXTAU]; 
  double  mTauPz[MAXTAU]; 
  double  mTauEta[MAXTAU]; 
  double  mTauPhi[MAXTAU];
  double  mTauEtaEtaMoment[MAXTAU]; 
  double  mTauPhiPhiMoment[MAXTAU]; 
  double  mTauEtaPhiMoment[MAXTAU]; 			
  double  mTauLeadPFChargedHadrCandsignedSipt[MAXTAU];
  double  mTauIsoPFChargedHadrCandsPtSum[MAXTAU];
  double  mTauIsoPFGammaCandsEtSum[MAXTAU];      
  double  mTauMaximumHCALPFClusterEt[MAXTAU];    
  double  mTauEmFraction[MAXTAU];                
  double  mTauHcalTotOverPLead[MAXTAU];          
  double  mTauHcalMaxOverPLead[MAXTAU];          
  double  mTauHcal3x3OverPLead[MAXTAU];          
  double  mTauEcalStripSumEOverPLead[MAXTAU];    
  double  mTauBremsRecoveryEOverPLead[MAXTAU];         
  double  mTauElectronPreIDOutput[MAXTAU];       
  double  mTauElectronPreIDDecision[MAXTAU];     
  double  mTauCaloComp[MAXTAU];                  
  double  mTauSegComp[MAXTAU];                   
  double  mTauMuonDecision[MAXTAU]; 
  //
  double  mTausignalPFChargedHadrCands[MAXTAU];
  double  mTausignalPFGammaCands[MAXTAU];
  //
  double  mTauDisAgainstElectronDeadECAL[MAXTAU];
  double  mTauDisAgainstElectronLoose[MAXTAU];
  double  mTauDisAgainstElectronLooseMVA2[MAXTAU];
  double  mTauDisAgainstElectronLooseMVA3[MAXTAU];
  double  mTauDisAgainstElectronMVA[MAXTAU];
  double  mTauDisAgainstElectronMVA2category[MAXTAU];
  double  mTauDisAgainstElectronMVA2raw[MAXTAU];
  double  mTauDisAgainstElectronMVA3category[MAXTAU];
  double  mTauDisAgainstElectronMVA3raw[MAXTAU];
  double  mTauDisAgainstElectronMedium[MAXTAU];
  double  mTauDisAgainstElectronMediumMVA2[MAXTAU];
  double  mTauDisAgainstElectronMediumMVA3[MAXTAU];
  double  mTauDisAgainstElectronTight[MAXTAU];
  double  mTauDisAgainstElectronTightMVA2[MAXTAU];
  double  mTauDisAgainstElectronTightMVA3[MAXTAU];
  double  mTauDisAgainstElectronVLooseMVA2[MAXTAU];
  double  mTauDisAgainstElectronVTightMVA3[MAXTAU];
  double  mTauDisAgainstMuonLoose[MAXTAU];
  double  mTauDisAgainstMuonLoose2[MAXTAU];
  double  mTauDisAgainstMuonMedium[MAXTAU];
  double  mTauDisAgainstMuonMedium2[MAXTAU];
  double  mTauDisAgainstMuonTight[MAXTAU];
  double  mTauDisAgainstMuonTight2[MAXTAU];
  double  mTauDisByCombinedIsolationDeltaBetaCorrRaw[MAXTAU];
  double  mTauDisByCombinedIsolationDeltaBetaCorrRaw3Hits[MAXTAU];
  double  mTauDisByIsolationMVA2raw[MAXTAU];
  double  mTauDisByIsolationMVAraw[MAXTAU];
  double  mTauDisByLooseCombinedIsolationDeltaBetaCorr[MAXTAU];
  double  mTauDisByLooseCombinedIsolationDeltaBetaCorr3Hits[MAXTAU];
  double  mTauDisByLooseIsolationMVA[MAXTAU];
  double  mTauDisByLooseIsolationMVA2[MAXTAU];
  double  mTauDisByMediumCombinedIsolationDeltaBetaCorr[MAXTAU];
  double  mTauDisByMediumCombinedIsolationDeltaBetaCorr3Hits[MAXTAU];
  double  mTauDisByMediumIsolationMVA[MAXTAU];
  double  mTauDisByMediumIsolationMVA2[MAXTAU];
  double  mTauDisByTightCombinedIsolationDeltaBetaCorr[MAXTAU];
  double  mTauDisByTightCombinedIsolationDeltaBetaCorr3Hits[MAXTAU];
  double  mTauDisByTightIsolationMVA[MAXTAU];
  double  mTauDisByTightIsolationMVA2[MAXTAU];
  double  mTauDisByVLooseCombinedIsolationDeltaBetaCorr[MAXTAU];
  double  mTauDisDecayModeFinding[MAXTAU];
  //
  double  mTauJetPt[MAXTAU];
  double  mTauJetEta[MAXTAU];
  double  mTauJetPhi[MAXTAU];
  
  ///------------------------------------------------
  /// PFTaus
  ///------------------------------------------------	
  int     mNPFTau;
  int     mPFTauCharge[MAXTAU];
  double  mPFTauE[MAXTAU]; 
  double  mPFTauPt[MAXTAU]; 
  double  mPFTauPx[MAXTAU]; 
  double  mPFTauPy[MAXTAU]; 
  double  mPFTauPz[MAXTAU]; 
  double  mPFTauEta[MAXTAU]; 
  double  mPFTauPhi[MAXTAU];
  double  mPFTauEtaEtaMoment[MAXTAU]; 
  double  mPFTauPhiPhiMoment[MAXTAU]; 
  double  mPFTauEtaPhiMoment[MAXTAU]; 			
  double  mPFTauLeadPFChargedHadrCandsignedSipt[MAXTAU];
  double  mPFTauIsoPFChargedHadrCandsPtSum[MAXTAU];
  double  mPFTauIsoPFGammaCandsEtSum[MAXTAU];      
  double  mPFTauMaximumHCALPFClusterEt[MAXTAU];    
  double  mPFTauEmFraction[MAXTAU];                
  double  mPFTauHcalTotOverPLead[MAXTAU];          
  double  mPFTauHcalMaxOverPLead[MAXTAU];          
  double  mPFTauHcal3x3OverPLead[MAXTAU];          
  double  mPFTauEcalStripSumEOverPLead[MAXTAU];    
  double  mPFTauBremsRecoveryEOverPLead[MAXTAU];         
  double  mPFTauElectronPreIDOutput[MAXTAU];       
  double  mPFTauElectronPreIDDecision[MAXTAU];     
  double  mPFTauCaloComp[MAXTAU];                  
  double  mPFTauSegComp[MAXTAU];                   
  double  mPFTauMuonDecision[MAXTAU]; 
  //
  double  mPFTausignalPFChargedHadrCands[MAXTAU];
  double  mPFTausignalPFGammaCands[MAXTAU];
  //
  double  mPFTauDisAgainstElectronDeadECAL[MAXTAU];
  double  mPFTauDisAgainstElectronLoose[MAXTAU];
  double  mPFTauDisAgainstElectronLooseMVA2[MAXTAU];
  double  mPFTauDisAgainstElectronLooseMVA3[MAXTAU];
  double  mPFTauDisAgainstElectronMVA[MAXTAU];
  double  mPFTauDisAgainstElectronMVA2category[MAXTAU];
  double  mPFTauDisAgainstElectronMVA2raw[MAXTAU];
  double  mPFTauDisAgainstElectronMVA3category[MAXTAU];
  double  mPFTauDisAgainstElectronMVA3raw[MAXTAU];
  double  mPFTauDisAgainstElectronMedium[MAXTAU];
  double  mPFTauDisAgainstElectronMediumMVA2[MAXTAU];
  double  mPFTauDisAgainstElectronMediumMVA3[MAXTAU];
  double  mPFTauDisAgainstElectronTight[MAXTAU];
  double  mPFTauDisAgainstElectronTightMVA2[MAXTAU];
  double  mPFTauDisAgainstElectronTightMVA3[MAXTAU];
  double  mPFTauDisAgainstElectronVLooseMVA2[MAXTAU];
  double  mPFTauDisAgainstElectronVTightMVA3[MAXTAU];
  double  mPFTauDisAgainstMuonLoose[MAXTAU];
  double  mPFTauDisAgainstMuonLoose2[MAXTAU];
  double  mPFTauDisAgainstMuonMedium[MAXTAU];
  double  mPFTauDisAgainstMuonMedium2[MAXTAU];
  double  mPFTauDisAgainstMuonTight[MAXTAU];
  double  mPFTauDisAgainstMuonTight2[MAXTAU];
  double  mPFTauDisByCombinedIsolationDeltaBetaCorrRaw[MAXTAU];
  double  mPFTauDisByCombinedIsolationDeltaBetaCorrRaw3Hits[MAXTAU];
  double  mPFTauDisByIsolationMVA2raw[MAXTAU];
  double  mPFTauDisByIsolationMVAraw[MAXTAU];
  double  mPFTauDisByLooseCombinedIsolationDeltaBetaCorr[MAXTAU];
  double  mPFTauDisByLooseCombinedIsolationDeltaBetaCorr3Hits[MAXTAU];
  double  mPFTauDisByLooseIsolation[MAXTAU];
  double  mPFTauDisByLooseIsolationDeltaBetaCorr[MAXTAU];
  double  mPFTauDisByLooseIsolationMVA[MAXTAU];
  double  mPFTauDisByLooseIsolationMVA2[MAXTAU];
  double  mPFTauDisByMediumCombinedIsolationDeltaBetaCorr[MAXTAU];
  double  mPFTauDisByMediumCombinedIsolationDeltaBetaCorr3Hits[MAXTAU];
  double  mPFTauDisByMediumIsolation[MAXTAU];
  double  mPFTauDisByMediumIsolationDeltaBetaCorr[MAXTAU];
  double  mPFTauDisByMediumIsolationMVA[MAXTAU];
  double  mPFTauDisByMediumIsolationMVA2[MAXTAU];
  double  mPFTauDisByTightCombinedIsolationDeltaBetaCorr[MAXTAU];
  double  mPFTauDisByTightCombinedIsolationDeltaBetaCorr3Hits[MAXTAU];
  double  mPFTauDisByTightIsolation[MAXTAU];
  double  mPFTauDisByTightIsolationDeltaBetaCorr[MAXTAU];
  double  mPFTauDisByTightIsolationMVA[MAXTAU];
  double  mPFTauDisByTightIsolationMVA2[MAXTAU];
  double  mPFTauDisByVLooseCombinedIsolationDeltaBetaCorr[MAXTAU];
  double  mPFTauDisByVLooseIsolation[MAXTAU];
  double  mPFTauDisByVLooseIsolationDeltaBetaCorr[MAXTAU];
  double  mPFTauDisDecayModeFinding[MAXTAU];
  //
  double  mPFTauJetPt[MAXTAU];
  double  mPFTauJetEta[MAXTAU];
  double  mPFTauJetPhi[MAXTAU];
  
  ///------------------------------------------------
  /// Photons
  ///------------------------------------------------	
  int     mNPhot;
  double  mPhotE[MAXPHOT];
  double  mPhotPt[MAXPHOT]; 
  double  mPhotPx[MAXPHOT]; 
  double  mPhotPy[MAXPHOT]; 
  double  mPhotPz[MAXPHOT]; 
  double  mPhotEta[MAXPHOT]; 
  double  mPhotPhi[MAXPHOT];
  double  mPhotE1x5[MAXPHOT]; 
  double  mPhotE2x5[MAXPHOT]; 
  double  mPhotE5x5[MAXPHOT]; 
  double  mPhotSigEta[MAXPHOT];
  double  mPhotSigPhi[MAXPHOT];
  double  mPhotEcalIso04[MAXPHOT]; 
  double  mPhotHcalIso04[MAXPHOT]; 
  double  mPhotTrackIso04[MAXPHOT]; 
  double  mPhotHasPixSeed[MAXPHOT];
  int     mPhotIsPhot[MAXPHOT];
  
  ///------------------------------------------------
  /// TIV
  ///------------------------------------------------
  float   TIV[TIVMAX];
  int     tivN;
  double  tiv_ltrack_thr_;
  double  tiv_strack_thr_;
  double  tiv_cone_;
  double  tiv_inner_cone_;
  double  tiv_dz_thr_;
  double  tiv_dxy_thr_;
  float   TIV_pt[TIVMAX];
  float   TIV_eta[TIVMAX];
  float   TIV_phi[TIVMAX];
  float   TIV_px[TIVMAX];
  float   TIV_py[TIVMAX];
  float   TIV_pz[TIVMAX];
  float   TIV_dxy[TIVMAX];
  float   TIV_dsz[TIVMAX];
  float   TIV_dR[TIVMAX];
  float   TIV_dz[TIVMAX];
  float   TIV_d0[TIVMAX];
  float   TIV_dxy_corr[TIVMAX];
  float   TIV_dsz_corr[TIVMAX];
  float   TIV_dz_corr[TIVMAX];
  float   TIV_d0_corr[TIVMAX];
  //
  int     TIV_lead[TIVMAX];
  //
  float   LowTIV;
  
  ///------------------------------------------------
  /// CaloTowers
  ///------------------------------------------------
  //double CaloTowerEt[30];
  //double CaloTowerPhi[30]; 
  double  mCaloTowerdEx;
  double  mCaloTowerdEy;
};


NtupleAnalyzer::NtupleAnalyzer(const edm::ParameterSet& iConfig):
  hltConfig_()
{
  mtree                  = fs->make<TTree>("ntuple","ntuple");
  WeightTag              = iConfig.getParameter<double>("weight");
  
  //
  debugMode              = iConfig.getParameter<bool>("debugMode");
  includeNonPFCollection = iConfig.getParameter<bool>("includeNonPFCollection");
  
  //
  TriggerTag_            = iConfig.getUntrackedParameter<edm::InputTag>("TriggerTag");
  triggerUsed            = iConfig.getParameter<double>("triggerUsed");
  hltPath1Name_          = iConfig.getUntrackedParameter<std::string>("hltPath1Name");
  hltPath2Name_          = iConfig.getUntrackedParameter<std::string>("hltPath2Name"); 
  hltBit1_               = 0;
  hltBit2_               = 0; 
  
  //
  JetPtCutTag            = iConfig.getParameter<double>("jetPtCut");
  JetEtaCutTag           = iConfig.getParameter<double>("jetEtaCut");
  METThresholdTag        = iConfig.getParameter<double>("METThreshold");
  
  //
  isMCTag                = iConfig.getParameter<bool>("isMC");
  isSignalTag            = iConfig.getParameter<double>("isSignal");
  chooseOnlyGenZmumu     = iConfig.getParameter<bool>("chooseOnlyGenZmumu"); 
  chooseOnlyGenZnunu     = iConfig.getParameter<bool>("chooseOnlyGenZnunu");
  
  // PF Collection
  PFElectronTag_         = iConfig.getUntrackedParameter<edm::InputTag>("PFElectronTag");
  PFMuonTag_             = iConfig.getUntrackedParameter<edm::InputTag>("PFMuonTag");
  PFTauTag_              = iConfig.getUntrackedParameter<edm::InputTag>("PFTauTag");
  PFJetTag_              = iConfig.getUntrackedParameter<edm::InputTag>("PFJetTag");
  PFJetAK8Tag_           = iConfig.getUntrackedParameter<edm::InputTag>("PFJetAK8Tag");
  PFMETTag_              = iConfig.getUntrackedParameter<edm::InputTag>("PFMETTag");
  
  // non PF collection 
  ElectronTag_           = iConfig.getUntrackedParameter<edm::InputTag>("ElectronTag");
  MuonTag_               = iConfig.getUntrackedParameter<edm::InputTag>("MuonTag");
  TauTag_                = iConfig.getUntrackedParameter<edm::InputTag>("TauTag");
  JetTag_                = iConfig.getUntrackedParameter<edm::InputTag>("JetTag");
  METTag_                = iConfig.getUntrackedParameter<edm::InputTag>("METTag");

  // PUJetID tag
  puJetIdTag_            = iConfig.getUntrackedParameter<edm::InputTag>("puJetIdTag");
 
  // TIV
  tiv_ltrack_thr_        = iConfig.getParameter<double>("TIV_ltrack_thr");
  tiv_strack_thr_        = iConfig.getParameter<double>("TIV_strack_thr");
  tiv_dz_thr_            = iConfig.getParameter<double>("TIV_dz_thr");
  tiv_dxy_thr_           = iConfig.getParameter<double>("TIV_dxy_thr");
  tiv_cone_              = iConfig.getParameter<double>("TIV_cone_thr");
  tiv_inner_cone_        = iConfig.getParameter<double>("TIV_inner_cone_thr");
  Tracks_                = iConfig.getUntrackedParameter<edm::InputTag>("Tracks");
}


NtupleAnalyzer::~NtupleAnalyzer()
{
}


float NtupleAnalyzer::deltaPhi(float v1, float v2)
{
  float diff = fabs(v2 - v1);
  float corr = 2*acos(-1.) - diff;
  if (diff < acos(-1.))
    return diff; 
  else
    return corr; 
}


float NtupleAnalyzer::deltaR(float eta1, float phi1, float eta2, float phi2)
{
  return sqrt( (eta1-eta2)*(eta1-eta2) 
	       + deltaPhi(phi1, phi2)*deltaPhi(phi1, phi2) );
}


float NtupleAnalyzer::checkPtMatch(float v1, float v2)
{
  return std::fabs((v1-v2)/v2);
}


void NtupleAnalyzer::beginRun(const edm::Run& iRun, const edm::EventSetup& iSetup)
{
  bool changed;
  hltConfig_.init(iRun, iSetup, TriggerTag_.process(), changed);
  
  try{
    hltBit1_=(hltConfig_.triggerNames()).size();  // default setting -- trigger not found
    hltBit2_=(hltConfig_.triggerNames()).size();  // default setting -- trigger not found
    for (uint i=0;i<(hltConfig_.triggerNames()).size();++i){ // loop over trigger names
      std::string trigName=hltConfig_.triggerName(i);
      if (hltPath1Name_.size()>0 &&
	  trigName.find(hltPath1Name_)!= std::string::npos){
	hltBit1_ = hltConfig_.triggerIndex(trigName);
      }
      if (hltPath2Name_.size()>0 &&
	  trigName.find(hltPath2Name_)!= std::string::npos){
	hltBit2_ = hltConfig_.triggerIndex(trigName);
      }
    }
    
    if (hltBit1_==(hltConfig_.triggerNames()).size())
      edm::LogWarning("InvHiggsInfoProducer") << "Could not find an HLT path matching "<<hltPath1Name_<<std::endl;
    else
      edm::LogInfo("InvHiggsInfoProducer") << hltPath1Name_ << " index is " << hltBit1_ << std::endl;
    
    if (hltBit2_==(hltConfig_.triggerNames()).size())
      edm::LogWarning("InvHiggsInfoProducer") << "Could not find an HLT path matching "<<hltPath2Name_<<std::endl;
    else
      edm::LogInfo("InvHiggsInfoProducer") << hltPath2Name_ << " index is " << hltBit2_ << std::endl;
  }
  catch (cms::Exception e) {
    edm::LogWarning("InvHiggsInfoProducer") << "Exception while trying to find HLT bit numbers" << std::endl;
    edm::LogWarning("InvHiggsInfoProducer") << e << std::endl;
  }
}

// ------------ method called to for each event  ------------

void NtupleAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  
  
  ///-------------------------------------------------------------------------- 
  /// Run  Event Lumi Bx
  ///--------------------------------------------------------------------------
  mRun   = iEvent.id().run();
  mEvent = iEvent.id().event();
  mLumi  = iEvent.luminosityBlock();
  mBX    = iEvent.bunchCrossing();
  if(debugMode){
    cout<<"----------------------------"<<endl;
    cout<<"Run= "<<mRun<<", Lumi= "<<mLumi<<", Event= "<<mEvent<<endl;
  }  
  //if(mEvent<128062500) return; //Quick check
  
  
  ///-------------------------------------------------------------------------- 
  /// NoiseFlag
  ///--------------------------------------------------------------------------
  //TriggerNames myTrigName;
  /*
  Handle<TriggerResults> myTrig;
  iEvent.getByLabel("TriggerResults", myTrig);
  int hltCount2 = 25;
  if(myTrig.isValid()){
    hltCount2 = myTrig->size();
    if(debugMode){
      cout<<"No. of NoiseFilter = "<<hltCount2<<endl;
    }
    for(int i=0; i<hltCount2; i++){
      NoiseFlag[i] = myTrig->accept(i);
    }
  }
  else{
    for(int i=0; i<hltCount2; i++){
      NoiseFlag[i] = 1;
    }
  }
  nNoiseFlag = hltCount2;
  if(debugMode){
    cout<<"Noise: ";
    for(int i=0; i<nNoiseFlag; i++)
      cout<<NoiseFlag[i]<<" ";
    cout<<endl;
  }
  if(NoiseFlag[0]==false){
    if(debugMode)
      std::cout<<"No good vertices in this events ... filter out"<<endl;
    return;
  }
  */
  
  
  ///--------------------------------------------------------------------------
  /// Check isMCTag
  ///--------------------------------------------------------------------------
  bool isMCTagCheck = !iEvent.eventAuxiliary().isRealData();
  if(isMCTag!=isMCTagCheck){
    cout<<"Wrong isMCTag! The GlobalTag is maybe wrong. Please check!"<<endl;
    return;
  } 
  
  
  ///--------------------------------------------------------------------------
  /// Prefilter
  ///--------------------------------------------------------------------------  
  //
  //if(mEvent!=128062935) return;
  //
  //edm::Handle<edm::View<pat::MET> > PFMetType1Filter;
  //iEvent.getByLabel(PFMETTag_,PFMetType1Filter);
  //if(!PFMetType1Filter.isValid()) return;
  //const edm::View<pat::MET> & pfmetsType1filter = *PFMetType1Filter;
  //if(pfmetsType1filter[0].et()<METThresholdTag) return;
  //
  // Select only Zmumu events
  // 
  //Handle<reco::GenParticleCollection>  GenParHand;
  //iEvent.getByLabel("genParticles", GenParHand);
  //if(chooseOnlyGenZmumu && isMCTagCheck && GenParHand.isValid()){
  //  bool checkZmumu = false;
  //  for( size_t i = 0; i < GenParHand->size(); ++ i ){
  //    const GenParticle &p = (*GenParHand)[i];
  //    int id     = p.pdgId();
  //    int status = p.status();
  //    if(status==3 && abs(id)==13){
  //	int n = p.numberOfMothers();
  //	if(n>0){
  //	  const reco::Candidate *pa = p.mother();
  //	  int parentID = pa->pdgId();
  //	  if(parentID==23)
  //	    checkZmumu = true;
  //	}
  //  }
  //}
  //if(!checkZmumu) return;
  //}
  //
  // Select only Znunu events
  // 
  //if(chooseOnlyGenZnunu && isMCTagCheck && GenParHand.isValid()){
  //bool checkZnunu = false;
  //for( size_t i = 0; i < GenParHand->size(); ++ i ){
  //  const GenParticle &p = (*GenParHand)[i];
  //  int id     = p.pdgId();
  //  int status = p.status();
  //  if(status==3 && (abs(id)==12 || abs(id)==14 || abs(id)==16)){
  //	int n = p.numberOfMothers();
  //	if(n>0){
  //	  const reco::Candidate *pa = p.mother();
  //	  int parentID = pa->pdgId();
  //	  if(parentID==23)
  //	    checkZnunu = true;
  //	}
  //  }
  //}
  //if(!checkZnunu) return;
  //}
  
  
  ///-------------------------------------------------------------------------- 
  /// Trigger filter
  ///--------------------------------------------------------------------------
  /*
  edm::Handle<TriggerResults> hltTriggerResultHandle;
  iEvent.getByLabel(TriggerTag_, hltTriggerResultHandle);
  if(!hltTriggerResultHandle.isValid()) return;

  bool triggerCheck1 = false;
  bool triggerCheck2 = false;

  if(hltBit1_ < hltTriggerResultHandle->size()){
    triggerCheck1 = hltTriggerResultHandle->accept(hltBit1_);
  }
  if(hltBit2_ < hltTriggerResultHandle->size()){
    triggerCheck2 = hltTriggerResultHandle->accept(hltBit2_);
  }
  if(debugMode){
    if(triggerUsed!=99){
      cout<<"Trigger-1: "<<hltPath1Name_<<" Result= "<<triggerCheck1<<endl;
      cout<<"Trigger-2: "<<hltPath2Name_<<" Result= "<<triggerCheck2<<endl;
    }
    else{
      cout<<"No trigger selection"<<endl;
    }
  }
  if(triggerUsed!=99 && !triggerCheck1 && !triggerCheck2) return;
  */
  
  /*
  ///-------------------------------------------------------------------------- 
  /// Store trigger information
  ///--------------------------------------------------------------------------
  trgnm_ = iEvent.triggerNames(*hltTriggerResultHandle);
  const TriggerResults &Hlt = *hltTriggerResultHandle;
  char tmpnm1[3000],tmpnm2[3000];
  nhlt=0;
  if(&Hlt){
    //cout <<Hlt.size()<<endl;
    //cout <<"---------------------------------"<<endl;
    nhlt = Hlt.size();
    //trgnm_.init(Hlt);
    
    for (int itrig = 0; itrig != nhlt; ++itrig){
      string trigName=trgnm_.triggerName(itrig);
      bool accept = Hlt.accept(itrig);
      if(accept && trigName.find("HLT")!=string::npos){
	sprintf(tmpnm1,"%s",tmpnm2);
	sprintf(tmpnm2,"%s%s",tmpnm1,trigName.c_str());
      }    
      //cout <<itrig<<" "<<trigName<<" "<<accept<<endl;
    }
    //cout <<trgnm<<endl;
    //string strtrg = trgnm;
  }
  string strtrg = tmpnm2;
  sprintf(trgnm,"TRG%s", tmpnm2); 
  */
    
  
  /*
  ///-------------------------------------------------------------------------- 
  ///
  ///--------------------------------------------------------------------------
  int hltCount=0;
  if(!hltTriggerResultHandle.isValid()){
    cout << "invalid handle for HLT TriggerResults" << std::endl;
  } 
  else{
    //HLTNames.init(*hltTriggerResultHandle);
    string tempnames;  
    hltCount = hltTriggerResultHandle->size();
    for(int i = 0 ; i < hltCount ; i++){
      //tempnames += HLTNames.triggerName(i) + "  ";
      HLTArray[i] = hltTriggerResultHandle->accept(i);
      
      //cout << i << "  " <<trgnm_.triggerName(i) << "  " << hltTriggerResultHandle->accept(i) << endl;
    }  
    nHLT = hltCount;
  }
  */
  
  
  ///-------------------------------------------------------------------------- 
  /// PileUP Summury
  ///--------------------------------------------------------------------------
  if(isMCTag){
    Handle<std::vector< PileupSummaryInfo > >  PupInfo;
    iEvent.getByLabel(edm::InputTag("addPileupInfo"), PupInfo);
    
    std::vector<PileupSummaryInfo>::const_iterator PVI;
    
    int nm1 = -1; int n0 = -1; int np1 = -1;
    for(PVI = PupInfo->begin(); PVI != PupInfo->end(); ++PVI){
      int BX = PVI->getBunchCrossing();
      
      if(BX == -1){ 
	nm1 = PVI->getPU_NumInteractions();
      }
      if(BX == 0){ 
	n0 = PVI->getPU_NumInteractions();
      }
      if(BX == 1){ 
	np1 = PVI->getPU_NumInteractions();
      }
    }
    //double MyWeight3D = LumiWeights_.weight3D( nm1,n0,np1);
    mnpvm1 = nm1; 
    mnpv0  = n0;
    mnpvp1 = np1;
  }

  
  ///-------------------------------------------------------------------------- 
  /// Rho
  ///--------------------------------------------------------------------------
  /*
  edm::Handle<double> rho;
  const edm::InputTag eventrho("kt6PFJets", "rho");
  iEvent.getByLabel(eventrho,rho);
  mfastJetRho = *rho;
  if(debugMode){
    cout<<"Rho= "<<mfastJetRho<<endl;
  }
  */
  
  ///-------------------------------------------------------------------------- 
  /// B-Field
  ///--------------------------------------------------------------------------
  /*
  edm::Handle<DcsStatusCollection> dcsHandle;
  iEvent.getByLabel("scalersRawToDigi", dcsHandle);
  double bField = 3.8112;
  if( !isMCTag && dcsHandle->size() > 0){
    float currentToBFieldScaleFactor = 2.09237036221512717e-04;
    float  current = (*dcsHandle)[0].magnetCurrent();
    bField = current*currentToBFieldScaleFactor;
  }
  else {
    edm::ESHandle<MagneticField> magneticField;
    iSetup.get<IdealMagneticFieldRecord>().get(magneticField);
    bField = magneticField->inTesla(GlobalPoint(0.,0.,0.)).z();
  }
  */
  
  ///-------------------------------------------------------------------------- 
  /// Tracks
  ///--------------------------------------------------------------------------
  /*
  edm::Handle<reco::TrackCollection> trkHandle;
  iEvent.getByLabel("generalTracks",trkHandle);
  */
  
  ///-------------------------------------------------------------------------- 
  /// AK4PFJet
  ///--------------------------------------------------------------------------
  edm::Handle<edm::View<pat::Jet> > JetHand;
  iEvent.getByLabel(PFJetTag_,JetHand);
  int jeti=0;
    
  for(unsigned int ind=0; ind<(*JetHand).size() && ind<MAXJET; ind++){
    const pat::Jet& jet2 = (*JetHand)[ind];
    const pat::Jet& jet = (*JetHand)[ind].correctedJet("Uncorrected");
    
    if( jet2.pt() < JetPtCutTag ) continue;
    
    mPFAK4JetIDLOOSE[jeti]              = 0.;//pfjetIDFunctorLoose(jet2);
    mPFAK4JetIDTIGHT[jeti]              = 0.;//pfjetIDFunctorTight(jet2); 
    
    mPFAK4JetPUFullJetId[jeti]          = jet2.userFloat("pileupJetId:fullDiscriminant");
        
    mPFAK4JetPt[jeti]                   = jet.pt();
    mPFAK4JetPx[jeti]                   = jet.momentum().X();
    mPFAK4JetPy[jeti]                   = jet.momentum().Y();
    mPFAK4JetPz[jeti]                   = jet.momentum().Z();
    mPFAK4JetE[jeti]                    = jet.energy();
    
    mPFAK4JetEta[jeti]                  = jet2.eta();
    mPFAK4JetPhi[jeti]                  = jet2.phi();		
    mPFAK4JetN90[jeti]                  = jet2.n90();
    mPFAK4JetNumOfDaughters[jeti]       = jet2.numberOfDaughters();
    
    ///use uncorrected jet for jetID
    mPFAK4JetN90Hits[jeti]              = jet.jetID().n90Hits ;
    mPFAK4JetfHPD[jeti]                 = jet.jetID().fHPD ;
    mPFAK4JetfRBX[jeti]                 = jet.jetID().fRBX ;
    mPFAK4JetIDEmf[jeti]                = jet.jetID().restrictedEMF;
    
    mPFAK4JetSigEta[jeti]               = jet2.etaetaMoment();
    mPFAK4JetSigPhi[jeti]               = jet2.phiphiMoment();
    mPFAK4JetPtCor[jeti]                = jet2.pt();
    mPFAK4JetPxCor[jeti]                = jet2.momentum().X();
    mPFAK4JetPyCor[jeti]                = jet2.momentum().Y();
    mPFAK4JetPzCor[jeti]                = jet2.momentum().Z();
    mPFAK4JetECor[jeti]                 = jet2.energy();

    ///JPT or PFJet specific information
    mPFAK4JetNumOfMuon[jeti]            = jet2.muonMultiplicity();
    mPFAK4JetNumOfChaMu[jeti]           = jet2.chargedMultiplicity();

    mPFAK4JetChaEmEng[jeti]             = jet2.chargedEmEnergy();
    mPFAK4JetNeuEmEng[jeti]             = jet2.neutralEmEnergy();
    mPFAK4JetChaHadEng[jeti]            = jet2.chargedHadronEnergy();
    mPFAK4JetNeuHadEng[jeti]            = jet2.neutralHadronEnergy();
    mPFAK4JetPhotEng[jeti]              = jet2.photonEnergy();
    mPFAK4JetElecEng[jeti]              = jet2.electronEnergy();
    mPFAK4JetMuonEng[jeti]              = jet2.muonEnergy();
    mPFAK4JetHFHadEng[jeti]             = jet2.HFHadronEnergy();
    mPFAK4JetHFEmEng[jeti]              = jet2.HFEMEnergy();
    
    //Relative to uncorrected jet energy
    mPFAK4JetNeuEmEngFrac[jeti]         = jet2.neutralEmEnergyFraction();
    mPFAK4JetChaEmEngFrac[jeti]         = jet2.chargedEmEnergyFraction();
    mPFAK4JetChaHadEngFrac[jeti]        = jet2.chargedHadronEnergyFraction();
    mPFAK4JetNeuHadEngFrac[jeti]        = jet2.neutralHadronEnergyFraction();
    
    mPFAK4JetNumOfChaHad[jeti]          = jet2.chargedHadronMultiplicity();
    mPFAK4JetNumOfNeuHad[jeti]          = jet2.neutralHadronMultiplicity();
    mPFAK4JetNumOfPhot[jeti]            = jet2.photonMultiplicity();
    mPFAK4JetNumOfElec[jeti]            = jet2.electronMultiplicity();
    mPFAK4JetNumOfHFHad[jeti]           = jet2.HFHadronMultiplicity();
    mPFAK4JetNumOfHFEM[jeti]            = jet2.HFEMMultiplicity();
    
    mPFAK4JetChaMuEng[jeti]             = jet2.chargedMuEnergy();
     
    mPFAK4JetNumOfNeu[jeti]             = jet2.neutralMultiplicity();
    mPFAK4JetNumOfCha[jeti]             = jet2.chargedMultiplicity();
    
    mPFAK4JetBtagTkCountHighEff[jeti]   = jet.bDiscriminator("trackCountingHighEffBJetTags");
    mPFAK4JetBtagTkCountHighPur[jeti]   = jet.bDiscriminator("trackCountingHighPurBJetTags");
    mPFAK4JetBtagJetProbability[jeti]   = jet.bDiscriminator("jetProbabilityBJetTags");
    mPFAK4JetBtagJetBProbability[jeti]  = jet.bDiscriminator("jetBProbabilityBJetTags");
    mPFAK4JetBTagCSVv2[jeti]            = jet.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags");
    mPFAK4JetBTagCombSecVtx[jeti]       = jet.bDiscriminator("combinedSecondaryVertexBJetTags");
    mPFAK4JetBTagCombSecVtxMVA[jeti]    = jet.bDiscriminator("combinedSecondaryVertexMVABJetTags");  
   
    ///-----------Jets Uncertanity --------------------------------------------------------
    if(isMCTag){
      //jecUnc->setJetEta(jet2.eta() ); // Give rapidity of jet you want tainty on
      //jecUnc->setJetPt( jet2.pt() );// Also give the corrected pt of the jet you want the uncertainty on
      // The following function gives the relative uncertainty in the jet Pt.
      // i.e. ptCorSmeared = (1 +- uncer) * ptCor  
      //mPFAK4uncer[jeti] =    jecUnc->getUncertainty(true); 
      // In principle, boolean controls if uncertainty on +ve or -ve side is returned (asymmetric errors) but not yet implemented.

      // Addition by Jyothsna: Jet flavour needed for b-tagging
      // Store algorithmic jet flavour definition
      mPFAK4JetFlavour[jeti] = jet.partonFlavour();
      // flavour of the parton matched to jet (0 if no match)
      mPFAK4JetGenPartonFlavour[jeti] = (jet.genParton() == nullptr) ? 0 : jet.genParton()->pdgId();
      
      if(jet2.genJet()!= 0){
	  mGenPFAK4JetPt[jeti]        = jet2.genJet()->pt();
	  mGenPFAK4JetE[jeti]         = jet2.genJet()->energy();
	  mGenPFAK4JetPx[jeti]        = jet2.genJet()->momentum().X();
	  mGenPFAK4JetPy[jeti]        = jet2.genJet()->momentum().Y();
	  mGenPFAK4JetPz[jeti]        = jet2.genJet()->momentum().z();
	  mGenPFAK4JetEta[jeti]       = jet2.genJet()->eta();
	  mGenPFAK4JetPhi[jeti]       = jet2.genJet()->phi();
	  mGenPFAK4JetEmEnergy[jeti]  = jet2.genJet()->emEnergy();
	  mGenPFAK4JetHadEnergy[jeti] = jet2.genJet()->emEnergy();  
	  if(fabs(mPFAK4JetEta[jeti])<0.5){
	    mPFAK4JERCentral[jeti] = 1.052;
	    mPFAK4JERUp[jeti]      = 0.990;
	    mPFAK4JERDown[jeti]    = 1.115;
	  } 
	  else if(fabs(mPFAK4JetEta[jeti])>=0.5 && fabs(mPFAK4JetEta[jeti])<1.1){
	    mPFAK4JERCentral[jeti] = 1.057;
	    mPFAK4JERUp[jeti]      = 1.001;
	    mPFAK4JERDown[jeti]    = 1.114;
	  }
	  else if(fabs(mPFAK4JetEta[jeti])>=1.1 && fabs(mPFAK4JetEta[jeti])<1.7){
	    mPFAK4JERCentral[jeti] = 1.096;
	    mPFAK4JERUp[jeti]      = 1.032;
	    mPFAK4JERDown[jeti]    = 1.161;
	  }
	  else if(fabs(mPFAK4JetEta[jeti])>=1.7 && fabs(mPFAK4JetEta[jeti])<2.3){
	    mPFAK4JERCentral[jeti] = 1.134;
	    mPFAK4JERUp[jeti]      = 1.042;
	    mPFAK4JERDown[jeti]    = 1.228;
	  }
	  else if(fabs(mPFAK4JetEta[jeti])>=2.3 && fabs(mPFAK4JetEta[jeti])<5.0){
	    mPFAK4JERCentral[jeti] = 1.288;
	    mPFAK4JERUp[jeti]      = 1.089;
	    mPFAK4JERDown[jeti]    = 1.488;
	  }
	  else{
	    mPFAK4JERCentral[jeti] = -999.;
	    mPFAK4JERUp[jeti]      = -999.;
	    mPFAK4JERDown[jeti]    = -999.;
	  }
	}
	else {
	  mGenPFAK4JetPt[jeti]        = -999;
	  mGenPFAK4JetE[jeti]         = -999;
	  mGenPFAK4JetPx[jeti]        = -999;
	  mGenPFAK4JetPy[jeti]        = -999;
	  mGenPFAK4JetPz[jeti]        = -999;
	  mGenPFAK4JetEta[jeti]       = -999;
	  mGenPFAK4JetPhi[jeti]       = -999;
	  mGenPFAK4JetEmEnergy[jeti]  = -999;
	  mGenPFAK4JetHadEnergy[jeti] = -999;
	  mPFAK4JERCentral[jeti]      = -999.;
	  mPFAK4JERUp[jeti]           = -999.;
	  mPFAK4JERDown[jeti]         = -999.;
	}
    }
    else{
      mPFAK4uncer[jeti]           = 0.;
      mGenPFAK4JetPt[jeti]        = -999;
      mGenPFAK4JetE[jeti]         = -999;
      mGenPFAK4JetPx[jeti]        = -999;
      mGenPFAK4JetPy[jeti]        = -999;
      mGenPFAK4JetPz[jeti]        = -999;
      mGenPFAK4JetEta[jeti]       = -999;
      mGenPFAK4JetPhi[jeti]       = -999;
      mGenPFAK4JetEmEnergy[jeti]  = -999;
      mGenPFAK4JetHadEnergy[jeti] = -999;
      mPFAK4JERCentral[jeti]      = -999.;
      mPFAK4JERUp[jeti]           = -999.;
      mPFAK4JERDown[jeti]         = -999.;
    }		
    jeti++;
  }
  mNPFAK4Jets = jeti; 
  if(debugMode){
    cout<<"N(PFJet): "<<mNPFAK4Jets<<endl;
  }
  if(debugMode && mNPFAK4Jets>0){
    cout<<"PFJet0: Pt= "<<mPFAK4JetPtCor[0]<<", Eta= "<<mPFAK4JetEta[0]<<", Phi= "<<mPFAK4JetPhi[0]<<", PUJetID= "<<mPFAK4JetPUFullJetId[0]<<endl;
    cout<<"      : NHEF= "<<mPFAK4JetNeuHadEngFrac[0]<<", CHEF= "<<mPFAK4JetChaHadEngFrac[0]<<", NEMF= "<<mPFAK4JetNeuEmEngFrac[0]<<", CEMF= "<<mPFAK4JetChaEmEngFrac[0]<<endl;
    cout<<"      : MuEF= "<<mPFAK4JetMuonEng[0]/mPFAK4JetE[0]<<", ElecEF= "<<mPFAK4JetElecEng[0]/mPFAK4JetE[0]<<", PhotonEF= "<<mPFAK4JetPhotEng[0]/mPFAK4JetE[0]<<endl;
  }
  if(debugMode && mNPFAK4Jets>1){
    cout<<"PFJet1: Pt= "<<mPFAK4JetPtCor[1]<<", Eta= "<<mPFAK4JetEta[1]<<", Phi= "<<mPFAK4JetPhi[1]<<", PUJetID= "<<mPFAK4JetPUFullJetId[1]<<endl;
    cout<<"      : NHEF= "<<mPFAK4JetNeuHadEngFrac[1]<<", CHEF= "<<mPFAK4JetChaHadEngFrac[1]<<", NEMF= "<<mPFAK4JetNeuEmEngFrac[1]<<", CEMF= "<<mPFAK4JetChaEmEngFrac[1]<<endl;
    cout<<"      : MuEF= "<<mPFAK4JetMuonEng[1]/mPFAK4JetE[1]<<", ElecEF= "<<mPFAK4JetElecEng[1]/mPFAK4JetE[1]<<", PhotonEF= "<<mPFAK4JetPhotEng[1]/mPFAK4JetE[1]<<endl;
  }
  
  

  ///-------------------------------------------------------------------------- 
  /// AK8PFJet
  ///--------------------------------------------------------------------------
  edm::Handle<edm::View<pat::Jet> > JetHandAK8;
  iEvent.getByLabel(PFJetAK8Tag_,JetHandAK8);
  jeti=0;
    
  for(unsigned int ind=0; ind<(*JetHandAK8).size() && ind<MAXJET; ind++){
    const pat::Jet& jet2 = (*JetHandAK8)[ind];
    const pat::Jet& jet = (*JetHandAK8)[ind].correctedJet("Uncorrected");
    
    if( jet2.pt() < JetPtCutTag ) continue;
    
    mPFAK8JetIDLOOSE[jeti]              = 0.;//pfjetIDFunctorLoose(jet2);
    mPFAK8JetIDTIGHT[jeti]              = 0.;//pfjetIDFunctorTight(jet2); 
    
    mPFAK8JetPUFullJetId[jeti]          = jet2.userFloat("pileupJetId:fullDiscriminant");
        
    mPFAK8JetPt[jeti]                   = jet.pt();
    mPFAK8JetPx[jeti]                   = jet.momentum().X();
    mPFAK8JetPy[jeti]                   = jet.momentum().Y();
    mPFAK8JetPz[jeti]                   = jet.momentum().Z();
    mPFAK8JetE[jeti]                    = jet.energy();
    
    mPFAK8JetEta[jeti]                  = jet2.eta();
    mPFAK8JetPhi[jeti]                  = jet2.phi();		
    mPFAK8JetN90[jeti]                  = jet2.n90();
    mPFAK8JetNumOfDaughters[jeti]       = jet2.numberOfDaughters();
    
    ///use uncorrected jet for jetID
    mPFAK8JetN90Hits[jeti]              = jet.jetID().n90Hits ;
    mPFAK8JetfHPD[jeti]                 = jet.jetID().fHPD ;
    mPFAK8JetfRBX[jeti]                 = jet.jetID().fRBX ;
    mPFAK8JetIDEmf[jeti]                = jet.jetID().restrictedEMF;
    
    mPFAK8JetSigEta[jeti]               = jet2.etaetaMoment();
    mPFAK8JetSigPhi[jeti]               = jet2.phiphiMoment();
    mPFAK8JetPtCor[jeti]                = jet2.pt();
    mPFAK8JetPxCor[jeti]                = jet2.momentum().X();
    mPFAK8JetPyCor[jeti]                = jet2.momentum().Y();
    mPFAK8JetPzCor[jeti]                = jet2.momentum().Z();
    mPFAK8JetECor[jeti]                 = jet2.energy();

    ///JPT or PFJet specific information
    mPFAK8JetNumOfMuon[jeti]            = jet2.muonMultiplicity();
    mPFAK8JetNumOfChaMu[jeti]           = jet2.chargedMultiplicity();

    mPFAK8JetChaEmEng[jeti]             = jet2.chargedEmEnergy();
    mPFAK8JetNeuEmEng[jeti]             = jet2.neutralEmEnergy();
    mPFAK8JetChaHadEng[jeti]            = jet2.chargedHadronEnergy();
    mPFAK8JetNeuHadEng[jeti]            = jet2.neutralHadronEnergy();
    mPFAK8JetPhotEng[jeti]              = jet2.photonEnergy();
    mPFAK8JetElecEng[jeti]              = jet2.electronEnergy();
    mPFAK8JetMuonEng[jeti]              = jet2.muonEnergy();
    mPFAK8JetHFHadEng[jeti]             = jet2.HFHadronEnergy();
    mPFAK8JetHFEmEng[jeti]              = jet2.HFEMEnergy();
    
    //Relative to uncorrected jet energy
    mPFAK8JetNeuEmEngFrac[jeti]         = jet2.neutralEmEnergyFraction();
    mPFAK8JetChaEmEngFrac[jeti]         = jet2.chargedEmEnergyFraction();
    mPFAK8JetChaHadEngFrac[jeti]        = jet2.chargedHadronEnergyFraction();
    mPFAK8JetNeuHadEngFrac[jeti]        = jet2.neutralHadronEnergyFraction();
    
    mPFAK8JetNumOfChaHad[jeti]          = jet2.chargedHadronMultiplicity();
    mPFAK8JetNumOfNeuHad[jeti]          = jet2.neutralHadronMultiplicity();
    mPFAK8JetNumOfPhot[jeti]            = jet2.photonMultiplicity();
    mPFAK8JetNumOfElec[jeti]            = jet2.electronMultiplicity();
    mPFAK8JetNumOfHFHad[jeti]           = jet2.HFHadronMultiplicity();
    mPFAK8JetNumOfHFEM[jeti]            = jet2.HFEMMultiplicity();
    
    mPFAK8JetChaMuEng[jeti]             = jet2.chargedMuEnergy();
     
    mPFAK8JetNumOfNeu[jeti]             = jet2.neutralMultiplicity();
    mPFAK8JetNumOfCha[jeti]             = jet2.chargedMultiplicity();
    
    mPFAK8JetBtagTkCountHighEff[jeti]   = jet.bDiscriminator("trackCountingHighEffBJetTags");
    mPFAK8JetBtagTkCountHighPur[jeti]   = jet.bDiscriminator("trackCountingHighPurBJetTags");
    mPFAK8JetBtagJetProbability[jeti]   = jet.bDiscriminator("jetProbabilityBJetTags");
    mPFAK8JetBtagJetBProbability[jeti]  = jet.bDiscriminator("jetBProbabilityBJetTags");
    mPFAK8JetBTagCSVv2[jeti]            = jet.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags");
    mPFAK8JetBTagCombSecVtx[jeti]       = jet.bDiscriminator("combinedSecondaryVertexBJetTags");
    mPFAK8JetBTagCombSecVtxMVA[jeti]    = jet.bDiscriminator("combinedSecondaryVertexMVABJetTags");  
   
    ///-----------Jets Uncertanity --------------------------------------------------------
    if(isMCTag){
      //jecUnc->setJetEta(jet2.eta() ); // Give rapidity of jet you want tainty on
      //jecUnc->setJetPt( jet2.pt() );// Also give the corrected pt of the jet you want the uncertainty on
      // The following function gives the relative uncertainty in the jet Pt.
      // i.e. ptCorSmeared = (1 +- uncer) * ptCor  
      //mPFAK8uncer[jeti] =    jecUnc->getUncertainty(true); 
      // In principle, boolean controls if uncertainty on +ve or -ve side is returned (asymmetric errors) but not yet implemented.

      // Addition by Jyothsna: Jet flavour needed for b-tagging
      // Store algorithmic jet flavour definition
      mPFAK8JetFlavour[jeti] = jet.partonFlavour();
      // flavour of the parton matched to jet (0 if no match)
      mPFAK8JetGenPartonFlavour[jeti] = (jet.genParton() == nullptr) ? 0 : jet.genParton()->pdgId();
      
      if(jet2.genJet()!= 0){
	  mGenPFAK8JetPt[jeti]        = jet2.genJet()->pt();
	  mGenPFAK8JetE[jeti]         = jet2.genJet()->energy();
	  mGenPFAK8JetPx[jeti]        = jet2.genJet()->momentum().X();
	  mGenPFAK8JetPy[jeti]        = jet2.genJet()->momentum().Y();
	  mGenPFAK8JetPz[jeti]        = jet2.genJet()->momentum().z();
	  mGenPFAK8JetEta[jeti]       = jet2.genJet()->eta();
	  mGenPFAK8JetPhi[jeti]       = jet2.genJet()->phi();
	  mGenPFAK8JetEmEnergy[jeti]  = jet2.genJet()->emEnergy();
	  mGenPFAK8JetHadEnergy[jeti] = jet2.genJet()->emEnergy();  
	  if(fabs(mPFAK8JetEta[jeti])<0.5){
	    mPFAK8JERCentral[jeti] = 1.052;
	    mPFAK8JERUp[jeti]      = 0.990;
	    mPFAK8JERDown[jeti]    = 1.115;
	  } 
	  else if(fabs(mPFAK8JetEta[jeti])>=0.5 && fabs(mPFAK8JetEta[jeti])<1.1){
	    mPFAK8JERCentral[jeti] = 1.057;
	    mPFAK8JERUp[jeti]      = 1.001;
	    mPFAK8JERDown[jeti]    = 1.114;
	  }
	  else if(fabs(mPFAK8JetEta[jeti])>=1.1 && fabs(mPFAK8JetEta[jeti])<1.7){
	    mPFAK8JERCentral[jeti] = 1.096;
	    mPFAK8JERUp[jeti]      = 1.032;
	    mPFAK8JERDown[jeti]    = 1.161;
	  }
	  else if(fabs(mPFAK8JetEta[jeti])>=1.7 && fabs(mPFAK8JetEta[jeti])<2.3){
	    mPFAK8JERCentral[jeti] = 1.134;
	    mPFAK8JERUp[jeti]      = 1.042;
	    mPFAK8JERDown[jeti]    = 1.228;
	  }
	  else if(fabs(mPFAK8JetEta[jeti])>=2.3 && fabs(mPFAK8JetEta[jeti])<5.0){
	    mPFAK8JERCentral[jeti] = 1.288;
	    mPFAK8JERUp[jeti]      = 1.089;
	    mPFAK8JERDown[jeti]    = 1.488;
	  }
	  else{
	    mPFAK8JERCentral[jeti] = -999.;
	    mPFAK8JERUp[jeti]      = -999.;
	    mPFAK8JERDown[jeti]    = -999.;
	  }
	}
	else {
	  mGenPFAK8JetPt[jeti]        = -999;
	  mGenPFAK8JetE[jeti]         = -999;
	  mGenPFAK8JetPx[jeti]        = -999;
	  mGenPFAK8JetPy[jeti]        = -999;
	  mGenPFAK8JetPz[jeti]        = -999;
	  mGenPFAK8JetEta[jeti]       = -999;
	  mGenPFAK8JetPhi[jeti]       = -999;
	  mGenPFAK8JetEmEnergy[jeti]  = -999;
	  mGenPFAK8JetHadEnergy[jeti] = -999;
	  mPFAK8JERCentral[jeti]      = -999.;
	  mPFAK8JERUp[jeti]           = -999.;
	  mPFAK8JERDown[jeti]         = -999.;
	}
    }
    else{
      mPFAK8uncer[jeti]           = 0.;
      mGenPFAK8JetPt[jeti]        = -999;
      mGenPFAK8JetE[jeti]         = -999;
      mGenPFAK8JetPx[jeti]        = -999;
      mGenPFAK8JetPy[jeti]        = -999;
      mGenPFAK8JetPz[jeti]        = -999;
      mGenPFAK8JetEta[jeti]       = -999;
      mGenPFAK8JetPhi[jeti]       = -999;
      mGenPFAK8JetEmEnergy[jeti]  = -999;
      mGenPFAK8JetHadEnergy[jeti] = -999;
      mPFAK8JERCentral[jeti]      = -999.;
      mPFAK8JERUp[jeti]           = -999.;
      mPFAK8JERDown[jeti]         = -999.;
    }		
    jeti++;
  }
  mNPFAK8Jets = jeti; 
  if(debugMode){
    cout<<"N(PFJet): "<<mNPFAK8Jets<<endl;
  }
  if(debugMode && mNPFAK8Jets>0){
    cout<<"PFJet0: Pt= "<<mPFAK8JetPtCor[0]<<", Eta= "<<mPFAK8JetEta[0]<<", Phi= "<<mPFAK8JetPhi[0]<<", PUJetID= "<<mPFAK8JetPUFullJetId[0]<<endl;
    cout<<"      : NHEF= "<<mPFAK8JetNeuHadEngFrac[0]<<", CHEF= "<<mPFAK8JetChaHadEngFrac[0]<<", NEMF= "<<mPFAK8JetNeuEmEngFrac[0]<<", CEMF= "<<mPFAK8JetChaEmEngFrac[0]<<endl;
    cout<<"      : MuEF= "<<mPFAK8JetMuonEng[0]/mPFAK8JetE[0]<<", ElecEF= "<<mPFAK8JetElecEng[0]/mPFAK8JetE[0]<<", PhotonEF= "<<mPFAK8JetPhotEng[0]/mPFAK8JetE[0]<<endl;
  }
  if(debugMode && mNPFAK8Jets>1){
    cout<<"PFJet1: Pt= "<<mPFAK8JetPtCor[1]<<", Eta= "<<mPFAK8JetEta[1]<<", Phi= "<<mPFAK8JetPhi[1]<<", PUJetID= "<<mPFAK8JetPUFullJetId[1]<<endl;
    cout<<"      : NHEF= "<<mPFAK8JetNeuHadEngFrac[1]<<", CHEF= "<<mPFAK8JetChaHadEngFrac[1]<<", NEMF= "<<mPFAK8JetNeuEmEngFrac[1]<<", CEMF= "<<mPFAK8JetChaEmEngFrac[1]<<endl;
    cout<<"      : MuEF= "<<mPFAK8JetMuonEng[1]/mPFAK8JetE[1]<<", ElecEF= "<<mPFAK8JetElecEng[1]/mPFAK8JetE[1]<<", PhotonEF= "<<mPFAK8JetPhotEng[1]/mPFAK8JetE[1]<<endl;
  }
  
  
  ///-------------------------------------------------------------------------- 
  /// MET: Initialization
  ///--------------------------------------------------------------------------
  mNMet = MAXMET;
  for(int i=0; i<mNMet; i++){
    mMetPt[i]=0; 
    mMetPhi[i]=0; 
    mMetSumEt[i]=0; 
    mMetPx[i]=0; 
    mMetPy[i]=0; 
    mMetSign[i]=0;
  }
  
  
  ///-------------------------------------------------------------------------- 
  /// CaloMET
  ///--------------------------------------------------------------------------
  /*
  Handle<View<pat::MET> > CaloMetHand;
  //iEvent.getByLabel("patMETsAK4Calo",CaloMetHand);
  iEvent.getByLabel(METTag_,CaloMetHand);
  if(CaloMetHand.isValid()){
    const View<pat::MET> & mets = *CaloMetHand;
    // 0=full corr	1=uncorrNone  2=uncorrALL 3=uncorrJES  4=uncorrMUON  5=TAU
    mMetPt[0]    = mets[0].et();
    mMetPhi[0]   = mets[0].phi();
    mMetPx[0]    = mets[0].px();
    mMetPy[0]    = mets[0].py();
    mMetSumEt[0] = mets[0].sumEt();
    //
    mMetPt[1]    = mets[0].uncorrectedPt(pat::MET::uncorrNONE); 
    mMetPhi[1]   = mets[0].uncorrectedPhi(pat::MET::uncorrNONE); 
    mMetPx[1]    = mets[0].corEx(pat::MET::uncorrNONE); 
    mMetPy[1]    = mets[0].corEy(pat::MET::uncorrNONE); 
    mMetSumEt[1] = mets[0].corSumEt(pat::MET::uncorrNONE); 
    //
    mMetPt[2]    = mets[0].uncorrectedPt(pat::MET::uncorrALL); 
    mMetPhi[2]   = mets[0].uncorrectedPhi(pat::MET::uncorrALL); 
    mMetPx[2]    = mets[0].corEx(pat::MET::uncorrALL); 
    mMetPy[2]    = mets[0].corEy(pat::MET::uncorrALL); 
    mMetSumEt[2] = mets[0].corSumEt(pat::MET::uncorrALL); 
    //
    mMetPt[3]    = mets[0].uncorrectedPt(pat::MET::uncorrJES); 
    mMetPhi[3]   = mets[0].uncorrectedPhi(pat::MET::uncorrJES); 
    mMetPx[3]    = mets[0].corEx(pat::MET::uncorrJES); 
    mMetPy[3]    = mets[0].corEy(pat::MET::uncorrJES); 
    mMetSumEt[3] = mets[0].corSumEt(pat::MET::uncorrJES); 
    //
    mMetPt[4]    = mets[0].uncorrectedPt(pat::MET::uncorrMUON); 
    mMetPhi[4]   = mets[0].uncorrectedPhi(pat::MET::uncorrMUON); 
    mMetPx[4]    = mets[0].corEx(pat::MET::uncorrMUON); 
    mMetPy[4]    = mets[0].corEy(pat::MET::uncorrMUON); 
    mMetSumEt[4] = mets[0].corSumEt(pat::MET::uncorrMUON); 
    //
    mMetPt[5]    = mets[0].uncorrectedPt(pat::MET::uncorrTAU); 
    mMetPhi[5]   = mets[0].uncorrectedPhi(pat::MET::uncorrTAU); 
    mMetPx[5]    = mets[0].corEx(pat::MET::uncorrTAU); 
    mMetPy[5]    = mets[0].corEy(pat::MET::uncorrTAU); 
    mMetSumEt[5] = mets[0].corSumEt(pat::MET::uncorrTAU); 
    if(debugMode){
      std::cout<<"CaloMET= "<<mMetPt[0]<<", METPhi= "<<mMetPhi[0]<<std::endl;
    }
  }
  */
  
  ///-------------------------------------------------------------------------- 
  /// GenMET
  ///--------------------------------------------------------------------------		
  /*
  if(isMCTag){	
    const GenMET *genmet;
    edm::Handle<GenMETCollection> gen;
    iEvent.getByLabel("genMetCalo", gen);
    if(!gen.isValid()){
      edm::LogInfo("OutputInfo") << " failed to retrieve data required by MET Task";
      edm::LogInfo("OutputInfo") << " MET Task cannot continue...!";
      return;
    } 
    else{
      const GenMETCollection *genmetcol = gen.product();
      genmet = &(genmetcol->front());
    }  
    
    mMetPt[6]    =  genmet->pt();
    mMetPhi[6]   =  genmet->phi();
    mMetPx[6]    =  genmet->px();
    mMetPy[6]    =  genmet->py();
    mMetSumEt[6] =  genmet->sumEt();
  }
  */
  
  ///-------------------------------------------------------------------------- 
  /// PF MetsType1 Corrected
  ///--------------------------------------------------------------------------
  edm::Handle<edm::View<pat::MET> > PFMetType1Hand;
  iEvent.getByLabel(PFMETTag_,PFMetType1Hand);
  if(PFMetType1Hand.isValid()){
    const edm::View<pat::MET> & pfmetsType1 = *PFMetType1Hand;
    mMetPt[10]    = pfmetsType1[0].et();
    mMetPhi[10]   = pfmetsType1[0].phi();
    mMetPx[10]    = pfmetsType1[0].px();
    mMetPy[10]    = pfmetsType1[0].py();
    mMetSumEt[10] = pfmetsType1[0].sumEt();
    if(debugMode){
      std::cout<<"PFMET= "<<mMetPt[10]<<", METPhi= "<<mMetPhi[10]<<std::endl;
    }
  }
  
  
  ///-------------------------------------------------------------------------- 
  /// PF Mets
  ///--------------------------------------------------------------------------
  Handle<View<pat::MET> > PFMetHand;
  iEvent.getByLabel("patMETsPF", PFMetHand);
  if(PFMetHand.isValid()){
    const View<pat::MET> & pfmets = *PFMetHand;
    //
    mMetPt[11]    = pfmets[0].et();
    mMetPhi[11]   = pfmets[0].phi();
    mMetPx[11]    = pfmets[0].px();
    mMetPy[11]    = pfmets[0].py();
    mMetSumEt[11] = pfmets[0].sumEt();
    // 
    // CSC removed uncorrected MET values
    /*mMetPt[12]    = pfmets[0].uncorrectedPt(pat::MET::uncorrALL); 
    mMetPhi[12]   = pfmets[0].uncorrectedPhi(pat::MET::uncorrALL); 
    mMetPx[12]    = pfmets[0].corEx(pat::MET::uncorrALL); 
    mMetPy[12]    = pfmets[0].corEy(pat::MET::uncorrALL); 
    mMetSumEt[12] = pfmets[0].corSumEt(pat::MET::uncorrALL); */
    //
    // CSC removed uncorrected MET values
    /*mMetPt[13]    = pfmets[0].uncorrectedPt(pat::MET::uncorrJES); 
    mMetPhi[13]   = pfmets[0].uncorrectedPhi(pat::MET::uncorrJES); 
    mMetPx[13]    = pfmets[0].corEx(pat::MET::uncorrJES); 
    mMetPy[13]    = pfmets[0].corEy(pat::MET::uncorrJES); 
    mMetSumEt[13] = pfmets[0].corSumEt(pat::MET::uncorrJES); */
    //
    // CSC removed uncorrected MET values
    /*mMetPt[14]    = pfmets[0].uncorrectedPt(pat::MET::uncorrMUON); 
    mMetPhi[14]   = pfmets[0].uncorrectedPhi(pat::MET::uncorrMUON); 
    mMetPx[14]    = pfmets[0].corEx(pat::MET::uncorrMUON); 
    mMetPy[14]    = pfmets[0].corEy(pat::MET::uncorrMUON); 
    mMetSumEt[14] = pfmets[0].corSumEt(pat::MET::uncorrMUON); */
    //
    // CSC removed uncorrected MET values
    /*mMetPt[15]    = pfmets[0].uncorrectedPt(pat::MET::uncorrTAU); 
    mMetPhi[15]   = pfmets[0].uncorrectedPhi(pat::MET::uncorrTAU); 
    mMetPx[15]    = pfmets[0].corEx(pat::MET::uncorrTAU); 
    mMetPy[15]    = pfmets[0].corEy(pat::MET::uncorrTAU); 
    mMetSumEt[15] = pfmets[0].corSumEt(pat::MET::uncorrTAU); */
  }
  
  
  ///-------------------------------------------------------------------------- 
  /// Pat MHT
  ///--------------------------------------------------------------------------
  /*
    edm::Handle<edm::View<pat::MHT> > MHTHand;
    iEvent.getByLabel("patMHTsAK4Calo",MHTHand);
    const edm::View<pat::MHT> & mhts = *MHTHand;
    if ( MHTHand->size()!=0){
    mMHTPt    = mhts[0].pt();
    mMHTPhi   = mhts[0].phi();
    mMHTSign  = mhts[0].significance();
    }
  */
  
  
  ///-------------------------------------------------------------------------- 
  /// Vertices
  ///--------------------------------------------------------------------------

   reco::Vertex primaryvtx;
  Handle<reco::VertexCollection> recVtxs;
  iEvent.getByLabel("offlineSlimmedPrimaryVertices",recVtxs);
  //iEvent.getByLabel("goodVertices",recVtxs);
  int  pvind=0;
  for(unsigned int ind=0;ind<recVtxs->size();ind++){
    if(!((*recVtxs)[ind].isFake())){
      mPVx[pvind]      = (*recVtxs)[ind].x();
      mPVy[pvind]      = (*recVtxs)[ind].y();
      mPVz[pvind]      = (*recVtxs)[ind].z();
      mPVchi2[pvind]   = (*recVtxs)[ind].chi2();
      mPVndof[pvind]   = (*recVtxs)[ind].ndof();
      //mPVntracks[pvind]= (*recVtxs)[ind].tracksSize();
      if(pvind == 0) primaryvtx = (*recVtxs)[ind];
      pvind++;
      if(pvind>=MAXVTX) break;
    }
  }
  mNPV = pvind; 

  
  ///-------------------------------------------------------------------------- 
  /// BeamSpot
  ///--------------------------------------------------------------------------
  BeamSpot beamSpot;
  Handle<reco::BeamSpot> beamSpotHandle;
  iEvent.getByLabel("offlineBeamSpot", beamSpotHandle);
  if (beamSpotHandle.isValid()){
    beamSpot = *beamSpotHandle;
  } 
  else{
    edm::LogInfo("MyAnalyzer")
      << "No beam spot available from EventSetup \n";
  }
  math::XYZPoint point(beamSpot.x0(),beamSpot.y0(), beamSpot.z0());

  
  ///-------------------------------------------------------------------------- 
  /// PFMuons
  ///--------------------------------------------------------------------------
  edm::Handle<edm::View<pat::Muon> > MuonHand;
  iEvent.getByLabel(PFMuonTag_,MuonHand);
  int muoni=0;
  for(unsigned int ind=0; ind<(*MuonHand).size() && ind<MAXMUON; ind++){ 
    const pat::Muon& muon             = (*MuonHand)[ind];
    mPFMuonPt[muoni]                  = muon.pt();
    mPFMuonEt[muoni]                  = muon.et();
    mPFMuonE[muoni]                   = muon.energy();
    mPFMuonPx[muoni]                  = muon.momentum().X();
    mPFMuonPy[muoni]                  = muon.momentum().Y();
    mPFMuonPz[muoni]                  = muon.momentum().Z();
    mPFMuonEta[muoni]                 = muon.eta();
    mPFMuonPhi[muoni]                 = muon.phi();
    mPFMuonCharge[muoni]              = muon.charge();
    //
    mPFMuonIsGlobal[muoni]            = muon.isGlobalMuon();
    mPFMuonIsStandAlone[muoni]        = muon.isStandAloneMuon();
    mPFMuonIsTracker[muoni]           = muon.isTrackerMuon();
    mPFMuonIsPF[muoni]                = muon.isPFMuon();
    //
    mPFMuonIsTight[muoni]             = muon.isTightMuon(primaryvtx);
    mPFMuonIsLoose[muoni]             = muon.isLooseMuon();
    //
    mPFMuonChargedHadronIso[muoni]    = muon.chargedHadronIso();
    mPFMuonPhotonIso[muoni]           = muon.photonIso();
    mPFMuonNeutralHadronIso[muoni]    = muon.neutralHadronIso();
    mPFMuonisGMPT[muoni]              = muon.muonID("GlobalMuonPromptTight");
    mPFMuonR04ChargedHadronPt[muoni]  = muon.pfIsolationR04().sumChargedHadronPt;
    mPFMuonR04NeutralHadronEt[muoni]  = muon.pfIsolationR04().sumNeutralHadronEt;
    mPFMuonR04PhotonEt[muoni]         = muon.pfIsolationR04().sumPhotonEt;
    mPFMuonR04PUPt[muoni]             = muon.pfIsolationR04().sumPUPt;
    //
    mPFMuonNumOfMatches[muoni]        = muon.numberOfMatches(); 
    mPFMuonNumOfMatchesStation[muoni] = muon.numberOfMatchedStations();
    // Global Track
    if(muon.globalTrack().isNonnull()){
      mPFMuonGlobChi2Norm[muoni]            = muon.globalTrack()->normalizedChi2();
      mPFMuonGlobNumOfValidMuonHits[muoni]  = muon.globalTrack()->hitPattern().numberOfValidMuonHits();
    }
    else{
      mPFMuonGlobChi2Norm[muoni]            = 999.;
      mPFMuonGlobNumOfValidMuonHits[muoni]  = 999.;
    }
    // BestTrack
    if(muon.muonBestTrack().isNonnull()){
      mPFMuonBestTrkdxy[muoni]              = fabs(muon.muonBestTrack()->dxy(primaryvtx.position()));
      mPFMuonBestTrkdz[muoni]               = fabs(muon.muonBestTrack()->dz(primaryvtx.position()));
    }
    else{
      mPFMuonBestTrkdxy[muoni]              = 999.;
      mPFMuonBestTrkdz[muoni]               = 999.;
    }
    // InnerTrack
    if(muon.innerTrack().isNonnull()){
      mPFMuoninnertrackPt[muoni]     = muon.innerTrack()->pt();
      mPFMuonnValidHits[muoni]       = muon.innerTrack()->numberOfValidHits();
      mPFMuonnValidPixelHits[muoni]  = muon.innerTrack()->hitPattern().numberOfValidPixelHits();
      mPFMuondxy[muoni]              = muon.innerTrack()->dxy(primaryvtx.position());
      mPFMuondz[muoni]               = muon.vz() - primaryvtx.position().z();
    }
    else{
      mPFMuoninnertrackPt[muoni]     = 999.;
      mPFMuonnValidHits[muoni]       = 999.;
      mPFMuonnValidPixelHits[muoni]  = 999.;
      mPFMuondxy[muoni]              = 999.;
      mPFMuondz[muoni]               = 999.;
    }
    // GlobalMuon
    if(muon.isGlobalMuon() && muon.combinedMuon().isNonnull()){
      mPFMuonCombChi2Norm[muoni]    = muon.combinedMuon()->normalizedChi2();
      mPFMuonCombValidHits[muoni]   = muon.combinedMuon()->found();
      mPFMuonCombLostHits[muoni]    = muon.combinedMuon()->lost();
      mPFMuonCombPt[muoni]          = muon.combinedMuon()->pt();
      mPFMuonCombPz[muoni]          = muon.combinedMuon()->pz();
      mPFMuonCombP[muoni]           = muon.combinedMuon()->p();
      mPFMuonCombEta[muoni]         = muon.combinedMuon()->eta();
      mPFMuonCombPhi[muoni]         = muon.combinedMuon()->phi();
      mPFMuonCombChi2[muoni]        = muon.combinedMuon()->chi2();
      mPFMuonCombCharge[muoni]      = muon.combinedMuon()->charge();
      mPFMuonCombQOverPError[muoni] = muon.combinedMuon()->qoverpError();		
      mPFMuonCombNdof[muoni]        = muon.combinedMuon()->ndof();
      mPFMuonCombVx[muoni]          = muon.combinedMuon()->vx();
      mPFMuonCombVy[muoni]          = muon.combinedMuon()->vy();
      mPFMuonCombVz[muoni]          = muon.combinedMuon()->vz();
      mPFMuonCombD0[muoni]          = muon.combinedMuon()->d0();
      mPFMuonCombDz[muoni]          = muon.combinedMuon()->dz();
    } 
    else{
      mPFMuonCombChi2Norm[muoni]    = 999.;
      mPFMuonCombValidHits[muoni]   = 999.;
      mPFMuonCombLostHits[muoni]    = 999.;
      mPFMuonCombPt[muoni]          = 999.;
      mPFMuonCombPz[muoni]          = 999.;
      mPFMuonCombP[muoni]           = 999.;
      mPFMuonCombEta[muoni]         = 999.;
      mPFMuonCombPhi[muoni]         = 999.;
      mPFMuonCombChi2[muoni]        = 999.;
      mPFMuonCombCharge[muoni]      = 999.;
      mPFMuonCombQOverPError[muoni] = 999.;
      mPFMuonCombNdof[muoni]        = 999.;
      mPFMuonCombVx[muoni]          = 999.;
      mPFMuonCombVy[muoni]          = 999.;
      mPFMuonCombVz[muoni]          = 999.;
      mPFMuonCombD0[muoni]          = 999.;
      mPFMuonCombDz[muoni]          = 999.;
    }
    // StandAloneMuon
    if(muon.standAloneMuon().isNonnull()){
      mPFMuonStandChi2Norm[muoni]    = muon.standAloneMuon()->normalizedChi2();
      mPFMuonStandValidHits[muoni]   = muon.standAloneMuon()->found();
      mPFMuonStandLostHits[muoni]    = muon.standAloneMuon()->lost();
      mPFMuonStandPt[muoni]          = muon.standAloneMuon()->pt();
      mPFMuonStandPz[muoni]          = muon.standAloneMuon()->pz();
      mPFMuonStandP[muoni]           = muon.standAloneMuon()->p();
      mPFMuonStandEta[muoni]         = muon.standAloneMuon()->eta();
      mPFMuonStandPhi[muoni]         = muon.standAloneMuon()->phi();
      mPFMuonStandChi2[muoni]        = muon.standAloneMuon()->chi2();
      mPFMuonStandCharge[muoni]      = muon.standAloneMuon()->charge();
      mPFMuonStandQOverPError[muoni] = muon.standAloneMuon()->qoverpError();
    } 
    else{
      mPFMuonStandChi2Norm[muoni]    = 999.; 
      mPFMuonStandValidHits[muoni]   = 999.; 
      mPFMuonStandLostHits[muoni]    = 999.; 
      mPFMuonStandPt[muoni]          = 999.; 
      mPFMuonStandPz[muoni]          = 999.; 
      mPFMuonStandP[muoni]           = 999.; 
      mPFMuonStandEta[muoni]         = 999.; 
      mPFMuonStandPhi[muoni]         = 999.; 
      mPFMuonStandChi2[muoni]        = 999.; 
      mPFMuonStandCharge[muoni]      = 999.; 
      mPFMuonStandQOverPError[muoni] = 999.; 
    }
    // Track
    if(muon.track().isNonnull()){
      mPFMuonTrkChi2Norm[muoni]    = muon.track()->normalizedChi2();
      mPFMuonTrkValidHits[muoni]   = muon.track()->found();
      mPFMuonTrkLostHits[muoni]    = muon.track()->lost();
      mPFMuonTrkPt[muoni]          = muon.track()->pt();
      mPFMuonTrkPz[muoni]          = muon.track()->pz();
      mPFMuonTrkP[muoni]           = muon.track()->p();
      mPFMuonTrkEta[muoni]         = muon.track()->eta();
      mPFMuonTrkPhi[muoni]         = muon.track()->phi();
      mPFMuonTrkChi2[muoni]        = muon.track()->chi2();
      mPFMuonTrkCharge[muoni]      = muon.track()->charge();
      mPFMuonTrkQOverPError[muoni] = muon.track()->qoverpError();
      mPFMuonTrkDxy[muoni]         = muon.track()->dxy( beamSpotHandle->position() );
      mPFMuonTrkNumOfValidPixHits[muoni] =  muon.track()->hitPattern().numberOfValidPixelHits();
      mPFMuonTrkLayerWithMeasurement[muoni] = muon.track()->hitPattern().trackerLayersWithMeasurement(); 
    }
    else{
      mPFMuonTrkChi2Norm[muoni]    = 999.; 
      mPFMuonTrkValidHits[muoni]   = 999.; 
      mPFMuonTrkLostHits[muoni]    = 999.; 
      mPFMuonTrkPt[muoni]          = 999.; 
      mPFMuonTrkPz[muoni]          = 999.; 
      mPFMuonTrkP[muoni]           = 999.; 
      mPFMuonTrkEta[muoni]         = 999.; 
      mPFMuonTrkPhi[muoni]         = 999.; 
      mPFMuonTrkChi2[muoni]        = 999.; 
      mPFMuonTrkCharge[muoni]      = 999.; 
      mPFMuonTrkQOverPError[muoni] = 999.; 
      mPFMuonTrkDxy[muoni]         = 999.;
      mPFMuonTrkNumOfValidPixHits[muoni] = 999.;
      mPFMuonTrkLayerWithMeasurement[muoni] = 999.;
    }
    muoni++;
    if(muoni>=MAXMUON) break;
  }
  mNPFMuon=muoni;
  if(debugMode){
    cout<<"NPFMuons: "<<mNPFMuon<<endl;
    if(mNPFMuon>0){
      cout<<"PFMuon0: Pt= "<<mPFMuonPt[0]
	  <<", Eta= "<<mPFMuonEta[0]
	  <<", Phi= "<<mPFMuonPhi[0]
	  <<", R04ChargedHadronPt= "<<mPFMuonR04ChargedHadronPt[0]
	  <<", R04NeutralHadronEt= "<<mPFMuonR04NeutralHadronEt[0]
	  <<", R04PhotonEt= "<<mPFMuonR04PhotonEt[0]
	  <<", R04PUPt="<<mPFMuonR04PUPt[muoni]
	  <<", Tight= "<<mPFMuonIsTight[0]
	  <<", Loose= "<<mPFMuonIsLoose[0]
	  <<endl;
    }
  }

  
  ///-------------------------------------------------------------------------- 
  /// PFElectrons
  ///--------------------------------------------------------------------------
  edm::Handle<edm::View<pat::Electron> > ElecHand;
  iEvent.getByLabel(PFElectronTag_,ElecHand);
  int eleci=0;
  for(unsigned int ind=0; ind<(*ElecHand).size() && ind<MAXELEC; ind++){ 
    const pat::Electron& elec = (*ElecHand)[ind];
    
    mPFElecE[eleci]                    = elec.energy();
    mPFElecPt[eleci]                   = elec.pt();
    mPFElecPx[eleci]                   = elec.px();
    mPFElecPy[eleci]                   = elec.py();
    mPFElecPz[eleci]                   = elec.pz();
    mPFElecEta[eleci]                  = elec.eta();
    mPFElecPhi[eleci]                  = elec.phi();
    mPFElecCharge[eleci]               = elec.charge();
    
    mPFElecCharHadIso[eleci]           = elec.chargedHadronIso();
    mPFElecPhoIso[eleci]               = elec.photonIso();
    mPFElecNeuHadIso[eleci]            = elec.neutralHadronIso();
    //mPFElecMva[eleci]                  = elec.mva();
    
    mPFElecEffArea[eleci]              = 0.;
    
    mPFElecdxy[eleci]                  = elec.gsfTrack()->dxy(primaryvtx.position());
    mPFElecdz[eleci]                   = elec.gsfTrack()->dz(primaryvtx.position());
    
    mPFElecIsEB[eleci]                 = elec.isEB();
    mPFElecHadOverEm[eleci]            = elec.hadronicOverEm();
    mPFElecHcalOverEm[eleci]           = elec.hcalOverEcal();
    mPFElecSupClusOverP[eleci]         = elec.eSuperClusterOverP();
    
    //mPFElecInnerHits[eleci]            = elec.gsfTrack()->trackerExpectedHitsInner().numberOfHits();
    mPFElecConvDist[eleci]             = elec.convDist();//convInfo.dist();
    mPFElecConvDcot[eleci]             = elec.convDcot();//convInfo.dcot();
    
    mPFElecEcalDrivenSeed[eleci]       = elec.ecalDrivenSeed();
    mPFElecdB[eleci]                   = elec.dB();
    mPFElecNumOfValidHits[eleci]       = elec.gsfTrack()->numberOfValidHits();
    mPFElecdr03HcalTowerSumEt[eleci]   = elec.dr03HcalTowerSumEt();//hcalIso();
    mPFElecdr03EcalRecHitSumEt[eleci]  = elec.dr03EcalRecHitSumEt();//ecalIso();
    mPFElecdr03TkSumPt[eleci]          = elec.dr03TkSumPt(); //trackIso();
    mPFElecNormChi2[eleci]             = elec.gsfTrack()->normalizedChi2();
    mPFElecfbrem[eleci]                = elec.fbrem();
    mPFElecDEtaSuTrAtVtx[eleci]        = elec.deltaEtaSuperClusterTrackAtVtx();
    mPFElecDPhiSuTrAtVtx[eleci]        = elec.deltaPhiSuperClusterTrackAtVtx();
    mPFElecHadronicOverEm[eleci]       = elec.hadronicOverEm();
    mPFElecHcalOverEcal[eleci]         = elec.hcalOverEcal();
    mPFElecSuperClusOverP[eleci]       = elec.eSuperClusterOverP();
    mPFElecetaeta[eleci]               = elec.sigmaEtaEta();
    mPFElecietaieta[eleci]             = elec.sigmaIetaIeta();
    
    mPFeidLoose[eleci]                 = ((int)elec.electronID("eidLoose") & 1);
    mPFeidRobustHighEnergy[eleci]      = ((int)elec.electronID("eidRobustHighEnergy") & 1);
    mPFeidRobustLoose[eleci]           = ((int)elec.electronID("eidRobustLoose") & 1);
    mPFeidRobustTight[eleci]           = ((int)elec.electronID("eidRobustTight") & 1);
    mPFeidTight[eleci]                 = ((int)elec.electronID("eidTight") & 1);
    //mPFevspi[eleci]                    = ((int)elec.electronID("pf_evspi") & 1);
    //mPFevsmu[eleci]                    = ((int)elec.electronID("pf_evsmu") & 1);

    //ID
    //float ooemoop                      = (1.0/elec.ecalEnergy() - elec.eSuperClusterOverP()/elec.ecalEnergy());
    //bool vtxFitConversion              = ConversionTools::hasMatchedConversion(elec, conversions_h, beamSpotHandle->position());
    mPFElecIsTight[eleci]              = 0.;
    mPFElecIsMedium[eleci]             = 0.;
    mPFElecIsLoose[eleci]              = 0.;
    mPFElecIsVeto[eleci]               = 0.;
    
    eleci++;
    if(eleci>=MAXELEC) break;
  }
  mNPFElec=eleci;
  if(debugMode){
    cout<<"NPFElectrons: "<<mNPFElec<<endl;
    if(mNPFElec>0){
      cout<<"PFElec0: Pt= "<<mPFElecPt[0]
	  <<", Eta= "<<mPFElecEta[0]
	  <<", Phi= "<<mPFElecPhi[0]
	  <<", CharHadIso= "<<mPFElecCharHadIso[0]
	  <<", PhoIso= "<<mPFElecPhoIso[0]
	  <<", NeuHadIso= "<<mPFElecNeuHadIso[0]
	  <<", EffArea= "<<mPFElecEffArea[0]
	  <<", Tight= "<<mPFElecIsTight[0]
	  <<", Veto= "<<mPFElecIsVeto[0]
	  <<endl;
    }
  }
  
  
  ///-------------------------------------------------------------------------- 
  /// PFTaus
  ///--------------------------------------------------------------------------
  //edm::Handle<pat::TauCollection> TauHand;
  //iEvent.getByLabel(PFTauTag_,TauHand);
  int taui=0;
  /*
  for(unsigned int ind=0; ind<(*TauHand).size() && ind<MAXTAU; ind++){ 
    const pat::Tau& tau = (*TauHand)[ind];
    
    mPFTauPt[taui]                                             = tau.pt();
    mPFTauPx[taui]                                             = tau.momentum().X();
    mPFTauPy[taui]                                             = tau.momentum().Y();
    mPFTauPz[taui]                                             = tau.momentum().Z();
    mPFTauE[taui]                                              = tau.energy();
    mPFTauEta[taui]                                            = tau.eta();
    mPFTauPhi[taui]                                            = tau.phi();
    mPFTauCharge[taui]                                         = tau.charge();
    
    //
    mPFTauEtaEtaMoment[taui]                                   = tau.etaetaMoment();
    mPFTauPhiPhiMoment[taui]                                   = tau.phiphiMoment();
    mPFTauEtaPhiMoment[taui]                                   = tau.etaphiMoment();
    
    //PF objects
    mPFTauLeadPFChargedHadrCandsignedSipt[taui]                = tau.leadPFChargedHadrCandsignedSipt();  
    mPFTauIsoPFChargedHadrCandsPtSum[taui]                     = tau.isolationPFChargedHadrCandsPtSum();
    mPFTauIsoPFGammaCandsEtSum[taui]                           = tau.isolationPFGammaCandsEtSum();      
    mPFTauMaximumHCALPFClusterEt[taui]                         = tau.maximumHCALPFClusterEt();          
    mPFTauEmFraction[taui]                                     = tau.emFraction();                      
    mPFTauHcalTotOverPLead[taui]                               = tau.hcalTotOverPLead();                
    mPFTauHcalMaxOverPLead[taui]                               = tau.hcalMaxOverPLead();                
    mPFTauHcal3x3OverPLead[taui]                               = tau.hcal3x3OverPLead();                
    mPFTauEcalStripSumEOverPLead[taui]                         = tau.ecalStripSumEOverPLead();          
    mPFTauBremsRecoveryEOverPLead[taui]                        = tau.bremsRecoveryEOverPLead();                 
    mPFTauElectronPreIDOutput[taui]                            = tau.electronPreIDOutput();             
    mPFTauElectronPreIDDecision[taui]                          = tau.electronPreIDDecision();            
    mPFTauCaloComp[taui]                                       = tau.caloComp();                        
    mPFTauSegComp[taui]                                        = tau.segComp();                         
    mPFTauMuonDecision[taui]                                   = tau.muonDecision(); 
    
    //
    mPFTausignalPFChargedHadrCands[taui]                       = tau.signalPFChargedHadrCands().size();
    mPFTausignalPFGammaCands[taui]                             = tau.signalPFGammaCands().size();
    
    //ID
    mPFTauDisAgainstElectronDeadECAL[taui]                     = tau.tauID("againstElectronDeadECAL"); //OK
    mPFTauDisAgainstElectronLoose[taui]                        = tau.tauID("againstElectronLoose");
    mPFTauDisAgainstElectronLooseMVA3[taui]                    = tau.tauID("againstElectronLooseMVA3");
    mPFTauDisAgainstElectronMedium[taui]                       = tau.tauID("againstElectronMedium");
    mPFTauDisAgainstElectronMediumMVA3[taui]                   = tau.tauID("againstElectronMediumMVA3");
    mPFTauDisAgainstElectronMVA3category[taui]                 = tau.tauID("againstElectronMVA3category");
    mPFTauDisAgainstElectronMVA3raw[taui]                      = tau.tauID("againstElectronMVA3raw");
    mPFTauDisAgainstElectronTight[taui]                        = tau.tauID("againstElectronTight");
    mPFTauDisAgainstElectronTightMVA3[taui]                    = tau.tauID("againstElectronTightMVA3");
    mPFTauDisAgainstElectronVTightMVA3[taui]                   = tau.tauID("againstElectronVTightMVA3");
    mPFTauDisAgainstElectronLooseMVA2[taui]                    = 0.;//tau.tauID("againstElectronLooseMVA2");
    mPFTauDisAgainstElectronMediumMVA2[taui]                   = 0.;//tau.tauID("againstElectronMediumMVA2");
    mPFTauDisAgainstElectronMVA2category[taui]                 = 0.;//tau.tauID("againstElectronMVA2category");
    mPFTauDisAgainstElectronMVA2raw[taui]                      = 0.;//tau.tauID("againstElectronMVA2raw");
    mPFTauDisAgainstElectronMVA[taui]                          = 0.;//tau.tauID("againstElectronMVA");
    mPFTauDisAgainstElectronTightMVA2[taui]                    = 0.;//tau.tauID("againstElectronTightMVA2");
    mPFTauDisAgainstElectronVLooseMVA2[taui]                   = 0.;//tau.tauID("againstElectronVLooseMVA2");
    // 
    mPFTauDisAgainstMuonLoose2[taui]                           = tau.tauID("againstMuonLoose2");
    //mPFTauDisAgainstMuonLoose3[taui]                           = tau.tauID("againstMuonLoose3");
    mPFTauDisAgainstMuonLoose[taui]                            = tau.tauID("againstMuonLoose");
    mPFTauDisAgainstMuonMedium2[taui]                          = tau.tauID("againstMuonMedium2");
    mPFTauDisAgainstMuonMedium[taui]                           = tau.tauID("againstMuonMedium");
    mPFTauDisAgainstMuonTight2[taui]                           = tau.tauID("againstMuonTight2");
    //mPFTauDisAgainstMuonTight3[taui]                           = tau.tauID("againstMuonTight3");
    mPFTauDisAgainstMuonTight[taui]                            = tau.tauID("againstMuonTight");
    //
    mPFTauDisByCombinedIsolationDeltaBetaCorrRaw3Hits[taui]    = tau.tauID("byCombinedIsolationDeltaBetaCorrRaw3Hits");
    mPFTauDisByCombinedIsolationDeltaBetaCorrRaw[taui]         = tau.tauID("byCombinedIsolationDeltaBetaCorrRaw");
    //
    mPFTauDisByIsolationMVA2raw[taui]                          = tau.tauID("byIsolationMVA2raw");
    mPFTauDisByIsolationMVAraw[taui]                           = tau.tauID("byIsolationMVAraw");
    //
    mPFTauDisByLooseCombinedIsolationDeltaBetaCorr3Hits[taui]  = tau.tauID("byLooseCombinedIsolationDeltaBetaCorr3Hits");
    mPFTauDisByLooseCombinedIsolationDeltaBetaCorr[taui]       = tau.tauID("byLooseCombinedIsolationDeltaBetaCorr");
    mPFTauDisByLooseIsolation[taui]                            = tau.tauID("byLooseIsolation");
    mPFTauDisByLooseIsolationDeltaBetaCorr[taui]               = tau.tauID("byLooseIsolationDeltaBetaCorr");
    mPFTauDisByLooseIsolationMVA2[taui]                        = tau.tauID("byLooseIsolationMVA2");
    mPFTauDisByLooseIsolationMVA[taui]                         = tau.tauID("byLooseIsolationMVA");
    //
    mPFTauDisByMediumCombinedIsolationDeltaBetaCorr3Hits[taui] = tau.tauID("byMediumCombinedIsolationDeltaBetaCorr3Hits");
    mPFTauDisByMediumCombinedIsolationDeltaBetaCorr[taui]      = tau.tauID("byMediumCombinedIsolationDeltaBetaCorr");
    mPFTauDisByMediumIsolation[taui]                           = tau.tauID("byMediumIsolation");
    mPFTauDisByMediumIsolationDeltaBetaCorr[taui]              = tau.tauID("byMediumIsolationDeltaBetaCorr");
    mPFTauDisByMediumIsolationMVA2[taui]                       = tau.tauID("byMediumIsolationMVA2");
    mPFTauDisByMediumIsolationMVA[taui]                        = tau.tauID("byMediumIsolationMVA");
    //
    mPFTauDisByTightCombinedIsolationDeltaBetaCorr3Hits[taui]  = tau.tauID("byTightCombinedIsolationDeltaBetaCorr3Hits");
    mPFTauDisByTightCombinedIsolationDeltaBetaCorr[taui]       = tau.tauID("byTightCombinedIsolationDeltaBetaCorr");
    mPFTauDisByTightIsolation[taui]                            = tau.tauID("byTightIsolation");
    mPFTauDisByTightIsolationDeltaBetaCorr[taui]               = tau.tauID("byTightIsolationDeltaBetaCorr");
    mPFTauDisByTightIsolationMVA2[taui]                        = tau.tauID("byTightIsolationMVA2");
    mPFTauDisByTightIsolationMVA[taui]                         = tau.tauID("byTightIsolationMVA");
    //
    mPFTauDisByVLooseCombinedIsolationDeltaBetaCorr[taui]      = tau.tauID("byVLooseCombinedIsolationDeltaBetaCorr");
    mPFTauDisByVLooseIsolation[taui]                           = tau.tauID("byVLooseIsolation");
    mPFTauDisByVLooseIsolationDeltaBetaCorr[taui]              = tau.tauID("byVLooseIsolationDeltaBetaCorr");
    //
    mPFTauDisDecayModeFinding[taui]                            = tau.tauID("decayModeFinding");

    //GenJet Pt
    mPFTauJetPt[taui]  =  -99.;
    mPFTauJetEta[taui] =  -99.;
    mPFTauJetPhi[taui] =  -99.;
    if(isMCTag && tau.genJet()!= 0){
      mPFTauJetPt[taui]  =  tau.genJet()->pt();
      mPFTauJetEta[taui] =  tau.genJet()->eta();
      mPFTauJetPhi[taui] =  tau.genJet()->phi();
    }
    
    taui++;  
    if(taui>=MAXTAU) break; 
  }
  */
  mNPFTau = taui;
  if(debugMode && mNPFTau>0){
    cout<<"PFTau0: Pt= "<<mPFTauPt[0]
	<<", Eta= "<<mPFTauEta[0]
	<<", Phi= "<<mPFTauPhi[0]
      //<<", ByDecayMode= "<<mPFTauDisDecayModeFinding[0]
      //<<", ByLooseCombinedIsolationDeltaBetaCorr= "<<mPFTauDisByLooseCombinedIsolationDeltaBetaCorr[0]
	<<endl;
  }
  
  
  ///-------------------------------------------------------------------------- 
  /// Photons
  ///--------------------------------------------------------------------------
  edm::Handle<edm::View<pat::Photon> > PhotHand;
  iEvent.getByLabel("slimmedPhotons",PhotHand);
  int photi=0;
  if(PhotHand.isValid()){
    for(unsigned int ind=0; ind<(*PhotHand).size() && ind<MAXPHOT; ind++){ 
      const pat::Photon& phot = (*PhotHand)[ind];
      
      mPhotPt[photi]  = phot.pt() ;
      mPhotPx[photi]  = phot.momentum().X() ;
      mPhotPy[photi]  = phot.momentum().Y() ;
      mPhotPz[photi]  = phot.momentum().Z() ;
      mPhotE[photi]   = phot.energy() ;
      mPhotEta[photi] = phot.eta() ;
      mPhotPhi[photi] = phot.phi() ;
      mPhotE1x5[eleci]= phot.e1x5(); 
      mPhotE2x5[photi]= phot.e2x5(); 
      mPhotE5x5[photi]= phot.e5x5(); 
      mPhotSigEta[photi] = phot.sigmaEtaEta();
      //mPhotSigPhi[photi] = phot.sigmaPhiPhi();
      mPhotEcalIso04[photi] = phot.ecalIso(); 
      mPhotHcalIso04[photi] = phot.hcalIso();
      mPhotTrackIso04[photi] = phot.trackIso();
      
      mPhotHasPixSeed[photi] = phot.hasPixelSeed();
      mPhotIsPhot[photi]     = phot.isPhoton();   
      
      photi++;
    }
  }
  mNPhot = photi;
  
  
  ///-------------------------------------------------------------------------- 
  /// MC Products
  ///--------------------------------------------------------------------------
  mNGenPar = 0;
 
  if(isMCTag) {
		/// Gen Particles
		Handle<reco::GenParticleCollection>  GenParHand;
		iEvent.getByLabel("prunedGenParticles", GenParHand);

		int igcount=0;
		
		for( size_t i = 0; i < GenParHand->size(); ++ i ) 
		{
			const reco::Candidate& p = (*GenParHand)[ i ];
			//int id = p.pdgId();

                        // cout<<"  i "<<i<<"  id "<<id<<"   status "<<p.status()<<endl;

                       
                        //int iwrite=0;
                        //if(i<15) iwrite=1; 
			//if( p.status()==1 && 
                        //(abs(id) == 11 || abs(id) == 12 || abs(id)==13 || abs(id) == 14 || abs(id) == 15 || abs(id)==16)) iwrite=1;  

			//if(iwrite==1) {
                         	mGenParId[igcount]     = p.pdgId() ;
				mGenParStatus[igcount] =p.status() ;
				mGenParCharge[igcount] = p.charge() ;
				mGenParE[igcount]      = p.energy();
				mGenParPt[igcount]     = p.pt();
				mGenParPx[igcount]     = p.px();
				mGenParPy[igcount]     = p.py();
				mGenParPz[igcount]     = p.pz();
				mGenParEta[igcount]    = p.eta();
				mGenParPhi[igcount]    = p.phi();
				mGenParMass[igcount]   =p.mass() ;
				
				//  get pid of parent particle...
				mGenParDoughterOf[igcount] = 0;
				int n = p.numberOfMothers();
				if(n>0) 
				{
					const reco::Candidate *parent = p.mother();
					int parentID = parent->pdgId();
					mGenParDoughterOf[igcount]=parentID;
				} 
				igcount++;
			}
	//	}
	
		mNGenPar=igcount;
  }

  ///-------------------------------------------------------------------------- 
  /// PDF
  ///--------------------------------------------------------------------------
  if(isMCTag && isSignalTag==1.){
    
    ///----------PDF Weight from file---------------------------------------
    edm::InputTag pdfWeightTag("pdfWeights1:cteq66"); // or any other PDF set
    edm::Handle<std::vector<double> > weightHandle;
    iEvent.getByLabel(pdfWeightTag, weightHandle);
    
    std::vector<double> weights = (*weightHandle);
    //std::cout << "Event weight for central PDF:" << weights[0] << std::endl;
    unsigned int nmembers = weights.size();
    
    for (unsigned int j=0; j<nmembers; j++){
      mPdfWeights1[j] = weights[j];
    }
    mNPdfWeights1 = nmembers;  
  }
  
  if(isMCTag && isSignalTag==2.){
  
    ///----------PDF Weight from file---------------------------------------
    edm::InputTag pdfWeightTag("pdfWeights2:MSTW2008nlo68cl"); // or any other PDF set
    edm::Handle<std::vector<double> > weightHandle;
    iEvent.getByLabel(pdfWeightTag, weightHandle);
    
    std::vector<double> weights = (*weightHandle);
    //std::cout << "Event weight for central PDF:" << weights[0] << std::endl;
    unsigned int nmembers = weights.size();
    
    for (unsigned int j=0; j<nmembers; j++){
      mPdfWeights[j] = weights[j];
    }
    mNPdfWeights = nmembers;  
  }
  
  if(isMCTag && isSignalTag==3.){
    ///----------PDF Weight from file---------------------------------------
    edm::InputTag pdfWeightTag("pdfWeights3:NNPDF20"); // or any other PDF set
    edm::Handle<std::vector<double> > weightHandle;
    iEvent.getByLabel(pdfWeightTag, weightHandle);
    
    std::vector<double> weights = (*weightHandle);
    //std::cout << "Event weight for central PDF:" << weights[0] << std::endl;
    unsigned int nmembers = weights.size();
    
    for (unsigned int j=0; j<nmembers; j++){
      mPdfWeights[j] = weights[j];
    }	
    mNPdfWeights = nmembers;  
  }

  
  ///-------------------------------------------------------------------------- 
  /// L1
  ///--------------------------------------------------------------------------
  /*
  if(!isMCTag){
    
    ///-------------------L1 Trigger----------------------------------------------
    Handle <L1GlobalTriggerReadoutRecord> gtRecord_h;
    iEvent.getByType (gtRecord_h); // assume only one L1 trigger record here
    const L1GlobalTriggerReadoutRecord* gtRecord = gtRecord_h.failedToGet () ? 0 : &*gtRecord_h;
    
    DecisionWord gtDecisionWord = gtRecord->decisionWord();
    
    for(int i=0; i<128; i++){
      L1Array[i] =  gtDecisionWord[i];
    }
    nL1 = 128;
    
    
    ///-------------------- L1 Technical Trigger----------------------------------
    Handle<L1GlobalTriggerReadoutRecord> gtRecord2;
    iEvent.getByLabel("gtDigis",gtRecord2);
    const TechnicalTriggerWord tWord = gtRecord2->technicalTriggerWord();
    
    for(int i=0; i<64; i++){
      L1TArray[i] = tWord.at(i);
    }
    nL1T = 64; 
  }
  */
  
  ///-------------------------------------------------------------------------- 
  /// Tracking Failure
  ///--------------------------------------------------------------------------
  /*
  edm::Handle<std::vector<reco::Track> > tracks2;
  iEvent.getByLabel(Tracks_, tracks2);
  
  edm::Handle<std::vector<reco::Vertex> > vtxs;
  iEvent.getByLabel("offlinePrimaryVertices", vtxs);
  
  // ht coming from pat jets see above
  double sumpt = 0;
  if (vtxs->size() > 0){
    const reco::Vertex * vtx = &((*vtxs)[0]);
    for (std::vector<reco::Track>::const_iterator tr = tracks2->begin(); tr != tracks2->end(); ++tr) {
      if (fabs(tr->dz(vtx->position())) > 1.0) continue;
      if (fabs(tr->dxy(vtx->position())) > 0.2) continue;
      sumpt += tr->pt();
    }
  }
  mSumTrackPt = sumpt;
  */
  
  ///-------------------------------------------------------------------------- 
  /// Beam Halo Flag
  ///--------------------------------------------------------------------------
  /*
  Handle<reco::BeamHaloSummary> beamHaloSummary;
  iEvent.getByLabel("BeamHaloSummary" , beamHaloSummary);
  if(beamHaloSummary.isValid()){
    mbeamHaloTight = beamHaloSummary->CSCTightHaloId();
    mbeamHaloLoose = beamHaloSummary->CSCLooseHaloId();
  }
  else{
    mbeamHaloTight = 0;
    mbeamHaloLoose = 0;
  } 
  */
  
  ///-------------------------------------------------------------------------- 
  /// Fill tree
  ///--------------------------------------------------------------------------
  mtree->Fill();
}


// ------------ method called once each job just before starting event loop  ------------
void NtupleAnalyzer::beginJob()
{
  ///-------------------------------------------------------------------------- 
  /// General event information
  ///--------------------------------------------------------------------------
  mtree->Branch("run"                                                ,&mRun                                               ,"run/I");
  mtree->Branch("event"                                              ,&mEvent                                             ,"event/L");
  mtree->Branch("lumi"                                               ,&mLumi                                              ,"lumi/I");	
  mtree->Branch("bx"                                                 ,&mBX                                                ,"bx/I");
  //
  mtree->Branch("npvm1"                                              ,&mnpvm1                                             ,"npvm1/I");
  mtree->Branch("npv0"                                               ,&mnpv0                                              ,"npv0/I");
  mtree->Branch("npvp1"                                              ,&mnpvp1                                             ,"npvp1/I");
  mtree->Branch("ptHat"                                              ,&WeightTag                                          ,"ptHat/D");	
  //
  mtree->Branch("CaloTowerdEx"                                       ,&mCaloTowerdEx                                      ,"CaloTowerdEx/D");	
  mtree->Branch("CaloTowerdEy"                                       ,&mCaloTowerdEy                                      ,"CaloTowerdEy/D");	
  //
  mtree->Branch("fastJetRho"                                         ,&mfastJetRho                                        ,"fastJetRho/D");
  
    
  ///-------------------------------------------------------------------------- 
  /// PF AK4 Jets
  ///-------------------------------------------------------------------------- 
  mtree->Branch("NPFAK4Jets"                                         ,&mNPFAK4Jets                                        ,"NPFAK4Jets/I");
  mtree->Branch("PFAK4JetE"                                          ,mPFAK4JetE                                          ,"PFAK4JetE[NPFAK4Jets]/D");
  mtree->Branch("PFAK4JetPt"                                         ,mPFAK4JetPt                                         ,"PFAK4JetPt[NPFAK4Jets]/D");
  mtree->Branch("PFAK4JetPx"                                         ,mPFAK4JetPx                                         ,"PFAK4JetPx[NPFAK4Jets]/D");
  mtree->Branch("PFAK4JetPy"                                         ,mPFAK4JetPy                                         ,"PFAK4JetPy[NPFAK4Jets]/D");
  mtree->Branch("PFAK4JetPz"                                         ,mPFAK4JetPz                                         ,"PFAK4JetPz[NPFAK4Jets]/D");
  mtree->Branch("PFAK4JetEta"                                        ,mPFAK4JetEta                                        ,"PFAK4JetEta[NPFAK4Jets]/D");
  mtree->Branch("PFAK4JetPhi"                                        ,mPFAK4JetPhi                                        ,"PFAK4JetPhi[NPFAK4Jets]/D");
  //
  mtree->Branch("PFAK4JetfHPD"                                       ,mPFAK4JetfHPD                                       ,"PFAK4JetfHPD[NPFAK4Jets]/D"); 
  mtree->Branch("PFAK4JetfRBX"                                       ,mPFAK4JetfRBX                                       ,"PFAK4JetfRBX[NPFAK4Jets]/D"); 
  mtree->Branch("PFAK4JetN90Hits"                                    ,mPFAK4JetN90Hits                                    ,"PFAK4JetN90Hits[NPFAK4Jets]/D"); 
  mtree->Branch("PFAK4JetN90"                                        ,mPFAK4JetN90                                        ,"PFAK4JetN90[NPFAK4Jets]/I"); 
  mtree->Branch("PFAK4JetSigEta"                                     ,mPFAK4JetSigEta                                     ,"PFAK4JetSigEta[NPFAK4Jets]/D"); 
  mtree->Branch("PFAK4JetSigPhi"                                     ,mPFAK4JetSigPhi                                     ,"PFAK4JetSigPhi[NPFAK4Jets]/D"); 
  mtree->Branch("PFAK4JetIDEmf"                                      ,mPFAK4JetIDEmf                                      ,"PFAK4JetIDEmf[NPFAK4Jets]/D"); 
  //
  mtree->Branch("PFAK4JetECor"                                       ,mPFAK4JetECor                                       ,"PFAK4JetECor[NPFAK4Jets]/D");
  mtree->Branch("PFAK4JetPtCor"                                      ,mPFAK4JetPtCor                                      ,"PFAK4JetPtCor[NPFAK4Jets]/D");
  mtree->Branch("PFAK4JetPxCor"                                      ,mPFAK4JetPxCor                                      ,"PFAK4JetPxCor[NPFAK4Jets]/D");
  mtree->Branch("PFAK4JetPyCor"                                      ,mPFAK4JetPyCor                                      ,"PFAK4JetPyCor[NPFAK4Jets]/D");
  mtree->Branch("PFAK4JetPzCor"                                      ,mPFAK4JetPzCor                                      ,"PFAK4JetPzCor[NPFAK4Jets]/D");
  //
  mtree->Branch("PFAK4JetBtagTkCountHighEff"                         ,mPFAK4JetBtagTkCountHighEff                         ,"PFAK4JetBtagTkCountHighEff[NPFAK4Jets]/D");
  mtree->Branch("PFAK4JetBtagTkCountHighPur"                         ,mPFAK4JetBtagTkCountHighPur                         ,"PFAK4JetBtagTkCountHighPur[NPFAK4Jets]/D");
  mtree->Branch("PFAK4JetBtagJetProbability"                         ,mPFAK4JetBtagJetProbability                         ,"PFAK4JetBtagJetProbability[NPFAK4Jets]/D");
  mtree->Branch("PFAK4JetBtagJetBProbability"                        ,mPFAK4JetBtagJetBProbability                        ,"PFAK4JetBtagJetBProbability[NPFAK4Jets]/D");
  mtree->Branch("PFAK4JetBTagCSVv2"                           ,mPFAK4JetBTagCSVv2                           ,"PFAK4JetBTagCSVv2[NPFAK4Jets]/D");
  mtree->Branch("PFAK4JetBTagCombSecVtx"                             ,mPFAK4JetBTagCombSecVtx                             ,"PFAK4JetBTagCombSecVtx[NPFAK4Jets]/D");
  mtree->Branch("PFAK4JetBTagCombSecVtxMVA"                          ,mPFAK4JetBTagCombSecVtxMVA                          ,"PFAK4JetBTagCombSecVtxMVA[NPFAK4Jets]/D");
  //
  mtree->Branch("PFAK4JetFlavour"                                    ,mPFAK4JetFlavour                                    ,"PFAK4JetFlavour[NPFAK4Jets]/I");
  mtree->Branch("PFAK4JetGenPartonFlavour"                           ,mPFAK4JetGenPartonFlavour                           ,"PFAK4JetGenPartonFlavour[NPFAK4Jets]/I");
  //
  mtree->Branch("PFAK4JetNeuEmEngFrac"                               ,mPFAK4JetNeuEmEngFrac                               ,"PFAK4JetNeuEmEngFrac[NPFAK4Jets]/D"); 
  mtree->Branch("PFAK4JetChaEmEngFrac"                               ,mPFAK4JetChaEmEngFrac                               ,"PFAK4JetChaEmEngFrac[NPFAK4Jets]/D"); 
  mtree->Branch("PFAK4JetChaHadEngFrac"                              ,mPFAK4JetChaHadEngFrac                              ,"PFAK4JetChaHadEngFrac[NPFAK4Jets]/D"); 
  mtree->Branch("PFAK4JetNeuHadEngFrac"                              ,mPFAK4JetNeuHadEngFrac                              ,"PFAK4JetNeuHadEngFrac[NPFAK4Jets]/D");
  // 
  mtree->Branch("PFAK4JetChaEmEng"                                   ,mPFAK4JetChaEmEng                                   ,"PFAK4JetChaEmEng[NPFAK4Jets]/D"); 
  mtree->Branch("PFAK4JetNeuEmEng"                                   ,mPFAK4JetNeuEmEng                                   ,"PFAK4JetNeuEmEng[NPFAK4Jets]/D"); 
  mtree->Branch("PFAK4JetChaHadEng"                                  ,mPFAK4JetChaHadEng                                  ,"PFAK4JetChaHadEng[NPFAK4Jets]/D"); 
  mtree->Branch("PFAK4JetNeuHadEng"                                  ,mPFAK4JetNeuHadEng                                  ,"PFAK4JetNeuHadEng[NPFAK4Jets]/D");
  mtree->Branch("PFAK4JetPhotEng"                                    ,mPFAK4JetPhotEng                                    ,"PFAK4JetPhotEng[NPFAK4Jets]/D"); 
  mtree->Branch("PFAK4JetElecEng"                                    ,mPFAK4JetElecEng                                    ,"PFAK4JetElecEng[NPFAK4Jets]/D");
  mtree->Branch("PFAK4JetMuonEng"                                    ,mPFAK4JetMuonEng                                    ,"PFAK4JetMuonEng[NPFAK4Jets]/D"); 
  mtree->Branch("PFAK4JetHFHadEng"                                   ,mPFAK4JetHFHadEng                                   ,"PFAK4JetHFHadEng[NPFAK4Jets]/D"); 
  mtree->Branch("PFAK4JetHFEmEng"                                    ,mPFAK4JetHFEmEng                                    ,"PFAK4JetHFEmEng[NPFAK4Jets]/D"); 
  mtree->Branch("PFAK4JetChaMuEng"                                   ,mPFAK4JetChaMuEng                                   ,"PFAK4JetChaMuEng[NPFAK4Jets]/D"); 
  //
  mtree->Branch("PFAK4JetNumOfChaMu"                                 ,mPFAK4JetNumOfChaMu                                 ,"PFAK4JetNumOfChaMu[NPFAK4Jets]/I"); 
  mtree->Branch("PFAK4JetNumOfMuon"                                  ,mPFAK4JetNumOfMuon                                  ,"PFAK4JetNumOfMuon[NPFAK4Jets]/I"); 
  mtree->Branch("PFAK4JetNumOfPhot"                                  ,mPFAK4JetNumOfPhot                                  ,"PFAK4JetNumOfPhot[NPFAK4Jets]/I"); 
  mtree->Branch("PFAK4JetNumOfElec"                                  ,mPFAK4JetNumOfElec                                  ,"PFAK4JetNumOfElec[NPFAK4Jets]/I"); 
  mtree->Branch("PFAK4JetNumOfNeu"                                   ,mPFAK4JetNumOfNeu                                   ,"PFAK4JetNumOfNeu[NPFAK4Jets]/I"); 
  mtree->Branch("PFAK4JetNumOfCha"                                   ,mPFAK4JetNumOfCha                                   ,"PFAK4JetNumOfCha[NPFAK4Jets]/I"); 
  mtree->Branch("PFAK4JetNumOfNeuHad"                                ,mPFAK4JetNumOfNeuHad                                ,"PFAK4JetNumOfNeuHad[NPFAK4Jets]/I"); 
  mtree->Branch("PFAK4JetNumOfChaHad"                                ,mPFAK4JetNumOfChaHad                                ,"PFAK4JetNumOfChaHad[NPFAK4Jets]/I"); 
  mtree->Branch("PFAK4JetNumOfHFHad"                                 ,mPFAK4JetNumOfHFHad                                 ,"PFAK4JetNumOfHFHad[NPFAK4Jets]/I"); 
  mtree->Branch("PFAK4JetNumOfHFEM"                                  ,mPFAK4JetNumOfHFEM                                  ,"PFAK4JetNumOfHFEM[NPFAK4Jets]/I"); 
  //
  mtree->Branch("PFAK4JetNumOfDaughters"                             ,mPFAK4JetNumOfDaughters                             ,"PFAK4JetNumOfDaughters[NPFAK4Jets]/I");
  mtree->Branch("PFAK4JetIDLOOSE"                                    ,mPFAK4JetIDLOOSE                                    ,"PFAK4JetIDLOOSE[NPFAK4Jets]/I");
  mtree->Branch("PFAK4JetIDTIGHT"                                    ,mPFAK4JetIDTIGHT                                    ,"PFAK4JetIDTIGHT[NPFAK4Jets]/I");
  mtree->Branch("PFAK4JetPUFullJetId"                                ,mPFAK4JetPUFullJetId                                ,"PFAK4JetPUFullJetId[NPFAK4Jets]/D");
  mtree->Branch("PFAK4uncer"                                         ,mPFAK4uncer                                         ,"PFAK4uncer[NPFAK4Jets]/D");
  /// Gen PFAK4 Jets
  mtree->Branch("GenPFAK4JetE"                                       ,mGenPFAK4JetE                                       ,"GenPFAK4JetE[NPFAK4Jets]/D");
  mtree->Branch("GenPFAK4JetPt"                                      ,mGenPFAK4JetPt                                      ,"GenPFAK4JetPt[NPFAK4Jets]/D");
  mtree->Branch("GenPFAK4JetPx"                                      ,mGenPFAK4JetPx                                      ,"GenPFAK4JetPx[NPFAK4Jets]/D");
  mtree->Branch("GenPFAK4JetPy"                                      ,mGenPFAK4JetPy                                      ,"GenPFAK4JetPy[NPFAK4Jets]/D");
  mtree->Branch("GenPFAK4JetPz"                                      ,mGenPFAK4JetPz                                      ,"GenPFAK4JetPz[NPFAK4Jets]/D");
  mtree->Branch("GenPFAK4JetEta"                                     ,mGenPFAK4JetEta                                     ,"GenPFAK4JetEta[NPFAK4Jets]/D");
  mtree->Branch("GenPFAK4JetPhi"                                     ,mGenPFAK4JetPhi                                     ,"GenPFAK4JetPhi[NPFAK4Jets]/D");
  mtree->Branch("GenPFAK4JetEmEnergy"                                ,mGenPFAK4JetEmEnergy                                ,"GenPFAK4JetEmEnergy[NPFAK4Jets]/D");
  mtree->Branch("GenPFAK4JetHadEnergy"                               ,mGenPFAK4JetHadEnergy                               ,"GenPFAK4JetHadEnergy[NPFAK4Jets]/D");
  //
  mtree->Branch("PFAK4JERCentral"                                    ,mPFAK4JERCentral                                    ,"PFAK4JERCentral[NPFAK4Jets]/D");
  mtree->Branch("PFAK4JERUp"                                         ,mPFAK4JERUp                                         ,"PFAK4JERUp[NPFAK4Jets]/D");
  mtree->Branch("PFAK4JERDown"                                       ,mPFAK4JERDown                                       ,"PFAK4JERDown[NPFAK4Jets]/D");


  ///-------------------------------------------------------------------------- 
  /// PF AK8 Jets
  ///-------------------------------------------------------------------------- 
  mtree->Branch("NPFAK8Jets"                                         ,&mNPFAK8Jets                                        ,"NPFAK8Jets/I");
  mtree->Branch("PFAK8JetE"                                          ,mPFAK8JetE                                          ,"PFAK8JetE[NPFAK8Jets]/D");
  mtree->Branch("PFAK8JetPt"                                         ,mPFAK8JetPt                                         ,"PFAK8JetPt[NPFAK8Jets]/D");
  mtree->Branch("PFAK8JetPx"                                         ,mPFAK8JetPx                                         ,"PFAK8JetPx[NPFAK8Jets]/D");
  mtree->Branch("PFAK8JetPy"                                         ,mPFAK8JetPy                                         ,"PFAK8JetPy[NPFAK8Jets]/D");
  mtree->Branch("PFAK8JetPz"                                         ,mPFAK8JetPz                                         ,"PFAK8JetPz[NPFAK8Jets]/D");
  mtree->Branch("PFAK8JetEta"                                        ,mPFAK8JetEta                                        ,"PFAK8JetEta[NPFAK8Jets]/D");
  mtree->Branch("PFAK8JetPhi"                                        ,mPFAK8JetPhi                                        ,"PFAK8JetPhi[NPFAK8Jets]/D");
  //
  mtree->Branch("PFAK8JetfHPD"                                       ,mPFAK8JetfHPD                                       ,"PFAK8JetfHPD[NPFAK8Jets]/D"); 
  mtree->Branch("PFAK8JetfRBX"                                       ,mPFAK8JetfRBX                                       ,"PFAK8JetfRBX[NPFAK8Jets]/D"); 
  mtree->Branch("PFAK8JetN90Hits"                                    ,mPFAK8JetN90Hits                                    ,"PFAK8JetN90Hits[NPFAK8Jets]/D"); 
  mtree->Branch("PFAK8JetN90"                                        ,mPFAK8JetN90                                        ,"PFAK8JetN90[NPFAK8Jets]/I"); 
  mtree->Branch("PFAK8JetSigEta"                                     ,mPFAK8JetSigEta                                     ,"PFAK8JetSigEta[NPFAK8Jets]/D"); 
  mtree->Branch("PFAK8JetSigPhi"                                     ,mPFAK8JetSigPhi                                     ,"PFAK8JetSigPhi[NPFAK8Jets]/D"); 
  mtree->Branch("PFAK8JetIDEmf"                                      ,mPFAK8JetIDEmf                                      ,"PFAK8JetIDEmf[NPFAK8Jets]/D"); 
  //
  mtree->Branch("PFAK8JetECor"                                       ,mPFAK8JetECor                                       ,"PFAK8JetECor[NPFAK8Jets]/D");
  mtree->Branch("PFAK8JetPtCor"                                      ,mPFAK8JetPtCor                                      ,"PFAK8JetPtCor[NPFAK8Jets]/D");
  mtree->Branch("PFAK8JetPxCor"                                      ,mPFAK8JetPxCor                                      ,"PFAK8JetPxCor[NPFAK8Jets]/D");
  mtree->Branch("PFAK8JetPyCor"                                      ,mPFAK8JetPyCor                                      ,"PFAK8JetPyCor[NPFAK8Jets]/D");
  mtree->Branch("PFAK8JetPzCor"                                      ,mPFAK8JetPzCor                                      ,"PFAK8JetPzCor[NPFAK8Jets]/D");
  //
  mtree->Branch("PFAK8JetBtagTkCountHighEff"                         ,mPFAK8JetBtagTkCountHighEff                         ,"PFAK8JetBtagTkCountHighEff[NPFAK8Jets]/D");
  mtree->Branch("PFAK8JetBtagTkCountHighPur"                         ,mPFAK8JetBtagTkCountHighPur                         ,"PFAK8JetBtagTkCountHighPur[NPFAK8Jets]/D");
  mtree->Branch("PFAK8JetBtagJetProbability"                         ,mPFAK8JetBtagJetProbability                         ,"PFAK8JetBtagJetProbability[NPFAK8Jets]/D");
  mtree->Branch("PFAK8JetBtagJetBProbability"                        ,mPFAK8JetBtagJetBProbability                        ,"PFAK8JetBtagJetBProbability[NPFAK8Jets]/D");
  mtree->Branch("PFAK8JetBTagCSVv2"                           ,mPFAK8JetBTagCSVv2                           ,"PFAK8JetBTagCSVv2[NPFAK8Jets]/D");
  mtree->Branch("PFAK8JetBTagCombSecVtx"                             ,mPFAK8JetBTagCombSecVtx                             ,"PFAK8JetBTagCombSecVtx[NPFAK8Jets]/D");
  mtree->Branch("PFAK8JetBTagCombSecVtxMVA"                          ,mPFAK8JetBTagCombSecVtxMVA                          ,"PFAK8JetBTagCombSecVtxMVA[NPFAK8Jets]/D");
  //
  mtree->Branch("PFAK8JetFlavour"                                    ,mPFAK8JetFlavour                                    ,"PFAK8JetFlavour[NPFAK8Jets]/I");
  mtree->Branch("PFAK8JetGenPartonFlavour"                           ,mPFAK8JetGenPartonFlavour                           ,"PFAK8JetGenPartonFlavour[NPFAK8Jets]/I");
  //
  mtree->Branch("PFAK8JetNeuEmEngFrac"                               ,mPFAK8JetNeuEmEngFrac                               ,"PFAK8JetNeuEmEngFrac[NPFAK8Jets]/D"); 
  mtree->Branch("PFAK8JetChaEmEngFrac"                               ,mPFAK8JetChaEmEngFrac                               ,"PFAK8JetChaEmEngFrac[NPFAK8Jets]/D"); 
  mtree->Branch("PFAK8JetChaHadEngFrac"                              ,mPFAK8JetChaHadEngFrac                              ,"PFAK8JetChaHadEngFrac[NPFAK8Jets]/D"); 
  mtree->Branch("PFAK8JetNeuHadEngFrac"                              ,mPFAK8JetNeuHadEngFrac                              ,"PFAK8JetNeuHadEngFrac[NPFAK8Jets]/D");
  // 
  mtree->Branch("PFAK8JetChaEmEng"                                   ,mPFAK8JetChaEmEng                                   ,"PFAK8JetChaEmEng[NPFAK8Jets]/D"); 
  mtree->Branch("PFAK8JetNeuEmEng"                                   ,mPFAK8JetNeuEmEng                                   ,"PFAK8JetNeuEmEng[NPFAK8Jets]/D"); 
  mtree->Branch("PFAK8JetChaHadEng"                                  ,mPFAK8JetChaHadEng                                  ,"PFAK8JetChaHadEng[NPFAK8Jets]/D"); 
  mtree->Branch("PFAK8JetNeuHadEng"                                  ,mPFAK8JetNeuHadEng                                  ,"PFAK8JetNeuHadEng[NPFAK8Jets]/D");
  mtree->Branch("PFAK8JetPhotEng"                                    ,mPFAK8JetPhotEng                                    ,"PFAK8JetPhotEng[NPFAK8Jets]/D"); 
  mtree->Branch("PFAK8JetElecEng"                                    ,mPFAK8JetElecEng                                    ,"PFAK8JetElecEng[NPFAK8Jets]/D");
  mtree->Branch("PFAK8JetMuonEng"                                    ,mPFAK8JetMuonEng                                    ,"PFAK8JetMuonEng[NPFAK8Jets]/D"); 
  mtree->Branch("PFAK8JetHFHadEng"                                   ,mPFAK8JetHFHadEng                                   ,"PFAK8JetHFHadEng[NPFAK8Jets]/D"); 
  mtree->Branch("PFAK8JetHFEmEng"                                    ,mPFAK8JetHFEmEng                                    ,"PFAK8JetHFEmEng[NPFAK8Jets]/D"); 
  mtree->Branch("PFAK8JetChaMuEng"                                   ,mPFAK8JetChaMuEng                                   ,"PFAK8JetChaMuEng[NPFAK8Jets]/D"); 
  //
  mtree->Branch("PFAK8JetNumOfChaMu"                                 ,mPFAK8JetNumOfChaMu                                 ,"PFAK8JetNumOfChaMu[NPFAK8Jets]/I"); 
  mtree->Branch("PFAK8JetNumOfMuon"                                  ,mPFAK8JetNumOfMuon                                  ,"PFAK8JetNumOfMuon[NPFAK8Jets]/I"); 
  mtree->Branch("PFAK8JetNumOfPhot"                                  ,mPFAK8JetNumOfPhot                                  ,"PFAK8JetNumOfPhot[NPFAK8Jets]/I"); 
  mtree->Branch("PFAK8JetNumOfElec"                                  ,mPFAK8JetNumOfElec                                  ,"PFAK8JetNumOfElec[NPFAK8Jets]/I"); 
  mtree->Branch("PFAK8JetNumOfNeu"                                   ,mPFAK8JetNumOfNeu                                   ,"PFAK8JetNumOfNeu[NPFAK8Jets]/I"); 
  mtree->Branch("PFAK8JetNumOfCha"                                   ,mPFAK8JetNumOfCha                                   ,"PFAK8JetNumOfCha[NPFAK8Jets]/I"); 
  mtree->Branch("PFAK8JetNumOfNeuHad"                                ,mPFAK8JetNumOfNeuHad                                ,"PFAK8JetNumOfNeuHad[NPFAK8Jets]/I"); 
  mtree->Branch("PFAK8JetNumOfChaHad"                                ,mPFAK8JetNumOfChaHad                                ,"PFAK8JetNumOfChaHad[NPFAK8Jets]/I"); 
  mtree->Branch("PFAK8JetNumOfHFHad"                                 ,mPFAK8JetNumOfHFHad                                 ,"PFAK8JetNumOfHFHad[NPFAK8Jets]/I"); 
  mtree->Branch("PFAK8JetNumOfHFEM"                                  ,mPFAK8JetNumOfHFEM                                  ,"PFAK8JetNumOfHFEM[NPFAK8Jets]/I"); 
  //
  mtree->Branch("PFAK8JetNumOfDaughters"                             ,mPFAK8JetNumOfDaughters                             ,"PFAK8JetNumOfDaughters[NPFAK8Jets]/I");
  mtree->Branch("PFAK8JetIDLOOSE"                                    ,mPFAK8JetIDLOOSE                                    ,"PFAK8JetIDLOOSE[NPFAK8Jets]/I");
  mtree->Branch("PFAK8JetIDTIGHT"                                    ,mPFAK8JetIDTIGHT                                    ,"PFAK8JetIDTIGHT[NPFAK8Jets]/I");
  mtree->Branch("PFAK8JetPUFullJetId"                                ,mPFAK8JetPUFullJetId                                ,"PFAK8JetPUFullJetId[NPFAK8Jets]/D");
  mtree->Branch("PFAK8uncer"                                         ,mPFAK8uncer                                         ,"PFAK8uncer[NPFAK8Jets]/D");
  /// Gen PFAK8 Jets
  mtree->Branch("GenPFAK8JetE"                                       ,mGenPFAK8JetE                                       ,"GenPFAK8JetE[NPFAK8Jets]/D");
  mtree->Branch("GenPFAK8JetPt"                                      ,mGenPFAK8JetPt                                      ,"GenPFAK8JetPt[NPFAK8Jets]/D");
  mtree->Branch("GenPFAK8JetPx"                                      ,mGenPFAK8JetPx                                      ,"GenPFAK8JetPx[NPFAK8Jets]/D");
  mtree->Branch("GenPFAK8JetPy"                                      ,mGenPFAK8JetPy                                      ,"GenPFAK8JetPy[NPFAK8Jets]/D");
  mtree->Branch("GenPFAK8JetPz"                                      ,mGenPFAK8JetPz                                      ,"GenPFAK8JetPz[NPFAK8Jets]/D");
  mtree->Branch("GenPFAK8JetEta"                                     ,mGenPFAK8JetEta                                     ,"GenPFAK8JetEta[NPFAK8Jets]/D");
  mtree->Branch("GenPFAK8JetPhi"                                     ,mGenPFAK8JetPhi                                     ,"GenPFAK8JetPhi[NPFAK8Jets]/D");
  mtree->Branch("GenPFAK8JetEmEnergy"                                ,mGenPFAK8JetEmEnergy                                ,"GenPFAK8JetEmEnergy[NPFAK8Jets]/D");
  mtree->Branch("GenPFAK8JetHadEnergy"                               ,mGenPFAK8JetHadEnergy                               ,"GenPFAK8JetHadEnergy[NPFAK8Jets]/D");
  //
  mtree->Branch("PFAK8JERCentral"                                    ,mPFAK8JERCentral                                    ,"PFAK8JERCentral[NPFAK8Jets]/D");
  mtree->Branch("PFAK8JERUp"                                         ,mPFAK8JERUp                                         ,"PFAK8JERUp[NPFAK8Jets]/D");
  mtree->Branch("PFAK8JERDown"                                       ,mPFAK8JERDown                                       ,"PFAK8JERDown[NPFAK8Jets]/D");

  ///-------------------------------------------------------------------------- 
  /// MET
  ///-------------------------------------------------------------------------- 
  mtree->Branch("NMet"                                               ,&mNMet                                              ,"NMet/I");
  mtree->Branch("MetPt"                                              ,mMetPt                                              ,"MetPt[NMet]/D");
  mtree->Branch("MetPx"                                              ,mMetPx                                              ,"MetPx[NMet]/D");
  mtree->Branch("MetPy"                                              ,mMetPy                                              ,"MetPy[NMet]/D");
  mtree->Branch("MetPhi"                                             ,mMetPhi                                             ,"MetPhi[NMet]/D");
  mtree->Branch("MetSumEt"                                           ,mMetSumEt                                           ,"MetSumEt[NMet]/D");
  mtree->Branch("MetSign"                                            ,mMetSign                                            ,"MetSign[NMet]/D");
  
  ///-------------------------------------------------------------------------- 
  /// MHT
  ///--------------------------------------------------------------------------  
  mtree->Branch("MHTPt"                                              ,&mMHTPt                                             ,"MHTPt/D");
  mtree->Branch("MHTPhi"                                             ,&mMHTPhi                                            ,"MHTPhi/D");
  mtree->Branch("MHTSign"                                            ,&mMHTSign                                           ,"MHTSign/D");
  
  ///-------------------------------------------------------------------------- 
  /// PF Muons
  ///-------------------------------------------------------------------------- 
  mtree->Branch("NPFMuon"                                            ,&mNPFMuon                                           ,"NPFMuon/I");
  mtree->Branch("PFMuonPt"                                           ,mPFMuonPt                                           ,"PFMuonPt[NPFMuon]/D");               
  mtree->Branch("PFMuonEt"                                           ,mPFMuonEt                                           ,"PFMuonEt[NPFMuon]/D");             
  mtree->Branch("PFMuonE"                                            ,mPFMuonE                                            ,"PFMuonE[NPFMuon]/D");                
  mtree->Branch("PFMuonPx"                                           ,mPFMuonPx                                           ,"PFMuonPx[NPFMuon]/D");               
  mtree->Branch("PFMuonPy"                                           ,mPFMuonPy                                           ,"PFMuonPy[NPFMuon]/D");               
  mtree->Branch("PFMuonPz"                                           ,mPFMuonPz                                           ,"PFMuonPz[NPFMuon]/D");               
  mtree->Branch("PFMuonEta"                                          ,mPFMuonEta                                          ,"PFMuonEta[NPFMuon]/D");              
  mtree->Branch("PFMuonPhi"                                          ,mPFMuonPhi                                          ,"PFMuonPhi[NPFMuon]/D");              
  mtree->Branch("PFMuonCharge"                                       ,mPFMuonCharge                                       ,"PFMuonCharge[NPFMuon]/I");           
  //
  mtree->Branch("PFMuonIsGlobal"                                     ,mPFMuonIsGlobal                                     ,"PFMuonIsGlobal[NPFMuon]/I");         
  mtree->Branch("PFMuonIsStandAlone"                                 ,mPFMuonIsStandAlone                                 ,"PFMuonIsStandAlone[NPFMuon]/I");     
  mtree->Branch("PFMuonIsTracker"                                    ,mPFMuonIsTracker                                    ,"PFMuonIsTracker[NPFMuon]/I");     
  mtree->Branch("PFMuonIsPF"                                         ,mPFMuonIsPF                                         ,"PFMuonIsPF[NPFMuon]/I");
  //
  mtree->Branch("PFMuonIsTight"                                      ,mPFMuonIsTight                                      ,"PFMuonIsTight[NPFMuon]/I");     
  mtree->Branch("PFMuonIsLoose"                                      ,mPFMuonIsLoose                                      ,"PFMuonIsLoose[NPFMuon]/I");
  //
  mtree->Branch("PFMuonChargedHadronIso"                             ,mPFMuonChargedHadronIso                             ,"PFMuonChargedHadronIso[NPFMuon]/D"); 
  mtree->Branch("PFMuonPhotonIso"                                    ,mPFMuonPhotonIso                                    ,"PFMuonPhotonIso[NPFMuon]/D");        
  mtree->Branch("PFMuonNeutralHadronIso"                             ,mPFMuonNeutralHadronIso                             ,"PFMuonNeutralHadronIso[NPFMuon]/D"); 
  mtree->Branch("PFMuonisGMPT"                                       ,mPFMuonisGMPT                                       ,"PFMuonisGMPT[NPFMuon]/I");
  mtree->Branch("PFMuonR04ChargedHadronPt"                           ,mPFMuonR04ChargedHadronPt                           ,"PFMuonR04ChargedHadronPt[NPFMuon]/D");
  mtree->Branch("PFMuonR04NeutralHadronEt"                           ,mPFMuonR04NeutralHadronEt                           ,"PFMuonR04NeutralHadronEt[NPFMuon]/D");
  mtree->Branch("PFMuonR04PhotonEt"                                  ,mPFMuonR04PhotonEt                                  ,"PFMuonR04PhotonEt[NPFMuon]/D");
  mtree->Branch("PFMuonR04PUPt"                                      ,mPFMuonR04PUPt                                      ,"PFMuonR04PUPt[NPFMuon]/D"); 
  // 
  mtree->Branch("PFMuonNumOfMatches"                                 ,mPFMuonNumOfMatches                                 ,"PFMuonNumOfMatches[NPFMuon]/I");  
  mtree->Branch("PFMuonNumOfMatchesStation"                          ,mPFMuonNumOfMatchesStation                          ,"PFMuonNumOfMatchesStation[NPFMuon]/I"); 
  //
  mtree->Branch("PFMuonGlobChi2Norm"                                 ,mPFMuonGlobChi2Norm                                 ,"PFMuonGlobChi2Norm[NPFMuon]/D");  
  mtree->Branch("PFMuonGlobNumOfValidMuonHits"                       ,mPFMuonGlobNumOfValidMuonHits                       ,"PFMuonGlobNumOfValidMuonHits[NPFMuon]/I"); 
  //
  mtree->Branch("PFMuonBestTrkdxy"                                   ,mPFMuonBestTrkdxy                                   ,"PFMuonBestTrkdxy[NPFMuon]/D");  
  mtree->Branch("PFMuonBestTrkdz"                                    ,mPFMuonBestTrkdz                                    ,"PFMuonBestTrkdz[NPFMuon]/D");
  //
  mtree->Branch("PFMuoninnertrackPt"                                 ,mPFMuoninnertrackPt                                 ,"PFMuoninnertrackPt[NPFMuon]/D");     
  mtree->Branch("PFMuonnValidHits"                                   ,mPFMuonnValidHits                                   ,"PFMuonnValidHits[NPFMuon]/I");       
  mtree->Branch("PFMuonnValidPixelHits"                              ,mPFMuonnValidPixelHits                              ,"PFMuonnValidPixelHits[NPFMuon]/I");  
  mtree->Branch("PFMuondxy"                                          ,mPFMuondxy                                          ,"PFMuondxy[NPFMuon]/D");              
  mtree->Branch("PFMuondz"                                           ,mPFMuondz                                           ,"PFMuondz[NPFMuon]/D");               
  //
  mtree->Branch("PFMuonCombChi2Norm"                                 ,mPFMuonCombChi2Norm                                 ,"PFMuonCombChi2Norm[NPFMuon]/D");    
  mtree->Branch("PFMuonCombValidHits"                                ,mPFMuonCombValidHits                                ,"PFMuonCombValidHits[NPFMuon]/I");   
  mtree->Branch("PFMuonCombLostHits"                                 ,mPFMuonCombLostHits                                 ,"PFMuonCombLostHits[NPFMuon]/I");    
  mtree->Branch("PFMuonCombPt"                                       ,mPFMuonCombPt                                       ,"PFMuonCombPt[NPFMuon]/D");          
  mtree->Branch("PFMuonCombPz"                                       ,mPFMuonCombPz                                       ,"PFMuonCombPz[NPFMuon]/D");          
  mtree->Branch("PFMuonCombP"                                        ,mPFMuonCombP                                        ,"PFMuonCombP[NPFMuon]/D");           
  mtree->Branch("PFMuonCombEta"                                      ,mPFMuonCombEta                                      ,"PFMuonCombEta[NPFMuon]/D");         
  mtree->Branch("PFMuonCombPhi"                                      ,mPFMuonCombPhi                                      ,"PFMuonCombPhi[NPFMuon]/D");         
  mtree->Branch("PFMuonCombChi2"                                     ,mPFMuonCombChi2                                     ,"PFMuonCombChi2[NPFMuon]/D");        
  mtree->Branch("PFMuonCombCharge"                                   ,mPFMuonCombCharge                                   ,"PFMuonCombCharge[NPFMuon]/D");      
  mtree->Branch("PFMuonCombQOverPError"                              ,mPFMuonCombQOverPError                              ,"PFMuonCombQOverPError[NPFMuon]/D"); 
  mtree->Branch("PFMuonCombNdof"                                     ,mPFMuonCombNdof                                     ,"PFMuonCombNdof[NPFMuon]/D");        
  mtree->Branch("PFMuonCombVx"                                       ,mPFMuonCombVx                                       ,"PFMuonCombVx[NPFMuon]/D");          
  mtree->Branch("PFMuonCombVy"                                       ,mPFMuonCombVy                                       ,"PFMuonCombVy[NPFMuon]/D");          
  mtree->Branch("PFMuonCombVz"                                       ,mPFMuonCombVz                                       ,"PFMuonCombVz[NPFMuon]/D");          
  mtree->Branch("PFMuonCombD0"                                       ,mPFMuonCombD0                                       ,"PFMuonCombD0[NPFMuon]/D");          
  mtree->Branch("PFMuonCombDz"                                       ,mPFMuonCombDz                                       ,"PFMuonCombDz[NPFMuon]/D");          
  //
  mtree->Branch("PFMuonStandChi2Norm"                                ,mPFMuonStandChi2Norm                                ,"PFMuonStandChi2Norm[NPFMuon]/D");   
  mtree->Branch("PFMuonStandValidHits"                               ,mPFMuonStandValidHits                               ,"PFMuonStandValidHits[NPFMuon]/I");  
  mtree->Branch("PFMuonStandLostHits"                                ,mPFMuonStandLostHits                                ,"PFMuonStandLostHits[NPFMuon]/I");   
  mtree->Branch("PFMuonStandPt"                                      ,mPFMuonStandPt                                      ,"PFMuonStandPt[NPFMuon]/D");         
  mtree->Branch("PFMuonStandPz"                                      ,mPFMuonStandPz                                      ,"PFMuonStandPz[NPFMuon]/D");         
  mtree->Branch("PFMuonStandP"                                       ,mPFMuonStandP                                       ,"PFMuonStandP[NPFMuon]/D");         
  mtree->Branch("PFMuonStandEta"                                     ,mPFMuonStandEta                                     ,"PFMuonStandEta[NPFMuon]/D");        
  mtree->Branch("PFMuonStandPhi"                                     ,mPFMuonStandPhi                                     ,"PFMuonStandPhi[NPFMuon]/D");        
  mtree->Branch("PFMuonStandChi2"                                    ,mPFMuonStandChi2                                    ,"PFMuonStandChi2[NPFMuon]/D");
  mtree->Branch("PFMuonStandCharge"                                  ,mPFMuonStandCharge                                  ,"PFMuonStandCharge[NPFMuon]/D");
  mtree->Branch("PFMuonStandQOverPError"                             ,mPFMuonStandQOverPError                             ,"PFMuonStandQOverPError[NPFMuon]/D" );
  //
  mtree->Branch("PFMuonTrkChi2Norm"                                  ,mPFMuonTrkChi2Norm                                  ,"PFMuonTrkChi2Norm[NPFMuon]/D"); 
  mtree->Branch("PFMuonTrkValidHits"                                 ,mPFMuonTrkValidHits                                 ,"PFMuonTrkValidHits[NPFMuon]/I");   
  mtree->Branch("PFMuonTrkLostHits"                                  ,mPFMuonTrkLostHits                                  ,"PFMuonTrkLostHits[NPFMuon]/I");
  mtree->Branch("PFMuonTrkPt"                                        ,mPFMuonTrkPt                                        ,"PFMuonTrkPt[NPFMuon]/D");     
  mtree->Branch("PFMuonTrkPz"                                        ,mPFMuonTrkPz                                        ,"PFMuonTrkPz[NPFMuon]/D");
  mtree->Branch("PFMuonTrkP"                                         ,mPFMuonTrkP                                         ,"PFMuonTrkP[NPFMuon]/D");           
  mtree->Branch("PFMuonTrkEta"                                       ,mPFMuonTrkEta                                       ,"PFMuonTrkEt[NPFMuon]/D");         
  mtree->Branch("PFMuonTrkPhi"                                       ,mPFMuonTrkPhi                                       ,"PFMuonTrkPhi[NPFMuon]/D");         
  mtree->Branch("PFMuonTrkChi2"                                      ,mPFMuonTrkChi2                                      ,"PFMuonTrkChi2[NPFMuon]/D");        
  mtree->Branch("PFMuonTrkCharge"                                    ,mPFMuonTrkCharge                                    ,"PFMuonTrkCharge[NPFMuon]/D");      
  mtree->Branch("PFMuonTrkQOverPError"                               ,mPFMuonTrkQOverPError                               ,"PFMuonTrkQOverPError[NPFMuon]/D"); 
  mtree->Branch("PFMuonTrkDxy"                                       ,mPFMuonTrkDxy                                       ,"PFMuonTrkDxy[NPFMuon]/D");    
  mtree->Branch("PFMuonTrkNumOfValidPixHits"                         ,mPFMuonTrkNumOfValidPixHits                         ,"PFMuonTrkNumOfValidPixHits[NPFMuon]/D"); 
  mtree->Branch("PFMuonTrkLayerWithMeasurement"                      ,mPFMuonTrkLayerWithMeasurement                      ,"PFMuonTrkLayerWithMeasurement[NPFMuon]/I");
  
  ///-------------------------------------------------------------------------- 
  /// PF Electrons
  ///-------------------------------------------------------------------------- 
  mtree->Branch("NPFElec"                                            ,&mNPFElec                                           ,"NPFElec/I");
  mtree->Branch("PFElecCharge"                                       ,mPFElecCharge                                       ,"PFElecCharge[NPFElec]/I");
  mtree->Branch("PFElecIsEB"                                         ,mPFElecIsEB                                         ,"PFElecIsEB[NPFElec]/I");  
  //
  mtree->Branch("PFElecE"                                            ,mPFElecE                                            ,"PFElecE[NPFElec]/D");                    
  mtree->Branch("PFElecPt"                                           ,mPFElecPt                                           ,"PFElecPt[NPFElec]/D");                  
  mtree->Branch("PFElecPx"                                           ,mPFElecPx                                           ,"PFElecPx[NPFElec]/D");                  
  mtree->Branch("PFElecPy"                                           ,mPFElecPy                                           ,"PFElecPy[NPFElec]/D");                  
  mtree->Branch("PFElecPz"                                           ,mPFElecPz                                           ,"PFElecPz[NPFElec]/D");                  
  mtree->Branch("PFElecEta"                                          ,mPFElecEta                                          ,"PFElecEta[NPFElec]/D");                
  mtree->Branch("PFElecPhi"                                          ,mPFElecPhi                                          ,"PFElecPhi[NPFElec]/D");                
  mtree->Branch("PFElecCharHadIso"                                   ,mPFElecCharHadIso                                   ,"PFElecCharHadIso[NPFElec]/D");  
  mtree->Branch("PFElecPhoIso"                                       ,mPFElecPhoIso                                       ,"PFElecPhoIso[NPFElec]/D");          
  mtree->Branch("PFElecNeuHadIso"                                    ,mPFElecNeuHadIso                                    ,"PFElecNeuHadIso[NPFElec]/D");    
  mtree->Branch("PFElecMva"                                          ,mPFElecMva                                          ,"PFElecMva[NPFElec]/D"); 
  mtree->Branch("PFElecEffArea"                                      ,mPFElecEffArea                                      ,"PFElecEffArea[NPFElec]/D");
  //
  mtree->Branch("PFElecdxy"                                          ,mPFElecdxy                                          ,"PFElecdxy[NPFElec]/D");                
  mtree->Branch("PFElecdz"                                           ,mPFElecdz                                           ,"PFElecdz[NPFElec]/D");                  
  mtree->Branch("PFElecHadOverEm"                                    ,mPFElecHadOverEm                                    ,"PFElecHadOverEm[NPFElec]/D");    
  mtree->Branch("PFElecHcalOverEm"                                   ,mPFElecHcalOverEm                                   ,"PFElecHcalOverEm[NPFElec]/D");  
  mtree->Branch("PFElecSupClusOverP"                                 ,mPFElecSupClusOverP                                 ,"PFElecSupClusOverP[NPFElec]/D");
  mtree->Branch("PFElecInnerHits"                                    ,mPFElecInnerHits                                    ,"PFElecInnerHits[NPFElec]/D");    
  mtree->Branch("PFElecConvDist"                                     ,mPFElecConvDist                                     ,"PFElecConvDist[NPFElec]/D");      
  mtree->Branch("PFElecConvDcot"                                     ,mPFElecConvDcot                                     ,"PFElecConvDcot[NPFElec]/D");
  //
  mtree->Branch("PFElecEcalDrivenSeed"                               ,mPFElecEcalDrivenSeed                               ,"PFElecEcalDrivenSeed[NPFElec]/I");
  mtree->Branch("PFElecdB"                                           ,mPFElecdB                                           ,"PFElecdB[NPFElec]/I");
  mtree->Branch("PFElecNumOfValidHits"                               ,mPFElecNumOfValidHits                               ,"PFElecNumOfValidHits[NPFElec]/I");
  mtree->Branch("PFElecdr03HcalTowerSumEt"                           ,mPFElecdr03HcalTowerSumEt                           ,"PFElecdr03HcalTowerSumEt[NPFElec]/D");
  mtree->Branch("PFElecdr03EcalRecHitSumEt"                          ,mPFElecdr03EcalRecHitSumEt                          ,"PFElecdr03EcalRecHitSumEt[NPFElec]/D");
  mtree->Branch("PFElecdr03TkSumPt"                                  ,mPFElecdr03TkSumPt                                  ,"PFElecdr03TkSumPt[NPFElec]/D");
  mtree->Branch("PFElecNormChi2"                                     ,mPFElecNormChi2                                     ,"PFElecNormChi2[NPFElec]/D");
  mtree->Branch("PFElecfbrem"                                        ,mPFElecfbrem                                        ,"PFElecfbrem[NPFElec]/D");
  mtree->Branch("PFElecDEtaSuTrAtVtx"                                ,mPFElecDEtaSuTrAtVtx                                ,"PFElecDEtaSuTrAtVtx[NPFElec]/D");
  mtree->Branch("PFElecDPhiSuTrAtVtx"                                ,mPFElecDPhiSuTrAtVtx                                ,"PFElecDPhiSuTrAtVtx[NPFElec]/D");
  mtree->Branch("PFElecHadronicOverEm"                               ,mPFElecHadronicOverEm                               ,"PFElecHadronicOverEm[NPFElec]/D");
  mtree->Branch("PFElecHcalOverEcal"                                 ,mPFElecHcalOverEcal                                 ,"PFElecHcalOverEcal[NPFElec]/D");
  mtree->Branch("PFElecSuperClusOverP"                               ,mPFElecSuperClusOverP                               ,"PFElecSuperClusOverP[NPFElec]/D");
  mtree->Branch("PFElecetaeta"                                       ,mPFElecetaeta                                       ,"PFElecetaeta[NPFElec]/D");
  mtree->Branch("PFElecietaieta"                                     ,mPFElecietaieta                                     ,"PFElecietaieta[NPFElec]/D");
  //
  mtree->Branch("PFeidLoose"                                         ,mPFeidLoose                                         ,"PFeidLoose[NPFElec]/I");
  mtree->Branch("PFeidRobustHighEnergy"                              ,mPFeidRobustHighEnergy                              ,"PFeidRobustHighEnergy[NPFElec]/I");
  mtree->Branch("PFeidRobustLoose"                                   ,mPFeidRobustLoose                                   ,"PFeidRobustLoose[NPFElec]/I");
  mtree->Branch("PFeidRobustTight"                                   ,mPFeidRobustTight                                   ,"PFeidRobustTight[NPFElec]/I");
  mtree->Branch("PFeidTight"                                         ,mPFeidTight                                         ,"PFeidTight[NPFElec]/I");
  mtree->Branch("PFevspi"                                            ,mPFevspi                                            ,"PFevspi[NPFElec]/I");
  mtree->Branch("PFevsmu"                                            ,mPFevsmu                                            ,"PFevsmu[NPFElec]/I");
  //
  mtree->Branch("PFElecIsTight"                                      ,mPFElecIsTight                                      ,"PFElecIsTight[NPFElec]/I");     
  mtree->Branch("PFElecIsMedium"                                     ,mPFElecIsMedium                                     ,"PFElecIsMedium[NPFElec]/I");
  mtree->Branch("PFElecIsLoose"                                      ,mPFElecIsLoose                                      ,"PFElecIsLoose[NPFElec]/I");     
  mtree->Branch("PFElecIsVeto"                                       ,mPFElecIsVeto                                       ,"PFElecIsVeto[NPFElec]/I");
  
  ///-------------------------------------------------------------------------- 
  /// PF Taus
  ///--------------------------------------------------------------------------
  mtree->Branch("NPFTau"                                             ,&mNPFTau                                            ,"NPFTau/I");
  mtree->Branch("PFTauE"                                             ,mPFTauE                                             ,"PFTauE[NPFTau]/D" );
  mtree->Branch("PFTauPt"                                            ,mPFTauPt                                            ,"PFTauPt[NPFTau]/D" );
  mtree->Branch("PFTauPx"                                            ,mPFTauPx                                            ,"PFTauPx[NPFTau]/D" );
  mtree->Branch("PFTauPy"                                            ,mPFTauPy                                            ,"PFTauPy[NPFTau]/D" );
  mtree->Branch("PFTauPz"                                            ,mPFTauPz                                            ,"PFTauPz[NPFTau]/D" );
  mtree->Branch("PFTauEta"                                           ,mPFTauEta                                           ,"PFTauEta[NPFTau]/D" );
  mtree->Branch("PFTauPhi"                                           ,mPFTauPhi                                           ,"PFTauPhi[NPFTau]/D" );
  mtree->Branch("PFTauCharge"                                        ,mPFTauCharge                                        ,"PFTauCharge[NPFTau]/I");
  //
  mtree->Branch("PFTauEtaEtaMoment"                                  ,mPFTauEtaEtaMoment                                  ,"PFTauEtaEtaMoment[NPFTau]/D" );
  mtree->Branch("PFTauPhiPhiMoment"                                  ,mPFTauPhiPhiMoment                                  ,"PFTauPhiPhiMoment[NPFTau]/D" );
  mtree->Branch("PFTauEtaPhiMoment"                                  ,mPFTauEtaPhiMoment                                  ,"PFTauEtaPhiMoment[NPFTau]/D" );
  //
  mtree->Branch("PFTauLeadPFChargedHadrCandsignedSipt"               ,mPFTauLeadPFChargedHadrCandsignedSipt               ,"PFTauLeadPFChargedHadrCandsignedSipt[NPFTau]/D" );
  mtree->Branch("PFTauIsoPFChargedHadrCandsPtSum"                    ,mPFTauIsoPFChargedHadrCandsPtSum                    ,"PFTauIsoPFChargedHadrCandsPtSum[NPFTau]/D" );
  mtree->Branch("PFTauIsoPFGammaCandsEtSum"                          ,mPFTauIsoPFGammaCandsEtSum                          ,"PFTauIsoPFGammaCandsEtSum[NPFTau]/D" );
  mtree->Branch("PFTauMaximumHCALPFClusterEt"                        ,mPFTauMaximumHCALPFClusterEt                        ,"PFTauMaximumHCALPFClusterEt[NPFTau]/D" );
  mtree->Branch("PFTauEmFraction"                                    ,mPFTauEmFraction                                    ,"PFTauEmFraction[NPFTau]/D" );
  mtree->Branch("PFTauHcalTotOverPLead"                              ,mPFTauHcalTotOverPLead                              ,"PFTauHcalTotOverPLead[NPFTau]/D" );
  mtree->Branch("PFTauHcalMaxOverPLead"                              ,mPFTauHcalMaxOverPLead                              ,"PFTauHcalMaxOverPLead[NPFTau]/D" );
  mtree->Branch("PFTauHcal3x3OverPLead"                              ,mPFTauHcal3x3OverPLead                              ,"PFTauHcal3x3OverPLead[NPFTau]/D" );
  mtree->Branch("PFTauEcalStripSumEOverPLead"                        ,mPFTauEcalStripSumEOverPLead                        ,"PFTauEcalStripSumEOverPLead[NPFTau]/D" );
  mtree->Branch("PFTauBremsRecoveryEOverPLead"                       ,mPFTauBremsRecoveryEOverPLead                       ,"PFTauBremsRecoveryEOverPLead[NPFTau]/D" );
  mtree->Branch("PFTauElectronPreIDOutput"                           ,mPFTauElectronPreIDOutput                           ,"PFTauElectronPreIDOutput[NPFTau]/D" );
  mtree->Branch("PFTauElectronPreIDDecision"                         ,mPFTauElectronPreIDDecision                         ,"PFTauElectronPreIDDecision[NPFTau]/D" );
  mtree->Branch("PFTauCaloComp"                                      ,mPFTauCaloComp                                      ,"PFTauCaloComp[NPFTau]/D" );
  mtree->Branch("PFTauSegComp"                                       ,mPFTauSegComp                                       ,"PFTauSegComp[NPFTau]/D" );
  mtree->Branch("PFTauMuonDecision"                                  ,mPFTauMuonDecision                                  ,"PFTauMuonDecision[NPFTau]/D" );
  //
  mtree->Branch("PFTausignalPFChargedHadrCands"                      ,mPFTausignalPFChargedHadrCands                      ,"PFTausignalPFChargedHadrCands[NPFTau]/D" );
  mtree->Branch("PFTausignalPFGammaCands"                            ,mPFTausignalPFGammaCands                            ,"PFTausignalPFGammaCands[NPFTau]/D" );
  //
  mtree->Branch("PFTauDisAgainstElectronDeadECAL"                    ,mPFTauDisAgainstElectronDeadECAL                    ,"PFTauDisAgainstElectronDeadECAL[NPFTau]/D");
  mtree->Branch("PFTauDisAgainstElectronLoose"                       ,mPFTauDisAgainstElectronLoose                       ,"PFTauDisAgainstElectronLoose[NPFTau]/D");
  mtree->Branch("PFTauDisAgainstElectronLooseMVA2"                   ,mPFTauDisAgainstElectronLooseMVA2                   ,"PFTauDisAgainstElectronLooseMVA2[NPFTau]/D");
  mtree->Branch("PFTauDisAgainstElectronLooseMVA3"                   ,mPFTauDisAgainstElectronLooseMVA3                   ,"PFTauDisAgainstElectronLooseMVA3[NPFTau]/D");
  mtree->Branch("PFTauDisAgainstElectronMVA2category"                ,mPFTauDisAgainstElectronMVA2category                ,"PFTauDisAgainstElectronMVA2category[NPFTau]/D");
  mtree->Branch("PFTauDisAgainstElectronMVA2raw"                     ,mPFTauDisAgainstElectronMVA2raw                     ,"PFTauDisAgainstElectronMVA2raw[NPFTau]/D");
  mtree->Branch("PFTauDisAgainstElectronMVA3category"                ,mPFTauDisAgainstElectronMVA3category                ,"PFTauDisAgainstElectronMVA3category[NPFTau]/D");
  mtree->Branch("PFTauDisAgainstElectronMVA3raw"                     ,mPFTauDisAgainstElectronMVA3raw                     ,"PFTauDisAgainstElectronMVA3raw[NPFTau]/D");
  mtree->Branch("PFTauDisAgainstElectronMVA"                         ,mPFTauDisAgainstElectronMVA                         ,"PFTauDisAgainstElectronMVA[NPFTau]/D");
  mtree->Branch("PFTauDisAgainstElectronMedium"                      ,mPFTauDisAgainstElectronMedium                      ,"PFTauDisAgainstElectronMedium[NPFTau]/D");
  mtree->Branch("PFTauDisAgainstElectronMediumMVA2"                  ,mPFTauDisAgainstElectronMediumMVA2                  ,"PFTauDisAgainstElectronMediumMVA2[NPFTau]/D");
  mtree->Branch("PFTauDisAgainstElectronMediumMVA3"                  ,mPFTauDisAgainstElectronMediumMVA3                  ,"PFTauDisAgainstElectronMediumMVA3[NPFTau]/D");
  mtree->Branch("PFTauDisAgainstElectronTight"                       ,mPFTauDisAgainstElectronTight                       ,"PFTauDisAgainstElectronTight[NPFTau]/D");
  mtree->Branch("PFTauDisAgainstElectronTightMVA2"                   ,mPFTauDisAgainstElectronTightMVA2                   ,"PFTauDisAgainstElectronTightMVA2[NPFTau]/D");
  mtree->Branch("PFTauDisAgainstElectronTightMVA3"                   ,mPFTauDisAgainstElectronTightMVA3                   ,"PFTauDisAgainstElectronTightMVA3[NPFTau]/D");
  mtree->Branch("PFTauDisAgainstElectronVLooseMVA2"                  ,mPFTauDisAgainstElectronVLooseMVA2                  ,"PFTauDisAgainstElectronVLooseMVA2[NPFTau]/D");
  mtree->Branch("PFTauDisAgainstElectronVTightMVA3"                  ,mPFTauDisAgainstElectronVTightMVA3                  ,"PFTauDisAgainstElectronVTightMVA3[NPFTau]/D");
  mtree->Branch("PFTauDisAgainstMuonLoose2"                          ,mPFTauDisAgainstMuonLoose2                          ,"PFTauDisAgainstMuonLoose2[NPFTau]/D");
  mtree->Branch("PFTauDisAgainstMuonLoose"                           ,mPFTauDisAgainstMuonLoose                           ,"PFTauDisAgainstMuonLoose[NPFTau]/D");
  mtree->Branch("PFTauDisAgainstMuonMedium2"                         ,mPFTauDisAgainstMuonMedium2                         ,"PFTauDisAgainstMuonMedium2[NPFTau]/D");
  mtree->Branch("PFTauDisAgainstMuonMedium"                          ,mPFTauDisAgainstMuonMedium                          ,"PFTauDisAgainstMuonMedium[NPFTau]/D");
  mtree->Branch("PFTauDisAgainstMuonTight2"                          ,mPFTauDisAgainstMuonTight2                          ,"PFTauDisAgainstMuonTight2[NPFTau]/D");
  mtree->Branch("PFTauDisAgainstMuonTight"                           ,mPFTauDisAgainstMuonTight                           ,"PFTauDisAgainstMuonTight[NPFTau]/D");
  mtree->Branch("PFTauDisByCombinedIsolationDeltaBetaCorrRaw3Hits"   ,mPFTauDisByCombinedIsolationDeltaBetaCorrRaw3Hits   ,"PFTauDisByCombinedIsolationDeltaBetaCorrRaw3Hits[NPFTau]/D");
  mtree->Branch("PFTauDisByCombinedIsolationDeltaBetaCorrRaw"        ,mPFTauDisByCombinedIsolationDeltaBetaCorrRaw        ,"PFTauDisByCombinedIsolationDeltaBetaCorrRaw[NPFTau]/D");
  mtree->Branch("PFTauDisByIsolationMVA2raw"                         ,mPFTauDisByIsolationMVA2raw                         ,"PFTauDisByIsolationMVA2raw[NPFTau]/D");
  mtree->Branch("PFTauDisByIsolationMVAraw"                          ,mPFTauDisByIsolationMVAraw                          ,"PFTauDisByIsolationMVAraw[NPFTau]/D");
  mtree->Branch("PFTauDisByLooseCombinedIsolationDeltaBetaCorr3Hits" ,mPFTauDisByLooseCombinedIsolationDeltaBetaCorr3Hits ,"PFTauDisByLooseCombinedIsolationDeltaBetaCorr3Hits[NPFTau]/D");
  mtree->Branch("PFTauDisByLooseCombinedIsolationDeltaBetaCorr"      ,mPFTauDisByLooseCombinedIsolationDeltaBetaCorr      ,"PFTauDisByLooseCombinedIsolationDeltaBetaCorr[NPFTau]/D");
  mtree->Branch("PFTauDisByLooseIsolationDeltaBetaCorr"              ,mPFTauDisByLooseIsolationDeltaBetaCorr              ,"PFTauDisByLooseIsolationDeltaBetaCorr[NPFTau]/D");
  mtree->Branch("PFTauDisByLooseIsolation"                           ,mPFTauDisByLooseIsolation                           ,"PFTauDisByLooseIsolation[NPFTau]/D");
  mtree->Branch("PFTauDisByLooseIsolationMVA2"                       ,mPFTauDisByLooseIsolationMVA2                       ,"PFTauDisByLooseIsolationMVA2[NPFTau]/D");
  mtree->Branch("PFTauDisByLooseIsolationMVA"                        ,mPFTauDisByLooseIsolationMVA                        ,"PFTauDisByLooseIsolationMVA[NPFTau]/D");
  mtree->Branch("PFTauDisByMediumCombinedIsolationDeltaBetaCorr3Hits",mPFTauDisByMediumCombinedIsolationDeltaBetaCorr3Hits,"PFTauDisByMediumCombinedIsolationDeltaBetaCorr3Hits[NPFTau]/D");
  mtree->Branch("PFTauDisByMediumCombinedIsolationDeltaBetaCorr"     ,mPFTauDisByMediumCombinedIsolationDeltaBetaCorr     ,"PFTauDisByMediumCombinedIsolationDeltaBetaCorr[NPFTau]/D");
  mtree->Branch("PFTauDisByMediumIsolationDeltaBetaCorr"             ,mPFTauDisByMediumIsolationDeltaBetaCorr             ,"PFTauDisByMediumIsolationDeltaBetaCorr[NPFTau]/D");
  mtree->Branch("PFTauDisByMediumIsolation"                          ,mPFTauDisByMediumIsolation                          ,"PFTauDisByMediumIsolation[NPFTau]/D");
  mtree->Branch("PFTauDisByMediumIsolationMVA2"                      ,mPFTauDisByMediumIsolationMVA2                      ,"PFTauDisByMediumIsolationMVA2[NPFTau]/D");
  mtree->Branch("PFTauDisByMediumIsolationMVA"                       ,mPFTauDisByMediumIsolationMVA                       ,"PFTauDisByMediumIsolationMVA[NPFTau]/D");
  mtree->Branch("PFTauDisByTightCombinedIsolationDeltaBetaCorr3Hits" ,mPFTauDisByTightCombinedIsolationDeltaBetaCorr3Hits ,"PFTauDisByTightCombinedIsolationDeltaBetaCorr3Hits[NPFTau]/D");
  mtree->Branch("PFTauDisByTightCombinedIsolationDeltaBetaCorr"      ,mPFTauDisByTightCombinedIsolationDeltaBetaCorr      ,"PFTauDisByTightCombinedIsolationDeltaBetaCorr[NPFTau]/D");
  mtree->Branch("PFTauDisByTightIsolationDeltaBetaCorr"              ,mPFTauDisByTightIsolationDeltaBetaCorr              ,"PFTauDisByTightIsolationDeltaBetaCorr[NPFTau]/D");
  mtree->Branch("PFTauDisByTightIsolation"                           ,mPFTauDisByTightIsolation                           ,"PFTauDisByTightIsolation[NPFTau]/D");
  mtree->Branch("PFTauDisByTightIsolationMVA2"                       ,mPFTauDisByTightIsolationMVA2                       ,"PFTauDisByTightIsolationMVA2[NPFTau]/D");
  mtree->Branch("PFTauDisByTightIsolationMVA"                        ,mPFTauDisByTightIsolationMVA                        ,"PFTauDisByTightIsolationMVA[NPFTau]/D");
  mtree->Branch("PFTauDisByVLooseCombinedIsolationDeltaBetaCorr"     ,mPFTauDisByVLooseCombinedIsolationDeltaBetaCorr     ,"PFTauDisByVLooseCombinedIsolationDeltaBetaCorr[NPFTau]/D");
  mtree->Branch("PFTauDisByVLooseIsolationDeltaBetaCorr"             ,mPFTauDisByVLooseIsolationDeltaBetaCorr             ,"PFTauDisByVLooseIsolationDeltaBetaCorr[NPFTau]/D");
  mtree->Branch("PFTauDisByVLooseIsolation"                          ,mPFTauDisByVLooseIsolation                          ,"PFTauDisByVLooseIsolation[NPFTau]/D");
  mtree->Branch("PFTauDisDecayModeFinding"                           ,mPFTauDisDecayModeFinding                           ,"PFTauDisDecayModeFinding[NPFTau]/D");
  //
  mtree->Branch("PFTauJetPt"                                         ,mPFTauJetPt                                         ,"PFTauJetPt[NPFTau]/D"); 
  mtree->Branch("PFTauJetEta"                                        ,mPFTauJetEta                                        ,"PFTauJetEta[NPFTau]/D"); 
  mtree->Branch("PFTauJetPhi"                                        ,mPFTauJetPhi                                        ,"PFTauJetPhi[NPFTau]/D");
  
  ///-------------------------------------------------------------------------- 
  /// Photons
  ///--------------------------------------------------------------------------
  mtree->Branch("NPhot"                                              ,&mNPhot                                             ,"NPhot/I");
  mtree->Branch("PhotE"                                              ,mPhotE                                              ,"PhotE[NPhot]/D");
  mtree->Branch("PhotPt"                                             ,mPhotPt                                             ,"PhotPt[NPhot]/D");
  mtree->Branch("PhotPx"                                             ,mPhotPx                                             ,"PhotPx[NPhot]/D");
  mtree->Branch("PhotPy"                                             ,mPhotPy                                             ,"PhotPy[NPhot]/D");
  mtree->Branch("PhotPz"                                             ,mPhotPz                                             ,"PhotPz[NPhot]/D");
  mtree->Branch("PhotEta"                                            ,mPhotEta                                            ,"PhotEta[NPhot]/D");
  mtree->Branch("PhotPhi"                                            ,mPhotPhi                                            ,"PhotPhi[NPhot]/D");
  mtree->Branch("PhotE1x5"                                           ,mPhotE1x5                                           ,"PhotE1x5[NPhot]/D");
  mtree->Branch("PhotE2x5"                                           ,mPhotE2x5                                           ,"PhotE2x5[NPhot]/D");
  mtree->Branch("PhotE5x5"                                           ,mPhotE5x5                                           ,"PhotE5x5[NPhot]/D");
  mtree->Branch("PhotSigEta"                                         ,mPhotSigEta                                         ,"PhotSigEta[NPhot]/D");
  mtree->Branch("PhotSigPhi"                                         ,mPhotSigPhi                                         ,"PhotSigPhi[NPhot]/D");
  mtree->Branch("PhotEcalIso04"                                      ,mPhotEcalIso04                                      ,"PhotEcalIso04[NPhot]/D");
  mtree->Branch("PhotHcalIso04"                                      ,mPhotHcalIso04                                      ,"PhotHcalIso04[NPhot]/D");
  mtree->Branch("PhotTrackIso04"                                     ,mPhotTrackIso04                                     ,"PhotTrackIso04[NPhot]/D");
  mtree->Branch("PhotHasPixSeed"                                     ,mPhotHasPixSeed                                     ,"PhotHasPixSeed[NPhot]/D");
  mtree->Branch("PhotIsPhot"                                         ,mPhotIsPhot                                         ,"PhotIsPhot[NPhot]/D");
  
  ///-------------------------------------------------------------------------- 
  /// Primiry vertices
  ///--------------------------------------------------------------------------
  mtree->Branch("NPV"                                                ,&mNPV                                               ,"NPV/I");
  mtree->Branch("PVx"                                                ,mPVx                                                ,"PVx[NPV]/D");
  mtree->Branch("PVy"                                                ,mPVy                                                ,"PVy[NPV]/D");
  mtree->Branch("PVz"                                                ,mPVz                                                ,"PVz[NPV]/D");
  mtree->Branch("PVchi2"                                             ,mPVchi2                                             ,"PVchi2[NPV]/D");
  mtree->Branch("PVndof"                                             ,mPVndof                                             ,"PVndof[NPV]/D");
  mtree->Branch("PVntracks"                                          ,mPVntracks                                          ,"PVntracks[NPV]/I");
  
  ///-------------------------------------------------------------------------- 
  /// Gen information
  ///--------------------------------------------------------------------------
  if(isMCTag){
    mtree->Branch("NGenPar"                                          ,&mNGenPar                                           ,"NGenPar/I");
    mtree->Branch("GenParId"                                         ,mGenParId                                           ,"GenParId[NGenPar]/I");
    mtree->Branch("GenParStatus"                                     ,mGenParStatus                                       ,"GenParStatus[NGenPar]/I");
    mtree->Branch("GenParE"                                          ,mGenParE                                            ,"GenParE[NGenPar]/D");
    mtree->Branch("GenParPx"                                         ,mGenParPx                                           ,"GenParPx[NGenPar]/D");
    mtree->Branch("GenParPy"                                         ,mGenParPy                                           ,"GenParPy[NGenPar]/D");
    mtree->Branch("GenParPz"                                         ,mGenParPz                                           ,"GenParPz[NGenPar]/D");
    mtree->Branch("GenParEta"                                        ,mGenParEta                                          ,"GenParEta[NGenPar]/D");
    mtree->Branch("GenParPhi"                                        ,mGenParPhi                                          ,"GenParPhi[NGenPar]/D");
    mtree->Branch("GenParCharge"                                     ,mGenParCharge                                       ,"GenParCharge[NGenPar]/I");
    mtree->Branch("GenParPt"                                         ,mGenParPt                                           ,"GenParPt[NGenPar]/D");
    mtree->Branch("GenParMass"                                       ,mGenParMass                                         ,"GenParMass[NGenPar]/D");
    mtree->Branch("GenParDoughterOf"                                 ,mGenParDoughterOf                                   ,"GenParDoughterOf[NGenPar]/I");
    mtree->Branch("GenParMother1"                                    ,mGenParMother1                                      ,"GenParMother1[NGenPar]/I");
    mtree->Branch("GenParMother2"                                    ,mGenParMother2                                      ,"GenParMother2[NGenPar]/I");	
    mtree->Branch("GenScale"                                         ,&mGenScale                                          ,"GenScale/D");
    // PDF
    mtree->Branch("PDFx1"                                            ,&mPdfx1                                             ,"PDFx1/D");
    mtree->Branch("PDFx2"                                            ,&mPdfx2                                             ,"PDFx2/D");
    mtree->Branch("PDFf1"                                            ,&mPdff1                                             ,"PDFf1/I");
    mtree->Branch("PDFf2"                                            ,&mPdff2                                             ,"PDFf2/I");
    mtree->Branch("PDFscale"                                         ,&mPdfscale                                          ,"PDFscale/D");
    mtree->Branch("PDFpdf1"                                          ,&mPdfpdf1                                           ,"PDFpdf1/D");
    mtree->Branch("PDFpdf2"                                          ,&mPdfpdf2                                           ,"PDFpdf2/D");
    // W-Decay mode
    mtree->Branch("WTauDecayMode"                                    ,&mWTauDecayMode                                     ,"WTauDecayMode/I");
    mtree->Branch("WTauN"                                            ,&mWTauN                                             ,"WTauN/I" );
    mtree->Branch("WTauDecayId"                                      ,mWTauDecayId                                        ,"WTauDecayId[WTauN]/I");
    mtree->Branch("WTauDecayStatus"                                  ,mWTauDecayStatus                                    ,"WTauDecayStatus[WTauN]/I");
    mtree->Branch("WTauDecayPt"                                      ,mWTauDecayPt                                        ,"WTauDecayPt[WTauN]/D");
    mtree->Branch("WTauDecayEta"                                     ,mWTauDecayEta                                       ,"WTauDecayEta[WTauN]/D");
    mtree->Branch("WTauDecayPhi"                                     ,mWTauDecayPhi                                       ,"WTauDecayPhi[WTauN]/D"); 
    mtree->Branch("WTauDecayMass"                                    ,mWTauDecayMass                                      ,"WTauDecayMass[WTauN]/D");
    // PDF
    if(isSignalTag==1.){
      mtree->Branch("NPDFWeights1"                                   ,&mNPdfWeights1                                      ,"NPDFWeights1/I");
      mtree->Branch("PDFWeights1"                                    ,mPdfWeights1                                        ,"PDFWeights1[NPDFWeights1]/D");
    }
    if(isSignalTag==2.){
      mtree->Branch("NPDFWeights"                                    ,&mNPdfWeights                                       ,"NPDFWeights/I");
      mtree->Branch("PDFWeights"                                     ,mPdfWeights                                         ,"PDFWeights[NPDFWeights]/D");
    }
    if(isSignalTag==3.){
      mtree->Branch("NPDFWeights"                                    ,&mNPdfWeights                                       ,"NPDFWeights/I");
      mtree->Branch("PDFWeights"                                     ,mPdfWeights                                         ,"PDFWeights[NPDFWeights]/D");
    }
  }
  
  ///-------------------------------------------------------------------------- 
  /// HLT
  ///--------------------------------------------------------------------------
  mtree->Branch("nHLT"                                               ,&nHLT                                               ,"nHLT/I");
  mtree->Branch("HLTArray"                                           ,HLTArray                                            ,"HLTArray[nHLT]/I");
  mtree->Branch("HLTArray2"                                          ,HLTArray2                                           ,"HLTArray2[100]/I");
  mtree->Branch("HLTNames"                                           ,trgnm                                               ,"HLTNames/C");
  mtree->Branch("HLTPreScale2"                                       ,HLTPreScale2                                        ,"HLTPreScale2[100]/I");
  
  ///-------------------------------------------------------------------------- 
  /// Noise flags
  ///--------------------------------------------------------------------------
  mtree->Branch("nNoiseFlag"                                         ,&nNoiseFlag                                         ,"nNoiseFlag/I");
  mtree->Branch("NoiseFlag"                                          ,NoiseFlag                                           ,"NoiseFlag[nNoiseFlag]/I");
  mtree->Branch("HCFlag"                                             ,&flg_hnoise                                         ,"HCFlag/I");
  mtree->Branch("HFFlag"                                             ,&flg_hfbadhit                                       ,"HFFlag/I");
  mtree->Branch("ESFlag"                                             ,&flg_ecalspike                                      ,"ESFlag/I");
  mtree->Branch("BeamHaloTight"                                      ,&mbeamHaloTight                                     ,"BeamHaloTight/I");
  mtree->Branch("BeamHaloLoose"                                      ,&mbeamHaloLoose                                     ,"BeamHaloLoose/I");
  mtree->Branch("SumTrackPt"                                         ,&mSumTrackPt                                        ,"SumTrackPt/D");
  
  ///-------------------------------------------------------------------------- 
  /// L1
  ///--------------------------------------------------------------------------
  if(!isMCTag){
    /// L1
    mtree->Branch("nL1"                                              ,&nL1                                                ,"nL1/I");
    mtree->Branch("L1Array"                                          ,L1Array                                             ,"L1Array[nL1]/I");
    
    /// L1T
    mtree->Branch("nL1T"                                             ,&nL1T                                               ,"nL1T/I");
    mtree->Branch("L1TArray"                                         ,L1TArray                                            ,"L1TArray[nL1T]/I");
  }
  
}

// ------------ method called once each job just after ending the event loop  ------------
void NtupleAnalyzer::endJob() 
{
}


//define this as a plug-in
DEFINE_FWK_MODULE(NtupleAnalyzer);
