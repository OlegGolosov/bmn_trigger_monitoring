#!/bin/bash

listRaw=$1

export nEvents=0
export digiPathScratch=/scratch2/ogolosov/bmndata/run8/digi
export digiPathEos=/eos/nica/mpd/users/ogolosov/bmndata/run8/digi
export qaPathScratch=/scratch2/ogolosov/bmndata/run8/qa
export qaPathEos=/eos/nica/mpd/users/ogolosov/bmndata/run8/qa
export digiMacro=/lhep/users/ogolosov/soft/bmn_trigger_monitoring/BmnDataToRoot.C
export qaMacro=/lhep/users/ogolosov/soft/bmn_trigger_monitoring/plotTriggers.C
export listUndone=${listRaw}_undone

mkdir -pv $digiPathScratch
mkdir -pv $qaPathScratch
mkdir -pv $digiPathEos
mkdir -pv $qaPathEos

rm $listUndone
for raw in $(cat $listRaw); do 
  qa=$(basename $raw)
  qa=$qaPathEos/${qa/.data/.qa.root}
  [ ! -e $qa ] && ls $raw >> $listUndone 
done

logDir=/scratch2/ogolosov/log
qsub -N digi_qa -t 1-$(cat $listUndone | wc -w) -o ${logDir} -e ${logDir} -V run_digi_qa.batch.sh
