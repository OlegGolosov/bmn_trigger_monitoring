using namespace ROOT;
using namespace ROOT::Math;
using namespace ROOT::RDF;
using fourVector=LorentzVector<PtEtaPhiE4D<double>>;
using filteredDF=ROOT::RDF::RInterface<ROOT::Detail::RDF::RJittedFilter, void>;
using definedDF=ROOT::RDF::RInterface<ROOT::Detail::RDF::RLoopManager, void>;

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

void saveHists(filteredDF &dd, vector <pair <vector<string>, TH1DModel>> &h1Models, vector <pair <vector<string>, TH2DModel>> &h2Models, TDirectory* dir)
{
  dir->cd();
  unordered_map <string, RResultPtr<::TH1D >> hists1d;
  for (auto& model:h1Models)
  {
    auto m=model.second;
    const char *v=model.first.at(0).c_str(), *w=model.first.at(1).c_str();
    if (m.fName=="") 
      m.fName=Form("h_%s", v);
    if (m.fTitle=="") 
      m.fTitle=Form("%s;%s;nEntries", v, v);
    if (!w[0])
    {
      if(m.fNbinsX==0)
        hists1d.emplace(m.fName, dd.Histo1D(v));
      else
        hists1d.emplace(m.fName, dd.Histo1D(m, v));
    }
    else
    {
      m.fName+=Form("_%s", w);
      m.fTitle+=Form(" {%s}", w);
      if(m.fNbinsX==0)
        hists1d.emplace(m.fName, dd.Histo1D(v, w));
      else
        hists1d.emplace(m.fName, dd.Histo1D(m, v, w)); 
    }
  }
  
  unordered_map <string, RResultPtr<::TH2D >> hists2d;
  for (auto& model:h2Models)
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
  
  for (auto& hist:hists1d)
  {
    cout << hist.first << endl;
    hist.second->Write(hist.first.c_str());
  }
  
  for (auto& hist:hists2d)
  {
    cout << hist.first << endl;
    hist.second->Write();
  }

//  TCanvas c;
//  c.Divide(2,1);
//  c.cd(1);
//  hists1d.end()->second->DrawClone();
//  c.cd(2);
//  hists2d.end()->second->DrawClone("colz");
}

void unfoldTH2 (TH2 *_h2, string projectionAxis="x", int rebinX=1, int rebinY=1, int firstBin=1, int lastBin=-1, bool log=true, const char *format="\%.0f")
{
  auto h2=(TH2*)_h2->Clone();
  h2->RebinX(rebinX);
  h2->RebinY(rebinY);
  TAxis *axis;
  const char* axisTitle;
  if (projectionAxis=="x")
  {
    axis=h2->GetYaxis();
    axisTitle=h2->GetXaxis()->GetTitle();
  }
  else if (projectionAxis=="y")
  {
    axis=h2->GetXaxis();
    axisTitle=h2->GetYaxis()->GetTitle();
  }
  else 
  {
    cout << "Not valid projection axis: " << projectionAxis << endl;
    return;
  }
  if (lastBin<0) 
    lastBin=axis->GetNbins();
  auto hs=new THStack(Form("hs_%s_log%i", h2->GetName(), log), h2->GetTitle());
  auto c=new TCanvas(Form("c_%s_log%i", h2->GetName(), log), h2->GetTitle(), 800, 600);
  int npads=lastBin-firstBin+1;
  int npadsx = int (ceil (sqrt (npads)));
  int npadsy = int (ceil (1. * npads / npadsx));
  c->Divide(npadsx, npadsy);
  const char* namePattern=Form("%s-%s", format, format);
  for (int i=firstBin;i<=lastBin;i++)
  {
    TH1* h;
    float binMin=axis->GetBinLowEdge(i), binMax=axis->GetBinUpEdge(i);
    if (projectionAxis=="x")
      h=h2->ProjectionX(Form(namePattern, binMin, binMax), i, i);
    else if (projectionAxis=="y")
      h=h2->ProjectionY(Form(namePattern, binMin, binMax), i, i);
    h->SetTitle(Form("%s;%s;N_{entries}", h->GetName(), axisTitle));
    h->SetLineColor(i);
    c->cd(i);
    if(log) gPad->SetLogy();
    h->Draw();
    hs->Add(h);
  }
//  c->Write();
//  hs->Write();
  delete h2;
}
