#include "qaUtils.h"

int peakBinMin=250, peakBinMax=320;

int triggerPeak(TClonesArray digis)
{
  auto digit=(BmnTrigWaveDigit*)digis.At(0);
  int peak=0;
  auto values=digit->GetShortValue();
  for (Int_t i = peakBinMin; i < digit->GetNSamples() && i < peakBinMax; ++i)
    if (values[i] > peak) 
      peak = values[i];
  return peak;
}

int triggerIntegral(TClonesArray digis)
{
  auto digit=(BmnTrigWaveDigit*)digis.At(0);
  int integral=0;
  auto values=digit->GetShortValue();
  for (Int_t i = peakBinMin; i < digit->GetNSamples() && i < peakBinMax; ++i)
    integral += values[i];
  return integral;
}

short triggerPeakBin (TClonesArray digis)
{
  auto digit=(BmnTrigWaveDigit*)digis.At(0);
  short peakBin=-1;
  auto values=digit->GetShortValue();
  for (Int_t i = 0; i < digit->GetNSamples(); ++i)
    if (values[i] > peakBin) 
      peakBin = i;
  return peakBin;
}

int triggerTime (TClonesArray digis)
{
  auto digit=(BmnTrigWaveDigit*)digis.At(0);
  int time=0;
  if(digit) time=digit->GetTime();
  return time;
}

RVec<int> triggerPeaks (TClonesArray digis)
{
  RVec<int>peaks; 
  for(auto d:digis)
    peaks.push_back(((BmnTrigWaveDigit*)d)->GetPeak());
  return peaks;
}

RVec<int> triggerIntegrals (TClonesArray digis)
{
  RVec<int>integrals; 
  for(auto d:digis)
    integrals.push_back(((BmnTrigWaveDigit*)d)->GetIntegral());
  return integrals;
}

RVec<int> FHCalDigiX(RVec<BmnFHCalDigi> digis)
{
  RVec<int> x;
  for(auto &d:digis)
    x.push_back(d.GetX());
  return x;
}

