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
    echo "<command_script>: script with commands that should be run by nodes (see commandScriptGeneral.txt)"
}


IMAGE_NAME="pqb_image"

# Argument parsing

if [ "$#" -ne 3 ]; then
    printHelp
    exit 1
fi

ALGORITHM_NAME="$1"
CONFIG_SOURCE="$2"
COMMAND_SCRIPT="$3"
DOCKER_COMPOSE_YAML="scripts/compose.yaml"


# Make configuration files
./build/src/App/conf-generator $ALGORITHM_NAME < $CONFIG_SOURCE
rm -rf scripts/confs/
mv tmp/confs scripts/confs
mkdir tmp/confs

# Generate command script
rm scripts/commands.txt
python3 scripts/commandGenerator.py scripts/confs/confs.txt $COMMAND_SCRIPT > scripts/commands.txt

# Create docker compose yaml
rm $DOCKER_COMPOSE_YAML
python3 scripts/yamlGenerator.py $CONFIG_SOURCE $ALGORITHM_NAME > $DOCKER_COMPOSE_YAML

# Create docker image
if docker images "$IMAGE_NAME" | grep -q "$IMAGE_NAME"; then
    docker rmi $IMAGE_NAME
fi

docker build -t $IMAGE_NAME .

# Run Docker compose
docker-compose -f $DOCKER_COMPOSE_YAML up
