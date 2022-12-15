#include "qaUtils.h"

int triggerPeak (TClonesArray digis)
{
  auto digit=(BmnTrigWaveDigit*)digis.At(0);
  int peak=0;
  if(digit) peak=digit->GetPeak();
  return peak;
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

void plotTriggers (string inDigi="mpd_run_Top_6723_ev0_p1.root", const char *out="mpd_run_Top_6723_ev0_p1.triggerQA.root")
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
//    .Range(0,1000)
    .Define("runId",[](FairEventHeader h){return h.GetRunId();}, {"BmnEventHeader."})
    .Define("evtId","BmnEventHeader.fEventId")
    .Define("ALL", "evtId>0")
    .Filter("ALL")
    .Define("BDmodId", "BD.fMod")
    .Define("BDmodTime", "BD.fTime")
    .Define("BDmodAmp", "BD.fAmp")
    .Define("BDgoodHit", "abs(BDmodTime-400)<100")
    .Define("BDcount", "Sum(BDgoodHit)")
    .Define("BDamp", "Sum(BDmodAmp*BDgoodHit)")
    .Define("SImodId", "SI.fMod")
    .Define("SImodTime", "SI.fTime")
    .Define("SImodAmp", "SI.fAmp")
    .Define("SIgoodHit", "abs(SImodTime-450)<50")
    .Define("SIcount", "Sum(SIgoodHit)")
    .Define("SIamp", "Sum(SImodAmp*SIgoodHit)")
    .Define("BDSIcount", "BDcount+SIcount")
    .Define("tFHCal", "Sum(FHCalDigi.fSignal)")
    .Define("FHCalDigiX", FHCalDigiX, {"FHCalDigi"})
    .Define("nFHCal", "Sum(FHCalDigi.fSignal*FHCalDigiX<2)")
    .Define("Hodo", "Sum(HodoDigi.fSignal)")
//    .Define("BT", "BC1Speak>0 && VCpeak<1000 && BC2Speak>0")
//    .Define("MBT", "BT && FDpeak<1000")
//    .Define("CCT1", "BT && BDcount>5")
//    .Define("CCT2", "BT && BDcount>5")
  ;
  for (auto &det:{"BC1S", "BC1T", "BC1B", "BC2AS", "BC2AT", "BC2AB", "FD", "FDx10", "FD1S", "FD1T", "FD1B", "VCS", "VCT", "VCB", "nFHCal", "tFHCal"})
    dd=dd
      .Define(Form("TQDC_%speak", det), triggerPeak, {Form("TQDC_%s", det)})
      .Define(Form("TQDC_%stime", det), Form("TQDC_%s.fTime", det));

  for (auto &det:{"BC2AT", "BC2AB"})
    dd=dd
      .Define(Form("%samp", det), Form("%s.fAmp", det))
      .Define(Form("%stime", det), Form("%s.fTime", det));

  dd.Foreach([](uint evtId){if (evtId % 1000 == 0) cout << "\r" << evtId;}, {"evtId"}); // progress display 
  cout << endl;

  for (auto &trigger:{"ALL"/*,"BT", "MBT", "CCT1", "CCT2"*/})
  {
    vector <pair <vector<string>, TH1DModel>> h1Models = {
//      {{"TQDC_BC1Speak", trigger},          {"", "", 0, 0, 0}},
//      {{"nFHCAL", trigger},          {"", "", 0, 0, 0}},
//      {{"BDcount", trigger},          {"", "", 40, 0, 40}},
    };

    for (auto& name:dd.GetDefinedColumnNames())
      h1Models.push_back({{name, trigger}, {"", "", 0, 0, 0}});

    vector <pair <vector<string>, TH2DModel>> h2Models = {
      {{"BDcount", "nFHCal", trigger},    {"", "", 40, 0, 40, 100, 0, 200}},
      {{"BDcount", "tFHCal", trigger},    {"", "", 40, 0, 40, 100, 0, 1000}},
      {{"BDcount", "Hodo", trigger},      {"", "", 40, 0, 40, 100, 0, 10000}},
      {{"BDcount", "TQDC_FDpeak", trigger},      {"", "", 40, 0, 40, 100, 0, 6000}},
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
