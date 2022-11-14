#include "utils.h"

vector <float> getRunIds(const vector<TFile*> &files);
vector <float> getRunTimes(vector<TFile*> &files);
vector <float> getRunIdsFromNames(const vector<TFile*> &files, const string &runIdPattern);
vector <string> replaceObjectNamePattern(vector <string> &objectNames, const string &pattern, const string &replacement);
void plot(vector <TFile*> &files, vector<string> &histnames, vector<float> &xAxis, const char *xAxisTitle, bool isTimeGraph, TDirectoryFile *outDir);

void plotTrends(string fileListPath="list", const char *outFilePath="trends.root", string pattern=".*GEM.*", string runIdPattern="\\d{6}")
{
  vector <TFile*> files=makeFileVector(fileListPath);
  vector <string> histNames;
  buildObjectList(histNames, files.at(0));
  vector <float> runIds;
  vector <float> runTimes;
  if (pattern.size()>0)
    histNames=filterObjectList(histNames, pattern);
  if (runIdPattern.size()>0)
  {
    runIds = getRunIdsFromNames(files, runIdPattern);
    histNames=replaceObjectNamePattern(histNames, runIdPattern, "\%06d");
  }
  else 
  {
    runIds = getRunIds(files);
    runTimes = getRunTimes(files);
  }
  auto outFile = new TFile(outFilePath, "recreate");
  plot(files, histNames, runIds, "runId", false, outFile);
  if (runTimes.size()>0)
    plot(files, histNames, runTimes, "time", true, outFile);
  outFile->Close();
}

void plot(vector <TFile*> &files, vector<string> &histNames, vector<float> &xAxis, const char *xAxisTitle, bool isTimeGraph, TDirectoryFile *outDir)
{
  printf ("\nPlotting %s trends:\n", xAxisTitle);
  outDir->mkdir(xAxisTitle);
  outDir->cd(xAxisTitle);
  for (auto histName:histNames)
  {
    cout << histName << endl;
    auto writeName=regex_replace(histName,regex("\%06d"),"");
    writeName=regex_replace(writeName,regex("/"),"_");
    auto templateHist=(TH1*)files.at(0)->Get(Form(histName.c_str(), int(xAxis.at(0)), int(xAxis.at(0))));
    auto histTitle=templateHist->GetTitle();
    string yAxisTitle=templateHist->GetXaxis()->GetTitle();
    uint commaPosition=yAxisTitle.rfind(",");
    string units;
    if (commaPosition < yAxisTitle.size())
    {
      units=yAxisTitle.substr(commaPosition, yAxisTitle.size()-commaPosition);
      yAxisTitle.erase(commaPosition, yAxisTitle.size()-commaPosition);
    }
    vector <string> moments={"mean", "RMS"};
    for (int i=0;i<moments.size();i++)
    {
      vector <float> trend;
      auto moment=moments.at(i);
      for (int f=0;f<files.size();f++)
      {
        auto file=files.at(f);
        auto h=(TH1*)file->Get(Form(histName.c_str(), int(xAxis.at(f)), int(xAxis.at(f))));
        if (!h) 
        {
          printf("\nNot found in file %s\n", file->GetName());
          continue;
        }
        if (moment=="mean") 
          trend.push_back(h->GetMean()); 
        else if (moment=="RMS") 
          trend.push_back(h->GetRMS()); 
        else 
        {
          printf ("Undefined moment: %s", moment.c_str());
          assert(0);
        }
      }
      auto g=new TGraph(files.size(), &xAxis[0], &trend[0]);
      g->SetTitle(Form("%s (%s);%s;%s %s%s", histTitle, moment.c_str(), xAxisTitle, yAxisTitle.c_str(), moment.c_str(), units.c_str()));
      g->SetName(Form("%s_%s", writeName.c_str(), moment.c_str()));
      if (isTimeGraph) 
      {
        g->GetXaxis()->SetTimeDisplay(1);
        g->GetXaxis()->SetTimeFormat("%d/%m-%H:%M:%S");
        g->GetXaxis()->SetNdivisions(507);
      }
      g->Write();
    }
  }
}

vector <float> getRunIds(const vector<TFile*> &files)
{
  cout << endl << __func__ << ":\n";
  vector <float> runIds;
  for (auto f:files)
  { 
    ulong id=0;
    auto runIdVector=(vector<ulong>*)f->Get("runId");
    if (runIdVector)
      id=runIdVector->at(0);
    cout << id << ", ";
    runIds.push_back(id);
  }
  cout << endl; 
  return runIds;
}

vector <float> getRunIdsFromNames(const vector<TFile*> &files, const string &runIdPattern)
{
  cout << endl << __func__ << ":\n";
  vector <float> runIds;
  regex re(runIdPattern);
  for (auto f:files)
  { 
    ulong id=0;
    auto runIdVector=(vector<ulong>*)f->Get("runId");
    auto l=f->GetListOfKeys();
    for (int i=0; i<l->GetSize(); i++)
    {
      string name=l->At(i)->GetName();
      smatch sm;
      if (regex_search(name, sm, re))
      {
        string sid=sm[0];
        id=stoi(sid);
        break;
      }
    }
    cout << id << ", ";
    runIds.push_back(id);
  }
  cout << endl; 
  return runIds;
}

vector <float> getRunTimes(vector<TFile*> &files)
{
  vector <float> runTimes;
  for (auto f:files)
  {
    auto runStart=((TTimeStamp*)f->Get("runStart"))->GetSec();
    auto runEnd=((TTimeStamp*)f->Get("runEnd"))->GetSec();
    float runMid=runStart+0.5*(runEnd-runStart);
//    cout << "\t" << runStart << "\t" << int(runMid) << "\t" << runEnd << endl;
    runTimes.push_back(runMid);
  }
  int nRuns=runTimes.size();
  for (int i=0;i<nRuns-1;i++)
    if (runTimes.at(i+1)-runTimes.at(i) > 31536000) // 1 year
      runTimes.at(i)=runTimes.at(i+1);
  return runTimes;
}

vector <string> replaceObjectNamePattern(vector <string> &objectNames, const string &pattern, const string &replacement)
{
  cout << endl <<__func__ << ":\n";
  vector <string> newObjectNames;
  regex re(pattern);
  for (auto &name:objectNames)
  {
    string newName=regex_replace(name,re,replacement);
    newObjectNames.push_back(newName);
    cout << name << " -> " << newName << endl; 
  }
  return newObjectNames;
}
