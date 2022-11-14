using namespace ROOT;
using namespace ROOT::Math;
using namespace ROOT::RDF;

TChain* makeChain(string& filename, const char* treename) {
  cout << "Adding files to chain:" << endl;
  TChain *chain = new TChain(treename);
  if (filename.rfind(".root") < filename.size())
    chain->Add(filename.data());
  else {
    TFileCollection fc("fc", "", filename.c_str());
    chain->AddFileInfoList((TCollection*)fc.GetList());
  }
  chain->ls();
  return chain;
}

void addHist1d (unordered_map <string, RResultPtr<::TH1D >> &hists1d, pair <vector<string>, TH1DModel> &model)
{
  auto m=model.second;
  const char *v=model.first.at(0).c_str(), *w=model.first.at(1).c_str();
  if (m.fName=="") 
    m.fName=Form("h_%s", v);
  if (m.fTitle=="") 
    m.fTitle=Form("%s;%s;nEntries", v, v);
  if (!w[0])
    hists1d.emplace(m.fName, dd.Histo1D(m, v)); 
  else
  {
    m.fName+=Form("_%s", w);
    m.fTitle+=Form(" {%s}", w);
    hists1d.emplace(m.fName, dd.Histo1D(m, v, w)); 
  }
}
  
void addHist2d (unordered_map <string, RResultPtr<::TH2D >> &hists2d, pair <vector<string>, TH1DModel> &model)
{
  auto m=model.second;
  const char *v1=model.first.at(0).c_str(), *v2=model.first.at(1).c_str(), *w=model.first.at(2).c_str();
  if (m.fName=="") 
    m.fName=Form("h2_%s_%s", v1, v2);
  if (m.fTitle=="") 
    m.fTitle=Form("%s:%s;%s;%s;nEntries", v2, v1, v1, v2);
  if (!w[0])
    hists2d.emplace(m.fName, dd.Histo2D(m, v1, v2));
  else
  {
    m.fName+=Form("_%s", w);
    m.fTitle+=Form(" {%s}", w);
    hists2d.emplace(m.fName, dd.Histo2D(m, v1, v2, w));
  }
}

RVec<float> triggerPeak (TClonesArray digis)
{
  RVec<float>peaks; 
  for(auto d:digis)
    peaks.push_back(((BmnTrigWaveDigit*)d)->GetPeak());
  return peaks;
}

RVec<float> triggerIntegral (TClonesArray digis)
{
  RVec<float>peaks; 
  for(auto d:digis)
    peaks.push_back(((BmnTrigWaveDigit*)d)->GetIntegral());
  return peaks;
}

