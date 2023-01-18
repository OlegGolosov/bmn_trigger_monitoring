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
  string log=""; 
  vector<int> colors={1,2,4,6,3,5,7,8};
  vector<rangeLine> lines={};
  vector<int> markerStyles={0};
  vector<int> markerSizes={0};
  vector<int> lineStyles={1,1,1,1};
  vector<int> lineWidths={3};
};

map <string, vector <histSettings>> 
histSettingsMap=
{
  {"time/BC", {
    {"time/h_TQDC_(BC1T|BC1B|BC1S)peak_mean", "apl", ""},
    {"time/h_TQDC_(BC1T|BC1B|BC1S)peak_RMS", "apl", ""},
    {"time/h_TQDC_(BC2AT|BC2AB|BC2AS)peak_mean", "apl", ""},
    {"time/h_TQDC_(BC2AT|BC2AB|BC2AS)peak_RMS", "apl", ""},
  }},
  {"runId/BC", {
    {"runId/h_TQDC_(BC1T|BC1B|BC1S)peak_mean", "apl", ""},
    {"runId/h_TQDC_(BC1T|BC1B|BC1S)peak_RMS", "apl", ""},
    {"runId/h_TQDC_(BC2AT|BC2AB|BC2AS)peak_mean", "apl", ""},
    {"runId/h_TQDC_(BC2AT|BC2AB|BC2AS)peak_RMS", "apl", ""},
  }},
  {"beamTimes", {
    {"h_TQDC_BC1Stime_(.*)", "nostack", "y"},
    {"h_TQDC_BC2AStime_(.*)", "nostack", "y"},
    {"h_TQDC_VCStime_(.*)", "nostack", "y"},
    {"h_TQDC_FDtime_(.*)", "nostack", "y"}, 
  }},
  {"beamAmps", {
    {"h_TQDC_BC1Speak_(.*)", "nostack", ""},
    {"h_TQDC_BC2ASpeak_(.*)", "nostack", ""},
    {"h_TQDC_VCSpeak_(.*)", "nostack", ""},
    {"h_TQDC_FDpeak_(.*)", "nostack", ""},
  }},
  {"beamPeakBin", {
    {"h_TQDC_BC1SpeakBin_(.*)", "nostack", ""},
    {"h_TQDC_BC2ASpeakBin_(.*)", "nostack", ""},
    {"h_TQDC_VCSpeakBin_(.*)", "nostack", ""},
    {"h_TQDC_FDpeakBin_(.*)", "nostack", ""},
  }},
  {"beamPeakIntegralBC1", {
    {"TQDC_BC1Speak_TQDC_BC1Sintegral_BT", "colz", ""},
    {"TQDC_BC1Speak_TQDC_BC1Sintegral_MBT", "colz", ""},
    {"TQDC_BC1Speak_TQDC_BC1Sintegral_CCT1", "colz", ""},
    {"TQDC_BC1Speak_TQDC_BC1Sintegral_CCT2", "colz", ""},
  }},
  {"beamPeakIntegralBC2", {
    {"TQDC_BC2ASpeak_TQDC_BC2ASintegral_BT", "colz", ""},
    {"TQDC_BC2ASpeak_TQDC_BC2ASintegral_MBT", "colz", ""},
    {"TQDC_BC2ASpeak_TQDC_BC2ASintegral_CCT1", "colz", ""},
    {"TQDC_BC2ASpeak_TQDC_BC2ASintegral_CCT2", "colz", ""},
  }},
  {"beamPeakIntegralVC", {
    {"TQDC_VCSpeak_TQDC_VCSintegral_BT", "colz", ""},
    {"TQDC_VCSpeak_TQDC_VCSintegral_MBT", "colz", ""},
    {"TQDC_VCSpeak_TQDC_VCSintegral_CCT1", "colz", ""},
    {"TQDC_VCSpeak_TQDC_VCSintegral_CCT2", "colz", ""},
  }},
  {"beamPeakIntegralFD", {
    {"TQDC_FDpeak_TQDC_FDintegral_BT", "nostack colz", ""},
    {"TQDC_FDpeak_TQDC_FDintegral_MBT", "colz", ""},
    {"TQDC_FDpeak_TQDC_FDintegral_CCT1", "colz", ""},
    {"TQDC_FDpeak_TQDC_FDintegral_CCT2", "colz", ""},
  }},
//  {"beamPeakBinTime_BT", {
//    {"h2_TQDC_BC1SpeakBin_TQDC_BC1Stime_BT", "colz", ""},
//    {"h2_TQDC_BC2ASpeakBin_TQDC_BC2AStime_BT", "colz", ""},
//    {"h2_TQDC_VCSpeakBin_TQDC_VCStime_BT", "colz", ""},
//    {"h2_TQDC_FDpeakBin_TQDC_FDtime_BT", "colz", ""},
//  }},
//  {"beamPeakBinTime_MBT", {
//    {"h2_TQDC_BC1SpeakBin_TQDC_BC1Stime_MBT", "colz", ""},
//    {"h2_TQDC_BC2ASpeakBin_TQDC_BC2AStime_MBT", "colz", ""},
//    {"h2_TQDC_VCSpeakBin_TQDC_VCStime_MBT", "colz", ""},
//    {"h2_TQDC_FDpeakBin_TQDC_FDtime_MBT", "colz", ""},
//  }},
  {"TOF_DCHtimes", {
    {"h_TOF400time_(.*)", "nostack", "y"},
    {"h_TOF700time_(.*)", "nostack", "y"},
    {"h_DCHtime_(.*)", "nostack", "y"},
  }},
  {"BDSItimeCount", {
    {"h_BDmodTime_(.*)", "nostack", "y"},
    {"h_BDmodAmp_(.*)", "nostack", "y"},
    {"h_BDcount_(.*)", "nostack", "y"},
    {"h_SImodTime_(.*)", "nostack", "y"},
    {"h_SImodAmp_(.*)", "nostack", "y"},
    {"h_SIcount_(.*)", "nostack", "y"},
  }},
  {"BDtimes_BT", {
    {"h2_BDmodId_BDmodTime_BT", "unfoldy", ""},
  }},
  {"BDamps_BT", {
    {"h2_BDmodId_BDmodAmp_BT", "unfoldy", ""},
  }},
  {"SItimes_BT", {
    {"h2_SImodId_SImodTime_BT", "unfoldy", ""},
  }},
  {"SIamps_BT", {
    {"h2_SImodId_SImodAmp_BT", "unfoldy", ""},
  }},
  {"BDcountVsFW_BT", {
    {"h2_BDcount_Hodo_BT", "colz", ""},
    {"h2_BDcount_tFHCal_BT", "colz", ""},
    {"h2_BDcount_nFHCal_BT", "colz", ""},
    {"h2_BDcount_TQDC_FDpeak_BT", "colz", ""},
  }},
  {"BDampVsFW_BT", {
    {"h2_BDamp_Hodo_BT", "colz", ""},
    {"h2_BDamp_tFHCal_BT", "colz", ""},
    {"h2_BDamp_nFHCal_BT", "colz", ""},
    {"h2_BDamp_TQDC_FDpeak_BT", "colz", ""},
  }},
  {"SIcountVsFW_BT", {
    {"h2_SIcount_Hodo_BT", "colz", ""},
    {"h2_SIcount_tFHCal_BT", "colz", ""},
    {"h2_SIcount_nFHCal_BT", "colz", ""},
    {"h2_SIcount_TQDC_FDpeak_BT", "colz", ""},
  }},
  {"SIampVsFW_BT", {
    {"h2_SIamp_Hodo_BT", "colz", ""},
    {"h2_SIamp_tFHCal_BT", "colz", ""},
    {"h2_SIamp_nFHCal_BT", "colz", ""},
    {"h2_SIamp_TQDC_FDpeak_BT", "colz", ""},
  }},
  {"FWvsFW_BT", {
    {"h2_tFHCal_Hodo_BT", "colz", ""},
    {"h2_nFHCal_Hodo_BT", "colz", ""},
    {"h2_TQDC_FDpeak_Hodo_BT", "colz", ""},
    {"h2_TQDC_FDpeak_tFHCal_BT", "colz", ""},
    {"h2_TQDC_FDpeak_nFHCal_BT", "colz", ""},
    {"h2_tFHCal_nFHCal_BT", "colz", ""},
  }},
  {"FWvsFW_MBT", {
    {"h2_tFHCal_Hodo_MBT", "colz", ""},
    {"h2_nFHCal_Hodo_MBT", "colz", ""},
    {"h2_TQDC_FDpeak_Hodo_MBT", "colz", ""},
    {"h2_TQDC_FDpeak_tFHCal_MBT", "colz", ""},
    {"h2_TQDC_FDpeak_nFHCal_MBT", "colz", ""},
    {"h2_tFHCal_nFHCal_MBT", "colz", ""},
  }},
  {"MIDvsMID_MBT", {
    {"h2_nDCH_nTOF400_MBT", "colz", ""},
    {"h2_nDCH_nTOF700_MBT", "colz", ""},
    {"h2_BDcount_nTOF400_MBT", "colz", ""},
    {"h2_BDcount_nTOF700_MBT", "colz", ""},
    {"h2_BDamp_nTOF400_MBT", "colz", ""},
    {"h2_BDamp_nTOF700_MBT", "colz", ""},
    {"h2_nTOF700_nTOF400_MBT", "colz", ""},
  }},
  {"TOFvsFW_BT", {
    {"h2_nTOF400_TQDC_FDpeak_BT", "colz", ""},
    {"h2_nTOF400_tFHCal_BT", "colz", ""},
    {"h2_nTOF400_nFHCal_BT", "colz", ""},
    {"h2_nTOF700_TQDC_FDpeak_BT", "colz", ""},
    {"h2_nTOF700_tFHCal_BT", "colz", ""},
    {"h2_nTOF700_nFHCal_BT", "colz", ""},
  }},
  {"TOF700vsFD", {
    {"h2_nTOF700_TQDC_FDpeak_BT", "colz", ""},
    {"h2_nTOF700_TQDC_FDpeak_NiT", "colz", ""},
    {"h2_nTOF700_TQDC_FDpeak_MBT", "colz", ""},
    {"h2_nTOF700_TQDC_FDpeak_CCT1", "colz", ""},
    {"h2_nTOF700_TQDC_FDpeak_CCT2", "colz", ""},
  }},
  {"TOF400vsFD", {
    {"h2_nTOF400_TQDC_FDpeak_BT", "colz", ""},
    {"h2_nTOF400_TQDC_FDpeak_NiT", "colz", ""},
    {"h2_nTOF400_TQDC_FDpeak_MBT", "colz", ""},
    {"h2_nTOF400_TQDC_FDpeak_CCT1", "colz", ""},
    {"h2_nTOF400_TQDC_FDpeak_CCT2", "colz", ""},
  }},
  {"DCHvsFD", {
    {"h2_nDCH_TQDC_FDpeak_BT", "colz", ""},
    {"h2_nDCH_TQDC_FDpeak_NiT", "colz", ""},
    {"h2_nDCH_TQDC_FDpeak_MBT", "colz", ""},
    {"h2_nDCH_TQDC_FDpeak_CCT1", "colz", ""},
    {"h2_nDCH_TQDC_FDpeak_CCT2", "colz", ""},
  }},
//  {"trends", {
//    {"time/h_Amp_(BC.*)_(RMS|mean)", "apl", "", {1,2,3,4,5,6,7,8}, {{27, 2, 3, 2}, {32, 3, 3, 2}, {25, 4, 3, 1}, {35, 5, 3, 1}}},
//    {"time/h_Amp_BC1_RMS", "apl", "", {2}, {{2.7, 2, 3, 2}, {3.2, 3, 3, 2}, {2.5, 4, 3, 1}, {3.5, 5, 3, 1}}},
//  }},
};
