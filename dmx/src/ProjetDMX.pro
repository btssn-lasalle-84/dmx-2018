#-------------------------------------------------
#
# Project created by QtCreator 2018-02-15T16:50:00
#
#-------------------------------------------------
include(qextserialport/qextserialport.pri)

QT       += core gui network\
		 xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ProjetDMX
TEMPLATE = app


SOURCES += main.cpp\
        IHM.cpp\
        PlaybackWing.cpp\
        DMXProjecteur.cpp\
        DMXLaser.cpp\
        DMXPAR.cpp\
        DMXLyre.cpp\
		DMXScanner.cpp\
		DMXSpecial.cpp\
		enttecdmxusb.cpp\
		myslider.cpp\
    xmlutilitaire.cpp \
    idProjecteurSauvegarde.cpp \
    ThreadAttente.cpp
		
HEADERS  += IHM.h\
		PlaybackWing.h\
		DMXProjecteur.h\
		DMXLaser.h\
		DMXPAR.h\
		DMXLyre.h\
		DMXScanner.h\
		DMXSpecial.h\
		enttecdmxusb.h\
		errcode.h\
		myslider.h\
		types.h \
    xmlutilitaire.h \
    idProjecteurSauvegarde.h \
    interface.h \
    scene.h \
    consoles.h \
    sequence.h \
    ThreadAttente.h \
    spectacle.h
    
				

FORMS    +=
