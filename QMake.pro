#-------------------------------------------------
#
# Project created by QtCreator 2013-11-03T10:51:55
#
#-------------------------------------------------

QT       += core gui
LIBS		+= -lusb-1.0\
	-lrt

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mp710_trigger
TEMPLATE = app


SOURCES += src/*

HEADERS  += include/*
