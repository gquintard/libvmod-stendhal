AM_CPPFLAGS = @VMOD_INCLUDES@ -Wall -Werror

TESTS_ENVIRONMENT = PATH=$(LIBVARNISHAPI_BINDIR):$(LIBVARNISHAPI_SBINDIR):$$PATH

vmoddir = @VMOD_DIR@
vmod_LTLIBRARIES = libvmod_stendhal.la

libvmod_stendhal_la_LDFLAGS = -module -export-dynamic -avoid-version -shared

libvmod_stendhal_la_SOURCES = \
	vmod_stendhal.c

nodist_libvmod_stendhal_la_SOURCES = \
	vcc_if.c \
	vcc_if.h

dist_man_MANS = vmod_stendhal.3

vmod_stendhal.3: vmod_stendhal.man.rst
	${RST2MAN} $< $@

vmod_stendhal.lo: vcc_if.c

vmod_stendhal.man.rst vcc_if.c: vcc_if.h

vcc_if.h: @VMODTOOL@ $(top_srcdir)/src/vmod_stendhal.vcc
	@PYTHON@ @VMODTOOL@ $(top_srcdir)/src/vmod_stendhal.vcc

VTC_LOG_COMPILER = $(VARNISHTEST) -v \
	-Dvmod_topbuild=$(abs_top_builddir)
TEST_EXTENSIONS = .vtc
TESTS = @VMOD_TESTS@

@VMOD_TESTS@: vmod_stendhal.lo

EXTRA_DIST = \
	vmod_stendhal.vcc \
	$(VMOD_TESTS)

CLEANFILES = \
	$(builddir)/vcc_if.c \
	$(builddir)/vcc_if.h \
	$(builddir)/vmod_stendhal.rst \
	$(builddir)/vmod_stendhal.man.rst \
	$(dist_man_MANS)
