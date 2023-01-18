#!/bin/bash

echo GO!!!

. /cvmfs/nica.jinr.ru/centos7/bmn/env.sh
. /cvmfs/nica.jinr.ru/centos7/bmnroot/dev/bmnroot_config.sh

filenum=${SGE_TASK_ID}
raw=$(sed -n "$filenum, $filenum p" $listUndone)
digi=$(basename $raw)
digi=${digi/.data/.root}
qa=$(basename $raw)
qa=${qa/.data/.qa.root}

if [ ! -e $digiPathEos/$digi ]; then
  root -l -b -q $digiMacro"(\"$raw\",\"$digiPathScratch/$digi\",$nEvents)"
  mv -v $digiPathScratch/$digi $digiPathEos
fi

if [ -e $digiPathEos/$digi ]; then
  root -l -b -q $qaMacro"(\"$digiPathEos/$digi\",\"$qaPathScratch/$qa\", true)"
  mv -v $qaPathScratch/$qa $qaPathEos
fi
