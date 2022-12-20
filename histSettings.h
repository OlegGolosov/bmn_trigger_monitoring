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
  {"beamTriggerTimes_BT", {
    {"h_TQDC_BC1(.*)time_BT", "nostack", "y"},
    {"h_TQDC_BC2A(.*)time_BT", "nostack", "y"},
    {"h_TQDC_VC(.*)time_BT", "nostack", "y"},
    {"h_TQDC_FD(.*)time_BT", "nostack", "y"}, 
  }},
  {"beamTriggerAmps_BT", {
    {"h_TQDC_BC1(.*)peak_BT", "nostack", ""},
    {"h_TQDC_BC2A(.*)peak_BT", "nostack", ""},
    {"h_TQDC_VC(.*)peak_BT", "nostack", ""},
    {"h_TQDC_FD(.*)peak_BT", "nostack", ""},
  }},
  {"bdSiTimeCount_BT", {
    {"h_BDmodTime_BT", "nostack", "y"},
    {"h_BDmodAmp_BT", "nostack", "y"},
    {"h_BDcount_BT", "nostack", "y"},
    {"h_SImodTime_BT", "nostack", "y"},
    {"h_SImodAmp_BT", "nostack", "y"},
    {"h_SIcount_BT", "nostack", "y"},
  }},
  {"bdAmps_BT", {
    {"h2_BDmodId_BDmodAmp_BT", "unfoldy", ""},
  }},
  {"siAmps_BT", {
    {"h2_SImodId_SImodAmp_BT", "unfoldy", ""},
  }},
  {"BDcountVsFW_BT", {
    {"h2_BDcount_Hodo_BT", "colz", "z"},
    {"h2_BDcount_tFHCal_BT", "colz", "z"},
    {"h2_BDcount_nFHCal_BT", "colz", "z"},
    {"h2_BDcount_TQDC_FDpeak_BT", "colz", "z"},
  }},
  {"BDampVsFW_BT", {
    {"h2_BDamp_Hodo_BT", "colz", "z"},
    {"h2_BDamp_tFHCal_BT", "colz", "z"},
    {"h2_BDamp_nFHCal_BT", "colz", "z"},
    {"h2_BDamp_TQDC_FDpeak_BT", "colz", "z"},
  }},
  {"SIcountVsFW_BT", {
    {"h2_SIcount_Hodo_BT", "colz", "z"},
    {"h2_SIcount_tFHCal_BT", "colz", "z"},
    {"h2_SIcount_nFHCal_BT", "colz", "z"},
    {"h2_SIcount_TQDC_FDpeak_BT", "colz", "z"},
  }},
  {"SIampVsFW_BT", {
    {"h2_SIamp_Hodo_BT", "colz", "z"},
    {"h2_SIamp_tFHCal_BT", "colz", "z"},
    {"h2_SIamp_nFHCal_BT", "colz", "z"},
    {"h2_SIamp_TQDC_FDpeak_BT", "colz", "z"},
  }},
  {"FWvsFW_BT", {
    {"h2_tFHCal_Hodo_BT", "colz", "z"},
    {"h2_nFHCal_Hodo_BT", "colz", "z"},
    {"h2_TQDC_FDpeak_Hodo_BT", "colz", "z"},
    {"h2_TQDC_FDpeak_tFHCal", "colz", "z"},
    {"h2_TQDC_FDpeak_nFHCal", "colz", "z"},
    {"h2_tFHCal_nFHCal", "colz", "z"},
  }},
  {"TOFvsFW_BT", {
    {"h2_nTOF400_TQDC_FDpeak_BT", "colz", "z"},
    {"h2_nTOF400_tFHCal_BT", "colz", "z"},
    {"h2_nTOF400_nFHCal_BT", "colz", "z"},
    {"h2_nTOF700_TQDC_FDpeak_BT", "colz", "z"},
    {"h2_nTOF700_tFHCal_BT", "colz", "z"},
    {"h2_nTOF700_nFHCal_BT", "colz", "z"},
  }},
  {"TOF700vsFD", {
    {"h2_nTOF700_TQDC_FDpeak_BT", "colz", "z"},
    {"h2_nTOF700_TQDC_FDpeak_NiT", "colz", "z"},
    {"h2_nTOF700_TQDC_FDpeak_MBT", "colz", "z"},
    {"h2_nTOF700_TQDC_FDpeak_CCT1", "colz", "z"},
    {"h2_nTOF700_TQDC_FDpeak_CCT2", "colz", "z"},
  }},
  {"TOF400vsFD", {
    {"h2_nTOF400_TQDC_FDpeak_BT", "colz", "z"},
    {"h2_nTOF400_TQDC_FDpeak_NiT", "colz", "z"},
    {"h2_nTOF400_TQDC_FDpeak_MBT", "colz", "z"},
    {"h2_nTOF400_TQDC_FDpeak_CCT1", "colz", "z"},
    {"h2_nTOF400_TQDC_FDpeak_CCT2", "colz", "z"},
  }},
  {"DCHvsFD", {
    {"h2_nDCH_TQDC_FDpeak_BT", "colz", "z"},
    {"h2_nDCH_TQDC_FDpeak_NiT", "colz", "z"},
    {"h2_nDCH_TQDC_FDpeak_MBT", "colz", "z"},
    {"h2_nDCH_TQDC_FDpeak_CCT1", "colz", "z"},
    {"h2_nDCH_TQDC_FDpeak_CCT2", "colz", "z"},
  }},
//  {"trends", {
//    {"time/h_Amp_(BC.*)_(RMS|mean)", "apl", "", {1,2,3,4,5,6,7,8}, {{27, 2, 3, 2}, {32, 3, 3, 2}, {25, 4, 3, 1}, {35, 5, 3, 1}}},
//    {"time/h_Amp_BC1_RMS", "apl", "", {2}, {{2.7, 2, 3, 2}, {3.2, 3, 3, 2}, {2.5, 4, 3, 1}, {3.5, 5, 3, 1}}},
//  }},
};
