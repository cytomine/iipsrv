. /tmp/iip-configuration.sh

COUNT=$(ps aux | grep /opt/iipsrv/src/iipsrv.fcgi | grep -v grep | wc -l)
if [ $COUNT -lt $NB_IIP_PROCESS ]; then
    echo "$COUNT / $NB_IIP_PROCESS on $(date)"
    killall -9 /opt/iipsrv/src/iipsrv.fcgi
    /tmp/start-iip.sh
fi;
