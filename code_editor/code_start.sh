#!/bin/bash
#  Shell script to start code editor, and keep it running.

cd `dirname $0`

(
while [ true ]
do
	mkdir logs
	date >> logs/log
	echo "Running code_start.sh by "`id` >> logs/log
	chown no_priv:no_priv logs

	su no_priv ./code_editor 2>&1 >> logs/log
	echo "ERROR -- code_editor killed off somehow" >> logs/log
	sleep 10
done
) &

