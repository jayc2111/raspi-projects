#!/bin/bash

SUNWAIT_CMD=/home/pi/sunwait-20041208/sunwait

cmdOFF()
{
  # https://www.timeanddate.com/sun/germany/lampertheim
  $SUNWAIT_CMD sun down -$1 49.599727N 8.466599E;echo "this is the OFF command"
}

cmdON()
{
  # https://www.timeanddate.com/sun/germany/lampertheim
  $SUNWAIT_CMD sun up +$1 49.599727N 8.466599E;echo "this is the ON command"
}


tmp=$($SUNWAIT_CMD -p 49.599727N 8.466599E | grep 'Day length')
part="$(echo "$tmp" | tr -cd '[:digit:]' )"
echo "part $part"

res=$(/home/pi/terrarium/calcOffset $part)
part="$(echo "$res" | awk '{print $1}' )"
echo "part1 $part"
part="$(echo "$res" | awk '{print $2}' )"
echo "part2 $part"


# Sun rises 0744 CET, sets 1736 CET
#cmdON  4:32:00
#cmdOFF 5:19:00

echo "========" > loop.log
#for i in `seq 930 1630`
for i in `seq 930 959`
do
  res=$(/home/pi/terrarium/calcOffset $i)
  echo "[$i] $res" >> loop.log
done


echo "--- EOS ---"
