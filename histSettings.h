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
  string log="z"; 
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
  {"beamTriggerTimes", {
    {"h_TQDC_BC1(.*)time_ALL", "nostack", ""},
    {"h_TQDC_BC2A(.*)time_ALL", "nostack", ""},
    {"h_TQDC_VC(.*)time_ALL", "nostack", ""},
    {"h_TQDC_FD(.*)time_ALL", "nostack", ""}, 
  }},
  {"beamTriggerAmps", {
    {"h_TQDC_BC1(.*)peak_ALL", "nostack", ""},
    {"h_TQDC_BC2A(.*)peak_ALL", "nostack", ""},
    {"h_TQDC_VC(.*)peak_ALL", "nostack", ""},
    {"h_TQDC_FD(.*)peak_ALL", "nostack", ""},
  }},
  {"bdSiTimeCount", {
    {"h_BDmodTime_ALL", "nostack", ""},
    {"h_SImodTime_ALL", "nostack", ""},
    {"h_BDmodAmp_ALL", "nostack", ""},
    {"h_SImodAmp_ALL", "nostack", ""},
    {"h_BDcount_ALL", "nostack", ""},
    {"h_SIcount_ALL", "nostack", ""},
  }},
  {"bdAmps", {
    {"h_BDmodId_BDmodAmp_ALL", "", "unfoldy"},
  }},
  {"siAmps", {
    {"h_SImodId_SImodAmp_ALL", "", "unfoldy"},
  }},
  {"BDcountVsFW", {
    {"h2_BDcount_Hodo_ALL", "colz", ""},
    {"h2_BDcount_tFHCal_ALL", "colz", ""},
    {"h2_BDcount_nFHCal_ALL", "colz", ""},
    {"h2_BDcount_TQDC_FDpeak_ALL", "colz", ""},
  }},
  {"BDampVsFW", {
    {"h2_BDamp_Hodo_ALL", "colz", ""},
    {"h2_BDamp_tFHCal_ALL", "colz", ""},
    {"h2_BDamp_nFHCal_ALL", "colz", ""},
    {"h2_BDamp_TQDC_FDpeak_ALL", "colz", ""},
  }},
  {"SIcountVsFW", {
    {"h2_SIcount_Hodo_ALL", "colz", ""},
    {"h2_SIcount_tFHCal_ALL", "colz", ""},
    {"h2_SIcount_nFHCal_ALL", "colz", ""},
    {"h2_SIcount_TQDC_FDpeak_ALL", "colz", ""},
  }},
  {"SIampVsFW", {
    {"h2_SIamp_Hodo_ALL", "colz", ""},
    {"h2_SIamp_tFHCal_ALL", "colz", ""},
    {"h2_SIamp_nFHCal_ALL", "colz", ""},
    {"h2_SIamp_TQDC_FDpeak_ALL", "colz", ""},
  }},
  {"FWvsFW", {
    {"h2_tFHCal_Hodo_ALL", "colz", ""},
    {"h2_nFHCal_Hodo_ALL", "colz", ""},
    {"h2_TQDC_FD_Hodo_ALL", "colz", ""},
    {"h2_TQDC_FD_tFHCal", "colz", ""},
    {"h2_TQDC_FD_nFHCal", "colz", ""},
    {"h2_tFHCal_nFHCal", "colz", ""},
  }},
  {"trends", {
    {"time/h_Amp_(BC.*)_(RMS|mean)", "apl", "", {1,2,3,4,5,6,7,8}, {{27, 2, 3, 2}, {32, 3, 3, 2}, {25, 4, 3, 1}, {35, 5, 3, 1}}},
    {"time/h_Amp_BC1_RMS", "apl", "", {2}, {{2.7, 2, 3, 2}, {3.2, 3, 3, 2}, {2.5, 4, 3, 1}, {3.5, 5, 3, 1}}},
  }},
};
