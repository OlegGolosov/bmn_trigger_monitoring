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

map <string, vector <histSettings>> 
histSettingsMap=
{
  {
    "trigger1d",
    {
      {"h_TQDC_BC1(.*)peak_ALL", "nostack"},
      {"h_TQDC_BC2(.*)peak_ALL", "nostack"},
      {"h_TQDC_VC(.*)peak_ALL", "nostack"},
      {"h_TQDC_FD(.*)peak_ALL", "nostack"},
      {"h_TQDC_BC1(.*)time_ALL", "nostack"},
      {"h_TQDC_BC2(.*)time_ALL", "nostack"},
      {"h_TQDC_VC(.*)time_ALL", "nostack"},
      {"h_TQDC_FD(.*)time_ALL", "nostack"},
    }
  },
  {
    "trigger2d",
    {
      {"h2_BDcount_Hodo_ALL", "colz"},
      {"h2_BDcount_nFHCal_ALL", "colz"},
      {"h2_BDcount_tFHCal_ALL", "colz"},
      {"h2_BDcount_TQDC_FDpeak_ALL", "colz"},
    }
  },
  {
    "trends",
    {
      {"time/h_Amp_(BC.*)_(RMS|mean)", "apl", {1,2,3,4,5,6,7,8}, {{27, 2, 3, 2}, {32, 3, 3, 2}, {25, 4, 3, 1}, {35, 5, 3, 1}}},
      {"time/h_Amp_BC1_RMS", "apl", {2}, {{2.7, 2, 3, 2}, {3.2, 3, 3, 2}, {2.5, 4, 3, 1}, {3.5, 5, 3, 1}}},
    }
  },
};
