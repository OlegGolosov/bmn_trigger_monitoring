#include "utils.h"

struct rangeLine
{
  float y=0;
  int color=2;
  int width=2;
  int style=1;
};

struct histSettings
{
  string name;
  string drawOption="nostack"; 
  vector<int> colors={1,2,3,4,5,6,7,8};
  vector<rangeLine> lines={};
  vector<int> markerStyles={0};
  vector<int> markerSizes={0};
  vector<int> lineStyles={1,2,3,4};
  vector<int> lineWidths={3};
};

int thisOrLast(vector<int> v, int i) {return v.size()>i ? v.at(i) : v.back();}
template <typename T, typename MT>
void drawPad(histSettings setting, vector<string> filteredNames, vector<vector <string>> captures, TFile *trendFile, TFile *currentFile);

int canvasWidth=1500;
int canvasHeight=800;
int nDivisions=705;
int currentMarkerStyle=kOpenStar;
float currentMarkerSize=1.5;

void display(const char *trendFilePath="trends.root", const char *currentFilePath="current.root", const char* canvasName="triggerQA", const char* canvasTitle="Trigger QA")
{
  vector <histSettings> hSettings=
  {
    {"time/h_Amp_(BC.*)_(RMS|mean)", "apl", {1,2,3,4,5,6,7,8}, {{27, 2, 3, 2}, {32, 3, 3, 2}, {25, 4, 3, 1}, {35, 5, 3, 1}}},
    {"time/h_Amp_BC1_RMS", "apl", {2}, {{2.7, 2, 3, 2}, {3.2, 3, 3, 2}, {2.5, 4, 3, 1}, {3.5, 5, 3, 1}}},
//  {"h_BDcount_(.*)", "nostack", {1,2,3,4,5}, {{2.7, 2, 3, 2}}},
//  {"h_BC1peak_(.*)", "nostack", {1,2,3,4,5}, {{2.7, 2, 3, 2}}},
//  {"h2_BDcount_nFHCALpeak_BT", "colz"},
//  {"h2_BDcount_nFHCALpeak_MBT", "colz"},
  };

  auto trendFile=new TFile(trendFilePath, "read");
  auto currentFile=new TFile(currentFilePath, "read");
  vector <string> histNames;
  buildObjectList(histNames, trendFile);
  TCanvas *c1 = new TCanvas(canvasName, canvasTitle, 50, 50, canvasWidth, canvasHeight);
  int nPads = hSettings.size();
  int nPadsx = int (ceil (sqrt (nPads)));
  int nPadsy = int (ceil (1. * nPads / nPadsx));
  c1 -> Divide (nPadsx, nPadsy);
  for(int i = 0; i < nPads; i++)
  {
    auto &setting=hSettings.at(i);
    c1->cd(i+1); 
    gPad->SetGrid();
    cout << endl << setting.name << ": ";
    vector <vector <string>> captures;
    vector <string> filteredNames{filterObjectList(histNames, setting.name, &captures)};
    if(filteredNames.size()==0)
      continue;
    auto testObj=trendFile->Get(filteredNames.at(0).c_str());
    if(dynamic_cast<TGraph*>(testObj))
      drawPad<TGraph,TMultiGraph>(setting, filteredNames, captures, trendFile, currentFile); 
    else if(dynamic_cast<TH2*>(testObj))
      drawPad<TH2,THStack>(setting, filteredNames, captures, trendFile, currentFile); 
    else if(dynamic_cast<TH1*>(testObj))
      drawPad<TH1,THStack>(setting, filteredNames, captures, trendFile, currentFile); 
  }
}

template <typename T, typename MT>
void drawPad(histSettings setting, vector<string> filteredNames, vector<vector <string>> captures, TFile *trendFile, TFile *currentFile)
{
  auto leg=new TLegend();
  auto multi=new MT(setting.name.c_str(), setting.name.c_str());
  for (int j=0; j<filteredNames.size();j++)
  {
    string name=filteredNames.at(j);
    int lineWidth=thisOrLast(setting.lineWidths, j);
    int lineStyle=thisOrLast(setting.lineStyles, j);
    int markerSize=thisOrLast(setting.markerSizes, j);
    int markerStyle=thisOrLast(setting.markerStyles, j);
    int color=thisOrLast(setting.colors, j);
    auto trend=dynamic_cast<T*>(trendFile->Get(name.c_str()));
    if(!trend)
    {
      printf("Error in %s: class differs from the previous one! The object will not be drawn!\n", name.c_str());
      continue; 
    }
    trend->SetLineWidth(lineWidth);
    trend->SetLineStyle(lineStyle);
    trend->SetLineColor(color);
    trend->SetMarkerColor(color);
    trend->SetMarkerSize(markerSize);
    trend->SetMarkerStyle(markerStyle);
    trend->GetXaxis()->SetNdivisions(nDivisions);
    //trend->GetXaxis()->SetRangeUser(trend->GetPointX(0), trend->GetXaxis()->GetXmax());
    //trend->SetDrawOption(setting.drawOption.c_str());
    multi->Add(trend);
    string entryLabel;
    for (auto &cap:captures.at(j))
    {
      entryLabel+=cap;
      entryLabel+=" ";
    }
    leg->AddEntry(trend, entryLabel.c_str());
    auto currentObj=currentFile->Get(name.c_str());
    if(currentObj)
    {
      auto current=dynamic_cast<T*>(currentObj);
      current->SetDrawOption("p");
      current->SetMarkerColor(color);
      current->SetMarkerSize(currentMarkerSize);
      current->SetMarkerStyle(currentMarkerStyle);
      multi->Add(current);
    }
  }
  multi->Draw(setting.drawOption.c_str());
  if(filteredNames.size()>1)
    leg->Draw();
  double lineX[2]={gPad->GetUxmin(), gPad->GetUxmax()};
  for (int j=0;j<setting.lines.size();j++)
  {
    auto &ls=setting.lines.at(j); 
    double lineY[2]={ls.y,ls.y};
    auto line=new TGraph(2, lineX, lineY);
    line->SetLineColor(ls.color);
    line->SetLineWidth(ls.width);
    line->SetLineStyle(ls.style);
    line->Draw("same");
  }
}
