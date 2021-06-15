#! /bin/bash
RUN=./Code/parser

echo -e "\e[1;31m This is red text \e[0m"
for fmm in ./Test/*.cmm; do
    $RUN $fmm ${fmm%.cmm}.ir
done