unit U_Const;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, Graphics, StdCtrls;

const
  _version = 'v1.25';
  TDataBlockCounterNbr = 256;

type
  charAr = array [0..1023] of char;

  TDataBlock = record
    counters : array[0..TDataBlockCounterNbr-1] of integer;
             // @0 -> sec number from 00:00 2020.01.01,
             // @1..81 reserved for BMN
             // @82... - counters BC1_low, BC1_h, ...
    textData : array [0..4095] of char;
             // date & time
             // subtrigger texts
             // loaded config file name
             // content of loaded config file
  end;



(*
comd=0x80 - wtinig
       <addr><0x80><5> <0000 0 P1_Enable P2_Enable N_Enable> <P1 DAC_L> <P1 DAC_H> <P2 DAC_L> <P2 DAC_H><CS>
answer
      <addr> <0x80> <0> <CS>
*)

  TLVComd = packed record
    addr : byte;
    comd : byte;
    lng : byte;
    enables : byte;
    DACCodes : array [0..1] of word;
    cs : byte;
  end;

(*
comd=0 - reading
       <addr><0><0><CS>
answer
       <addr> <0> <14> <iP1L><iP1H> <iP2L><iP2H> <iNL><iNH> <vP1_L><vP1H> <vP2_L><vP2H> <CS>
*)
  TLVState = packed record
    addr : byte;
    comd : byte;
    lng : byte;
    data : array [0..6] of word;
    cs : byte;
  end;



const
  gainADCtoIp=14.70585242;
  gainADCtoIn=15.89931224;
  gainADCtoV = 7.85/3749.0;
  _LV_ADCArrayLng = 10;
type

  TLVDataBlock = record
    IP1ADC : word;
    IP2ADC : word;
    INADC : word;
    VP1ADC : word;
    VP2ADC : word;

    I_P1 : double;
    I_P2 : double;
    I_N : double;
    V_P1 : double;
    V_P2 : double;
  end;

const

  _servicePageIdx = 3;

  _expertText = 'EXPERT';

  _FPGA_Addr = 24;

  posDiscrMSIdx = 0;
  negDiscrMSIdx = 1;
  posSGDiscrMSIdx = 2;
  negSGDiscrMSIdx = 3;
  swOnMSIdx = 4;
  swOffMSIdx = 5;
  delMSIdx = 6;

  delayCount = 10;
  discrCount = 12;
  switchCount = 17;
  downScalerCount = 18;
  LVChannelCount = 12;
  lCountNbr = 21;
  ratioCountNbr = 20;

  selectedPanelColor = clMoneyGreen;

  _spillGateDiscrIdx=12; // to suppress blinking of the SpillGate discriminator

  discrPref = 'imDiscr';
  pDiscrPref = 'pDscr';
  seDiscrPref = 'seDiscr';
  rgDscrPref = 'rgDscr';
  pSwPref = 'pSw';
  cbSwPref = 'cbSwOn';
  cbTestSignalOnPref = 'cbTestSignalOn';
  swPref = 'ImSw';
  delPref = 'imDL';
  pDelPref = 'pDL';
  caenPref = 'seDSK';
  delayPref = 'seDelay';

  sgDiscrPos = 'DiscriminatorSGPos.png';
  sgDiscrNeg = 'DiscriminatorSGNeg.png';
  discrPicture = 'Discriminator.png';
  discrNegPicture = 'DiscriminatorNeg.png';
  discrPosPicture = 'DiscriminatorPos.png';

  swOnPicture = 'swOn.png';
  swOFFPicture = 'swOFF.png';
  lCountsPref = 'lCount';
  lCountsDisplayPref = 'lCountD';
  lCountsDisplaySPref = 'lCountS';
  lRationAPref = 'lRefA';
  lRationBPref = 'lRefB';
  eRationAPref = 'eRefA';
  eRationBPref = 'eRefB';


  //===============================================
  // === config tree ===
  //===============================================

  tmpFileName = 'tmp.txt';
  tComment = 'Comment';

  tDiscriminators = 'Discriminators';
  tDiscrPref = 'Discriminator';
  tPolarityPref = 'Polarity';
  tPolarityPos = 'Positive';
  tPolarityNeg = 'Negative';
  tDiscrValue = 'Threshold, mV';

  tDelays = 'Delay lines';
  tDelayPref = 'Delay';

  tSwitches = 'Switches';
  tSwitchPref = 'Switch';
  tSwOn = 'On';
  tSwOff = 'Off';

  tLVPref = 'LV';
  tChannelPref = 'Channel';
  tNamePref = 'Channel name';
  tCHNamePref = 'lCHName';
  tP1Pref = 'V_P1';
  tP2Pref = 'V_P2';
  tActivePref = 'Active';
  tStatePref = 'State';
  tLVOnPref = 'On';
  tLVOffPref = 'Off';
  tLVActive = '1';
  tLVInActive = '0';


  seVP1Pref = 'seVP1_';
  seVP2Pref = 'seVP2_';
  seCbActivePref = 'cbActive';
  sbOnOffPref = 'sbOnOff';
  lVP1Pref = 'lVP1Actual';
  lVP2Pref = 'lVP2Actual';
  lIP1Pref = 'lIP1Actual';
  lIP2Pref = 'lIP2Actual';
  lINPref = 'lINActual';

  inactiveChannelColor = clGrayText;

  //===============================================
  // === end of config tree ===
  //===============================================

  //===============================================
  // === config selector ===
  //===============================================

  _defaultConfigExt = 'SRCCfg';
  _configFileExt = _defaultConfigExt;

  _rootImgIdx = 0;
  _dirImgIdx = 1;
  _grpImgIdx = 2;
  _cfImgIdx = 3;

  //===============================================
  // === end of config selector ===
  //===============================================


  //===============================================
  // === channel addresses ===
  //===============================================
