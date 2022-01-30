#include "DMXLaser.h"
#include <QDebug>

/**
  *\file DMXLaser.cpp
  *
  *\brief Definition de la classe DMXLaser
  *
  *
  *\author Demont Thomas, Reynier Tony
  *
  *\version 1.0
  */

DMXLaser::DMXLaser() : DMXProjecteur()
{

}

DMXLaser::DMXLaser(QString nom, int canalDebut, int nombreCanaux, QString uuid, QString type) : DMXProjecteur(nom, canalDebut, nombreCanaux, uuid, type)
{

}

DMXLaser::~DMXLaser()
{
    //qDebug() << Q_FUNC_INFO;
}

void DMXLaser::setCanaux(int controle, int extinction, int pictures, int deplacement, int rotation, int sens, int stretch, int speed, int slowDrawSpeed, int taille)
{
    canalControle = controle;
    canalExtinction = extinction;
    canalPictures = pictures;
    canalDeplacement = deplacement;
    canalRotation = rotation;
    canalSens = sens;
    canalStretch = stretch;
    canalSpeed = speed;
    canalSlowDrawSpeed = slowDrawSpeed;
    canalTaille = taille;
}
