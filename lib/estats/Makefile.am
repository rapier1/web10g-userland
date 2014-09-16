# Header files for public installation
estats_pub_h_sources = \
	error.h \
	estats.h \
	connection.h \
	data.h \
	genl.h \
	list.h \
	mib_var.h \
	nl-client.h \
	record.h \
	tcp_estats_nl.h \
	types.h \
	val.h \
	lookup3.h \
	uthash.h 

# Header files not installed
estats_pri_h_sources = \
	debug-int.h \
	error-int.h \
	libcwrap-int.h \
	resolve-int.h \
	estats-int.h \
	types-int.h

# C sources to build the library from
estats_c_sources = \
	debug-int.c \
	error.c \
	libcwrap-int.c \
	resolve-int.c \
	connection.c \
	data.c \
	genl.c \
	list.c \
	nl-client.c \
	val.c \
	var.c \
	lookup3.c \
	record.c

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
	-version-info $(LIBESTATS_LT_CURRENT):$(LIBESTATS_LT_REVISION):$(LIBESTATS_LT_AGE) \
	-export-symbols-regex '^estats_.*|^list_check' \
	@STRIP_END@

# Libraries to compile and install
lib_LTLIBRARIES = libestats.la

libestats_la_SOURCES = $(estats_c_sources) $(estats_pub_h_sources) $(estats_pri_h_sources)

# Install headers into include/estats
# NOTE: If you update this, be sure to update estats-config
libestatsincludedir = $(ESTATS_INCLUDE_DIR)/estats
libestatsinclude_HEADERS = $(estats_pub_h_sources)
