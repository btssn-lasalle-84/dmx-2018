#include "PlaybackWing.h"
#include "console.h"
#include "xmlutilitaire.h"
#include <bitset>
#include <QUdpSocket>
#include <QByteArray>
#include <QBitArray>
#include <QHostAddress>
#include <QMessageBox>
#include <QChar>
#include <QDebug>

/**
  *
  * \file PlaybackWing.cpp
  *
  * \brief Définition de la classe PlaybackWing
  *
  * \author Demont Thomas
  *
  * \version 1.0
  *
  */

/**
  * @brief Constructeur, créé et connecte le socket udp
  *
  * @fn PlaybackWing::PlaybackWing
  *
  * @param pParent QObject
  */
PlaybackWing::PlaybackWing(QObject *pParent):QObject(pParent)
{
   udpSocket = new QUdpSocket(this);

   utilitaireDocuments = new XMLUtilitaire;
   utilitaireDocuments->lireConsoles(consoles);
   // Attachement locale de la socket UDP :
   statut = udpSocket->bind(QHostAddress((QString)"0.0.0.0"), PORT_ENTTEC);
   if(!statut)
   {
       QMessageBox::critical(NULL, QString::fromUtf8("PlaybackWing"), QString::fromUtf8("Erreur bind sur le port %1").arg(PORT_ENTTEC));
   }
   //qDebug() << Q_FUNC_INFO << statut;
}

/**
  * @brief Destructeur
  *
  * @fn PlaybackWing::~PlaybackWing
  */
PlaybackWing::~PlaybackWing()
{
   udpSocket->close();
   //qDebug() << Q_FUNC_INFO;
}

/**
  * @brief Connecte le slot ReceptionnerDatagrames à la possibilité de lire la trame
  *
  * @fn PlaybackWing::demarrer
  *
  * @return void
  */
void PlaybackWing::demarrer()
{
    //qDebug() << Q_FUNC_INFO;
    if(statut)
    {
       connect(udpSocket, SIGNAL(readyRead()), this, SLOT(ReceptionnerDatagrammes()));
       //connect(this, SIGNAL(terminer()), this, SLOT(quit()));
    }
}

QList<Console> PlaybackWing::getListeConsoles()
{
    return consoles;
}

/**
  *@brief Retourne l'adresse IP d'une console par rapport à son numéro dans la liste consoles
  *
  *@fn PlaybackWing::getAdresseIPConsole
  *
  *@return QString
  */
QString PlaybackWing::getAdresseIPConsole(int numeroConsole)
{
    return consoles.at(numeroConsole).adresseIP;
}

quint16 PlaybackWing::getPort(int numeroConsole)
{
    return consoles.at(numeroConsole).port;
}

/**
  *@brief Retourne le nombre de console dans la liste consoles
  *
  *@fn PlaybackWing::getNombreConsoles
  *
  *@return int
  */
int PlaybackWing::getNombreConsoles()
{
    return consoles.count();
}

QList<Console>::iterator PlaybackWing::getDebutListeConsoles()
{
    return consoles.begin();
}

Console PlaybackWing::getConsole(int numeroConsole)
{
    return consoles[numeroConsole];
}

/**
 * @brief Verifie que la trame reçue est bien une trame WODD
 *
 * @fn PlaybackWing::VerifierDatagramme
 * @param donneesBrutes char* les données reçues
 * @param nbOctets int le nombre d'octets du datagramme
 * @return int 1 si c'est une trame WOOD sinon 0
 */
int PlaybackWing::VerifierDatagramme(char *donneesBrutes, int nbOctets)
{
    QString typeTrame = "";
    //qDebug() << donneesBrutes;
    //qDebug() << sizeof(donneesBrutes);
    if(sizeof(donneesBrutes) != 8)
    {
        return 0;
    }

    for(int i=0;i<INDEX_FIRMWARE;i++)
    {
        //qDebug() << donneesBrutes[i];
        typeTrame.append(donneesBrutes[i]);
    }
    //qDebug() << typeTrame;
    if(typeTrame == "WODD")
    {
        return 1;
    }
    else return 0;
}

void PlaybackWing::ajouterConsoleListe(QString adresseIP, QString port/*=3330*/)
{
    Console consoleAjoutee;

    consoleAjoutee.adresseIP = adresseIP;
    consoleAjoutee.port = port.toInt();

    consoles.push_back(consoleAjoutee);

    utilitaireDocuments->ecrireConsole(consoles);
}