void plotTriggers (string inDigi="digi/mpd_run_Top_6822*.root", const char *out="qa.root")
{
  TChain *chainDigi=makeChain(inDigi, "bmndata");
  cout << "Digi: " << chainDigi->GetEntries() << " events\n";
  ROOT::RDataFrame d(*chainDigi);
  TFile outFile(out,"recreate");

  // read first Run Header if present
  DigiRunHeader* digiHeader = (DigiRunHeader*) chainDigi->GetCurrentFile()->Get("DigiRunHeader");
  if (digiHeader)
  {
    digiHeader->Dump();
    vector<ulong> runId={digiHeader->GetRunId()};
    auto runStart=digiHeader->GetRunStartTime();
    auto runEnd=digiHeader->GetRunEndTime();
    outFile.WriteObject(digiHeader, "digiHeader"); 
    outFile.WriteObject(&runId, "runId"); 
    outFile.WriteObject(&runStart, "runStart"); 
    outFile.WriteObject(&runEnd, "runEnd"); 
  }
  else 
    cout << chainDigi->GetCurrentFile()->GetName();

  auto dd=d
    //.Range(0,1000)
    .Filter("BmnEventHeader.fEventType==1")
    .Define("runId",[](FairEventHeader h){return h.GetRunId();}, {"BmnEventHeader."})
    .Define("evtId","BmnEventHeader.fEventId")
    .Define("BDmodId", "BD.fMod")
    .Define("BDmodTime", "BD.fTime")
    .Define("BDmodAmp", "BD.fAmp")
    .Define("BDgoodHit", "abs(BDmodTime-2000)<150")
    .Define("BDcount", "Sum(BDgoodHit)")
    .Define("BDamp", "Sum(BDmodAmp*BDgoodHit)")
    .Define("SImodId", "SI.fMod")
    .Define("SImodTime", "SI.fTime")
    .Define("SImodAmp", "SI.fAmp")
    .Define("SIgoodHit", "abs(SImodTime-2000)<100")
    .Define("SIcount", "Sum(SIgoodHit)")
    .Define("SIamp", "Sum(SImodAmp*SIgoodHit)")
    .Define("BDSIcount", "BDcount+SIcount")
    .Define("tFHCal", "Sum(FHCalDigi.fSignal)")
    .Define("FHCalDigiX", FHCalDigiX, {"FHCalDigi"})
    .Define("nFHCal", "Sum(FHCalDigi.fSignal*FHCalDigiX<2)")
    .Define("Hodo", "Sum(HodoDigi.fSignal)")
//    .Define("ScWall", "Sum(ScWallDigi.fSignal)")
    .Define("TOF400time", "TOF400.fTime")
    .Define("TOF400goodHit", "abs(TOF400.fTime-1450)<150")
    .Define("nTOF400", "Sum(TOF400goodHit)")
    .Define("TOF700time", "TOF700.fTime")
    .Define("TOF700goodHit", "abs(TOF700.fTime-1850)<200")
    .Define("nTOF700", "Sum(TOF700goodHit)")
    .Define("DCHtime", "DCH.fTime")
    .Define("DCHgoodHit", "abs(DCH.fTime-800)<300")
    .Define("nDCH", "Sum(DCHgoodHit)")
    .Define("BC1L", "BmnEventHeader.fInputsBR>>0&1")
    .Define("pBT", "BmnEventHeader.fInputsBR>>1&1")
    .Define("BT", "BmnEventHeader.fInputsBR>>2&1")
    .Define("NiT", "BmnEventHeader.fInputsBR>>3&1")
    .Define("CCT1", "BmnEventHeader.fInputsBR>>4&1")
    .Define("MBT", "BmnEventHeader.fInputsBR>>5&1")
    .Define("BTnBUSY", "BmnEventHeader.fInputsBR>>6&1")
    .Define("CCT2", "BmnEventHeader.fInputsBR>>7&1")
  ;
  for (auto &det:{"BC1S", "BC1T", "BC1B", "BC2AS", "BC2AT", "BC2AB", "FD", "FDx10", "VCS", "VCT", "VCB", "nFHCal", "tFHCal"})
    dd=dd
      .Define(Form("TQDC_%speak", det), triggerPeak, {Form("TQDC_%s", det)})
      .Define(Form("TQDC_%stime", det), triggerTime, {Form("TQDC_%s", det)})
      .Define(Form("TQDC_%sintegral", det), triggerIntegral, {Form("TQDC_%s", det)})
      .Define(Form("TQDC_%speakBin", det), triggerPeakBin, {Form("TQDC_%s", det)})
  ;
  for (auto &det:{"BC2AT", "BC2AB"})
    dd=dd
      .Define(Form("%samp", det), Form("%s.fAmp", det))
      .Define(Form("%stime", det), Form("%s.fTime", det));

  dd=dd
    //.Filter("abs(TQDC_FDtime-2200)<100")
    //.Filter("abs(TQDC_BC1Stime-2050)<100")
    //.Filter("abs(TQDC_BC1SpeakBin-285)<35")
    //.Filter("abs(TQDC_BC2ASpeakBin-285)<35")
    //.Filter("abs(TQDC_VCSpeakBin-285)<35")
    //.Filter("abs(TQDC_FDpeakBin-285)<35")
  ; 
  dd.Foreach([](uint evtId){if (evtId % 1000 == 0) cout << "\r" << evtId;}, {"evtId"}); // progress display 
  cout << endl;

  for (auto &cut:{"BT", "NiT", "MBT", "CCT1", "CCT2"})
  {
    const int nBins=100, nTOF400Max=100, nTOF700Max=100, nDCHMax=2500, BDcountMax=40, SIcountMax=64;
    const float HodoMax=7000, TQDC_FDMax=6000, tFHCalMax=1000, nFHCalMax=500, SIampMax=11000, BDampMax=500;

    vector <pair <vector<string>, TH1DModel>> h1Models = {
//      {{"TQDC_BC1Speak", cut},          {"", "", 0, 0, 0}},
//      {{"nFHCAL", cut},          {"", "", 0, 0, 0}},
//      {{"BDcount", cut},          {"", "", 40, 0, 40}},
    };

    for (auto& name:dd.GetDefinedColumnNames())
    {
      h1Models.push_back({{name, cut}, {"", "", 0, 0, 0}});
    }

    vector <pair <vector<string>, TH2DModel>> h2Models = {
      {{"TQDC_BC1Speak", "TQDC_BC1Sintegral", cut}, {"", "", nBins, 0, 20000, nBins, -20000, 80000}},
      {{"TQDC_BC2ASpeak", "TQDC_BC2ASintegral", cut}, {"", "", nBins, 0, 20000, nBins, -20000, 80000}},
      {{"TQDC_VCSpeak", "TQDC_VCSintegral", cut}, {"", "", nBins, 0, 20000, nBins, -20000, 80000}},
      {{"TQDC_FDpeak", "TQDC_FDintegral", cut}, {"", "", nBins, 0, 20000, nBins, -20000, 80000}},
      {{"BDmodId", "BDmodAmp", cut},    {"", "", BDcountMax, 0, BDcountMax, nBins, 0, 50}},
      {{"SImodId", "SImodAmp", cut},    {"", "", SIcountMax, 0, SIcountMax, nBins, 0, 250}},
      {{"BDmodId", "BDmodTime", cut},    {"", "", BDcountMax, 0, BDcountMax, nBins, 0, 3500}},
      {{"SImodId", "SImodTime", cut},    {"", "", SIcountMax, 0, SIcountMax, nBins, 0, 3500}},
      {{"BDcount", "nFHCal", cut},    {"", "", BDcountMax, 0, BDcountMax, nBins, 0, nFHCalMax}},
      {{"BDcount", "tFHCal", cut},    {"", "", BDcountMax, 0, BDcountMax, nBins, 0, tFHCalMax}},
      {{"BDcount", "Hodo", cut},      {"", "", BDcountMax, 0, BDcountMax, nBins, 0, HodoMax}},
      {{"BDcount", "TQDC_FDpeak", cut},      {"", "", BDcountMax, 0, BDcountMax, nBins, 0, TQDC_FDMax}},
      {{"BDcount", "Hodo", cut}, {"", "", BDcountMax, 0, BDcountMax, nBins, 0, HodoMax}},
      {{"BDcount", "tFHCal", cut}, {"", "", BDcountMax, 0, BDcountMax, nBins, 0, tFHCalMax}},
      {{"BDcount", "nFHCal", cut}, {"", "", BDcountMax, 0, BDcountMax, nBins, 0, nFHCalMax}},
      {{"BDcount", "TQDC_FDpeak", cut}, {"", "", BDcountMax, 0, BDcountMax, nBins, 0, TQDC_FDMax}},
      {{"BDamp", "Hodo", cut}, {"", "",   nBins, 0, BDampMax, nBins, 0, HodoMax}},
      {{"BDamp", "tFHCal", cut}, {"", "", nBins, 0, BDampMax, nBins, 0, tFHCalMax}},
      {{"BDamp", "nFHCal", cut}, {"", "", nBins, 0, BDampMax, nBins, 0, nFHCalMax}},
      {{"BDamp", "TQDC_FDpeak", cut}, {"", "", nBins, 0, BDampMax, nBins, 0, TQDC_FDMax}},
      {{"SIcount", "Hodo", cut}, {"", "", SIcountMax, 0, SIcountMax, nBins, 0, HodoMax}},
      {{"SIcount", "tFHCal", cut}, {"", "", SIcountMax, 0, SIcountMax, nBins, 0, tFHCalMax}},
      {{"SIcount", "nFHCal", cut}, {"", "", SIcountMax, 0, SIcountMax, nBins, 0, nFHCalMax}},
      {{"SIcount", "TQDC_FDpeak", cut}, {"", "", SIcountMax, 0, SIcountMax, nBins, 0, TQDC_FDMax}},
      {{"SIamp", "Hodo", cut}, {"", "",   nBins, 0, SIampMax, nBins, 0, HodoMax}},
      {{"SIamp", "tFHCal", cut}, {"", "", nBins, 0, SIampMax, nBins, 0, tFHCalMax}},
      {{"SIamp", "nFHCal", cut}, {"", "", nBins, 0, SIampMax, nBins, 0, nFHCalMax}},
      {{"SIamp", "TQDC_FDpeak", cut}, {"", "", BDcountMax, 0, SIampMax, nBins, 0, TQDC_FDMax}},
      {{"tFHCal", "Hodo", cut}, {"", "", nBins, 0, tFHCalMax, nBins, 0, HodoMax}},
      {{"nFHCal", "Hodo", cut}, {"", "", nBins, 0, nFHCalMax, nBins, 0, HodoMax}},
      {{"TQDC_FDpeak", "Hodo", cut},   {"", "", nBins, 0, TQDC_FDMax, nBins, 0, HodoMax}},
      {{"TQDC_FDpeak", "tFHCal", cut}, {"", "", nBins, 0, TQDC_FDMax, nBins, 0, tFHCalMax}},
      {{"TQDC_FDpeak", "nFHCal", cut}, {"", "", nBins, 0, TQDC_FDMax, nBins, 0, nFHCalMax}},
      {{"tFHCal", "nFHCal", cut}, {"", "", BDcountMax, 0, tFHCalMax, nBins, 0, nFHCalMax}},
      {{"nTOF400", "TQDC_FDpeak", cut}, {"", "", nTOF400Max, 0 , nTOF400Max, nBins, 0, TQDC_FDMax}},
      {{"nTOF400", "tFHCal", cut}, {"", "", nTOF400Max, 0 , nTOF400Max, nBins, 0, tFHCalMax}},
      {{"nTOF400", "nFHCal", cut}, {"", "", nTOF400Max, 0 , nTOF400Max, nBins, 0, nFHCalMax}},
      {{"nTOF700", "TQDC_FDpeak", cut}, {"", "", nTOF700Max, 0 , nTOF700Max, nBins, 0, TQDC_FDMax}},
      {{"nTOF700", "tFHCal", cut}, {"", "", nTOF700Max, 0 , nTOF700Max, nBins, 0, tFHCalMax}},
      {{"nTOF700", "nFHCal", cut}, {"", "", nTOF700Max, 0 , nTOF700Max, nBins, 0, nFHCalMax}},
      {{"nDCH", "TQDC_FDpeak", cut}, {"", "", nDCHMax, 0 , nDCHMax, nBins, 0, TQDC_FDMax}},
      {{"nDCH", "tFHCal", cut}, {"", "", nDCHMax, 0 , nDCHMax, nBins, 0, tFHCalMax}},
      {{"nDCH", "nFHCal", cut}, {"", "", nDCHMax, 0 , nDCHMax, nBins, 0, nFHCalMax}},
      {{"nDCH", "nTOF700", cut}, {"", "", nDCHMax, 0 , nDCHMax, nTOF700Max, 0, nTOF700Max}},
      {{"nDCH", "nTOF400", cut}, {"", "", nDCHMax, 0 , nDCHMax, nTOF400Max, 0, nTOF400Max}},
      {{"BDcount", "nTOF700", cut}, {"", "", BDcountMax, 0 , BDcountMax, nTOF700Max, 0, nTOF700Max}},
      {{"BDcount", "nTOF400", cut}, {"", "", BDcountMax, 0 , BDcountMax, nTOF400Max, 0, nTOF400Max}},
      {{"BDamp", "nTOF400", cut}, {"", "", nBins, 0 , BDampMax, nTOF400Max, 0, nTOF400Max}},
      {{"BDamp", "nTOF700", cut}, {"", "", nBins, 0 , BDampMax, nTOF700Max, 0, nTOF700Max}},
      {{"nTOF700", "nTOF400", cut}, {"", "", nTOF700Max, 0 , nTOF700Max, nTOF400Max, 0, nTOF400Max}},
    };                                                              

    saveHists(dd, h1Models, h2Models, outFile.GetDirectory(""));
  }
  outFile.Close();

//  vector<string> definedNames;
//  vector<string> toExclude={/*"scwallModPos","fhcalModPos","hodoModPos"*/};
//  for (auto& definedName:dd.GetDefinedColumnNames())
//  {
//    bool exclude=false;
//    for (auto &nameToExclude:toExclude)
//      if (definedName==nameToExclude)
//        exclude=true;
//    if (!exclude)
//      definedNames.push_back(definedName);
//  }

//  ROOT::RDF::RSnapshotOptions opts;
//  opts.fMode = "update";
//  dd.Snapshot("t",out, definedNames, opts);
}
