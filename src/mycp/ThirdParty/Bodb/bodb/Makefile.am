lib_LIBRARIES = libbodb.a

THESOURCES = bobase.cpp bodb2.cpp database.cpp databaseinfo.cpp \
	dbservice.cpp fieldinfo.cpp fieldvariant.cpp pageheadinfo.cpp recordline.cpp \
	resultset.cpp tableinfo.cpp

libbodb_a_SOURCES = $(THESOURCES)
		
libbodb_a_HEADERS = bobase.h bodb2.h bodb.h bodbdef.h data2headinfo.h \
	data2offsetinfo.h database.h databaseinfo.h dataheadinfo.h dbservice.h \
	fieldcompare.h fielddois.h fieldinfo.h fieldvariant.h infoheadinfo.h \
	modifyinfo.h pageheadinfo.h recorddois.h recordline.h resultset.h tableinfo.h \
	bodbhandler.h

AM_CFLAGS=-fPIC -Wall
AM_CXXFLAGS=-fPIC -Wall

libbodb_adir = $(includedir)/bodb
