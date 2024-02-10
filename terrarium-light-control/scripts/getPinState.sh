#!/bin/bash

#set -x
winterflag=0

GPIO_CMD=/usr/local/bin/gpio
CALSUN_CMD=/home/pi/terrarium/main/CalSunDay
TERRARIUM_CMD=/home/pi/terrarium/main/terrarium
SUNWAIT_CMD=/home/pi/sunwait-20041208/sunwait
SCRIPT_CMD=/home/pi/terrarium/main/scripts/getPinState.sh
LOG_FILE=/home/web-usr/logs/terrarium.txt

# helping functions
cmdOFF()
{
  $SUNWAIT_CMD sun down -$1 49.902877N 8.668429E;$SCRIPT_CMD
}

cmdON()
{
  $SUNWAIT_CMD sun up   +$1 49.902877N 8.668429E;$SCRIPT_CMD
}

check_day_len()
{ # check lenght ot the day for winterflag
  local tmp
  local part
  tmp=$($SUNWAIT_CMD -p 49.902877N 8.668429E | grep 'Day length')
  part="$(echo "$tmp" | tr -cd '[:digit:]' )"

  # "winter end": 12.02. 09:59 hours daylight
  # longest day:  20.06. 16:18 hours daylight
  # "summer end": 29.10. 10:10 hours daylight
  # shortest day: 21.12. 08:08 hours daylight

  if [ $((${part#0})) -lt  900 ]
  then
    echo -en "[w] "
    winterflag=1
  else
    echo -en "[s] "
  fi
  #echo -en "($part) "
}


## start of the script
#######################
# sync internal clock
# see https://blog.doenselmann.com/richtige-uhrzeit-raspberry-pi-mit-ntp/
echo -en "$(date +"%d/%m/%y-%X") "

# extract start and stop time from sunwait printout
tmp=$($SUNWAIT_CMD -p 49.902877N 8.668429E | grep 'Sun rises')
part=$(echo "$tmp" | tr -cd '[:digit:]' )
endTime="${part:4:4}"
startTime="${part:0:4}"

# determine start and endtime applying dynamic offset
times=$($CALSUN_CMD  $startTime $endTime)
endTime=$(echo "$times" | awk '{print $2}')
offs_str=$(echo "$times" | awk '{print $3}')
startTime=$(echo "$times" | awk '{print $1}')
#echo -en " $startTime/$endTime/$offs_str "

# check the Pin's state
PinState=$($GPIO_CMD -g read 18)

# check lenght ot the day for winterflag
check_day_len

# get current time
curTime=$(date +%H%M)
# add 1 minute to stop error message when dayligh decreases
curTime=$((${curTime#0}+1))

# check if we are in target time
if [[ ${curTime#0} -lt ${endTime#0} && ${curTime#0} -ge ${startTime#0} ]]
then
  # target time
  echo -en "$startTime <= $curTime < $endTime"
  if [[ (0 -eq winterflag) && (0 -ge $PinState) ]]
  then
    echo -ne "\tstart "
    $TERRARIUM_CMD 600
  else
    echo -e "\tlamp already on"
  fi
  # schedule sun-down
  cmdOFF $offs_str >> $LOG_FILE 2>&1 &
else
  # non target time
  echo -en "$endTime <= $curTime < $startTime"
  if [ 0 -lt $PinState ]
  then
    echo -ne "\tstop "
    $TERRARIUM_CMD -600
  else
    echo -e "\tlamp already off"
  fi
  # schedule sun-rise
  cmdON $offs_str >> $LOG_FILE 2>&1 &
fi
