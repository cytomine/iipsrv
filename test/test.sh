#!/usr/bin/env bash

VERSION=dev

docker stop iipCyto
docker rm iipCyto

tar -cvf v$VERSION.tar.gz ../
cp v$VERSION.tar.gz ../docker/v$VERSION.tar.gz
docker build --build-arg FROM_NAMESPACE=cytomineuliege --build-arg FROM_VERSION=v1.2.0 --build-arg RELEASE_PATH="." --build-arg VERSION=$VERSION -t iip-cyto-test ../docker

docker create --name iipCytoTest \
-v /data/images:/data/images \
--restart=unless-stopped \
-p 8888:80 \
iip-cyto-test

docker cp nginx.conf.sample iipCytoTest:/tmp/nginx.conf.sample
docker cp iip-configuration.sh iipCytoTest:/tmp/iip-configuration.sh
docker start iipCytoTest