MAINTAINERCLEANFILES = Makefile.in

bin_PROGRAMS = edoc
efl_cflags_safe = '@EFL_CFLAGS@'

AM_CPPFLAGS = -DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
-I$(top_builddir)/src/bin/ \
-I$(top_srcdir)/src/bin/ \
-I$(top_builddir)/src/lib/ \
-I$(top_srcdir)/src/lib/ \
-DEFL_BETA_API_SUPPORT \
-DEFL_CFLAGS=\"$(efl_cflags_safe)\" \
@EFL_CFLAGS@

edoc_SOURCES = \
edoc_main.c \
edoc_search.c \
edoc_private.h

edoc_LDADD = @EFL_LIBS@ $(top_builddir)/src/lib/libedoc.la

localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

EXTRA_DIST = edoc_private.h

