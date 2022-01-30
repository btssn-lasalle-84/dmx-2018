#ifndef THREADATTENTE_H
#define THREADATTENTE_H
#include <QThread>

class QThread;


/**
  *
  * \file ThreadAttente.h
  *
  * \brief Déclaration de la classe ThreadAttente
  *
  * \author Reynier Tony
  *
  * \version 1.1
  *
  */



/**
  *
  * \struct ThreadAttente
  *
  * \brief Classe heritant de QThread et servant à génerer les temps d'attentes entre les scènes d'une séquence
  *
  * \author Reynier Tony
  *
  * \version 1.1
  *
  */

class ThreadAttente : public QThread
{
    public:
        void attendre(int temps);
};

#endif // THREADATTENTE_H
