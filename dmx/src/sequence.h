#ifndef SEQUENCE_H
#define SEQUENCE_H

#include "scene.h"


/**
  *
  * \file sequence.h
  *
  * \brief Déclaration de la structure Sequence
  *
  * \author Reynier Tony
  *
  * \version 1.1
  *
  */



/**
  *
  * \struct Sequence
  *
  * \brief Structure permettant de représenter le contenu d'une séquence d'enchainement de scènes temporisées
  *
  * \author Reynier Tony
  *
  * \version 1.1
  *
  */

typedef struct
{
    QString nom;
    QString uuid;
    QVector<SceneSequence> ensembleScenes;

} Sequence;

#endif // SEQUENCE_H
