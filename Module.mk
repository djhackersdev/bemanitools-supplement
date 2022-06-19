#
# Distribution build rules
#

zipdir          := $(BUILDDIR)/zip

$(zipdir)/:
	$(V)mkdir -p $@

$(zipdir)/ddr.zip: \
		ddr \
		| $(zipdir)/
	$(V)echo ... $@
	$(V)zip -r $@ $^

$(zipdir)/gfdm.zip: \
		gfdm \
		| $(zipdir)/
	$(V)echo ... $@
	$(V)zip -r $@ $^

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

$(zipdir)/popn.zip: \
		popn \
		| $(zipdir)/
	$(V)echo ... $@
	$(V)zip -r $@ $^

$(BUILDDIR)/bemanitools-supplement.zip: \
		$(zipdir)/ddr.zip \
		$(zipdir)/gfdm.zip \
		$(zipdir)/iidx.zip \
		$(zipdir)/misc.zip \
		$(zipdir)/popn.zip \
		README.md \
		version \

	$(V)echo ... $@
	$(V)zip -j $@ $^

all: $(BUILDDIR)/bemanitools-supplement.zip
