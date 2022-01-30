#include "DMXSpecial.h"
#include <QDebug>


/**
  *\file DMXSpecial.cpp
  *
  *\brief Definition de la classe DMXSpecial
  *
  *
  *\author Demont Thomas, Reynier Tony
  *
  *\version 1.0
  */
DMXSpecial::DMXSpecial()
{

}

DMXSpecial::DMXSpecial(QString nom, int canalDebut, int nombreCanaux, QString uuid, QString type) : DMXProjecteur(nom, canalDebut, nombreCanaux, uuid, type)
{

}

DMXSpecial::~DMXSpecial()
{
    //qDebug() << Q_FUNC_INFO;
}


void DMXSpecial::setCanaux()
{

}
