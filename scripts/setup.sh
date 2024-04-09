#!/bin/bash

# Author: Michal Ľaš
# Date: 08.04.2024

# TODO: YAML file genarator for algorithm setup


printHelp()
{
    echo "This script will set up docker containers with PQ Blockchain applications running inside them."
    echo "Required arguments:"
    echo "<algorithm_name>: name of digital signature algorithm which will be used"
    echo "<generation_conf>: configuration .txt file for generating wallet files"
    echo "<docker_compose_yaml>: .yaml file with docker compose configurations"
}


IMAGE_NAME="pqb_image"

# Argument parsing

if [ "$#" -ne 3 ]; then
    printHelp
    exit 1
fi

ALGORITHM_NAME="$1"
CONFIG_SOURCE="$2"
DOCKER_COMPOSE_YAML="$3"


# Make configuration files
./build/src/App/conf-generator $ALGORITHM_NAME < $CONFIG_SOURCE
rm -rf scripts/confs/
mv tmp/confs scripts/confs
mkdir tmp/confs

# Create docker image
if docker images "$IMAGE_NAME" | grep -q "$IMAGE_NAME"; then
    docker rmi $IMAGE_NAME
fi

docker build -t $IMAGE_NAME . 

# Run Docker compose
docker-compose -f $DOCKER_COMPOSE_YAML up
