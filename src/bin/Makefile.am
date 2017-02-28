MAINTAINERCLEANFILES = Makefile.in

bin_PROGRAMS = edoc

AM_CPPFLAGS = -DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
-I$(top_builddir)/src/bin/ \
-I$(top_srcdir)/src/bin/ \
-I$(top_builddir)/src/lib/ \
-I$(top_srcdir)/src/lib/ \
@EFL_CFLAGS@

edoc_SOURCES = edoc_main.c
edoc_LDADD = @EFL_LIBS@ $(top_builddir)/src/lib/libedoc.la

localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

EXTRA_DIST = edoc_private.h

