# Header files for public installation
estats_pub_h_sources = \
	error.h \
	estats.h \
	estats-nl-client.h \
	estats-data.h \
	estats-genl.h \
	estats-nl.h \
	types.h

# Header files not installed
estats_pri_h_sources = \
	debug-int.h \
	error-int.h \
	libcwrap-int.h \
	list-int.h \
	resolve-int.h \
	estats-int.h \
	types-int.h

# C sources to build the library from
estats_c_sources = \
	debug-int.c \
	error.c \
	libcwrap-int.c \
	resolve-int.c \
	estats-nl-client.c \
	estats-data.c \
	estats-genl.c \
	estats-var.c

# cpp flags to be used with all compilation
# Note: -D_GNU_SOURCE required so headers declare vfscanf (possibly among
#       others)
AM_CPPFLAGS = @STRIP_BEGIN@ \
	$(LIBMNL_CFLAGS) \
	-I$(top_srcdir)/lib \
	-D_GNU_SOURCE \
	@STRIP_END@

AM_LDFLAGS = @STRIP_BEGIN@ \
	$(LIBMNL_LIBS) \
	-version-info $(LIBTCPE_LT_CURRENT):$(LIBTCPE_LT_REVISION):$(LIBTCPE_LT_AGE) \
	-export-symbols-regex '^estats_.*' \
	@STRIP_END@

# Libraries to compile and install
lib_LTLIBRARIES = libestats.la

libestats_la_SOURCES = $(estats_c_sources) $(estats_pub_h_sources) $(estats_pri_h_sources)
#libestats_la_LDFLAGS = -version-info=0:1:0

# Install headers into include/estats
# NOTE: If you update this, be sure to update estats-config
libestatsincludedir = $(TCPE_INCLUDE_DIR)/estats
libestatsinclude_HEADERS = $(estats_pub_h_sources)
