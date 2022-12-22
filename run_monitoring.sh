#/bin/bash

. /cvmfs/nica.jinr.ru/centos7/bmn/env.sh
. /cvmfs/nica.jinr.ru/centos7/bmnroot/dev/bmnroot_config.sh
 
rawPath=/eos/nica/bmn/exp/raw/run8/beam/BMN_2022
firstRawFile=/eos/nica/bmn/exp/raw/run8/beam/BMN_2022/mpd_run_Top_6939_ev0_p0.data
digiPath=/scratch2/ogolosov/bmndata/run8/digi
digiPathEos=/eos/nica/mpd/users/ogolosov/bmndata/run8/digi
qaPath=/scratch2/ogolosov/bmndata/run8/qa/trigger
decoderMacro=BmnDataToRoot.C
qaMacro=plotTriggers.C
trendMacro=plotTrends.C
trendHistPattern=".*"
trendFile=$qaPath/trends.root
displayMacro=display.C
listRaw=raw.list
listDigiPrev=digiPrev.list
listDigiCurr=digiCurr.list
listQa=qa.list
nEvents=0
sleepDuration=10 #sec

mkdir -pv $digiPath 
mkdir -pv $digiPathEos 
mkdir -pv $qaPath 
touch $listDigiPrev  

while [ true ];do
  #raw to digi
  echo $firstRawFile > $listRaw
  find $rawPath -name '*.data' -newer $firstRawFile >> $listRaw
  for rawFile in $(cat $listRaw);do
    digiFile=$(basename $rawFile)
    digiFile=${digiFile/\.data/.root}
    if [ ! -e $digiPathEos/$digiFile ];then
      root -b -l -q $decoderMacro"(\"$rawFile\", \"$digiPath/$digiFile\", $nEvents)"
      mv $digiPath/$digiFile $digiPathEos
    fi
  done
  
  #run QA
  ls $digiPathEos/* > $listDigiCurr
  if [ $(cat $listDigiCurr | wc -w) -gt $(cat $listDigiPrev | wc -w) ]; then # new digi files added
    runIdPrev=none
    for digiFile in $(cat $listDigiCurr);do
      digiFileBase=$(basename $digiFile)
      runId=$(echo $digiFileBase | grep -P '\d{4,5}' -o)
      if [ $(cat $listDigiCurr | grep $runId -c) -gt $(cat $listDigiPrev | grep $runId -c) ] && [ $runId != $runIdPrev ];then
        # new files added for runId and it has not been analyzed 
        qaFile=$qaPath/$(echo $digiFileBase | grep -P '.*\d{4,5}' -o).qa.root
        root -b -l -q $qaMacro"(\"$digiPathEos/*_${runId}_*.root\", \"$qaFile\")"
        runIdPrev=$runId
      fi
    done 
    mv -v $listDigiCurr $listDigiPrev
    
    rm $trendFile
  fi

  #plot trends
  if [ ! -e $trendFile ];then
    ls $qaPath/* > $listQa
    root -b -q $trendMacro"(\"$listQa\", \"$trendHistPattern\", \"$trendFile\")"
  fi
  sleep $sleepDuration
done
