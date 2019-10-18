V               ?= @
BUILDDIR        ?= build

gitrev          := $(shell git rev-parse HEAD)

all:
# Generate a version file to identify the build
	@echo "$(gitrev)" > version

.PHONY: clean

clean:
	$(V)rm -rf $(BUILDDIR)

include Module.mk
