#!/bin/bash

# Author: Michal Ľaš
# Date: 08.04.2024


IMAGE_NAME="pqb_image"



# Clear configuration files
rm -rf scripts/confs/

# Delete containers
docker rm -f $(docker ps -aq)

# Delete docker image
docker rmi $IMAGE_NAME