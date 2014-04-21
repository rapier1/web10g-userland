bin_PROGRAMS = web10g-deltavars web10g-getmib web10g-listconninfo web10g-listconns web10g-readvars web10g-record_read web10g-record_write web10g-watchvars web10g-writevar web10g-logger

LIBESTATS_LDADDS = @STRIP_BEGIN@ \
	$(top_builddir)/lib/estats/libestats.la \
	@STRIP_END@

AM_CPPFLAGS = @STRIP_BEGIN@ \
	$(LIBMNL_CFLAGS) \
	-I$(top_srcdir)/lib \
	@STRIP_END@

web10g_deltavars_SOURCES = web10g-deltavars.c scripts.h
web10g_deltavars_LDADD = $(LIBESTATS_LDADDS)

web10g_getmib_SOURCES = web10g-getmib.c scripts.h
web10g_getmib_LDADD = $(LIBESTATS_LDADDS)

web10g_listconninfo_SOURCES = web10g-listconninfo.c scripts.h
web10g_listconninfo_LDADD = $(LIBESTATS_LDADDS)

web10g_listconns_SOURCES = web10g-listconns.c scripts.h
web10g_listconns_LDADD = $(LIBESTATS_LDADDS)

web10g_readvars_SOURCES = web10g-readvars.c scripts.h
web10g_readvars_LDADD = $(LIBESTATS_LDADDS)

web10g_record_read_SOURCES = web10g-record_read.c scripts.h
web10g_record_read_LDADD = $(LIBESTATS_LDADDS)

web10g_record_write_SOURCES = web10g-record_write.c scripts.h
web10g_record_write_LDADD = $(LIBESTATS_LDADDS)

web10g_watchvars_SOURCES = web10g-watchvars.c scripts.h
web10g_watchvars_LDADD = $(LIBESTATS_LDADDS)

web10g_writevar_SOURCES = web10g-writevar.c scripts.h
web10g_writevar_LDADD = $(LIBESTATS_LDADDS)

web10g_logger_SOURCES = web10g-logger.c scripts.h
web10g_logger_LDADD = $(LIBESTATS_LDADDS)
