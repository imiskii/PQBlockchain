##
# Makefile - general project commands
# Author: Michal Ľaš
# date: 16.01.2024
#

# DOXYGEN PATH
DOXYGEN=doc/doxygen
# AUTHOR NAME
LOGIN=xlasmi00
# PROJECT NAME
PROJECT=PQBlockchain
# EXECUTABLES NAMES
EX=./build/src/App/pqb
# REMOVE
RM=$(DOXYGEN)/html *.zip build/*

.PHONY: clean pack doc opendoc configure compile run

##
# MAKE & RUN

configure:
	cmake -DENABLE_DEBUG=OFF -S . -B build/

configureg:
	cmake -DENABLE_DEBUG=ON -S . -B build/

compile:
	make -C build/ -j12

run:
	$(EX) $(ARGS)

# generate configuration files in scripts/confs with txt file in scripts/confs.txt. Require one program argument, which is the name of signature algorith
genconfs:
	./build/src/App/conf-generator $(ARGS)
	rm -rf scripts/confs/
	mv tmp/confs scripts/confs
	mkdir tmp/confs

##################################################################

##
# RUN TEST/ALLTESTS

test:
	GTEST_COLOR=1 ctest -R $(ARGS) --test-dir build --output-on-failure -j12


alltests:
	GTEST_COLOR=1 ctest --test-dir build --output-on-failure -j12


##################################################################

##
# Docker

# Create the docker image
dimagec:
	docker build -t pqb_image .

# Delete the docker image
dimager:
	docker rmi pqb_image

# Compose docker containers. Requires one argument and it is the file which .yaml (docker-compose) should it use
dcomposeup:
	docker-compose -f $(ARGS) up --rm

drm:
	docker rm -f $$(sudo docker ps -aq)

# Attach to docker image
datt:
	docker attach node$(ARGS)

# Copy log file from container
dlog:
	docker cp node$(ARGS):/PQB/tmp/log.txt .

##################################################################

##
# CLEN

clean:
	rm -rf $(RM)

##################################################################

##
# PACK

pack: clean
	git archive --format=zip HEAD -o $(LOGIN)_$(PROJECT).zip

##################################################################

##
# MAKE DOCUMENTATION

doc:
	doxygen $(DOXYGEN)/Doxyfile


opendoc: doc
	firefox $(DOXYGEN)/html/index.html