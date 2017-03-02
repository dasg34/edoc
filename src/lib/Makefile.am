MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
-I$(top_builddir)/src/lib \
-DPACKAGE_LIB_DIR=\"$(libdir)\" \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
-DEFL_BETA_API_SUPPORT \
@EFL_CFLAGS@ \
-DEFL_EDOC_BUILD

lib_LTLIBRARIES = libedoc.la

includes_HEADERS = edoc.h
includesdir = $(includedir)/edoc-@VMAJ@

libedoc_la_SOURCES = \
${efl_name}_private.h \
edoc.h \
edoc.c

libedoc_la_LIBADD = @EFL_LIBS@ -lm
libedoc_la_LDFLAGS = -no-undefined @EFL_LTLIBRARY_FLAGS@
