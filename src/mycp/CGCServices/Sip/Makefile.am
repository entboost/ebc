lib_LTLIBRARIES = libSipService.la
THESOURCES = SipService.cpp
libSipService_la_SOURCES = $(THESOURCES)

libSipService_la_HEADERS = cgcSip.h

libSipService_la_LIBADD = $(libdir)/libSIP.a \
	$(libdir)/libeXosip2.a $(libdir)/libosip2.a $(libdir)/libosipparser2.a

AM_CFLAGS=-fPIC -shared -Wall -ldns
AM_CXXFLAGS=-fPIC -shared -Wall -ldns

libSipService_ladir = $(includedir)/CGCServices/Sip

