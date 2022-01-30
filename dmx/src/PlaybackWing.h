#ifndef PLAYBACKWING__H
#define PLAYBACKWING__H

/**
  *
  * \file PlaybackWing.h
  *
  * \brief Déclaration de la classe PlaybackWing
  *
  * \author Demont Thomas, Reynier Tony
  *
  * \version 1.0
  *
  */

#include "xmlutilitaire.h"
#include "console.h"
#include <QUdpSocket>

#define PORT_ENTTEC      3330 //Enttec Port

// Protocole Wings (cf. wings_API.pdf)
#define INDEX_FIRMWARE 4 //nombre d'octets de l'index de la trame Wing
#define INDEX_FLAGS 5 //type matériel
#define INDEX_TOUCHES_CONTROLE 6 // cf. wings_API.pdf page 2

#define LG_MESSAGE_WODD 28 //taille du message WODD

#define FADER_NUMERO_0 15 //fader numero 1 en partant de la gauche, cf. wings_API.pdf page 3
#define FADER_NUMERO_1 16
#define FADER_NUMERO_2 17
#define FADER_NUMERO_3 18
#define FADER_NUMERO_4 19
#define FADER_NUMERO_5 20
#define FADER_NUMERO_6 21
#define FADER_NUMERO_7 22
#define FADER_NUMERO_8 23
#define FADER_NUMERO_9 24


// Gestion état des touches
#define KEY_PRESSED 0
#define KEY_RELEASED 1
// Touches de controles
#define PAGE_UP     7
#define PAGE_DOWN   6
#define BACK        5
#define GO          4



/**
  * @brief Structure permettant de stocker les valeurs des touches de contrôle (Page Up, Page Back, Back, Go)
  *
  * @struct EtatTouchesControle
  *
  * @author Demont Thomas
  */
typedef struct
{
    int touchePageUp;
    int touchePageDown;
    int toucheBack;
    int toucheGo;
} EtatTouchesControle;

/**
  * @brief Structure permettant de stocker les valeurs des faders
  *
  * @struct EtatFaders
  *
  * @author Demont Thomas
  */
typedef struct
{
    int fader0;
    int fader1;
    int fader2;
    int fader3;
    int fader4;
    int fader5;
    int fader6;
    int fader7;
    int fader8;
    int fader9;

} EtatFaders;

class XMLUtilitaire;

/**
  *
  * \class PlaybackWing
  *
  * \brief la classe gérant la console wing
  *
  * \author Demont Thomas
  *
  * \version 1.0
  *
  */
class PlaybackWing : public QObject
{
   Q_OBJECT

   public:

      PlaybackWing(QObject *pParent = 0);
      ~PlaybackWing();

      void demarrer();
      QList<Console> getListeConsoles();
      QString getAdresseIPConsole(int numeroConsole);
      quint16 getPort(int numeroConsole);
      int getNombreConsoles();
      QList<Console>::iterator getDebutListeConsoles();
      Console getConsole(int numeroConsole);

      void ajouterConsoleListe(QString adresseIP, QString port);
      void supprimerConsoleListe(QList<Console>::iterator it);

   public slots:
      void ReceptionnerDatagrammes();
      void envoyerCanalDepartWing(int canalDepart);
      void enregistrerModificationConsole(Console console, int numeroConsoleAModifier);

   signals :
      void terminer();
      void envoyerEtatTouchesControle(EtatTouchesControle etatTouchesControle);
      void envoyerEtatFaders(EtatFaders etatFaders);

   private:
      XMLUtilitaire *utilitaireDocuments; /**< \brief Utilitaire de gestion des fichiers xml */
      QList<Console> consoles;

      QUdpSocket *udpSocket;
      bool statut;
      EtatTouchesControle etatTouchesControle;
      EtatFaders etatFaders;

      int  VerifierDatagramme(char *donneesBrutes, int nbOctets);
      int  TraiterDatagramme(const QByteArray &datagramme);
      int  debuguerDatagramme(char *donneesBrutes, int nbOctets);
      void DecoderTouchesControle(unsigned char champTouche);
      void ExtraireFaders(const QByteArray &datagramme);
};

#endif
