R__ADD_INCLUDE_PATH($VMCWORKDIR)

#include "macro/run/bmnloadlibs.C"

const int nTqdcBeamDet =  8;
const int nTdcDet      =  8;

//                                             0       1        2         3          4          5        6          7          
TString  sTqdcBeamDetName[nTqdcBeamDet] = {  "BC1", "VETO", "T0_1_A", "T0_2_A", "BC2_1_A", "BC2_2_A", "T0_SUM", "BC2_SUM" };
float      fBeamMeanBC1dT[nTqdcBeamDet] = {    0,     -0.3,   -42.0,    -42.9,     -42.6,     -44.9,   -41.7,     -42.0   };
float         fBeamMeanPH[nTqdcBeamDet] = {   7530,   1500,    1950,     2400,      2000,      2600,    2200,      2450   };
float        fBeamMeanInt[nTqdcBeamDet] = {  25900,   5300,    -100,     -100,      -100,      -100,    -100,      -100   }; 

//                                     0         1          2          3         4         5          6          7  
TString   sTdcDetName[nTdcDet] = { "T0_1_A", "T0_2_A", "BC2_1_A", "BC2_2_A", "T0_1_M", "T0_2_M", "BC2_1_M", "BC2_2_M" };
float    fTimeFromBC1[nTdcDet] = {   84.3,      81.9,     80.8,      80.1,     84.8,     82.5,      81.1,      80.7   };

const float  kdT =  5.;     // max of abs(Time_BC2 - <Time_BC2 - Time_BC1>) for individual BC2 Anode and MCP signals in TDC

const float kC12   = 36.;           // aimed position for mean amplitude of the Carbon peak

const float kVetoCut      =  20.;   // max amplitude of VC TQDC.GetPeak() normalized to Carbon peak 
const float kBC1MinIntCut =  10.;   // min value of BC1 TQDC.GetIntegral(),  removes upstream interactions  
const float kBC1MaxIntCut =  54.;   // max value of BC1 TQDC.GetIntegral(),  removes pile-up events  
const float kBC1MinTime   =  30.;   // min value of BC1 TQDC.GetTime(),  removes events with unexpected BC1 vs trigger timing   
const float kBC1MaxTime   = 100.;   // max value of BC1 TQDC.GetTime(),  removes events with unexpected BC1 vs trigger timing

const float  kT0_SUM_MaxPH    = 54.;   // max value of  T0_SUM TQDC.GetPeak(),  removes interactions in T0 and BC2  
const float kBC2_SUM_MaxPH    = 54.;   // max value of BC2_SUM TQDC.GetPeak(),  removes interactions in T0 and BC2  
const float kT0_BC2_SUM_MinPH = 52.;   // min value of T0_SUM TQDC.GetPeak() + BC2_SUM TQDC.GetPeak(),  removes interactions in T0 and BC2 



