#
# Distribution build rules
#

zipdir          := $(BUILDDIR)/zip

$(zipdir)/:
	$(V)mkdir -p $@

$(zipdir)/iidx.zip: \
		iidx \
		| $(zipdir)/
	$(V)echo ... $@
	$(V)zip -r $@ $^

$(zipdir)/misc.zip: \
		misc \
		| $(zipdir)/
	$(V)echo ... $@
	$(V)zip -r $@ $^

$(BUILDDIR)/bemanitools-supplement.zip: \
		$(zipdir)/iidx.zip \
		$(zipdir)/misc.zip \
		README.md \
		version \

	$(V)echo ... $@
	$(V)zip -j $@ $^

all: $(BUILDDIR)/bemanitools-supplement.zip
