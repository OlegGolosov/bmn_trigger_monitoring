#/bin/bash

. /cvmfs/nica.jinr.ru/centos7/bmn/env.sh
. /cvmfs/nica.jinr.ru/centos7/bmnroot/dev/bmnroot_config.sh
 
rawPath=/eos/nica/bmn/exp/raw/run8/beam/BMN_2022
lastUnneededRawFile=/eos/nica/bmn/exp/raw/run8/beam/BMN_2022/mpd_run_Top_6784_ev1_p2.data
digiPath=/scratch2/ogolosov/bmndata/run8/digi
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
nEvents=10000
sleepDuration=10 #sec

mkdir -pv $digiPath 
mkdir -pv $qaPath 
touch $listDigiPrev  

while [ true ];do
  #raw to digi
  find $rawPath -name '*.data' -newer $lastUnneededRawFile > $listRaw
  for rawFile in $(cat $listRaw);do
    digiFile=$digiPath/$(basename $rawFile)
    digiFile=${digiFile/\.data/.root}
    if [ ! -e $digiFile ];then
      root -b -l -q $decoderMacro"(\"$rawFile\", \"$digiFile\", $nEvents)"
    fi
  done
  
  #run QA
  ls $digiPath/* > $listDigiCurr
  if [ $(cat $listDigiCurr | wc -w) -gt $(cat $listDigiPrev | wc -w) ]; then # new digi files added
    runIdPrev=none
    for digiFile in $(cat $listDigiCurr);do
      digiFileBase=$(basename $digiFile)
      runId=$(echo $digiFileBase | grep -P '\d{4,5}' -o)
      if [ $(cat $listDigiCurr | grep $runId -c) -gt $(cat $listDigiPrev | grep $runId -c) ] && [ $runId != $runIdPrev ];then
        # new files added for runId and it has not been analyzed 
        qaFile=$qaPath/$(echo $digiFileBase | grep -P '.*\d{4,5}' -o).qa.root
        root -b -l -q $qaMacro"(\"$digiPath/*_${runId}_*.root\", \"$qaFile\")"
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
