
include $(top_srcdir)/build-aux/autotroll.mk

LIBESTATS_LDADDS = @STRIP_BEGIN@ \
	$(top_builddir)/lib/estats/libestats.la \
	@STRIP_END@

INCLUDES = @STRIP_BEGIN@ \
	-I$(top_srcdir)/lib \
	@STRIP_END@
			
bin_PROGRAMS = estatsview
estatsview_SOURCES = EstatsView.cpp EstatsView.hpp conn_tablemodel.cpp conn_tablemodel.hpp estats_itemdelegate.cpp estats_itemdelegate.hpp estats_tablemodel.cpp estats_tablemodel.hpp graph.cpp graph.hpp main.cpp mainwindow.cpp mainwindow.hpp statswindow.cpp statswindow.hpp var_tablemodel.cpp var_tablemodel.hpp varwindow.cpp varwindow.hpp conn_tablemodel.moc.cpp estats_itemdelegate.moc.cpp estats_tablemodel.moc.cpp graph.moc.cpp mainwindow.moc.cpp statswindow.moc.cpp var_tablemodel.moc.cpp varwindow.moc.cpp application.qrc.cpp

estatsview_CXXFLAGS = $(QT_CXXFLAGS) -fpermissive $(AM_CXXFLAGS)
estatsview_CPPFLAGS = $(QT_CPPFLAGS) $(AM_CPPFLAGS)
estatsview_LDFLAGS  = $(QT_LDFLAGS) $(LDFLAGS)
estatsview_LDADD    = $(QT_LIBS) $(LIBESTATS_LDADDS) $(LDADD)

BUILT_SOURCES = conn_tablemodel.moc.cpp estats_itemdelegate.moc.cpp estats_tablemodel.moc.cpp graph.moc.cpp mainwindow.moc.cpp statswindow.moc.cpp var_tablemodel.moc.cpp varwindow.moc.cpp application.qrc.cpp
