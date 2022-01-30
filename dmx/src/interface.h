#ifndef INTERFACE_H
#define INTERFACE_H

#include <QString>
#include "enttecdmxusb.h"

/**
  *\file interface.h
  *
  *\struct Interface
  *
  *\brief Structure comprenant les différents paramètre d'une interface USB/DMX
  *
  *\version 1.0
  */
typedef struct
{
    int id;
    QString portInterface;
    EnttecInterfaces typeInterface;
    bool utilisee; // si 1 l'interface est la derniere utilisee si 0 elle ne l'est pas
} Interface;

#endif // INTERFACE_H
