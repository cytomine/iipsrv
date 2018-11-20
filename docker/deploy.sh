#!/bin/bash
#
# Copyright (c) 2009-2018. Authors: see NOTICE file.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# Logs
echo "/tmp/iip.out {"          > /etc/logrotate.d/iip
echo "  copytruncate"                   >> /etc/logrotate.d/iip
echo "  daily"                          >> /etc/logrotate.d/iip
echo "  rotate 14"                      >> /etc/logrotate.d/iip
echo "  compress"                       >> /etc/logrotate.d/iip
echo "  missingok"                      >> /etc/logrotate.d/iip
echo "  create 640 root root"           >> /etc/logrotate.d/iip
echo "  su root root"                   >> /etc/logrotate.d/iip
echo "}"                                >> /etc/logrotate.d/iip

# IIP configuration options
export VERBOSITY=10
export LOGFILE=/tmp/iip.out
. /tmp/iip-configuration.sh
sysctl -w net.core.somaxconn=2048

env

# Configure Nginx
PORT=9000
COUNTER=0
while [  $COUNTER -lt $NB_IIP_PROCESS ]; do
    sed -i "s/IIP_PROCESS/        	server 127.0.0.1:$(($PORT+$COUNTER)); \nIIP_PROCESS/g" /tmp/nginx.conf.sample
    let COUNTER=COUNTER+1
done
sed -i "s/IIP_PROCESS//g" /tmp/nginx.conf.sample
mv /tmp/nginx.conf.sample /usr/local/nginx/conf/nginx.conf
echo "start nginx"
/usr/local/nginx/sbin/nginx &

# Spawn FCGI processes
COUNTER=0
while [  $COUNTER -lt $NB_IIP_PROCESS ]; do
    echo "spawn process"
    spawn-fcgi -f /opt/iipsrv/src/iipsrv.fcgi -a 127.0.0.1 -p $(($PORT+$COUNTER))
    let COUNTER=COUNTER+1
done

tail -F /tmp/iip.out
