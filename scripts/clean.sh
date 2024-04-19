#!/bin/bash

# Author: Michal Ľaš
# Date: 08.04.2024


IMAGE_NAME="pqb_image"


# Clean configuration files
rm -rf scripts/confs/

# Clean Commands
rm scripts/commands.txt

# Clean compose.yaml
rm -f scripts/compose.yaml

# Delete containers
docker rm -f $(docker ps -aq)

# Delete docker image
docker rmi $IMAGE_NAME