(*
1.	@81 – BC1_low
2.	@82 - BC1
3.	@83 - BC2
4.	@84 - T0
5.	@85 - VC
6.	@86 - BC3
7.	@87 - BC4
8.	@88 - BC5
9.	@89 - Cal1
10.	@90 - Cal2
11.	@91 - Neutron
12.	@92 – DAQ_Busy
13.	@93 - BT
14.	@94 - GBT
15.	@95 - Fragment
16.	@96 - SRC-OR
17.	@97 - ARM-OR
18.	@98 - ARM-AND
19.	@99 - SRC
20.	@100 – Recoil
21.	@101 – Spill_Gate
*)

_BC1_low_SSIdx = 81;
_BC1_SSIdx = 82;
_BC2_SSIdx = 83;
_T0_SSIdx = 84;
_VC_SSIdx = 85;
_BC3_SSIdx = 86;
_BC4_SSIdx = 87;
_BC5_SSIdx = 88;
_Cal1_SSIdx = 89;
_Cal2_SSIdx = 90;
_Neutron_SSIdx = 91;
_DAQ_Busy_SSIdx = 92;
_BT_SSIdx = 93;
_GBT_SSIdx = 94;
_Frag_SSIdx = 95;
_SRC_OR_SSIdx = 96;
_ARM_OR_SSIdx = 97;
_ARM_AND_SSIdx = 98;
_SRC_SSIdx = 99;
_Recoil_N_SSIdx = 100;

  _chAddrss : array [1..discrCount] of integer = (
    81,82,83,84,85,
    86,87,88,89,90,
    91,101
    );
  _cal1Addr = 89;
  _cal2Addr = 90;

  //===============================================
  // === end of channel addresses ===
  //===============================================

  //===============================================
  // === channel tag to delay number ===
  //===============================================
(*
  1 BC1->BT
  2 BC2->BT
  3 T0->BT
  4 BC3->Frag
  5 BC4->Frag
  6 BC5->Frag
  7 GBT->ARM
  8 GBT->Recoil-N
  9 FRAG->SRC-OR, SRC
*)
  _keyIdxShift = 100;

  _chDelayIdxs : array [1..discrCount] of integer = (
    0,1,2,3,4,5,
    6,7,_keyIdxShift+1,_keyIdxShift+2,_keyIdxShift+3,
    12 // there is a spill gate
    );

  _GBT_ARM_DelIdx = 8;
  _GBT_N_DelIdx = 9;
  _FRAG_SRC_DelIdx =10;

  //===============================================
  // === end of channel tag to delay number ===
  //===============================================

  //===============================================
  // === channel on/off to input channel number ===
  //===============================================
