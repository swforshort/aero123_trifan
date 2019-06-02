CC := g++ 
SRCDIR := src
BUILDDIR := build
BINDIR := bin

build:
	$(CC) -o $(BINDIR)/trifan $(SRCDIR)/TrifanHarness.cpp

clean:
	@echo " Cleaning..."; 
	rm $(BUILDDIR)/*
	rm $(BINDIR)/*

.PHONY: clean build