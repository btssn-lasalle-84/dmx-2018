#include "DMXScanner.h"
#include <QDebug>

/**
  *\file DMXScanner.cpp
  *
  *\brief Definition de la classe DMXScanner
  *
  *
  *\author Demont Thomas, Reynier Tony
  *
  *\version 1.0
  */

DMXScanner::DMXScanner()
{

}

DMXScanner::DMXScanner(QString nom, int canalDebut, int nombreCanaux, QString uuid, QString type) : DMXProjecteur(nom, canalDebut, nombreCanaux, uuid, type)
{

}

DMXScanner::~DMXScanner()
{
    //qDebug() << Q_FUNC_INFO;
}


void DMXScanner::setCanaux(int pan, int tilt, int couleur)
{
    canalPan = pan;
    canalTilt = tilt;
    canalCouleur = couleur;
}