void plotTriggers (string inDigi="data/run8/sim.root", const char *out="data/run8/tree.root")
{
  TChain *chainDigi=makeChain(inDigi, "bmndata");
  cout << "Digi: " << chainDigi->GetEntries() << " events\n";
  ROOT::RDataFrame d(*chainDigi);

  // read first Run Header if present
  DigiRunHeader* digiHeader = (DigiRunHeader*) chainDigi->GetFile()->Get("DigiRunHeader");
  if (!digiHeader)
  {
    cout << "No digi header!!! Aborting!!!\n";
    return;
  }
  digiHeader->Dump();

  auto dd=d
//    .Range(0,1000)
    .Define("runId",[](FairEventHeader h){return h.GetRunId();}, {"BmnEventHeader."})
    .Define("evtId","BmnEventHeader.fEventId")
    .Define("BC1times", "TQDC_BC1.fTime")
    .Define("BC1amps", triggerPeaks, {"TQDC_BC1"})
    .Define("BC2times", "TQDC_BC2.fTime")
    .Define("BC2amps", triggerPeaks, {"TQDC_BC2"})
    .Define("VCtimes", "TQDC_VETO.fTime")
    .Define("VCamps", triggerPeaks, {"TQDC_VETO"})
    .Define("BTtimes", "TQDC_BT.fTime")
    .Define("BTamps", triggerPeaks, {"TQDC_BT"})
    .Define("FDtimes", "TQDC_FD.fTime")
    .Define("FDamps", triggerPeaks, {"TQDC_FD"})
    .Define("bdModAmp", "BdDigit.fAmp")
    .Define("bdModTime", "BdDigit.fTime")
    .Define("bdGoodHit", "bdModTime>0")
    .Define("bdNch", "Sum(bdGoodHit)")
    .Define("bdSumAmp", "Sum(bdGoodHit*bdModAmp)")
    .Define("simdModAmp", "SiMDDigit.fAmp")
    .Define("simdModTime", "SiMDDigit.fTime")
    .Define("simdGoodHit", "simdModTime>0")
    .Define("simdNch", "Sum(simdGoodHit)")
    .Define("simdSumAmp", "Sum(simdGoodHit*simdModAmp)")
    .Define("bdSimdNch", "bdNch+simdNch")
    .Define("bdSimdSumAmp", "bdSumAmp+simdSumAmp")
    .Define("bdAmpSum", "Sum(bdModAmp*bdGoodHit)")
    .Define("simdAmpSum", "Sum(simdModAmp*simdGoodHit)")
    .Define("bdSimdAmpSum", "bdAmpSum+simdAmpSum")
//    .Define("nHitsTof400", "")
//    .Define("nHitsTof700", "")
//    .Define("nHitsTof", "nHitsTof400+nHitsTof700")
//    .Define("hodoModId", "")
//    .Define("hodoModSide", "")
    .Define("hodoModAmp", "HodoDigi.fAmpl")
    .Define("hodoSumAmp", "Sum(hodoModAmp)")
//    .Define("fhcalModX", "vector<float> x;for(auto &d:FHCalDigi) x.push_back(HodoDigi.GetX()); return x;")
//    .Define("fhcalModY", "vector<float> y;for(auto &d:FHCalDigi) y.push_back(HodoDigi.GetY()); return y;")
    .Define("fhcalModAmp", "FHCalDigi.fAmpl")
    .Define("fhcalSumAmp", "Sum(fhcalModAmp)")
//    .Define("MBT", "")
//    .Define("CCT1", "")
//    .Define("CCT2", "")
//    .Define("NIT", "")
  ;
  dd.Foreach([](uint evtId){if (evtId % 1000 == 0) cout << "\r" << evtId;}, {"evtId"}); // progress display 
  cout << endl;
  
  vector<string> triggers={""/*"MBT","CCT1","CCT2","NIT"*/};

  vector <pair <vector<string>, TH1DModel>> h1Models = {
    {{"bdNch", ""},               {"", "Fired channels (BD);N_{channels};N_{events}",100,0,100}},
    {{"simdNch", ""},             {"", "Fired channels (SiMD);N_{channels};N_{events}",100,0,100}},
    {{"bdSimdNch", ""},           {"", "Fired channels (BD+SiMD);N_{channels};N_{events}",100,0,100}},
    {{"bdAmpSum", ""},            {"", "Amplitude sum (BD);#sum amp;N_{events}",100,0,2200}},
    {{"simdAmpSum", ""},          {"", "Amplitude sum (SiMD);#sum amp;N_{events}",100,0,2200}},
    {{"bdSimdAmpSum", ""},        {"", "Amplitude sum (BD+SiMD);#sum amp;N_{events}",200,0,4400}},
  };
  
  vector <pair <vector<string>, TH2DModel>> h2Models = {
    {{"bdNch", "hodoSumAmp", ""},      {"", "Hodo amplitude sum vs fired channels in BD;N_{channels};amp",50,0,50,110,0,110}},
    {{"bdSumAmp", "hodoSumAmp", ""},      {"", "Hodo amplitude sum vs BD amplitude sum;N_{channels};amp",50,0,50,110,0,110}},
  };
  
  vector<ulong> runId={digiHeader->GetRunId()};
  auto runStart=digiHeader->GetRunStartTime();
  auto runEnd=digiHeader->GetRunEndTime();

  unordered_map <string, RResultPtr<::TH1D >> hists1d;
  unordered_map <string, RResultPtr<::TH2D >> hists2d;
  for(auto &trigger:triggers)
  {
    for (auto &model:h1Models)
    {
      model.first.at(1)=trigger;
      addHist1d(hists1d, model);
    }
    for (auto &model:h2Models)
    {
      model.first.at(2)=trigger;
      addHist2d(hists2d, model);
    }
  }
  
  TFile fOut(out,"recreate");
  f.WriteObject(digiHeader, "digiHeader"); 
  f.WriteObject(&runId, "runId"); 
  f.WriteObject(&runStart, "runStart"); 
  f.WriteObject(&runEnd, "runEnd"); 
  for (auto& hist:hists1d)
  {
    cout << hist.first << endl;
    hist.second->Write();
  }
  for (auto& hist:hists2d)
  {
    cout << hist.first << endl;
    hist.second->Write();
  }
  fOut.Close();

//  hists.back()->DrawClone();
//  hists2d.back()->DrawClone("colz");

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
//  dd.Snapshot("t",out, definedNames);
}
