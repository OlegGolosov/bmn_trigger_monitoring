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
  vector<int> colors;
  vector<rangeLine> lines={};
  vector<int> markerStyles={0};
  vector<int> markerSizes={0};
  vector<int> lineStyles={1,2,3,4};
  vector<int> lineWidths={3};
  string drawOption="apl"; 
};

int thisOrLast(vector<int> v, int i) {return v.size()>i ? v.at(i) : v.back();}

void display(const char *trendFilePath="~/desktop/bman/data/run8/raw/qa/trends.root", const char *currentFilePath="~/desktop/bman/data/run8/raw/qa/current.root")
{
  int canvasWidth=1500;
  int canvasHeight=800;
  int nDivisions=705;
  int currentMarkerStyle=kOpenStar;
  float currentMarkerSize=1.5;
  const char* canvasName="trigger_trends";
  const char* canvasTitle="Trigger trends";

  vector <histSettings> hSettings=
  {
    {"time/h_Amp_(BC.*)_(RMS|mean)", {1,2,3,4,5,6,7,8}, {{27, 2, 3, 2}, {32, 3, 3, 2}, {25, 4, 3, 1}, {35, 5, 3, 1}}},
    {"time/h_Amp_BC1_RMS", {2}, {{2.7, 2, 3, 2}, {3.2, 3, 3, 2}, {2.5, 4, 3, 1}, {3.5, 5, 3, 1}}},
  };
  TFile trendFile(trendFilePath, "read");
  TFile currentFile(currentFilePath, "read");
  vector <string> histNames;
  buildObjectList(histNames, &trendFile);
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
    auto mg=new TMultiGraph(setting.name.c_str(), setting.name.c_str());
    auto leg=new TLegend();
    for (int j=0; j<filteredNames.size();j++)
    {
      string name=filteredNames.at(j);
      int lineWidth=thisOrLast(setting.lineWidths, j);
      int lineStyle=thisOrLast(setting.lineStyles, j);
      int markerSize=thisOrLast(setting.markerSizes, j);
      int markerStyle=thisOrLast(setting.markerStyles, j);
      int color=thisOrLast(setting.colors, j);
      auto gTrend=(TGraph*)trendFile.Get(name.c_str());
      gTrend->SetLineWidth(lineWidth);
      gTrend->SetLineStyle(lineStyle);
      gTrend->SetLineColor(color);
      gTrend->SetMarkerColor(color);
      gTrend->SetMarkerSize(markerSize);
      gTrend->SetMarkerStyle(markerStyle);
      gTrend->GetXaxis()->SetNdivisions(nDivisions);
      gTrend->GetXaxis()->SetRangeUser(gTrend->GetPointX(0), gTrend->GetXaxis()->GetXmax());
      gTrend->SetDrawOption(setting.drawOption.c_str());
      mg->Add(gTrend);
      string entryLabel;
      for (auto &cap:captures.at(j))
      {
        entryLabel+=cap;
        entryLabel+=" ";
      }
      leg->AddEntry(gTrend, entryLabel.c_str());
      if(currentFilePath[0])
      {
        auto gCurrent=(TGraph*)currentFile.Get(name.c_str());
        gCurrent->SetDrawOption("p");
        gCurrent->SetMarkerColor(color);
        gCurrent->SetMarkerSize(currentMarkerSize);
        gCurrent->SetMarkerStyle(currentMarkerStyle);
        mg->Add(gCurrent);
      }
    }
    mg->Draw("apl");
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
}
