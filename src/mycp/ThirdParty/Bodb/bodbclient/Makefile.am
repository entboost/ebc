bin_PROGRAMS = bodbclient

bodbclient_SOURCES = bodbclient.cpp

#AM_CFLAGS = -lboost_thread -lboost_filesystem 
#AM_CXXFLAGS = -lboost_thread -lboost_filesystem 

bodbclient_LDADD = $(top_srcdir)/bodb/libbodb.a \
	$(top_srcdir)/sqlparser/libsqlparser.a \
	-lboost_thread -lboost_filesystem

