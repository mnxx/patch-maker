##############################################################
# MAKEFILE                                                   #
#                                                            #
# CREATED BY:   Franck Maillot, Manuel Roth                  #
#                                                            #
# EXECUTABLES IN /BIN:   /bin/computePatchOpt                 #
##############################################################

CC=gcc
LATEXC=pdflatex
DOCC=doxygen
CFLAGS=-g -Wall 

REFDIR=.
SRCDIR=$(REFDIR)/src
BINDIR=$(REFDIR)/bin
DOCDIR=$(REFDIR)/doc
REPORTDIR=$(REFDIR)/rapport

LATEXSOURCE=$(wildcard $(REPORTDIR)/*.tex)
CSOURCE1=$(wildcard $(SRCDIR)/applyPatch.c)
CSOURCE2=$(wildcard $(SRCDIR)/computePatchOpt.c)
PDF=$(LATEXSOURCE:.tex=.pdf)

# MAKE OPTIONS:
# make all / make binary / make report / make doc
all: binary report doc 

$(BINDIR)/applyPatch: $(CSOURCE1)
	$(CC) $(CFLAGS)  $^ -o $@

$(BINDIR)/computePatchOpt: $(CSOURCE2)
	$(CC) $(CFLAGS)	 $^ -o $@

%.pdf: $(LATEXSOURCE)
	$(LATEXC) -output-directory $(REPORTDIR) $^ 

$(DOCDIR)/index.html: $(SRCDIR)/Doxyfile $(CSOURCE) 
	$(DOCC) $(SRCDIR)/Doxyfile

binary: $(BINDIR)/applyPatch $(BINDIR)/computePatchOpt

report: $(PDF) 

doc: $(DOCDIR)/index.html

# CLEAN UP
clean:
	rm -rf $(DOCDIR) $(BINDIR)/* $(REPORTDIR)/*.aux $(REPORTDIR)/*.log  $(REPORTDIR)/rapport.pdf 

.PHONY: all doc binary report 
