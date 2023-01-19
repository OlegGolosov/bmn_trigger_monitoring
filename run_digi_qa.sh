#!/bin/bash

listRaw=/lhep/users/ogolosov/soft/bmn_trigger_monitoring/raw_ev0_p0.list
rawPath=/eos/nica/bmn/exp/raw/run8/beam/BMN_2022
startTime="1/06/2023 15:00:00"
sleepDuration=3600 #sec
pattern=*_ev0_p0
trendMacro=plotTrends.C
trendHistPattern=h_TQDC_BC.*
listQa=qa.list
displayMacro=display.C
displayMacro=display.C
jobName=digi_qa
logDir=/scratch2/ogolosov/log

export nEvents=0
export digiPathScratch=/scratch2/ogolosov/bmndata/run8/digi
export digiPathEos=/eos/nica/mpd/users/ogolosov/bmndata/run8/digi
export qaPathScratch=/scratch2/ogolosov/bmndata/run8/qa
export qaPathEos=/eos/nica/mpd/users/ogolosov/bmndata/run8/qa
trendFile=$qaPathEos/trends.root
trendOutputFile=$qaPathEos/BC_trends.root
export digiMacro=/lhep/users/ogolosov/soft/bmn_trigger_monitoring/BmnDataToRoot.C
export qaMacro=/lhep/users/ogolosov/soft/bmn_trigger_monitoring/plotTriggers.C
export listUndone=${listRaw}_undone

mkdir -pv $digiPathScratch
mkdir -pv $qaPathScratch
mkdir -pv $digiPathEos
mkdir -pv $qaPathEos

. /cvmfs/nica.jinr.ru/centos7/bmn/env.sh
. /cvmfs/nica.jinr.ru/centos7/bmnroot/dev/bmnroot_config.sh

while [ true ];do
  if [ $(qstat | grep "$jobName.*qw" -c) == 0 ];then
    rm $listUndone
    find $rawPath -type f -newermt "$startTime" -name "${pattern}.data" -size +1G > $listRaw
    firstRun=$(echo $(basename $(head -n1 $listRaw)) | grep -E [0-9]{4} -o | awk '{print $1}')
    
    for raw in $(cat $listRaw); do 
      qa=$(basename $raw)
      qa=$qaPathEos/${qa/.data/.qa.root}
      [ ! -e $qa ] && ls $raw >> $listUndone 
    done
    
    qsub -N $jobName -t 1-$(cat $listUndone | wc -w) -o ${logDir} -e ${logDir} -V run_digi_qa.batch.sh
    
    #plot trends
    find $qaPathEos -type f -name "${pattern}.qa.root" -size +124k > $listQa
    for f in $(cat $listQa);do
      runId=$(echo $(basename $f) | grep -E [0-9]{4} -o | awk '{print $1}')
  [ $runId -lt $firstRun ] && sed -i "s~$f~~" $listQa && sed -i '/^$/d' $listQa
    done
    time root -b -q $trendMacro"(\"$listQa\", \"$trendHistPattern\", \"$trendFile\")"
    time root -b -q $displayMacro"(\"$trendFile\", \"\", \"$trendOutputFile\")"
  fi
  sleep $sleepDuration
done
