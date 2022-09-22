vector <TFile*> makeFileVector (string fileListPath);
vector <string> buildObjectList (TDirectory *folder, int depth=1);
vector <float> getRunIds(vector<TFile*> files);
vector <float> getRunTimes(vector<TFile*> files);
void plot(vector <TFile*> files, vector<string> histnames, vector<float> xAxis, const char *xAxisTitle, bool isTime, TDirectoryFile *outDir);

void plotTrends(string fileListPath="list", const char *outFilePath="trends.root")
{
  vector <TFile*> files=makeFileVector(fileListPath);
  vector <string> histNames=buildObjectList(files.at(0));
  vector <float> runIds = getRunIds(files);
  vector <float> runTimes = getRunTimes(files);
  auto outFile = new TFile(outFilePath, "recreate");
  plot(files, histNames, runIds, "runId", false, outFile);
  plot(files, histNames, runTimes, "time", true, outFile);
  outFile->Close();
}

void plot(vector <TFile*> files, vector<string> histNames, vector<float> xAxis, const char *xAxisTitle, bool isTime, TDirectoryFile *outDir)
{
  printf ("\nPlotting %s trends:\n", xAxisTitle);
  outDir->mkdir(xAxisTitle);
  outDir->cd(xAxisTitle);
  for (auto histName:histNames)
  {
    cout << histName << endl;
    auto templateHist=(TH1*)files.at(0)->Get(histName.c_str());
    const char *histTitle=templateHist->GetTitle();
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
      for (auto file:files)
      {
        auto h=(TH1*)file->Get(histName.c_str());
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
      g->SetName(Form("%s_%s", histName.c_str(), moment.c_str()));
      if (isTime) 
      {
        g->GetXaxis()->SetTimeDisplay(1);
        g->GetXaxis()->SetTimeFormat("%d/%m-%H:%M:%S");
        g->GetXaxis()->SetNdivisions(507);
      }
      g->Write();
    }
  }
}

vector <TFile*> makeFileVector (string fileListPath)
{
  cout << "\nFile list:\n";
  vector <TFile*> files;
  if(fileListPath.rfind(".root") == fileListPath.size()-5) // root file
    files.push_back(new TFile(fileListPath.c_str()));
  else // file list
  {
    ifstream fileList;
    fileList.open(fileListPath);
    if(!fileList)
      printf("File %s does not exist\n", fileListPath.c_str());
    string name;
    while(getline(fileList, name))
    {
      cout << name << endl;
      files.push_back(new TFile(name.c_str()));
    }
    fileList.close();
  }
  return files;
}

vector <float> getRunIds(vector<TFile*> files)
{
  vector <float> runIds;
  for (auto f:files)
  { 
    auto runIdVector=(vector<ulong>*)f->Get("runId");
    runIds.push_back(runIdVector->at(0));
  }
  return runIds;
}

vector <float> getRunTimes(vector<TFile*> files)
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

vector <string> buildObjectList (TDirectory *folder, int depth)
{
  int maxDepth=10000;
  vector<string> excludedFolders;
  vector <string> objectNames;
  string folderPath = folder -> GetPath();
  folderPath.erase (0,folderPath.rfind(":/")+2);
  while (folderPath.find("/")==0)
    folderPath.erase (0,1);
  TList *keys = folder -> GetListOfKeys ();
  cout << "\nBuilding object list:\n";
  for (auto key : *keys)
  {
    string objectName = key -> GetName();
    if (objectName.find("run")==0)
      continue;
    auto object = dynamic_cast <TKey*> (key) -> ReadObj ();
    string className = object -> ClassName();
    if (!(className.find("3") < className.size()) && 
      (className.find("TH")==0 || className.find("TProfile")==0 || className.find("Graph")==0))
    {
      string objectPath=Form("%s/%s", folderPath.c_str(), objectName.c_str());
      if(objectPath.find("/")==0)
        objectPath.erase(0,1);
      cout << objectPath << endl;
      objectNames.push_back(objectPath);
    }
    else if (className == "TDirectoryFile" && depth < maxDepth)
    {
      bool skipFolder = false;
      for (auto exFolder : excludedFolders)
        if (objectName == exFolder) skipFolder = true;  
      if (!skipFolder) buildObjectList (dynamic_cast <TDirectoryFile*> (object), depth + 1);
    }
    delete object;
  }
  return objectNames; 
}
