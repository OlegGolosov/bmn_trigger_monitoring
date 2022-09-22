#!/bin/bash

list=$1
nEvents=0

outPathRaw=/scratch1/ogolosov/bmndata/run8/raw
outPathQa=/scratch1/ogolosov/bmndata/run8/qa
mkdir -pv $outPathRaw
mkdir -pv $outPathQa
for fInRaw in $(cat $list);do
  fOutRaw=${outPathRaw}/$(basename $fInRaw).root
  root -l -b -q BmnDataToRoot.C"(\"$fInRaw\",\"$fOutRaw\",$nEvents)"
  fOutQa=${outPathQa}/$(basename $fInRaw).qa.root
  root -l -b -q AmplitudesInBC.C"(\"$fOutRaw\",\"$fOutQa\")"
done
