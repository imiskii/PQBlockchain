#!/bin/bash

# Author: Michal Ľaš
# Date: 08.04.2024



printHelp()
{
    echo "This script will run defined commands inside set up docker containers and then clean used stuff ofterwards"
    echo "Required arguments:"
    echo "<algorithm_name>: name of digital signature algorithm which will be used"
    echo "<docker_compose_yaml>: .yaml file with docker compose configurations"
}


#IMAGE_NAME="pqb_image"

# Argument parsing

#if [ "$#" -ne 2 ]; then
#    printHelp
#    exit 1
#fi

#ALGORITHM_NAME="$1"
#DOCKER_COMPOSE_YAML="$2"

# Run setup
#./scripts/setup.sh $ALGORITHM_NAME "scripts/3conf.txt" $DOCKER_COMPOSE_YAML


# Run exercise


#docker attach "node2" &
#docker attach "node3" &



# Run teardown
#./scripts/teardown.sh