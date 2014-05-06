AUTOMAKE_OPTIONS = 1.6 gnu
check_PROGRAMS = 	tfloateq tfloatge tfloatgt tfloatle tfloatlt tfloatne tdoubleeq \
			tdoublege tdoublegt tdoublele tdoublelt tdoublene tfloatadd \
			tfloatsub tfloatmul tfloatdiv tfloatneg tdoubleadd tdoublesub \
			tdoublemul tdoublediv tdoubleneg tlongeq tlongge tlonggt tlongle \
			tlonglt tlongne tlongadd tlongsub tlongmul tlongdiv tlongneg
			

AM_CPPFLAGS = -DSRCDIR='"$(srcdir)"'
LDADD = $(top_builddir)/src/libmca.la
INCLUDES = -I$(top_srcdir)/src -I$(top_builddir)/src

LOADLIBES=$(DEFS) $(INCLUDES) $(CPPFLAGS) $(CFLAGS) $(top_builddir)/src/.libs/libmca.a $(LIBS)

$(top_builddir)/src/libmca.la:
	cd $(top_builddir)/src && $(MAKE) $(AM_MAKEFLAGS) libmca.la

TESTS = $(check_PROGRAMS)
TESTS_ENVIRONMENT = $(VALGRIND)
AM_LDFLAGS = -no-install -L$(top_builddir)/src/.libs -lm
