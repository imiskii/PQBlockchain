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
RM=$(DOXYGEN)/html *.zip
# Temporary configuration folder
TMP=tmp

.PHONY: clean pack doc opendoc configure configureg compile run

all:
	make configure
	make compile


##################################################################

##
# SET UP

setup:
	mkdir $(TMP)
	mkdir build

##################################################################

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

##################################################################

##
# RUN TEST/ALLTESTS (has to ne -j1 because of Storage and Wallet tests)

test:
	GTEST_COLOR=1 ctest -R $(ARGS) --test-dir build --output-on-failure -j1


testall:
	GTEST_COLOR=1 ctest --test-dir build/tests --output-on-failure -j1


##################################################################

##
# Docker

# Create the docker image
dimagec:
	docker build -t pqb_image .

# Delete the docker image
dimager:
	docker rmi pqb_image

# Compose docker containers. Requires one argument and it is the .yaml file which docker-compose should use to compose containers
dcomposeup:
	docker-compose -f $(ARGS) up --rm

# Delete created containers
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
# CLEAN

# Clean just temorary file creted by application
clean:
	rm -rf $(TMP)/*
	rm -rf $(RM)

# Clean whole build and all generated files
cleanall:
	make clean
	rm -rf build/*


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