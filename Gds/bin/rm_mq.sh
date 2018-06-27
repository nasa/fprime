#!/bin/bash

mq_list=`ipcs -q | sed -n '/^q/p' | grep 0x00000000 | sed 's/^q *//g' | cut -f 1 -d ' '`
for i in $mq_list
do
   ipcrm -q $i 
done
