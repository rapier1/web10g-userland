ACLOCAL_AMFLAGS = -I build-aux

SUBDIRS = lib util
#SUBDIRS = lib

bin_SCRIPTS = estats-config
CLEANFILES = estats-config

m4datadir = $(datadir)/aclocal
m4data_DATA = estats.m4

EXTRA_DIST = estats-config.in estats.m4 libmnl-1.0.3.tar.bz2 web10g-0.7-patches.tar.gz

edit = @STRIP_BEGIN@ \
   sed -e 's,@ESTATS_VERSION\@,$(ESTATS_VERSION),g' \
       -e 's,@PREFIX\@,$(prefix),g' \
       -e 's,@EXEC_PREFIX\@,$(exec_prefix),g' \
       -e 's,@ESTATS_INCLUDE_DIR\@,$(ESTATS_INCLUDE_DIR),g' \
       -e 's,@LIBDIR\@,$(libdir),g' \
       @STRIP_END@

estats-config: $(srcdir)/estats-config.in
	rm -f estats-config estats-config.tmp
	$(edit) $(srcdir)/estats-config.in > estats-config.tmp
	chmod +x estats-config.tmp
	mv estats-config.tmp estats-config
