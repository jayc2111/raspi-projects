#!/bin/bash
CALSUN_CMD=/home/pi/terrarium/main/CalSunDay
SUNWAIT_CMD=/home/pi/sunwait-20041208/sunwait

## start of the script
#######################

# extract start and stop time from sunwait printout
tmp=$($SUNWAIT_CMD -p 49.902877N 8.668429E | grep 'Sun rises')
part=$(echo "$tmp" | tr -cd '[:digit:]' )
endTime="${part:4:4}"
startTime="${part:0:4}"

# determine start and endtime applying dynamic offset
times=$($CALSUN_CMD  $startTime $endTime)
endTime=$(echo "$times" | awk '{print $2}')
startTime=$(echo "$times" | awk '{print $1}')
echo -e "Terrarium rises $startTime, sets $endTime"
