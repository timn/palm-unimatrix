#
# File: Makefile
#
# Written by Tim Niemueller <tim@niemueller.de>
# Created on 2002/05/07
#

# ressource path. some people might want to change this to another
# directory than the source files
R = ./

include *.mk

CC = m68k-palmos-gcc
ifdef DEBUG
CFLAGS = -g
LDFLAGS = -g
else
CFLAGS = -O2 -Wall -Os
endif
MULTIGEN = m68k-palmos-multigen

LANGUAGE ?= german
SHORTLANGUAGE := $(shell grep "$(LANGUAGE)" languages | awk -F : '{ print $$1 }')
PRINTLANGUAGE := $(shell echo "$(LANGUAGE)" | awk '{ print sprintf("%s%s", toupper(substr($$1, 1, 1)), substr($$1, 2)) }')

LANGUAGES := $(shell awk -F \: '{ print $$2 }' languages)
PILRC = pilrc
PFLAGS = -q -I $(R) -L $(LANGUAGE)


PILOTRATE=115200
PILOTXFER=/usr/bin/pilot-xfer
PILOT_PORT=/dev/ttyUSB1

SILENT=@
ECHO=$(SILENT)echo

OBJECT_FILES=$(shell ls *.c | sed -e "s/.c$$/.o/") $(PROGNAME)-sections.o
OBJECTS=$(OBJECT_FILES:%=.obj/%)

RSCBINS_FILES:=$(shell ls resources/*.rcp.in | sed -e "s/rcp.in/bin/g")
RSCBINS := $(RSCBINS_FILES:resources/%=.obj/%)
RSCBINSPRINT:=$(shell echo "$(RSCBINS)" | sed -e "s/resources\///g")

$(PROGNAME).prc: setup $(PROGNAME) $(RSCBINS) $(PROGNAME).mk $(PROGNAME).def
	$(ECHO) "--> Creating PRC file"
	$(SILENT)build-prc $(PROGNAME).def "$(PROGDESC)" .obj/*.bin -o $(PROGNAME).prc
	$(ECHO) -e "\n\n================================================================================\n"
	$(ECHO) "RSC BINS: $(RSCBINSPRINT)"
	$(ECHO) "OBJECTS : $(OBJECTS)"
	$(ECHO) "COMPILER: $(CC) $(CFLAGS)"
	$(ECHO) "LANGUAGE: $(PRINTLANGUAGE) [$(SHORTLANGUAGE):$(LANGUAGE)]"
	$(SILENT)ls -l $(PROGNAME).prc | awk '{ print sprintf("PRC size: %s", $$5) }'
	$(ECHO) -e "\n================================================================================\n\n"

all: clean $(PROGNAME).prc

$(PROGNAME): $(OBJECTS) .obj/$(PROGNAME)-sections.ld
	$(ECHO) "--> Creating $(PROGNAME)"
	$(SILENT)$(CC) $(CFLAGS) $(OBJECTS) .obj/$(PROGNAME)-sections.ld -o $@


.obj/%.o: %.c
	$(ECHO) "--- Compiling $<"
	$(SILENT)$(CC) $(CFLAGS) -o $@ -c $<

.obj/%.bin: resources/%.rcp $(PROGNAME).h
	$(ECHO) "--- Compiling resource file $<"
	$(SILENT)$(PILRC) $(PFLAGS) $< .obj/

%.rcp: %.rcp.in
	$(ECHO) "--- Generating $@"
	$(SILENT)sed -e 's/##VERSION##/$(VERSION)/g' -e 's/##LANGUAGE##/$(PRINTLANGUAGE)/' \
        -e '/##TRANSLATION##/{' \
	-e 'r resources/translation.$(SHORTLANGUAGE)' -e 's/##TRANSLATION##//' \
	-e '}' \
        -e 's/##APPID##/$(APPID)/g' -e 's/##APPNAME##/$(PROGDESC)/g' \
        < $< > $@

.obj/$(PROGNAME)-sections.o: .obj/$(PROGNAME)-sections.s
	$(ECHO) "--- Compiling sections information"
	$(SILENT)$(CC) -o $@ -c $<

.obj/$(PROGNAME)-sections.s .obj/$(PROGNAME)-sections.ld: $(PROGNAME).def
	$(ECHO) "--- Generating sections file"
	$(SILENT)$(MULTIGEN) -b .obj/$(PROGNAME)-sections $(PROGNAME).def

install: clean all
	$(ECHO) -e "No going to install $(PROGDESC) on PalmOS device\n"
	$(ECHO) -e "If you have a USB device press NOW the HotSync Button"
	$(ECHO) -e "and THEN press enter to continue.\n"
	$(SILENT)read foo </dev/tty
	$(SILENT)PILOTRATE=$(PILOTRATE) $(PILOTXFER) -p $(PILOT_PORT) -i $(PROGNAME).prc

setup:
	$(SILENT)if [ ! -d .obj ]; then \
		echo "-- Creating .obj"; \
		mkdir .obj; \
	fi

clean:
	$(ECHO) "--- Cleaning up"
	$(SILENT)-rm -f $(PROGNAME) $(PROGNAME).prc .obj/*.o .obj/*.bin .obj/*.ld .obj/*.s

dist:
	$(ECHO) "--- Creating dist directory '$(PROGNAME)-$(VERSION)_dist'"
	$(SILENT)mkdir -p $(PROGNAME)-$(VERSION)_dist
	$(SILENT)for L in $(LANGUAGES); do LANGUAGE=$$L $(MAKE) --no-print-directory package; done
	$(SILENT)mv -f $(PROGNAME)-$(VERSION)-* $(PROGNAME)-$(VERSION)_dist

package: clean all
	$(ECHO) -e "\n\n--> Packaging $(PROGNAME) for language $(LANGUAGE) ($(SHORTLANGUAGE))\n\n"

	$(SILENT)mkdir -p $(PROGNAME)-$(VERSION)-$(SHORTLANGUAGE)
	$(SILENT)cp -ar docs $(PROGNAME)-$(VERSION)-$(SHORTLANGUAGE)
	$(SILENT)rm -rf $(PROGNAME)-$(VERSION)-$(SHORTLANGUAGE)/docs/CVS
	$(SILENT)cp $(PROGNAME).prc $(PROGNAME)-$(VERSION)-$(SHORTLANGUAGE)
	$(SILENT)tar cvfz $(PROGNAME)-$(VERSION)-$(SHORTLANGUAGE).tar.gz $(PROGNAME)-$(VERSION)-$(SHORTLANGUAGE)
	$(SILENT)zip -r $(PROGNAME)-$(VERSION)-$(SHORTLANGUAGE).zip $(PROGNAME)-$(VERSION)-$(SHORTLANGUAGE)
	$(SILENT)rm -rf $(PROGNAME)-$(VERSION)-$(SHORTLANGUAGE)

upload: dist
	$(ECHO) "--- Generating PHP version file"
	$(SILENT)echo -e "<?php\n$$version=\"$(VERSION)\";\n?>" > webpage/version.inc.php
	$(ECHO) "--> Uploading to $(SSH_HOST)"
	$(SILENT)scp -r $(PROGNAME)-$(VERSION)_dist $(SSH_USER)@$(SSH_HOST):$(SSH_PATH)
	$(SILENT)scp -r webpage/*.inc.php docs/CHANGES $(SSH_USER)@$(SSH_HOST):$(SSH_PATH)
	$(SILENT)rm webpage/version.inc.php

status:
	cvs status | grep File | grep -v Up-to-date