void PlaybackWing::supprimerConsoleListe(QList<Console>::iterator it)
{
    consoles.erase(it);
}

/**
  * @brief Réceptionne les données envoyées par la console
  *
  * @fn PlaybackWing::ReceptionnerDatagrammes
  *
  * @return void
  */
void PlaybackWing::ReceptionnerDatagrammes()
{
   //qDebug() << Q_FUNC_INFO;
   int nbOctets = 0;
   int etat;

   // datagramme en attente d'être lu ?
   while (udpSocket->hasPendingDatagrams())
   {
      QByteArray donneesDatagramme;
      QHostAddress emetteurAdresse;
      quint16 emetteurPort;


      // Fixe la taille du tableau au nombre d'octets reçus en attente
      donneesDatagramme.resize(udpSocket->pendingDatagramSize());

      // Lit le datagramme en attente
      //nbOctets = udpSocket->readDatagram(datagram.data(), datagram.size());
      nbOctets = udpSocket->readDatagram(donneesDatagramme.data(), donneesDatagramme.size(), &emetteurAdresse, &emetteurPort);

      // Vérifie si le datagramme est celui d'une console
      for(int i = 0; i < consoles.count(); i++)
      {
        if(emetteurAdresse == (QHostAddress)consoles.at(i).adresseIP && emetteurPort == (quint16)consoles.at(i).port)
        {
            // Vérifie la validité du datagramme
            etat = VerifierDatagramme(donneesDatagramme.data(), nbOctets);
            if(etat == 1)
            {
                #ifdef DEBUG_PlaybackWing
                QString qs_emetteurAdresse = emetteurAdresse.toString();
                cout << "<" << qs_emetteurAdresse.toStdString() << ":" << emetteurPort << "> datagramme de " << nbOctets << " octet(s) reçu(s)" << endl;
                #endif

                TraiterDatagramme(donneesDatagramme);
                //debuguerDatagramme(donneesDatagramme.data(), donneesDatagramme.size());
            }
            else
            {
                #ifdef DEBUG_PlaybackWing
                QString qs_emetteurAdresse = emetteurAdresse.toString();
                cout << "<" << qs_emetteurAdresse.toStdString() << ":" << emetteurPort << "> datagramme de " << nbOctets << " octet(s) reçu(s) : INVALIDE !" << endl;
                #endif
                emit terminer(); /* sinon on quitte ! (à modifier par la suite) */
            }
        }
      }
   }
}

/**
  *@brief envoi la valeur du canal de départ actuel à la console afin Playback Wing de l'afficher sur l'écran LED
  *
  *@fn PlaybackWing::envoyerCanalDepartWing
  *
  *@return void
  */
void PlaybackWing::envoyerCanalDepartWing(int canalDepart)
{
    //qDebug()<< Q_FUNC_INFO << canalDepart;

    QByteArray donnees = '\0';
    donnees.append("WIDD");
    donnees.append((char)1);
    for(int i = 0; i < 32; i++)
    {
        donnees.append((char)0);
    }
    donnees.append((char)(canalDepart/10));
    for(int i = 0; i < 4; i++)
    {
        donnees.append((char)0);
    }

    //qDebug()<< Q_FUNC_INFO << (QString)donnees;
    debuguerDatagramme(donnees.data(), donnees.length());
    for(int i = 0; i < consoles.count(); i++)
    {
        udpSocket->writeDatagram(donnees, (QHostAddress)consoles.at(i).adresseIP, consoles.at(i).port);
    }

}

/**
  *@brief Enregistre les modifications apportées a la console
  *
  *@fn PlaybackWing::enregistrerModificationConsole
  *
  *@param Console console
  *
  *@return void
  */
void PlaybackWing::enregistrerModificationConsole(Console console, int numeroConsoleAModifier)
{
    //qDebug() << "nombre de consoles" << consoles.count();
    consoles.removeAt(numeroConsoleAModifier);
    consoles.append(console);
    utilitaireDocuments->ecrireConsole(consoles);
}


/**
  * @brief Traite les données envoyées par la console
  *
  * @fn PlaybackWing::TraiterDatagramme
  *
  * @param datagramme le datagramme a traiter
  *
  * @return int
  */
