#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([libmca], [1.0.1], [michael.frechtling@sydney.edu.au])
AC_CONFIG_SRCDIR([src/mcalib.h])

#Setup automake
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE

#Libtool Iinitialisation
LT_INIT
AC_CONFIG_MACRO_DIR([m4])
AC_PROG_LIBTOOL

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CHECK_PROG(CILLY, cilly, cilly)
if test "x$CILLY" == x; then
	AC_MSG_ERROR(Could not find valid cilly installation - download and install from http://sourceforge.net/projects/cil/)
fi
AC_CHECK_PROG(M4, m4, m4)
if test "x$M4" == x; then
	AC_MSG_ERROR(Could not find valid m4 installation - please install using apt-get install m4)
fi
AC_CHECK_PROG(OCAML, ocaml, ocaml)
if test "x$OCAML" == x; then
	AC_MSG_ERROR(Could not find valid ocaml installation - please install using apt-get install ocaml)
fi
AC_CHECK_PROG(OCAMLFIND, ocamlfind, ocamlfind)
if test "x$OCAMLFIND" == x; then
	AC_MSG_ERROR(Could not find valid ocaml-findlib installation - please install using apt-get install ocaml-findlib)
fi
AC_CHECK_FUNCS([pow gettimeofday rand])
AC_CHECK_LIB([mpfr], [mpfr_clear], , AC_MSG_ERROR([Couldn't find mpfr shared lib... try downloading the source \
from http://www.mpfr.org]))
AC_CHECK_HEADERS([mpfr.h], , AC_MSG_ERROR([Couldn't find mpfr.h... try downloading the source \
from http://www.mpfr.org]))
AC_CHECK_LIB([gsl], [gsl_qrng_get], , )
AC_CHECK_LIB([gslcblas], [cblas_caxpy], , AC_MSG_ERROR([Couldn't find gsl shared lib...]))
AC_CONFIG_FILES([Makefile src/Makefile doc/Makefile tests/Makefile])
AC_OUTPUT
