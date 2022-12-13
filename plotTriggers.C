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

void plotTriggers (string inDigi="mpd_run_Top_6573_ev1.root", const char *out="mpd_run_Top_6573_ev1.triggerQA.root")
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

  auto dd=d
//    .Range(0,1000)
    .Define("runId",[](FairEventHeader h){return h.GetRunId();}, {"BmnEventHeader."})
    .Define("evtId","BmnEventHeader.fEventId")
    .Filter("evtId>0")
    .Define("BC1peak", triggerPeak, {"TQDC_BC1S"})
    .Define("BC2peak", triggerPeak, {"TQDC_BC2MS"})
    .Define("VCpeak", triggerPeak, {"TQDC_VCS"})
    .Define("FDpeak", triggerPeak, {"TQDC_FD"})
    .Define("nFHCALpeak", triggerPeak, {"TQDC_nFHCal"})
    .Define("BDamp", "Sum(BD.fAmp)")
    .Define("BDcount", "Sum(BD.fAmp>0)")
    .Define("BT", "BC1peak>0 && VCpeak<1000 && BC2peak>0")
    .Define("MBT", "BT && FDpeak<1000")
    .Define("CCT1", "BT && BDcount>5")
  ;
  dd.Foreach([](uint evtId){if (evtId % 1000 == 0) cout << "\r" << evtId;}, {"evtId"}); // progress display 
  cout << endl;

  for (auto &trigger:{"BT", "MBT", "CCT1"})
  {
    vector <pair <vector<string>, TH1DModel>> h1Models = {
      {{"BC1peak", trigger},          {"", "", 0, 0, 0}},
      {{"nFHCALpeak", trigger},          {"", "", 0, 0, 0}},
      {{"BDcount", trigger},          {"", "", 40, 0, 40}},
    };
    
    vector <pair <vector<string>, TH2DModel>> h2Models = {
      {{"BDcount", "nFHCALpeak", trigger},    {"", "",40, 0, 40, 1, 0, 1}},
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
