MAINTAINERCLEANFILES = Makefile.in
CLEANFILES = check-results.xml

if EFL_HAVE_TESTS

check_PROGRAMS = edoc_tests

edoc_tests_SOURCES = test_edoc.c
edoc_tests_CPPFLAGS = -I$(top_builddir)/src/lib/ \
-I$(top_srcdir)/src/lib \
-DPACKAGE_TESTS_DIR=\"$(top_srcdir)/src/tests/\" \
-DPACKAGE_BUILD_DIR=\"`pwd`/$(top_builddir)/src/tests/\" \
@EFL_CFLAGS@ \
@CHECK_CFLAGS@
edoc_tests_LDADD = @CHECK_LIBS@ $(top_builddir)/src/lib/libedoc.la
edoc_tests_DEPENDENCIES = $(top_builddir)/src/lib/libedoc.la

endif

EXTRA_DIST = test_edoc.c
