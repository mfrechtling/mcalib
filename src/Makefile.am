lib_LTLIBRARIES = libmca.la
libmca_la_SOURCES = mcalib.c lookup_table.c
EXTRA_DIST=mcalib.h mcalib_types.h lookup_table.h
MCA_LIBRARY_VERSION=0:0:0
libmca_la_LDFLAGS = -version-info $(MCA_LIBRARY_VERSION) -lgsl -lm
library_includedir=$(includedir)/
library_include_HEADERS = mcalib.h mcalib_types.h lookup_table.h
