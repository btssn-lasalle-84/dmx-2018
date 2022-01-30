#ifndef SCENE_H
#define SCENE_H

#include <QTime>


/**
  *
  * \file scene.h
  *
  * \brief Déclaration des structures Scene et SceneSequence
  *
  * \author Reynier Tony
  *
  * \version 1.1
  *
  */



/**
  *
  * \struct Scene
  *
  * \brief Structure permettant de représenter le contenu d'un plan d'éclairage d'appareils XML
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
    // QVector<AppareilScene> appareilsScene;
    QMap<int,int> valeursCanauxscene;
} Scene;


/**
  *
  * \struct Scene
  *
  * \brief Structure permettant de représenter un plan d'eclairage XML implémenté dans une séquence avec une durée donnée pour ledit plan
  *
  * \author Reynier Tony
  *
  * \version 1.1
  *
  */

typedef struct
{
    Scene scene;
    int tempo;

} SceneSequence;



#endif // SCENE_H

