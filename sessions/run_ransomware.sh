#!/bin/bash

DURATION=$((60*10))
CHECK_DURATION=10
RANSOMWARE_ON=0 #off
TECHNIQUE_ON=0 #off
TECHNIQUE_START_TIME=$((60*1))
RANSOMWARE_START_TIME=$((60*3))
RANSOMWARE_STOP_TIME=$((60*7))
TECHNIQUE_STOP_TIME=$((60*9))
limit=19

COMMAND1="./btrace.sh /dev/sda"

for RUN_ID in $(seq 0 1 $limit)
do
  echo ""
  echo "Run: $RUN_ID"
  COMMAND2="./ransomRescue.out $RUN_ID"
  TIME="$(date +%s)"
  ./getFileExtents.py "$RUN_ID"
  free && sync && echo 3 > /proc/sys/vm/drop_caches && free
  sleep 15
  ./psutil_usages.py "$RUN_ID" "$TIME" "START"
  echo "$RUN_ID" > /var/www/html/upload/status
  CHECK_END_TIME=$(($TIME+$CHECK_DURATION))
  END_TIME=$(($TIME+$DURATION))
  TECHNIQUE_START=$(($TIME+$TECHNIQUE_START_TIME))
  RANSOMWARE_START=$(($TIME+$RANSOMWARE_START_TIME))
  RANSOMWARE_STOP=$(($TIME+$RANSOMWARE_STOP_TIME))
  TECHNIQUE_STOP=$(($TIME+$TECHNIQUE_STOP_TIME))
  RANSOMWARE_ENCRYPT_COMMAND="./encScript.py Test1234 $RUN_ID"
  while :
  do
    TIME="$(date +%s)"
    if [ "$TIME" -ge "$TECHNIQUE_STOP" ]
    then
      if [ "$TECHNIQUE_ON" -eq 1 ]
      then
        pkill -P $C1_PID
        kill -9 $C2_PID
        ./psutil_usages.py "$RUN_ID" "$TIME" "TECHNIQUE_STOP"
        CHECK_END_TIME=$(($TIME+$CHECK_DURATION))
        TECHNIQUE_ON=0
      fi
    elif [ "$TIME" -ge "$RANSOMWARE_STOP" ]
    then
      if [ "$RANSOMWARE_ON" -eq 1 ]
      then
        kill -9 $RANSOMWARE_PID
        ./psutil_usages.py "$RUN_ID" "$TIME" "RANSOMWARE_STOP-kill"
        CHECK_END_TIME=$(($TIME+$CHECK_DURATION))
        RANSOMWARE_ON=0
      fi
    elif [ "$TIME" -ge "$RANSOMWARE_START" ]
    then
      if [ "$RANSOMWARE_ON" -eq 0 ]
      then
        free && sync && echo 3 > /proc/sys/vm/drop_caches && free
        RANSOMWARE_ON=1
        bash -c "${RANSOMWARE_ENCRYPT_COMMAND}" &
        RANSOMWARE_PID=$!
        ./psutil_usages.py "$RUN_ID" "$TIME" "RANSOMWARE_START"
        CHECK_END_TIME=$(($TIME+$CHECK_DURATION))
      fi
    elif [ "$TIME" -ge "$TECHNIQUE_START" ]
    then
      if [ "$TECHNIQUE_ON" -eq 0 ]
      then
        TECHNIQUE_ON=1
        bash -c "${COMMAND1}" &
        C1_PID=$!
        bash -c "${COMMAND2}" &
        C2_PID=$!
        ./psutil_usages.py "$RUN_ID" "$TIME" "TECHNIQUE_START"
        CHECK_END_TIME=$(($TIME+$CHECK_DURATION))
      fi
    fi
    if [ $TIME -ge $END_TIME ]
    then
      ./psutil_usages.py "$RUN_ID" "$TIME" "STOP"
      break
    fi
    if [ $TIME -ge $CHECK_END_TIME ]
    then
      ./psutil_usages.py "$RUN_ID" "$TIME"
      CHECK_END_TIME=$(($TIME+$CHECK_DURATION))
    fi
  done
  rm /var/www/html/upload/status
  rm -rf /var/www/html/upload/files/
  cp -R /home/user/Desktop/files/ /var/www/html/upload/
  chmod -R 777 /var/www/html/upload/files/
  tree /media/user/VD/data | tail -n 1
  rm -rf /media/user/VD/data/
  mkdir /media/user/VD/data/
  sleep 15
done