int PlaybackWing::TraiterDatagramme(const QByteArray &datagramme)
{
    DecoderTouchesControle((unsigned char)datagramme[INDEX_TOUCHES_CONTROLE]);
    ExtraireFaders(datagramme);

    emit envoyerEtatTouchesControle(etatTouchesControle);
    emit envoyerEtatFaders(etatFaders);

    return 1;
}

/**
  * @brief Débugue le datagramme
  *
  * @fn PlaybackWing::debuguerDatagramme
  *
  * @param donneesBrutes char* les octets du datagramme
  * @param nbOctets int nombre d'octets dans la datagramme
  *
  * @return int
  */
int PlaybackWing::debuguerDatagramme(char *donneesBrutes, int nbOctets)
{
  //qDebug() << Q_FUNC_INFO;
  bool commande = false;
  bool enregistrement = false;
  int etat;
  int valeur;

  //qDebug() << nbOctets;
  //#ifdef DEBUG_PlaybackWing
  if(nbOctets == LG_MESSAGE_WODD)
  {
      for(int i=0;i<INDEX_FIRMWARE;i++)
      {
          printf("%c", donneesBrutes[i]); //WODD
      }
      printf(" ");
      for(int i=INDEX_FIRMWARE;i<nbOctets;i++)
      {
          printf("0x%02X ", (unsigned char)donneesBrutes[i]); // le reste des données
      }
      printf("\n\n");
  }
  //#endif

  return 0;
}

/**
  * @brief Decode les trames si une touche de contrôle est appuyée
  *
  * @fn PlaybackWing::DecoderTouchesControle
  *
  * @param champTouche le champ de la trame correspondant au touches de contrôle
  *
  * @return void
  */
void PlaybackWing::DecoderTouchesControle(unsigned char champTouche)
{
    // Touches de contrôle (INDEX_TOUCHES_CONTROLE 6)
    /*
        Bit 7: 0=PageUp key pressed, 1=PageUp key released.
        Bit 6: 0=PageDown key pressed, 1=PageDown key released.
        Bit 5: 0=Back key pressed, 1=Back key released.
        Bit 4: 0=Go key pressed, 1=Go key released
    */
    unsigned char masque = 0;

    #ifdef DEBUG_PlaybackWing
    qDebug("0x%02X", champTouche);
    #endif
    masque = 1 << PAGE_UP;
    etatTouchesControle.touchePageUp = (champTouche & masque) >> PAGE_UP;
    masque = 1 << PAGE_DOWN;
    etatTouchesControle.touchePageDown = (champTouche & masque) >> PAGE_DOWN;
    masque = 1 << BACK;
    etatTouchesControle.toucheBack = (champTouche & masque) >> BACK;
    masque = 1 << GO;
    etatTouchesControle.toucheGo = (champTouche & masque) >> GO;

}

/**
  * @brief Attribue les valeurs des faders depuis la trame wing
  *
  * @fn PlaybackWing::ExtraireFaders
  *
  * @param datagramme la trame wing reçue
  *
  * @return void
  */
void PlaybackWing::ExtraireFaders(const QByteArray &datagramme)
{
    //qDebug() << Q_FUNC_INFO;
    /*
        Fader state for faders 0 to 9. Valid range is 0 to 255.
    */
    etatFaders.fader0 = (unsigned char)datagramme[FADER_NUMERO_0];
    etatFaders.fader1 = (unsigned char)datagramme[FADER_NUMERO_1];
    etatFaders.fader2 = (unsigned char)datagramme[FADER_NUMERO_2];
    etatFaders.fader3 = (unsigned char)datagramme[FADER_NUMERO_3];
    etatFaders.fader4 = (unsigned char)datagramme[FADER_NUMERO_4];
    etatFaders.fader5 = (unsigned char)datagramme[FADER_NUMERO_5];
    etatFaders.fader6 = (unsigned char)datagramme[FADER_NUMERO_6];
    etatFaders.fader7 = (unsigned char)datagramme[FADER_NUMERO_7];
    etatFaders.fader8 = (unsigned char)datagramme[FADER_NUMERO_8];
    etatFaders.fader9 = (unsigned char)datagramme[FADER_NUMERO_9];

    //qDebug() << "Etat faders" << ":" << etatFaders.fader0 << etatFaders.fader1 << etatFaders.fader2 << etatFaders.fader3 << etatFaders.fader4 << etatFaders.fader5 << etatFaders.fader6 << etatFaders.fader7 << etatFaders.fader8 << etatFaders.fader9;
}

