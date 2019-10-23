#!/bin/bash

echo "Script per analisi dati"

for ((i=2; i<4; i++))
do
string1="ANAL Th229notte"
string2=".root outanal"
string3=".root"
string4=$string1$i$string2$i$string3
./$string4
done

RET=$?
echo "exit status: $RET"

