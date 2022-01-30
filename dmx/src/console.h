#ifndef CONSOLE_H
#define CONSOLE_H

#include <QString>

/**
 *\file console.h
 *
 *\struct Console
 *
 *\brief Structure comprenant les diférents paramètre d'une interface
 *
 *\version 1.0
 */

typedef struct
{
    QString adresseIP;
    quint16 port;
} Console;

#endif // INTERFACE_H
