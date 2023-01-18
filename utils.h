vector <TFile*> makeFileVector (string &fileListPath)
{
  cout << "\nFile list:\n";
  vector <TFile*> files;
  if(fileListPath.rfind(".root") < fileListPath.size()) // root file
    files.push_back(new TFile(fileListPath.c_str()));
  else // file list
  {
    ifstream fileList;
    fileList.open(fileListPath);
    if(!fileList)
      printf("Error opening file: %s\n", fileListPath.c_str());
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

vector <string> buildObjectList (vector <string> &objectNames, TDirectory *folder, int depth=1)
{
  int maxDepth=10000;
  string folderPath = folder -> GetPath();
  folderPath.erase (0,folderPath.rfind(":/")+2);
  while (folderPath.find("/")==0)
    folderPath.erase (0,1);
  TList *keys = folder -> GetListOfKeys ();
  if (depth==1)
    cout << endl <<__func__ << ":\n";
  for (auto key : *keys)
  {
    string objectName = key -> GetName();
    auto object = dynamic_cast <TKey*> (key) -> ReadObj ();
    if(!object)
      continue;
    string className = object -> ClassName();
    if (!(className.find("3") < className.size()) && 
      (className.find("TH2")==0 || className.find("TH1")==0 || className.find("TProfile")==0 || className.find("TGraph")==0))
    {
      string objectPath=Form("%s/%s", folderPath.c_str(), objectName.c_str());
      if(objectPath.find("/")==0)
        objectPath.erase(0,1);
      cout << objectPath << endl;
      objectNames.push_back(objectPath);
    }
    else if (className == "TDirectoryFile" && depth < maxDepth)
      buildObjectList (objectNames, dynamic_cast <TDirectoryFile*> (object), depth + 1);
    delete object;
  }
  if (depth==1)
    cout << objectNames.size() << " objects added.\n";
  return objectNames; 
}

vector <string> filterObjectList (const vector <string> &objectNames, const string &pattern, vector<vector<string>> *captures=nullptr)
{
  if (pattern.size()==0)
    return objectNames;
  cout << endl <<__func__ << ":\n";
  regex re(pattern);
  vector <string> filteredNames;
  for (auto &name:objectNames)
  {
    smatch matches;
    if(regex_search(name, matches, regex(pattern)))
    {
      cout << name;
      filteredNames.push_back(name);
      if (captures)
      {
        vector<string> thisNameCaptures;
        cout << "\tCaptured:";
        for (uint i = 1; i < matches.size(); i++)
        {
          thisNameCaptures.push_back(matches[i].str());
          printf(" (%s)",thisNameCaptures.back().c_str());
        }
        captures->push_back(thisNameCaptures);
      }
      cout << endl;
    }
  }
  cout << filteredNames.size() << " objects selected.\n";
  return filteredNames;
}

