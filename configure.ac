EFL_VERSION([0], [0], [1], [dev])
AC_INIT([edoc], [efl_version], [enlightenment-devel@lists.sourceforge.net])

AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_HEADERS([config.h])

AC_CANONICAL_BUILD
AC_CANONICAL_HOST

AM_INIT_AUTOMAKE([1.10 dist-bzip2 -Wall color-tests])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
AM_PROG_AR

# this will speed up libtool checks
LT_PREREQ([2.2])
LT_INIT([win32-dll pic-only])
EFL_INIT

AC_PROG_CC
AM_PROG_CC_C_O
EFL_ATTRIBUTE_UNUSED
ELM_QUICKLAUNCH
EFL_COMPILER_FLAG([-Wall])
EFL_COMPILER_FLAG([-W])

# Checks for header files.
EFL_CHECK_PATH_MAX

# Check for EFL
PKG_CHECK_MODULES([EFL],
[
 eina >= 1.8.0
 evas >= 1.8.0
 ecore >= 1.8.0
 edje >= 1.8.0
 elementary >= 1.8.0
])

# Check for Clang
have_clang="no"
AC_CHECK_HEADERS([clang-c/Index.h], [have_clang="yes"], [have_clang="no"])
if test "${have_clang}" = "yes"; then
   ldflags="$LDFLAGS"
   LDFLAGS+=" -L${libdir}/llvm"
   AC_CHECK_LIB([clang], [clang_Cursor_getArgument],
      [
       AC_DEFINE([HAVE_LIBCLANG], [1], [Set to 1 if clang is found])
       EFL_LIBS="${EFL_LIBS} -lclang"
      ],
      [have_clang="no"])
fi
if test "${have_clang}" = "no"; then
   AC_MSG_ERROR([Could not find a usable libclang.])
fi
CLANG_INCLUDE=`clang -E - -v < /dev/null 2>&1 | grep "^ /usr" | grep clang`
AC_SUBST([CLANG_INCLUDE])

# Check for tests and coverage
AC_ARG_WITH([tests],
   [AC_HELP_STRING([--with-tests=none|regular|coverage],
                   [choose testing method: regular, coverage or none.
                    @<:@default=none@:>@])],
   [build_tests=${withval}],
   [build_tests=auto])

EFL_TESTS([${build_tests}])

# doxygen program for documentation building
EFL_CHECK_DOXYGEN([build_doc="yes"], [build_doc="no"])

# Check edje_cc
EFL_WITH_BIN([edje], [edje-cc], [edje_cc])

# Checks for library functions.
AC_CHECK_FUNCS([setlocale])

AM_GNU_GETTEXT_VERSION([0.17])
AM_GNU_GETTEXT([external])

AC_CONFIG_FILES([
Makefile
data/Makefile
data/desktop/Makefile
data/desktop/edoc.desktop
data/themes/Makefile
data/themes/default/Makefile
doc/Makefile
po/Makefile.in
src/Makefile
src/bin/Makefile
src/lib/Makefile
src/tests/Makefile
doc/edoc.1
])
AC_OUTPUT

echo
echo
echo "------------------------------------------------------------------------"
echo "$PACKAGE_NAME $PACKAGE_VERSION"
echo "------------------------------------------------------------------------"
echo
echo "Compilation..............: make (or gmake)"
echo "  CFLAGS.................: $CFLAGS"
echo "  edje_cc................: ${edje_cc}"
echo "  libclang...............: ${have_clang}"
echo
echo "Building documentation...: ${build_doc}"
echo "Building tests...........: ${have_tests}"
echo "Generate coverage .......: ${have_lcov}"
echo
echo "Installation.............: make install (as root if needed, with 'su' or 'sudo')"
echo "  prefix.................: $prefix"
echo
