#!/bin/bash

UUID=a495bb30-c5b1-4b44-b512-1370f02d74de

while true; do
  major=`seq 0 185 | sort -R | head -n 1`
  minor=`seq 990 1120 | sort -R | head -n 1`

  echo "Sending beacon with temp $major and gravity $minor"

  mbeacon -uuid ${UUID} -major ${major} -minor ${minor} & 

  sleep 0.250
 
  pkill mbeacon

  sleep 5  
done
