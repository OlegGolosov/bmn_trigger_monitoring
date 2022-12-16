#/bin/bash

qaFile=qa.root

#for histSet in \
for histSet in \
  beamTriggerTimes\
  beamTriggerAmps\
  bdSiTimeCount\
  bdAmps\
  siAmps\
  BDcountVsFW\
  BDampVsFW\
  SIcountVsFW\
  SIampVsFW\
  FWvsFW\
;do
  root -l display.C"(\"$qaFile\",\"$histSet\")"&
done