void AmplitudesInBC(const char *sFileName="raw_1000.root", const char* fOutName="plots_1000.root") {

  bmnloadlibs(); // load libraries

  TChain *tree = new TChain("bmndata");

  TString sBranchName;

  tree->Add(sFileName);

  BmnEventHeader *pEventHeader = NULL;
  tree->SetBranchAddress("BmnEventHeader.", &pEventHeader);

  TClonesArray  *TqdcBeamDetDigits[nTqdcBeamDet];
  TClonesArray  *TdcDigits[nTdcDet];

  Int_t   iDet;
  
  for(iDet = 0; iDet < nTqdcBeamDet; iDet++) {
    TqdcBeamDetDigits[iDet] = new TClonesArray("BmnTrigWaveDigit");
    sBranchName = Form("TQDC_%s",sTqdcBeamDetName[iDet].Data());
    tree->SetBranchAddress(sBranchName, &TqdcBeamDetDigits[iDet]); 
  }
  for(iDet = 0; iDet < nTdcDet; iDet++) {
    TdcDigits[iDet] = new TClonesArray("BmnTrigDigit");
    sBranchName = Form("%s",sTdcDetName[iDet].Data());
    tree->SetBranchAddress(sBranchName, &TdcDigits[iDet]);
  }


  TString sHistName, sHistTitle, sXaxisTitle, sYaxisTitle;

  TH1F *hTqdcAmp[nTqdcBeamDet];
  TH1F *hTdcAmp[nTdcDet];

  for(iDet = 0; iDet < nTqdcBeamDet; iDet++) {
    sHistName  = Form("h_Amp_%s", sTqdcBeamDetName[iDet].Data());
    sHistTitle = Form("TQDC fMax distribution in %s", sTqdcBeamDetName[iDet].Data());
    hTqdcAmp[iDet] = new TH1F(sHistName,sHistTitle, 100, 0, 3*kC12);
    hTqdcAmp[iDet]->GetXaxis()->SetTitle("Amplitude, a.u.");
    hTqdcAmp[iDet]->GetYaxis()->SetTitle("Counts");
  }

  for(iDet = 0; iDet < nTdcDet; iDet++) {
    sHistName  = Form("h_Width_%s", sTdcDetName[iDet].Data());
    sHistTitle = Form("TDC fAmp distribution in %s", sTdcDetName[iDet].Data());
    hTdcAmp[iDet] = new TH1F(sHistName,sHistTitle, 100, 0, 50);
    hTdcAmp[iDet]->GetXaxis()->SetTitle("Width, ns");
    hTdcAmp[iDet]->GetYaxis()->SetTitle("Counts");
  }
  
  Int_t   nDigits, iDigit, nTdcDigits[nTdcDet];


  Long64_t nEvents, iEvent, iEventId;
  
  BmnTrigWaveDigit *pTqdcBeamDetDigit[nTqdcBeamDet];
  BmnTrigDigit     *pTdcDigit[nTdcDet];
  BmnTrigDigit     *pTmpTdcDigit;

  Bool_t iEmptyTDC[nTdcDet];
  Int_t  nEventsWithEmptyTDC[nTdcDet];
  for(iDet = 0; iDet < nTdcDet;  iDet++) nEventsWithEmptyTDC[iDet] = 0;


  Double_t fTime, fMin_dT;
  Double_t fAmp1, fAmp2;

  nEvents = tree->GetEntries();

  for(iEvent = 0; iEvent < nEvents; iEvent++) {
    
    for(iDet = 0; iDet < nTqdcBeamDet; iDet++) TqdcBeamDetDigits[iDet]->Delete();
    for(iDet = 0; iDet < nTdcDet;  iDet++)  TdcDigits[iDet]->Delete();
    for(iDet = 0; iDet < nTdcDet;  iDet++)  iEmptyTDC[iDet] = kFALSE;

    tree->GetEntry(iEvent);
    iEventId = pEventHeader->GetEventId();
    
    if(iEvent%1000 == 0)  cout<<"iEvent = " << iEvent << " out of "<< nEvents << endl;
    
    for(iDet = 0; iDet < nTqdcBeamDet; iDet++) {
      nDigits = TqdcBeamDetDigits[iDet]->GetEntriesFast();
      if(nDigits != 1) {
        cout << "*** Event No. " << iEvent << endl;
        cout << "*** Unexpected number of TQDC digits " << nDigits << "for detector " << sTqdcBeamDetName[iDet].Data() << endl;
        return;
      }
      pTqdcBeamDetDigit[iDet] = (BmnTrigWaveDigit*) TqdcBeamDetDigits[iDet]->At(0);
    }

    //                         Event Selection
    //========================================================================

    if(kC12 * pTqdcBeamDetDigit[1]->GetPeak() / fBeamMeanPH[1]  >  kVetoCut)  continue;   //  VC cut

    if(kC12 * pTqdcBeamDetDigit[0]->GetIntegral() / fBeamMeanInt[0] < kBC1MinIntCut) continue;   // BC1 cut
    if(kC12 * pTqdcBeamDetDigit[0]->GetIntegral() / fBeamMeanInt[0] > kBC1MaxIntCut) continue;   // BC1 cut
    if(pTqdcBeamDetDigit[0]->GetTime() < kBC1MinTime) continue;          // BC1 cut
    if(pTqdcBeamDetDigit[0]->GetTime() > kBC1MaxTime) continue;          // BC1 cut

    fAmp1 = pTqdcBeamDetDigit[6]->GetPeak()*kC12/fBeamMeanPH[6];   //  T0_SUM
    fAmp2 = pTqdcBeamDetDigit[7]->GetPeak()*kC12/fBeamMeanPH[7];   // BC2_SUM

    if(fAmp1 > kT0_SUM_MaxPH) continue;                              
    if(fAmp2 > kBC2_SUM_MaxPH) continue;
    if(fAmp1 + fAmp2 < kT0_BC2_SUM_MinPH) continue;

    //                   End of event selection based on TQDC info
    //==========================================================================

    for(iDet = 0; iDet < nTdcDet; iDet++) {
      nTdcDigits[iDet] = TdcDigits[iDet]->GetEntriesFast();

      nDigits = nTdcDigits[iDet];
      pTdcDigit[iDet] = NULL;
      fMin_dT = kdT;

      for(iDigit = 0; iDigit < nDigits; iDigit++) {
        pTmpTdcDigit = (BmnTrigDigit*) TdcDigits[iDet]->At(iDigit);
        fTime = TMath::Abs(pTmpTdcDigit->GetTime() - pTqdcBeamDetDigit[0]->GetTime() - fTimeFromBC1[iDet]);   // dT between TDC digit and BC1 TQDC digit
        if(fTime > kdT) {
          nTdcDigits[iDet]--;
          continue;
        }
        if(fTime < fMin_dT) {
          fMin_dT = fTime;
          pTdcDigit[iDet] = pTmpTdcDigit;
        }
      }
      if(nTdcDigits[iDet] < 1) {
        //        cout << "Empty TDC for detector " << sTdcDetName[iDet].Data() << endl;
        iEmptyTDC[iDet] = kTRUE;
        nEventsWithEmptyTDC[iDet]++; 
      }
    }

    //                            Filling histograms
    //============================================================================

    for(iDet = 0; iDet < nTqdcBeamDet; iDet++) {
      hTqdcAmp[iDet]->Fill(kC12 * pTqdcBeamDetDigit[iDet]->GetPeak() / fBeamMeanPH[iDet]);
    }

    for(iDet = 0; iDet < nTdcDet; iDet++) {
      if(iEmptyTDC[iDet] == kTRUE) continue;
      hTdcAmp[iDet]->Fill(pTdcDigit[iDet]->GetAmp());
    }
  }


  
//  TCanvas *c1 = new TCanvas("c1","TQDC Amplitudes",50,50,2050,1050);
//  gStyle->SetOptStat(1111);
//  c1->Divide(4,2);
//  iDet = 0;
//  for(int iPad = 1; iPad < 9; iPad++) {
//    c1->cd(iPad); 
//    gPad->SetGrid();
//    hTqdcAmp[iDet]->Draw();
//    iDet++;
//  }
//
//  TCanvas *c2 = new TCanvas("c2","TDC Amplitudes",50,50,2050,1050);
//  gStyle->SetOptStat(1111);
//  c2->Divide(4,2);
//  iDet = 0;
//  for(int iPad = 1; iPad < 9; iPad++) {
//    c2->cd(iPad); 
//    gPad->SetGrid();
//    hTdcAmp[iDet]->Draw();
//    cout << sTdcDetName[iDet].Data() << ": " << nEventsWithEmptyTDC[iDet] << " empty TDC events" << endl; 
//    iDet++;
//  }

  TFile f(fOutName, "recreate");
  
  auto digiHeader = (DigiRunHeader*)(tree->GetFile()->Get("DigiRunHeader"));
  vector<ulong> runId={digiHeader->GetRunId()};
  auto runStart=digiHeader->GetRunStartTime();
  auto runEnd=digiHeader->GetRunEndTime();
  
  f.WriteObject(digiHeader, "digiHeader"); 
  f.WriteObject(&runId, "runId"); 
  f.WriteObject(&runStart, "runStart"); 
  f.WriteObject(&runEnd, "runEnd"); 
  
  for (auto h:hTdcAmp)
    h->Write();

  for (auto h:hTqdcAmp)
    h->Write();
  f.Close();

  return;
}
