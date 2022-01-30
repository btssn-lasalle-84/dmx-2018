#include "DMXPAR.h"
#include <QDebug>

/**
  *\file DMXPAR.cpp
  *
  *\brief Definition de la classe DMXPAR
  *
  *
  *\author Demont Thomas, Reynier Tony
  *
  *\version 1.0
  */

DMXPAR::DMXPAR()
{

}

DMXPAR::DMXPAR(QString nom, int canalDebut, int nombreCanaux, QString uuid, QString type) : DMXProjecteur(nom, canalDebut, nombreCanaux, uuid, type)
{

}

DMXPAR::~DMXPAR()
{
    //qDebug() << Q_FUNC_INFO;
}

void DMXPAR::setCanaux(int rouge, int bleu, int vert)
{
    canalCouleurRouge = rouge;
    canalCouleurBleue = bleu;
    canalCouleurVerte = vert;
}