(*
  1 BC1->BT
  2 BC2->BT
  3 T0->BT
  4 VC->GBT
  5 GBT->Frag

  6 BC3->Frag
  7 BC4->Frag
  8 BC5->Frag
  9 Cal1->ARM-OR
  10 CAL2->ARM-OR

  11 Cal1->ARM-AND
  12 CAL2->ARM-AND
  13 Neutron->Recoil-N
  14 ARM-OR->SRC-OR
  15 ARM-AND->SRC

  16 SpillGate->BT

  1.	@81 – BC1_low
  2.	@82 - BC1
  3.	@83 - BC2
  4.	@84 - T0
  5.	@85 - VC
  6.	@86 - BC3
  7.	@87 - BC4
  8.	@88 - BC5
  9.	@89 - Cal1
  10.	@90 - Cal2
  11.	@91 - Neutron
  12.	@92 – DAQ_Busy
  13.	@93 - Beam
  14.	@94 - GoodBeam
  15.	@95 - Fragment
  16.	@96 - SRC-OR
  17.	@97 - ARM-OR
  18.	@98 - ARM-AND
  19.	@99 - SRC
  20.	@100 – Recoil
  21.	@101 – Spill_Gate

*)


  _onOffIdxs : array [1..switchCount] of integer = (
    2,3,4,5,_keyIdxShift+1, // BC1_idx=2,BC2(3),T0(4),VC(5),GBT->Frag(101) (idx>100 -> the delay not in channel)
    6,7,8,_keyIdxShift+2,_keyIdxShift+3,  // BC3,BC4,BC5,Cal1 - ordinary channel without delay,Cal2 - ordinary channel without delay
    _keyIdxShift+4,_keyIdxShift+5,11,_keyIdxShift+6,_keyIdxShift+7, // Cal1 external delay,Cal2 external delay, neutron, ARM-OR external delay, ARM-AND external delay
    12, // spill gate  - ordinary channel without delay
    _keyIdxShift+8 // BC3-5 anti
    );
  _BC1_low_idx = 1;
  _cal1Idx = 9;
  _cal2Idx = 10;
  _BC3_5_AntiSwIdx = 17;

  (* control bits
  	<regData2[7] -> OFF switch GBT->FRAG
  	<regData2[6] -> OFF switch CAL1->ARM-OR
  	<regData2[5] -> OFF switch CAL2->ARM-OR
  	<regData2[4] -> OFF switch CAL1->ARM-AND
  	<regData2[3] -> OFF switch CAL2->ARM-AND
  	<regData2[2] -> OFF switch ARM-OR->SRC_OR
  	<regData2[1] -> OFF switch ARM-AND->SRC
  *)
  _keyIdxToSwCBsIdx : array [1..8] of integer  = (
    5,9,10,11,12,14,15,17);  // idx for CBs for special bits

  _seDelay2InputChIdx : array [1..discrCount] of integer = (
  0, // BC1_loww, does not have delay line
  1,2,3, // BC1, BC2, T0,
  4, // VC
  5,6,7, // BC3, BC4, BC5
  0,0,0,0 // Cal1, Cal2, Neutron, spill gate
  );


