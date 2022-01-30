#ifndef XMLUTILITAIRE_H
#define XMLUTILITAIRE_H

#include <QtCore>
#include <QtXml>
#include "scene.h"
#include "sequence.h"
#include "spectacle.h"
#include "idProjecteurSauvegarde.h"
#include "interface.h"
#include "console.h"
#include "PlaybackWing.h"

/**
  *
  * \file xmlutilitaire.h
  *
  * \brief Déclaration de la classe XMLUtilitaire
  *
  * \author Demont Thomas, Reynier Tony
  *
  * \version 1.1
  *
  */

class DMXProjecteur;


/**
  *
  * \class XMLUtilitaire
  *
  * \brief Classe gérant les fichiers XML de l'application
  *
  * \author Demont Thomas, Reynier Tony
  *
  * \version 1.1
  *
  */
class XMLUtilitaire : public QObject
{
    Q_OBJECT
    public:
        XMLUtilitaire(QObject *parent = 0);
        ~XMLUtilitaire();

        bool lireAppareils(QVector<DMXProjecteur*> &projecteursDMX);
        bool ecrireInterfaces(const QList<Interface> &interfaces);
        bool lireInterfaces(QList<Interface> &interfaces);
        bool afficherProjecteursEnregistres(IDProjecteurSauvegarde *projecteur[]);
        bool supprimerProjecteursEnregistres();
        bool enregistrerAppareil(QString nom, QString type, int nbCanaux, int canalDepart, QString nomsCanaux[]);
        bool supprimerAppareil(QString uuid);
        bool modifierAppareil(QString uuid, QString nom, QString type, int nbCanaux, int canalDepart, QString nomsCanaux[]);
        int getNbAppareils();
        int lireScenes(QVector<Scene> &scenesEnregistrees);
        bool enregistrerScene(Scene scene);
        bool supprimerScene(QString uuid);
        int lireSequences(QVector<Sequence> &sequencesEnregistrees);
        bool lireSceneSequence(SceneSequence &scene);
        bool enregistrerSequence(Sequence sequence);
        bool supprimerSequence(QString uuid);
        int lireSpectacles(QVector<Spectacle> &spectaclesEnregistres);
        bool lireSequenceSpectacle(Sequence &sequence);
        bool ecrireSpectacles(QVector<Spectacle> &spectacles);
        bool lireConsoles(QList<Console> &consoles);
        bool ecrireConsole(const QList<Console> &consoles);

    private:
        QFile fichierAppareils;
        QFile fichierSpectacles;
        QFile fichierScenes;
        QFile fichierSequences;
        QFile fichierInterfaces;
        QFile fichierConsoles;

        DMXProjecteur* creerAppareil(const QDomElement &elementAppareil);
};

#endif // XMLUTILITAIRE_H
