#include "utils.h"
#include "qaUtils.h"
#include "histSettings.h"

int canvasWidth=1500;
int canvasHeight=800;
int nDivisions=705;
int currentMarkerStyle=kOpenStar;
float currentMarkerSize=1.5;

TFile *trendFile, *currentFile;
vector <string> histNames;

int thisOrLast(vector<int> v, int i) {return v.size()>i ? v.at(i) : v.back();}
float thisOrLast(vector<float> v, int i) {return v.size()>i ? v.at(i) : v.back();}
TCanvas* drawCanvas(const string &histSettingName);
template <typename T, typename MT>
void drawPad(histSettings setting, vector<string> filteredNames, vector<vector <string>> captures, TFile *trendFile, TFile *currentFile);


void display(const char *trendFilePath="qa.root", const char *currentFilePath="", const char *outputPath="display.root")
{
  gStyle->SetOptStat(111111);
  trendFile=new TFile(trendFilePath, "read");
  currentFile=new TFile(currentFilePath, "read");
  auto dc = new TDialogCanvas("canvases", "canvases", 50, 50, canvasWidth, canvasHeight);
  buildObjectList(histNames, trendFile);
  float xStep=0.1, yStep=0.1;
  int i=0, j=0;
  for (auto &setting:histSettingsMap)
  {
    const char *name=setting.first.c_str();
    if (xStep*(i+1)>1)
    {
      i=0;
      j++;
    }
//    printf("%s: %d %d %.1f %.1f %.1f %.1f\n", name, i, j, xStep*i, yStep*j, xStep*(i+1), yStep*(j+1));
    auto b=new TButton(name, Form("drawCanvas(\"%s\")", name), xStep*i, 1-yStep*(j+1), xStep*(i+1), 1-yStep*j); 
    b->SetTextSize(0.2);
    b->Draw();
    i++;
  }

  TFile f(outputPath, "recreate");
  for (auto &name:{"BC_time_trends", "BC_runId_trends"})
    drawCanvas(name)->Write();
  f.Close();
}

TCanvas* drawCanvas(const string &histSettingName)
{
  auto hSettings=histSettingsMap.at(histSettingName);
  const char* canvasName=histSettingName.c_str();
  const char* canvasTitle=histSettingName.c_str();
  auto c = new TCanvas(canvasName, canvasTitle, 50, 50, canvasWidth, canvasHeight);
  int nPads = hSettings.size();
  int nPadsx = int (ceil (sqrt (nPads)));
  int nPadsy = int (ceil (1. * nPads / nPadsx));
  c -> Divide (nPadsx, nPadsy);
  for(int i = 0; i < nPads; i++)
  {
    auto &setting=hSettings.at(i);
    c->cd(i+1); 
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
    {
      if(setting.drawOption.find("unfold")<setting.drawOption.size())
      {
        string axis="x";
        if(setting.drawOption.find("y")<setting.drawOption.size())
          axis="y";
        unfoldTH2(dynamic_cast<TH2*>(testObj), axis); 
      }
      else
        dynamic_cast<TH2*>(testObj)->Draw(setting.drawOption.c_str());
//        drawPad<TH2,THStack>(setting, filteredNames, captures, trendFile, currentFile);
    }
    else if(dynamic_cast<TH1*>(testObj))
      drawPad<TH1,THStack>(setting, filteredNames, captures, trendFile, currentFile); 
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
    if(setting.log.find("x")<setting.log.size()) 
      gPad->SetLogx();
    if(setting.log.find("y")<setting.log.size()) 
      gPad->SetLogy();
    if(setting.log.find("z")<setting.log.size()) 
      gPad->SetLogz();
  }
  return c; 
}

template <typename T, typename MT>
void drawPad(histSettings setting, vector<string> filteredNames, vector<vector <string>> captures, TFile *trendFile, TFile *currentFile)
{
  int nHists=filteredNames.size();
  auto leg=new TLegend(0.63,0.9-0.07*nHists,0.78,0.9);
  leg->SetFillColor(0);
  auto firstObj=dynamic_cast<T*>(trendFile->Get(filteredNames.at(0).c_str()));
  auto multi=new MT(setting.name.c_str(), Form("%s;%s;%s", setting.name.c_str(), firstObj->GetXaxis()->GetTitle(), firstObj->GetYaxis()->GetTitle()));
  for (int j=0; j<nHists;j++)
  {
    string name=filteredNames.at(j);
    float lineWidth=thisOrLast(setting.lineWidths, j);
    int lineStyle=thisOrLast(setting.lineStyles, j);
    float markerSize=thisOrLast(setting.markerSizes, j);
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
    //multi->Add(trend, "sames");
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
      multi->Add(current, "sames");
    }
  }
  multi->Draw(setting.drawOption.c_str());
  string xAxisTitle=multi->GetXaxis()->GetTitle();
  if(xAxisTitle=="time")
  {
    multi->GetXaxis()->SetTimeDisplay(1);
    multi->GetXaxis()->SetTimeFormat("%d/%m-%H:%M:%S");
    multi->GetXaxis()->SetNdivisions(507);
  }
  if(filteredNames.size()>1)
    leg->Draw();
}
