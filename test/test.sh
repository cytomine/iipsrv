#!/usr/bin/env bash

VERSION=dev

docker stop iipCyto
docker rm iipCyto

tar -cvf v$VERSION.tar.gz ../
cp v$VERSION.tar.gz ../docker/v$VERSION.tar.gz
docker build --build-arg FROM_NAMESPACE=cytomineuliege --build-arg FROM_VERSION=v1.2.0 --build-arg RELEASE_PATH="." --build-arg VERSION=$VERSION -t iip-cyto-test ../docker

docker create --name iipCyto \
-v /data/images:/data/images \
--restart=unless-stopped \
-p 80:80 \
iip-cyto-test

docker cp nginx.conf.sample iipCyto:/tmp/nginx.conf.sample
docker cp iip-configuration.sh iipCyto:/tmp/iip-configuration.sh
docker start iipCyto