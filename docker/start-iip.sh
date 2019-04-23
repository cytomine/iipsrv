export VERBOSITY=10
export LOGFILE=/tmp/iip.out
. /tmp/iip-configuration.sh

PORT=9000
COUNTER=0
while [  $COUNTER -lt $NB_IIP_PROCESS ]; do
    echo "spawn process"
    spawn-fcgi -f /opt/iipsrv/src/iipsrv.fcgi -a 127.0.0.1 -p $(($PORT+$COUNTER))
    let COUNTER=COUNTER+1
done