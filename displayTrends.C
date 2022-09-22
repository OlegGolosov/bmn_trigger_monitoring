struct rangeLine
{
  float y=0;
  int color=2;
  int width=2;
  int style=1;
};

struct histSettings
{
  const char *name;
  int color;
  vector<rangeLine> lines={};
  int markerStyle=0;
  int markerSize=0;
  int lineStyle=1;
  int lineWidth=3;
  const char* drawOption="apl"; 
};

void displayTrends(const char *trendFilePath="trends.root", const char *xAxisName="time", const char *currentFilePath="current.root")
{
  int canvasWidth=1500;
  int canvasHeight=800;
  int nDivisions=705;
  vector <histSettings> hSettings=
  {
    {"h_Amp_BC1_mean", 1, {{27, 2, 3, 2}, {32, 3, 3, 2}, {25, 4, 3, 1}, {35, 5, 3, 1}}},
    {"h_Amp_BC1_RMS", 2, {{2.7, 2, 3, 2}, {3.2, 3, 3, 2}, {2.5, 4, 3, 1}, {3.5, 5, 3, 1}}},
  };
  TFile trendFile(trendFilePath, "read");
  TFile currentFile(currentFilePath, "read");
  TCanvas *c1 = new TCanvas("trigger_trends","Trigger trends",50,50,canvasWidth,canvasHeight);
  int nPads = hSettings.size();
  int nPadsx = int (ceil (sqrt (nPads)));
  int nPadsy = int (ceil (1. * nPads / nPadsx));
  c1 -> Divide (nPadsx, nPadsy);
  for(int i = 0; i < nPads; i++)
  {
    auto &setting=hSettings.at(i);
    c1->cd(i+1); 
    gPad->SetGrid();
    auto gTrend=(TGraph*)trendFile.Get(Form("%s/%s", xAxisName, setting.name));
    gTrend->Draw(setting.drawOption);
    gTrend->SetLineWidth(setting.lineWidth);
    gTrend->SetLineStyle(setting.lineStyle);
    gTrend->SetLineColor(setting.color);
    gTrend->SetMarkerColor(setting.color);
    gTrend->SetMarkerSize(setting.markerSize);
    gTrend->SetMarkerStyle(setting.markerStyle);
    gTrend->GetXaxis()->SetNdivisions(nDivisions);
    gTrend->GetXaxis()->SetRangeUser(gTrend->GetPointX(0), gTrend->GetXaxis()->GetXmax());
    auto gCurrent=(TGraph*)currentFile.Get(Form("%s/%s", xAxisName, setting.name));
    gCurrent->Draw("same p");
    gCurrent->SetMarkerColor(setting.color);
    gCurrent->SetMarkerSize(1.5);
    gCurrent->SetMarkerStyle(kOpenStar);
    double lineX[2]={gTrend->GetXaxis()->GetXmin(), gTrend->GetXaxis()->GetXmax()};
    for (int j=0;j<setting.lines.size();j++)
    {
      auto &ls=setting.lines.at(j); 
      double lineY[2]={ls.y,ls.y};
      auto line=new TGraph(2, lineX, lineY);
      line->SetLineColor(ls.color);
      line->SetLineWidth(ls.width);
      line->SetLineStyle(ls.style);
      line->Draw("same l");
    }
  }
}