// synchronous delays GTB->ARM and Frag->SRC (bits 15..8), extra switches bits
_GBT_ARM_DelayAddr = 230;
_frag_SRC_ExtraBitsAddr = 231;
_frag_SRC_seIdx = 10;
_fragSwitchesMask = $80; // mask to provide proper On/Off settings of switches

  //===============================================
  // === end of channel tag to delay number ===
  //===============================================


  //===============================================
  // === discriminator to module address ===
  //===============================================
  _discrAddr : array [1..discrCount] of byte = (
    //230,230,230,230,   // M5, left module, module nbr=3, channels 15-12
    233,233,233,233,   // M3  module nbr=1, channels 7-4
    232,232,232,232,   // M4  module nbr=2, channels 11-8
    231,231,231,231    // M2  module nbr=0, channels 3-0
    );
  //_discrAddr : array [1..discrCount] of byte = (
  //  //230,230,230,230,   // M5, left module, module nbr=3, channels 15-12
  //  231,231,231,231,   // M4  module nbr=2, channels 11-8
  //  232,232,232,232   // M3  module nbr=1, channels 7-4
  //  ,233,233,233,233    // M2  module nbr=0, channels 3-0
  //  );
  _discrSubAddr : array [0..3] of word = ($C000,$8000,$4000,0);

  _discrComdBase = 233; // E8 - address of the most left discr board  (M5)
  _zeroDACCode = 741; // DAC code to get 0V at a discriminator

  _antiPanelIdx = 17;

//===============================================
// === end of spill counter reading   ===========
//===============================================
(*
1.	@81 – BC1_low
2.	@82 - BC1
3.	@83 - BC2
4.	@84 - T0
5.	@85 - VC
6.	@86 - BC3
7.	@87 - BC4
8.	@88 - BC5
9.	@89 - Cal1
10.	@90 - Cal2
11.	@91 - Neutron
12.	@92 – DAQ_Busy
13.	@93 - BT
14.	@94 - GBT
15.	@95 - Fragment
16.	@96 - SRC-OR
17.	@97 - ARM-OR
18.	@98 - ARM-AND
19.	@99 - SRC
20.	@100 – Recoil-N
21.	@101 – Spill_Gate
22.	@102 –  end_of_Spill_Gate counter
23.	@103 – BT * /Busy


*)
   _counterNbr = 23;
   _endOfSpillCounterNbr = 102;
   //_endOfSpillCounterNbr = 101;
   _firstCounterAddr = 81; // BC1_low

   _conterAddrsToLabel : array [0.._counterNbr] of integer = ( // idxs of labels to display spill summary from dataBlock
   0,11,1,2,3, // sec count (no label), BC1_low, BC1, BC2, T0
   4,5,6,7,8,  // VC, BC3, BC4, BC5, Cal1
   9,10,21,12,13,  // CAL2, Neutron, DAQ busy, BT, GBT
   14,15,16,19,17, // Frag, SRC-OR, ARM-Or, ARM-And, SRC
   18,0,0,20 // Recoil, Spill_Gate, end_of_Spill_Gate counter(no label) , BT * /Busy
   );

(*
_tabCaptions : array [0..42] of string = (
'NAME',
'Spill nbr',

// Beam intensity
'BC1','BC2','T0','VC','BC3','BC4','BC5','Cal1','Cal2','Neurt',
 1     2     3    4    5     6     7     8      9      10

// Triggers
'BT','GBT','Frag','SRC-OR','ARM-OR','ARM-AND','SRC','Recoil-N',
11     12    13     14       15       16        17     18

// DAQ
'BC1_low','DAQ_Busy','BT*DAQ_Busy',
  19        20          21
*)
   _conterAddrsToWebData : array [0.._counterNbr] of integer = ( // idxs of count value in web DATA
      0,19,1,2,3, // sec count (no label), BC1_low, BC1, BC2, T0
      4,5,6,7,8,  // VC, BC3, BC4, BC5, Cal1
      9,10,  // CAL2, Neutron,
      20,11,12, // DAQ busy, BT, GBT
      13,14,15,16,17, // Frag, SRC-OR, ARM-Or, ARM-And, SRC
      18,0,0,21 // Recoil, Spill_Gate, end_of_Spill_Gate counter(no label) , BT * /Busy
      );

   _secsPedDay : TDateTime = 24.0*60.0*60.0;
//===============================================
// === end of "end of spill counter reading"   ==
//===============================================

_downScaler1Addr = 236;
_downScaler2Addr = 237;

_sharedMemoryName : string = 'SpillSummary' ;
_sharedMemoryWebDataName = 'WebData';

_webSpillNbrLng = 10;

_spillSummaryFileName : string = 'SpillSummary.dta';



type
  als = array [1..ratioCountNbr] of TLabel;


implementation

end.

