#include "DMXLyre.h"
#include <QDebug>
/**
  *\file DMXLyre.cpp
  *
  *\brief Definition de la classe DMXLyre
  *
  *
  *\author Demont Thomas, Reynier Tony
  *
  *\version 1.0
  */

DMXLyre::DMXLyre()
{

}

DMXLyre::DMXLyre(QString nom, int canalDebut, int nombreCanaux, QString uuid, QString type) : DMXProjecteur(nom, canalDebut, nombreCanaux, uuid, type)
{

}

DMXLyre::~DMXLyre()
{
    //qDebug() << Q_FUNC_INFO;
}

void DMXLyre::setCanaux(int pan, int tilt, int globos, int couleur)
{
    canalPan = pan;
    canalTilt = tilt;
    canalGlobos = globos;
    canalCouleur = couleur;
}
