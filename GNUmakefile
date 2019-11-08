V               ?= @
BUILDDIR        ?= build

gitrev          := $(shell git rev-parse HEAD)

all: print-building

.PHONY: clean print-building

print-building:
	@echo "Building gitrev "$(gitrev)"..."

clean:
	$(V)rm -rf $(BUILDDIR)
	
# Generate a version file to identify the build
version:
	@echo "$(gitrev)" > version

include Module.mk
