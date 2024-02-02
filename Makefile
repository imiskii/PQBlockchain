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
	cmake -S . -B build/

compile:
	make -C build/

run:
	$(EX) $(ARGS)

##################################################################

##
# RUN TEST



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

doc: clean
	doxygen $(DOXYGEN)/Doxyfile


opendoc: doc
	firefox $(DOXYGEN)/html/index.html