const int nDataBlockCounterNbr = 256;
const int nCharArrayLength     = 1024;
const int nTextDataLength      = 4096;
const int nTimeDataLength      = 19;
const int offset             = 81;

vector <TString> sScalerNamesBMN = {"BC1", "BC2", "BC1low", "VC", "FD", "nZDC", "NBDgtL1", "NBDgtH1", "NSiDgtL2", "NSiDgtH2", "BT", "MBT1", "NIT1", "pCCT1", "pCCT2", "CCT1", "CCT2", "BTbusy", "DAQbusy", "spillGate", "endOfSpillGate", "MBT", "NIT", "pBT", "CAENtriggers"}; 

vector <TString> sScalerNamesSRC = { "BC1low", "BC1", "BC2", "T0", "VC", "BC3", "BC4", "BC5", "Cal1", "Cal2", "Neutron",
				  "DAQ_Busy", "BT", "GBT", "Fragment", "SRC_OR", "ARM_OR", "ARM_AND", "SRC", "Recoil", "spillNo" };

enum eSetup
{
  kBMN=1,
  kSRC=2
};

typedef struct {
  int  iScalerCount[nDataBlockCounterNbr];
  char cTextData[nTextDataLength];
} Spill_Record;

void t0u_scalers_to_tree(int firstSpill = 0, int lastSpill = -1, const char *inputFilePath="BMN_TrgMgr22.SSbin", const char *outputFilePath="BMN_TrgMgr22.root", int setup=kBMN, bool verbose=false) {
  vector <TString> sScalerNames;
  if (setup==kBMN)
    sScalerNames=sScalerNamesBMN;
  else if (setup==kSRC)
    sScalerNames=sScalerNamesSRC;
  else 
  {
    printf("Unknown setup Id: %d", setup);
    return;
  }
  int nScalers = sScalerNames.size();
  gROOT->Reset();
  ifstream *infile;

  infile = new ifstream(inputFilePath,ios::in | ios::binary);
  if (!infile->is_open()) { cout << "Cannot find data file: " << inputFilePath << endl; return; }
  TFile *outFile=new TFile(outputFilePath,"recreate");

  if (!outFile) { cout << "Cannot create output file: " << outputFilePath << endl; return; }
  TTree t("scalers","scalers");
  vector<int> scalers(nScalers);
  int spillTime, BDcount[40], BDmult[40];
  t.Branch("spillTime", &spillTime, "spillTime/I");
  if (setup==kBMN)
  {
    t.Branch("BDcount", &BDcount, "BDcount[40]/I");
    t.Branch("BDmult", &BDmult, "BDmult[40]/I");
  }
  for (int i=0;i<nScalers;i++)
    t.Branch(sScalerNames.at(i), &scalers.at(i), sScalerNames.at(i)+"/I");
  
  int iSpill;
  for(iSpill = 0; !infile->eof(); iSpill++) {
    if(iSpill < firstSpill) continue;
    if(lastSpill>0 && iSpill > lastSpill)
      break;
    Spill_Record  sSpillRecord; 
    infile->read((char*) &sSpillRecord, sizeof(Spill_Record));


    if(sSpillRecord.iScalerCount[0 + offset] < 1000) continue; // what's that?
    spillTime=sSpillRecord.iScalerCount[0];
    if (verbose)
      printf("\nSpill number %i (time %i):\n==============\n", iSpill, spillTime);

    int offset=1; // spillTime
    int nBDstrips=40;
    if (setup==kBMN)
    {
      for (int i=0;i<nBDstrips;i++)
        BDcount[i]=sSpillRecord.iScalerCount[i+offset];
      offset+=nBDstrips; 
      for (int i=0;i<nBDstrips;i++)
        BDmult[i]=sSpillRecord.iScalerCount[i+offset];
      offset+=nBDstrips; 
    }
    else if (setup==kSRC)
      offset+=2*nBDstrips;

    for(int iScaler=0; iScaler < nScalers; iScaler++) 
    {
      scalers.at(iScaler)=sSpillRecord.iScalerCount[iScaler + offset];
      if (verbose)
        cout << "  " << sScalerNames.at(iScaler) << " = " << sSpillRecord.iScalerCount[iScaler + offset] << endl;
    }

    string sTextData((char*) sSpillRecord.cTextData, nTextDataLength);
    string sRecordTime((char*) sSpillRecord.cTextData, nTimeDataLength);

    if (verbose) {
      cout << "  " << sTextData << endl;
      cout << "  " << sRecordTime << endl;
    }
    t.Fill();
  }
  t.Write();
  outFile->Close();
  if (verbose)
    cout << "Last spill number = " << iSpill << endl;

  return;
}
