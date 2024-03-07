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
EX=./build/src/App/main
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

##################################################################

##
# RUN TEST/ALLTESTS

test:
	GTEST_COLOR=1 ctest -R $(ARGS) --test-dir build --output-on-failure -j12


alltests:
	GTEST_COLOR=1 ctest --test-dir build --output-on-failure -j12


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