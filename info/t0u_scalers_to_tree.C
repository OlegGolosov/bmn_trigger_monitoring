const int nDataBlockCounterNbr = 256;
const int nCharArrayLength     = 1024;
const int nTextDataLength      = 4096;
const int nTimeDataLength      = 19;

const int nScalers = 21;
const int iOffset = 81;

TString sScalerNames[nScalers] = { "BC1_low", "BC1", "BC2", "T0", "VC", "BC3", "BC4", "BC5", "Cal1", "Cal2", "Neutron",
				  "DAQ_Busy", "BT", "GBT", "Fragment", "SRC_OR", "ARM_OR", "ARM_AND", "SRC", "Recoil", "Spill_No" };

typedef struct {
  int  iScalerCount[nDataBlockCounterNbr];
  char cTextData[nTextDataLength];
} Spill_Record;

void t0u_scalers_to_tree(int firstSpill = 70000, int lastSpill = 90000, const char *inputFilePath="SRC_TrgMgr22.SSbin", const char *outputFilePath="SRC_TrgMgr22.root", bool verbose=false) {

  gROOT->Reset();
  ifstream *infile;

  infile = new ifstream(inputFilePath,ios::in | ios::binary);
  if (!infile->is_open()) { cout << "Cannot find data file: " << inputFilePath << endl; return; }
  TFile *outFile=new TFile(outputFilePath,"recreate");

  Bool_t eof = false;

  int iSpill, iScaler, i;

  if (!outFile) { cout << "Cannot create output file: " << outputFilePath << endl; return; }
  TTree t("scalers","scalers");
  int scalers[nScalers];
  int spillTime;
  t.Branch("spillTime", &spillTime, "spillTime/I");
  for (int i=0;i<nScalers;i++)
    t.Branch(sScalerNames[i], &scalers[i], sScalerNames[i]+"/I");

  for(iSpill = 0; iSpill <= lastSpill; iSpill++) {
    Spill_Record  sSpillRecord; 
    if(infile->eof()) eof = true;
    if(eof) break;

    infile->read((char*) &sSpillRecord, sizeof(Spill_Record));

    if(iSpill < firstSpill) continue;

    if(sSpillRecord.iScalerCount[0 + iOffset] < 1000) continue; // what's that?
    spillTime=sSpillRecord.iScalerCount[0];
    if (verbose)
      printf("\nSpill number %i (time %i):\n==============\n", iSpill, spillTime);

    for(iScaler = 0; iScaler < nScalers; iScaler++) {
      scalers[iScaler]=sSpillRecord.iScalerCount[iScaler + iOffset];
      if (verbose)
        cout << "  " << sScalerNames[iScaler] << " = " << sSpillRecord.iScalerCount[iScaler + iOffset] << endl;
    }

    TString sTextData((char*) sSpillRecord.cTextData, nTextDataLength);
    TString sRecordTime((char*) sSpillRecord.cTextData, nTimeDataLength);

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
