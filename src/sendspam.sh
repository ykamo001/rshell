#! /bin/bash

r=$1
s=$2

export bodytemp=""

while read -e body; do bodytemp+="$bodytemp $body"; done

./takebody.sh $r $s | telnet #> /dev/null
