#include "xmlutilitaire.h"
#include "DMXProjecteur.h"
#include "DMXLaser.h"
#include "DMXLyre.h"
#include "DMXPAR.h"
#include "DMXScanner.h"
#include "DMXSpecial.h"
#include "interface.h"
#include <QDebug>
#include <QMessageBox>

/**
  *
  * \file xmlutilitaire.cpp
  *
  * \brief Définition de la classe XMLUtilitaire

  *
  * \author Demont Thomas, Reynier Tony
  *
  * \version 1.0
  *
  */

XMLUtilitaire::XMLUtilitaire(QObject *parent) : QObject(parent)
{
    fichierAppareils.setFileName(QCoreApplication::applicationDirPath() + "/" + "appareils.xml");

    fichierSpectacles.setFileName(QCoreApplication::applicationDirPath() + "/" + "spectacles.xml");

    fichierScenes.setFileName(QCoreApplication::applicationDirPath() + "/" + "scenes.xml");

    fichierSequences.setFileName(QCoreApplication::applicationDirPath() + "/" + "sequences.xml");

    fichierInterfaces.setFileName(QCoreApplication::applicationDirPath() + "/" + "adaptateurs.xml");

    fichierConsoles.setFileName(QCoreApplication::applicationDirPath() + "/" + "consoles.xml");
}

XMLUtilitaire::~XMLUtilitaire()
{
    if (fichierAppareils.isOpen())
    {
        fichierAppareils.close();
    }

    if (fichierSpectacles.isOpen())
    {
        fichierSpectacles.close();
    }

    if (fichierScenes.isOpen())
    {
        fichierScenes.close();
    }

    if (fichierSequences.isOpen())
    {
        fichierSequences.close();
    }

    if (fichierInterfaces.isOpen())
    {
        fichierInterfaces.close();
    }

    if (fichierConsoles.isOpen())
    {
        fichierConsoles.close();
    }

    //qDebug() << Q_FUNC_INFO;
}

/**
 * @brief Accesseur du nombres d'appareils depuis appareils.xml
 *
 * @fn XMLUtilitaire::getNbAppareils
 *
 * @return int
 */
int XMLUtilitaire::getNbAppareils()
{
    if (!fichierAppareils.open(QIODevice::ReadOnly))
    {
        qDebug() << Q_FUNC_INFO << "Erreur ouverture !" << fichierAppareils.fileName();
        return 0;
    }
    else
    {
        QDomDocument docAppareils;
        docAppareils.setContent(&fichierAppareils, false);
        QDomElement root = docAppareils.documentElement();
        //qDebug() << Q_FUNC_INFO << "nbAppareils:" << root.attribute("nbAppareils") ;
        fichierAppareils.close();
        return root.attribute("nbAppareils").toInt();
    }
}


/**
 * @brief Méthode recuperant tous les appareils depuis appareils.xml pour affecter leurs differentes valeurs dans le conteneur projecteursDMX
 *
 * @fn XMLUtilitaire::lireAppareils
 *
 * @param &projecteursDMX
 *
 * @return bool
 */
bool XMLUtilitaire::lireAppareils(QVector<DMXProjecteur*> &projecteursDMX)
{
    if (!fichierAppareils.open(QIODevice::ReadOnly))
    {
        qDebug() << Q_FUNC_INFO << "Erreur ouverture !" << fichierAppareils.fileName();
    }
    else
    {
        // Vider le conteneur avant de le remplir
        for(int i = 0; i < projecteursDMX.count(); i++)
        {
            delete projecteursDMX.at(i);
        }
        projecteursDMX.clear();

        QDomDocument documentXML;

        documentXML.setContent(&fichierAppareils, false);
        QDomElement racine = documentXML.documentElement(); // <appareils>
        if(racine.isNull())
        {
            qDebug() << Q_FUNC_INFO << "Erreur racine !";
            fichierAppareils.close();
            return false;
        }

        QDomNode noeudAppareil = racine.firstChild();
        if(noeudAppareil.isNull())
        {
            qDebug() << Q_FUNC_INFO << "Erreur racine vide !";
            fichierAppareils.close();
            return false;
        }

        while(!noeudAppareil.isNull())
        {
            QDomElement elementAppareil = noeudAppareil.toElement(); // <appareil nom="" nbCanal="" type="" uuid="">
            if(elementAppareil.isNull())
            {
                qDebug() << Q_FUNC_INFO << "Erreur element !";
                break;
            }

            DMXProjecteur* projecteurDMX = creerAppareil(elementAppareil);

            QDomNode noeudCanal = elementAppareil.firstChild();
            if(!noeudCanal.isNull())
            {
                int canalDebut = 1;
                QStringList nomCanaux;
                int i = 0;
                while(!noeudCanal.isNull())
                {
                    QDomElement elementCanal = noeudCanal.toElement(); // <canal id="">XXX</canal>

                    // Lit les attributs du canal
                    //qDebug() << Q_FUNC_INFO << elementCanal.attribute("id").toInt();
                    if(i == 0)
                        canalDebut = elementCanal.attribute("id").toInt();
                    // Lit l'élément du canal
                    //qDebug() << Q_FUNC_INFO << elementCanal.text();
                    nomCanaux << elementCanal.text();

                    noeudCanal = noeudCanal.nextSibling();
                    ++i;
                }
                projecteurDMX->setCanalDebut(canalDebut);
                projecteurDMX->setNomCanaux(nomCanaux);
            }

            projecteursDMX.push_back(projecteurDMX);
            noeudAppareil = noeudAppareil.nextSibling();
        }
    }
    fichierAppareils.close();

    return true;
}

/**
  *@brief Réecrit le fichier adaptateurs.xml et supprimant les éléments et en écrivant ceux présent dans la QList interfaces passée en paramètre
  *
  *@fn XMLUtilitaire::ecrireInterfaces
  *
  *@param interfaces const QList<Interface>& une référence constante sur la liste des interfaces
  *
  *@return bool
  */
bool XMLUtilitaire::ecrireInterfaces(const QList<Interface> &interfaces)
{
    if (!fichierInterfaces.open(QIODevice::ReadWrite))
    {
        qDebug() << Q_FUNC_INFO << "Erreur ouverture !" << fichierInterfaces.fileName();
    }
    if(!(fichierInterfaces.isOpen()))
    {
        QMessageBox::critical(0, QString::fromUtf8("Erreur"), QString::fromUtf8("Le fichier %1 n'a pas pu être ouvert !").arg(fichierInterfaces.fileName()));
        return false;
    }
    else
    {
        QDomDocument documentXML;

        fichierInterfaces.resize(0);
        //documentXML.setContent(&fichierInterfaces, false);
        QDomNode xmlNode = documentXML.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
        documentXML.insertBefore(xmlNode, documentXML.firstChild());

        if(interfaces.size() > 0)
        {
            QDomElement root = documentXML.createElement("interfaces");
            documentXML.appendChild(root);

            //qDebug() << Q_FUNC_INFO << interfaces.size() << interfaces.count();
            for(int i = 0; i < interfaces.size(); i++)
            {
                QDomElement elementInterface = documentXML.createElement("interface");
                elementInterface.setAttribute("id", interfaces[i].id);
                elementInterface.setAttribute("utilisee", interfaces[i].utilisee);
                root.appendChild(elementInterface);

                QDomElement elementPeripherique = documentXML.createElement("port");
                elementInterface.appendChild(elementPeripherique);

                QDomText text = documentXML.createTextNode(interfaces[i].portInterface);
                elementPeripherique.appendChild(text);

                QDomElement elementType = documentXML.createElement("type");
                elementInterface.appendChild(elementType);

                text = documentXML.createTextNode(QString::number(interfaces[i].typeInterface));
                elementType.appendChild(text);
            }
        }

        //qDebug() << documentXML.toByteArray();
        //qDebug() << fichierInterfaces.readAll();
        QTextStream out(&fichierInterfaces);
        documentXML.save(out, 2);
    }

    fichierInterfaces.close();

    return true;
}

/**
  *@brief Lit le fichier adaptateurs.xml et remplis la QList interfaces passée en paramètre en conséquence
  *
  *@fn XMLUtilitaire::lireInterfaces
  *
  *@param interfaces QList<Interface>& une référence sur la liste des interfaces
  *
  *@return bool true si la liste a été modifiée sinon false
  */
bool XMLUtilitaire::lireInterfaces(QList<Interface> &interfaces)
{
    if (!fichierInterfaces.open(QIODevice::ReadWrite))
    {
        qDebug() << Q_FUNC_INFO << "Erreur ouverture !" << fichierInterfaces.fileName();
    }
    if(!(fichierInterfaces.isOpen()))
    {
        QMessageBox::critical(0, QString::fromUtf8("Erreur"), QString::fromUtf8("Le fichier %1 n'a pas pu être ouvert !").arg(fichierInterfaces.fileName()));
        return false;
    }
    else
    {
        QDomDocument documentXML;

        documentXML.setContent(&fichierInterfaces, false);
        QDomElement racine = documentXML.documentElement(); // <interfaces>
        if(racine.isNull())
        {
            qDebug() << "<XMLUtilitaire::lireInterfaces(QList<Interface> interfaces)> erreur racine !";
            fichierInterfaces.close();
            return false;
        }

        QDomNode noeudInterface = racine.firstChild(); // <interface>
        if(noeudInterface.isNull())
        {
            qDebug() << "<XMLUtilitaire::lireInterfaces(QList<Interface> interfaces)> erreur racine vide !";
            fichierInterfaces.close();
            return false;
        }

        while(!noeudInterface.isNull())
        {
            Interface intf;
            QDomElement elementInterface = noeudInterface.toElement(); // <interface>
            if(elementInterface.isNull())
            {
                qDebug() << "<XMLUtilitaire::lireInterfaces(QList<Interface> interfaces)> erreur element interface !";
                fichierInterfaces.close();
                break;
            }

            QDomNode noeudPeripherique = elementInterface.firstChild();
            QDomNode noeudTypeInterface = noeudPeripherique.nextSibling();
            if(!noeudPeripherique.isNull() && !noeudTypeInterface.isNull())
            {
                QDomElement elementPeripherique = noeudPeripherique.toElement(); // <peripherique>
                QDomElement elementTypeInterface = noeudTypeInterface.toElement(); //<type>

                intf.id = elementInterface.attribute("id").toInt(); // l'id
                //qDebug() << intf.id;
                intf.utilisee = elementInterface.attribute("utilisee").toInt(); // l'utilisation
                //qDebug() << intf.utilisee;
                intf.portInterface = elementPeripherique.text(); // le device
                //qDebug() << intf.portInterface;
                intf.typeInterface = (EnttecInterfaces)elementTypeInterface.text().toInt(); // le type d'interface
                //qDebug() << intf.typeInterface;
            }
            else qDebug() << "<XMLUtilitaire::lireInterfaces(QList<Interface> interfaces)> erreur noeud peripherique/type !";

            interfaces.push_back(intf);
            //qDebug() <<"interfaces = " << interfaces.count();
            noeudInterface = noeudInterface.nextSibling();
        }
    }

    fichierInterfaces.close();

    return true;
}

/**
 * @brief Méthode retournant un objet herité de DMXProjecteur dont la classe dépend du QDomElement en argument
 *
 * @fn XMLUtilitaire::creerAppareil
 *
 * @param elementAppareil const QDomElement& un référence constante sur un élément XML représentant un appareil
 *
 * @return DMXProjecteur* un pointeur vers un projecteur DMX
 */
DMXProjecteur* XMLUtilitaire::creerAppareil(const QDomElement &elementAppareil)
{
    QString type = elementAppareil.attribute("type");

    // Lit les attributs de l'appareil
    //qDebug() << Q_FUNC_INFO << elementAppareil.attribute("nom") << elementAppareil.attribute("nbCanal").toInt() << elementAppareil.attribute("type") << elementAppareil.attribute("uuid");

    if(type == "LASER")
    {
        return new DMXLaser(elementAppareil.attribute("nom"), 1, elementAppareil.attribute("nbCanal").toInt(), elementAppareil.attribute("uuid"), type);
    }
    else if(type == "LYRE")
    {
        return new DMXLyre(elementAppareil.attribute("nom"), 1, elementAppareil.attribute("nbCanal").toInt(), elementAppareil.attribute("uuid"), type);
    }
    else if(type == "PAR LED")
    {
        return new DMXPAR(elementAppareil.attribute("nom"), 1, elementAppareil.attribute("nbCanal").toInt(), elementAppareil.attribute("uuid"), type);
    }
    else if(type == "SCANNER")
    {
        return new DMXScanner(elementAppareil.attribute("nom"), 1, elementAppareil.attribute("nbCanal").toInt(), elementAppareil.attribute("uuid"), type);
    }
    else if(type == "SPECIAL")
    {
        return new DMXSpecial(elementAppareil.attribute("nom"), 1, elementAppareil.attribute("nbCanal").toInt(), elementAppareil.attribute("uuid"), type);
    }
    else
    {
        return new DMXProjecteur(elementAppareil.attribute("nom"), 1, elementAppareil.attribute("nbCanal").toInt(), elementAppareil.attribute("uuid"), type);
    }
}

/**
 * @brief Méthode recuperant les caracteristiques des projecteurs enregistrés dans appareils.xml pour les retranscrir sous forme de widget IDProjecteurSauvegarde affiché dans l'IHM
 *
 * @fn XMLUtilitaire::afficherProjecteursEnregistres
 *
 * @param *projecteur[]
 *
 * @return bool
 */
bool XMLUtilitaire::afficherProjecteursEnregistres(IDProjecteurSauvegarde *projecteur[])
{
    if(!fichierAppareils.open(QIODevice::ReadOnly))
    {
        qDebug() << Q_FUNC_INFO << " Erreur ouverture " << fichierAppareils.fileName();
        return false;
    }
    else
    {
        //qDebug() << Q_FUNC_INFO;
        int numProjecteur = 0;
        QDomDocument documentAppareils;
        documentAppareils.setContent(&fichierAppareils, false);
        QDomElement root = documentAppareils.documentElement();

        QDomNode noeudAppareils = root.firstChild();
        while(!noeudAppareils.isNull())
        {
            QDomElement appareil = noeudAppareils.toElement();

            projecteur[numProjecteur] = new IDProjecteurSauvegarde();
            projecteur[numProjecteur]->setLabel(appareil.attribute("nom"));
            projecteur[numProjecteur]->setUuid(appareil.attribute("uuid"));

            numProjecteur++;
            noeudAppareils = noeudAppareils.nextSibling();
        }
        fichierAppareils.close();

        return true;
    }
}

/**
 * @brief Méthode de suppression de tous les projecteurs depuis appareils.xml
 *
 * @fn XMLUtilitaire::supprimerProjecteursEnregistres
 *
 * @return bool
 */
bool XMLUtilitaire::supprimerProjecteursEnregistres()
{
    if(!fichierAppareils.open(QIODevice::ReadWrite))
    {
        qDebug() << Q_FUNC_INFO << "Erreur ouverture" << fichierAppareils.fileName();
        return false;
    }
    else
    {
        QDomDocument documentAppareils;
        documentAppareils.setContent(&fichierAppareils, false);
        QDomElement root = documentAppareils.documentElement();

        QDomNode noeudAppareils = root.firstChild();

        while(!noeudAppareils.isNull())
        {

            QDomElement appareil = noeudAppareils.toElement();
            qDebug() << "Suppression" << appareil.attribute("nom");

            if(noeudAppareils.nextSibling().isNull())
            {
                root.removeChild(noeudAppareils);
                noeudAppareils = noeudAppareils.nextSibling();
            }
            else
            {
                noeudAppareils = noeudAppareils.nextSibling();
                root.removeChild(noeudAppareils.previousSibling());
            }
        }
        root.setAttribute("nbAppareils", 0);
        fichierAppareils.resize(0);
        fichierAppareils.write(documentAppareils.toByteArray());
        fichierAppareils.close();

        return true;
    }

}

/**
 * @brief Méthode d'ajout d'un projecteur dans appareils.xml
 *
 * @fn XMLUtilitaire::enregistrerAppareil
 *
 * @param nom QString le nom de l'appareil
 * @param type QString le type d'appareil
 * @param nbCanaux int le nombre de canaux pour l'appareil
 * @param canalDepart int le numéro de canal de départ de l'appareil
 * @param nomsCanaux[] QString un tableau contenant lesz libellés des canaux de l'appareil
 *
 * @return bool
 */
bool XMLUtilitaire::enregistrerAppareil(QString nom, QString type, int nbCanaux, int canalDepart, QString nomsCanaux[])
{
    qDebug() << Q_FUNC_INFO;
    if(!fichierAppareils.open(QIODevice::ReadWrite))
    {
        qDebug() << Q_FUNC_INFO << "Erreur ouverture" << fichierAppareils.fileName();
        return false;
    }
    else
    {
        QUuid monUuid = QUuid::createUuid();
        QDomDocument documentAppareils;
        documentAppareils.setContent(&fichierAppareils, false);
        QDomElement root = documentAppareils.documentElement();

        QDomElement nouvelAppareil = documentAppareils.createElement("appareil");
        nouvelAppareil.setAttribute("nom", nom);
        nouvelAppareil.setAttribute("nbCanaux", nbCanaux);
        nouvelAppareil.setAttribute("type", type);
        nouvelAppareil.setAttribute("uuid", monUuid.toString());

        QDomElement nouveauxCanaux[10];
        QDomText texteNomCanal;


        for(int i = 0; i < nbCanaux; i++ )
        {
            nouveauxCanaux[i] = documentAppareils.createElement("canal");
            nouveauxCanaux[i].setAttribute("id", canalDepart+i);
            texteNomCanal = documentAppareils.createTextNode(nomsCanaux[i]);
            nouveauxCanaux[i].appendChild(texteNomCanal);
            nouvelAppareil.appendChild(nouveauxCanaux[i]);
        }

        root.appendChild(nouvelAppareil);
        int nouveauNbAppareils = root.attribute("nbAppareils").toInt() + 1 ;
        root.setAttribute("nbAppareils", nouveauNbAppareils);

        QTextStream out(&fichierAppareils);
        fichierAppareils.resize(0);
        documentAppareils.save(out, 2);
        fichierAppareils.close();

        return true;
    }
}

/**
 * @brief Méthode de suppression d'un projecteur dans appareils.xml
 *
 * @fn XMLUtilitaire::supprimerAppareil
 *
 * @param uuid
 *
 * @return bool
 */
bool XMLUtilitaire::supprimerAppareil(QString uuid)
{
    qDebug() << Q_FUNC_INFO;
    if(!fichierAppareils.open(QIODevice::ReadWrite))
    {
        qDebug() << Q_FUNC_INFO << "Erreur ouverture" << fichierAppareils.fileName();
        return false;
    }
    else
    {
        QDomDocument documentAppareils;
        documentAppareils.setContent(&fichierAppareils, false);
        QDomElement root = documentAppareils.documentElement();
        QDomNode noeudAppareils = root.firstChild();
        while(!noeudAppareils.isNull())
        {
            QDomElement appareil = noeudAppareils.toElement();

            if(appareil.attribute("uuid") == uuid)
            {
                root.removeChild(noeudAppareils);
                int nouveauNbAppareils = root.attribute("nbAppareils").toInt() - 1 ;
                root.setAttribute("nbAppareils", nouveauNbAppareils);
                QTextStream out(&fichierAppareils);
                fichierAppareils.resize(0);
                documentAppareils.save(out, 2);
                fichierAppareils.close();
                return true;
            }
            noeudAppareils = noeudAppareils.nextSibling();
        }
        qDebug() << Q_FUNC_INFO << "Erreur supression !" ;
        fichierAppareils.close();

        return false;
    }
}

/**
 * @brief Méthode de modification d'un projecteur dans appareils.xml
 *
 * @fn XMLUtilitaire::modifierAppareil
 *
 * @param uuid QString l'uuid de l'appareil
 * @param nom QString le nom de l'appareil
 * @param type QString le type de l'appareil
 * @param nbCanaux int le nombre de canaux de l'appareil
 * @param canalDepart int le numéro de canal de départ de l'appareil
 * @param nomsCanaux[] QString un tableau contenant les libellés des canaux de l'appareil
 *
 * @return bool true si la modification a été réalisée sinon false
 */
bool XMLUtilitaire::modifierAppareil(QString uuid, QString nom, QString type, int nbCanaux, int canalDepart, QString nomsCanaux[])
{

    if(!fichierAppareils.open(QIODevice::ReadWrite))
    {
        qDebug() << Q_FUNC_INFO << "Erreur ouverture" << fichierAppareils.fileName();
        return false;
    }
    else
    {

        QDomDocument documentAppareils;
        documentAppareils.setContent(&fichierAppareils, false);
        QDomElement root = documentAppareils.documentElement();
        QDomNode noeudAppareils = root.firstChild();
        while(!noeudAppareils.isNull())
        {

            QDomElement appareil = noeudAppareils.toElement();
            QDomText texteNomCanal;
            QDomElement nouveauxCanaux[10];
            if(appareil.attribute("uuid") == uuid)
            {

                appareil.setAttribute("nom", nom);
                appareil.setAttribute("nbCanaux", nbCanaux);
                appareil.setAttribute("type", type);

                QDomNode noeudCanaux = appareil.firstChild();

                while(!noeudCanaux.isNull())
                {

                    if(!noeudCanaux.nextSibling().isNull())
                    {

                        noeudCanaux = noeudCanaux.nextSibling();
                        appareil.removeChild(noeudCanaux.previousSibling());
                    }
                    else
                    {

                        appareil.removeChild(noeudCanaux);
                        noeudCanaux = noeudCanaux.nextSibling();
                    }
                }

                for(int i = 0; i < nbCanaux; i++ )
                {
                    nouveauxCanaux[i] = documentAppareils.createElement("canal");
                    nouveauxCanaux[i].setAttribute("id", canalDepart+i);
                    texteNomCanal = documentAppareils.createTextNode(nomsCanaux[i]);
                    nouveauxCanaux[i].appendChild(texteNomCanal);
                    appareil.appendChild(nouveauxCanaux[i]);
                }


                QTextStream out(&fichierAppareils);
                fichierAppareils.resize(0);
                documentAppareils.save(out, 2);
                fichierAppareils.close();
                return true;
            }
            noeudAppareils = noeudAppareils.nextSibling();
        }
        qDebug() << Q_FUNC_INFO << "Erreur modification !" ;
        fichierAppareils.close();

        return false;
    }
}

/**
 * @brief Méthode recuperant toutes les scenes depuis scenes.xml pour affecter leurs differentes valeurs dans le conteneur scenesEnregistrees
 *
 * @fn XMLUtilitaire::lireScenes
 *
 * @param &scenesEnregistrees
 *
 * @return int
 */
int XMLUtilitaire::lireScenes(QVector<Scene> &scenesEnregistrees)
{
    //qDebug() << Q_FUNC_INFO;
    if(!fichierScenes.open(QIODevice::ReadWrite))
    {
        qDebug() << Q_FUNC_INFO << "Erreur ouverture" << fichierScenes.fileName();
        return 0;
    }
    else
    {
        scenesEnregistrees.clear();


        QDomDocument documentScenes;
        documentScenes.setContent(&fichierScenes, false);
        QDomElement root = documentScenes.documentElement();
        QDomNode noeudScenes = root.firstChild();
        Scene sceneAjoutee;

        while(!noeudScenes.isNull())
        {

            sceneAjoutee.nom = noeudScenes.toElement().attribute("nom");
            sceneAjoutee.uuid = noeudScenes.toElement().attribute("uuid");
            QDomNode canal = noeudScenes.firstChild();
            for( int i = 1; i <= 512; i++)
            {
               sceneAjoutee.valeursCanauxscene[i] = 0;
            }
            while(!canal.isNull())
            {
                sceneAjoutee.valeursCanauxscene[canal.toElement().attribute("id").toInt()] = canal.toElement().text().toInt() ;
                canal = canal.nextSibling();
            }
            scenesEnregistrees.push_back(sceneAjoutee);
            noeudScenes = noeudScenes.nextSibling();
        }
        fichierScenes.close();

        return scenesEnregistrees.size();
    }
}

/**
  *@brief Lit le fichier consoles.xml et remplis la QList consoles passée en paramètre en conséquence
  *
  *@fn XMLUtilitaire::lireConsoles
  *
  *@param consoles QList<Console>& une référence sur la liste des consoles
  *
  *@return bool true si la liste a été modifiée sinon false
  */
bool XMLUtilitaire::lireConsoles(QList<Console> &consoles)
{
    if (!fichierConsoles.open(QIODevice::ReadWrite))
    {
        qDebug() << Q_FUNC_INFO << "Erreur ouverture !" << fichierConsoles.fileName();
    }
    if(!(fichierConsoles.isOpen()))
    {
        QMessageBox::critical(0, QString::fromUtf8("Erreur"), QString::fromUtf8("Le fichier %1 n'a pas pu être ouvert !").arg(fichierConsoles.fileName()));
        return false;
    }
    else
    {
        QDomDocument documentXML;

        documentXML.setContent(&fichierConsoles, false);
        QDomElement racine = documentXML.documentElement(); // <consoles>
        if(racine.isNull())
        {
            qDebug()<< Q_FUNC_INFO << "<erreur racine !";
            fichierConsoles.close();
            return false;
        }

        QDomNode noeudConsole = racine.firstChild(); // <console>
        if(noeudConsole.isNull())
        {
            qDebug()<< Q_FUNC_INFO << "erreur racine vide !";
            fichierConsoles.close();
            return false;
        }

        while(!noeudConsole.isNull())
        {
            Console console;
            QDomElement elementConsole = noeudConsole.toElement(); // <console>
            if(elementConsole.isNull())
            {
                qDebug()<< Q_FUNC_INFO << "erreur element interface !";
                break;
            }

            QDomNode noeudAdresseIP = elementConsole.firstChild();
            QDomNode noeudPort = noeudAdresseIP.nextSibling();
            if(!noeudAdresseIP.isNull() && !noeudPort.isNull())
            {
                QDomElement elementAdresseIP = noeudAdresseIP.toElement(); // <adresseIP>
                QDomElement elementPort = noeudPort.toElement(); //<port>

                console.adresseIP = elementAdresseIP.text(); // l'adresse IP
                //qDebug() << console.adresseIP;
                console.port= elementPort.text().toInt(); // le port
                //qDebug() << console.port;
            }
            else qDebug()<< Q_FUNC_INFO << "erreur noeud adresseIP/port !";

            consoles.push_back(console);
            //qDebug() <<"consoles = " << consoles.count();
            noeudConsole = noeudConsole.nextSibling();
        }
    }

    fichierConsoles.close();

    return true;
}

/**
 * @brief Méthode d'enregistrement d'une scene dans scenes.xml
 *
 * @fn XMLUtilitaire::enregistrerScene
 *
 * @param scene Scene la nouvelle scene implémentée dans le fichier XML
 *
 * @return bool true si la modification a été réalisée sinon false
 */
bool XMLUtilitaire::enregistrerScene(Scene scene)
{

    if (!fichierScenes.open(QIODevice::ReadWrite))
    {
        qDebug() << Q_FUNC_INFO << "Erreur ouverture !" << fichierScenes.fileName();
        return false;
    }
    else
    {
        QDomDocument documentScenes;
        documentScenes.setContent(&fichierScenes, false);
        QDomElement root = documentScenes.documentElement();
        QDomText texteNomCanal;
        QDomElement nouvelleScene = documentScenes.createElement("scene");
        nouvelleScene.setAttribute("nom", scene.nom);
        nouvelleScene.setAttribute("uuid", scene.uuid);
        QDomElement nouveauxCanaux[512];
        for(int i = 0; i < 512; i ++)
        {
            if(scene.valeursCanauxscene[i] != 0)
            {
                nouveauxCanaux[i] =  documentScenes.createElement("canal");
                nouveauxCanaux[i].setAttribute("id", i);

                texteNomCanal = documentScenes.createTextNode(QString::number(scene.valeursCanauxscene[i]));
                nouveauxCanaux[i].appendChild(texteNomCanal);
                nouvelleScene.appendChild(nouveauxCanaux[i]);
            }
        }
        root.appendChild(nouvelleScene);

        QTextStream out(&fichierScenes);
        fichierScenes.resize(0);
        documentScenes.save(out, 2);
        fichierScenes.close();

        return true;
    }
}

/**
 * @brief Méthode de suppression d'une scene dans scenes.xml
 *
 * @fn XMLUtilitaire::supprimerScene
 *
 * @param uuid QString l'uuid de la scène à supprimer
 *
 * @return bool true si la modification a été réalisée sinon false
 */
bool XMLUtilitaire::supprimerScene(QString uuid)
{
    qDebug() << Q_FUNC_INFO;
    if(!fichierScenes.open(QIODevice::ReadWrite))
    {
        qDebug() << Q_FUNC_INFO << "Erreur ouverture" << fichierScenes.fileName();
        return false;
    }
    else
    {
        QDomDocument documentScenes;
        documentScenes.setContent(&fichierScenes, false);
        QDomElement root = documentScenes.documentElement();
        QDomNode noeudScenes = root.firstChild();
        while(!noeudScenes.isNull())
        {
            QDomElement scene = noeudScenes.toElement();
            qDebug() << "uuid XML:" << scene.attribute("uuid") ;
            qDebug() << "uuid argument:" << uuid ;
            if(scene.attribute("uuid") == uuid)
            {
                root.removeChild(noeudScenes);
                QTextStream out(&fichierScenes);
                fichierScenes.resize(0);
                documentScenes.save(out, 2);
                fichierScenes.close();
                return true;
            }
            noeudScenes = noeudScenes.nextSibling();
        }
        qDebug() << Q_FUNC_INFO << "Erreur supression !" ;
        fichierScenes.close();

        return false;
    }
}

/**
 * @brief Méthode d'enregistrement d'une séquence dans sequences.xml
 *
 * @fn XMLUtilitaire::enregistrerSequence
 *
 * @param sequence Sequence la nouvelle séquence à implémenter dans sequences.xml
 *
 * @return bool true si la modification a été réalisée sinon false
 */
bool XMLUtilitaire::enregistrerSequence(Sequence sequence)
{

    if (!fichierSequences.open(QIODevice::ReadWrite))
    {
        qDebug() << Q_FUNC_INFO << "Erreur ouverture !" << fichierSequences.fileName();
        return false;
    }
    else
    {
        QDomDocument documentSequences;
        documentSequences.setContent(&fichierSequences, false);
        QDomElement root = documentSequences.documentElement();
        QDomElement nouvelleSequence = documentSequences.createElement("sequence");
        nouvelleSequence.setAttribute("nom", sequence.nom);
        nouvelleSequence.setAttribute("uuid", sequence.uuid);
        qDebug() << Q_FUNC_INFO << "nombre de scenes:" << sequence.ensembleScenes.count();
        for(int i = 0; i < sequence.ensembleScenes.count(); i++)
        {

            QDomElement elementScene = documentSequences.createElement("scene");
            elementScene.setAttribute("uuid", sequence.ensembleScenes.at(i).scene.uuid);
            elementScene.setAttribute("temps", sequence.ensembleScenes.at(i).tempo);
            nouvelleSequence.appendChild(elementScene);
        }

        root.appendChild(nouvelleSequence);

        QTextStream out(&fichierSequences);
        fichierSequences.resize(0);
        documentSequences.save(out, 2);
        fichierSequences.close();

        return true;
    }

}

/**
  *@brief Réecrit le fichier consoles.xml et supprimant les éléments et en écrivant ceux présent dans la QList consoles passée en paramètre
  *
  *@fn XMLUtilitaire::ecrireConsole
  *
  *@param consoles const QList<Console>& une référence constante sur la liste des consoles
  *
  *@return bool
  */
bool XMLUtilitaire::ecrireConsole(const QList<Console> &consoles)
{
    if (!fichierConsoles.open(QIODevice::ReadWrite))
    {
        qDebug() << Q_FUNC_INFO << "Erreur ouverture !" << fichierConsoles.fileName();
    }
    if(!(fichierConsoles.isOpen()))
    {
        QMessageBox::critical(0, QString::fromUtf8("Erreur"), QString::fromUtf8("Le fichier %1 n'a pas pu être ouvert !").arg(fichierConsoles.fileName()));
        return false;
    }
    else
    {
        QDomDocument documentXML;

        fichierConsoles.resize(0);
        //documentXML.setContent(&fichierInterfaces, false);
        QDomNode xmlNode = documentXML.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
        documentXML.insertBefore(xmlNode, documentXML.firstChild());

        if(consoles.size() > 0)
        {
            QDomElement root = documentXML.createElement("consoles");
            documentXML.appendChild(root);

            for(int i = 0; i < consoles.size(); i++)
            {
                QDomElement elementConsole = documentXML.createElement("console");
                root.appendChild(elementConsole);

                QDomElement elementAdresseIP = documentXML.createElement("adresseIP");
                elementConsole.appendChild(elementAdresseIP);
                QDomText text = documentXML.createTextNode(consoles[i].adresseIP);
                elementAdresseIP.appendChild(text);

                QDomElement elementPort = documentXML.createElement("port");
                elementConsole.appendChild(elementPort);
                text = documentXML.createTextNode(QString::number(consoles[i].port));
                elementPort.appendChild(text);
            }
        }
        QTextStream out(&fichierConsoles);
        documentXML.save(out, 2);
    }

    fichierConsoles.close();

    return true;
}

/**
 * @brief Méthode de lecture des élements contenus dans sequences.xml
 *
 * @fn XMLUtilitaire::lireSequences
 *
 * @param &sequencesEnregistrees QVector<Sequence> réference à un conteneur d'objets Sequence
 *
 * @return int donnant le nombre de sequences recuperees
 */
int XMLUtilitaire::lireSequences(QVector<Sequence> &sequencesEnregistrees)
{
    //qDebug() << Q_FUNC_INFO;
    if(!fichierSequences.open(QIODevice::ReadWrite))
    {
        qDebug() << Q_FUNC_INFO << "Erreur ouverture" << fichierSequences.fileName();
        return 0;
    }
    else
    {
        sequencesEnregistrees.clear();


        QDomDocument documentSequences;
        documentSequences.setContent(&fichierSequences, false);
        QDomElement root = documentSequences.documentElement();
        QDomNode noeudSequences = root.firstChild();
        Sequence sequenceAjoutee;
        SceneSequence sceneSequenceAjoutee;
        while(!noeudSequences.isNull())
        {

            sequenceAjoutee.nom = noeudSequences.toElement().attribute("nom");
            sequenceAjoutee.uuid = noeudSequences.toElement().attribute("uuid");
            QDomNode noeudScenesSequence = noeudSequences.firstChild();

            sequenceAjoutee.ensembleScenes.clear();
            while(!noeudScenesSequence.isNull())
            {

                sceneSequenceAjoutee.scene.uuid = noeudScenesSequence.toElement().attribute("uuid");
                sceneSequenceAjoutee.tempo = noeudScenesSequence.toElement().attribute("temps").toInt();
                lireSceneSequence(sceneSequenceAjoutee);
                sequenceAjoutee.ensembleScenes.append(sceneSequenceAjoutee);
                noeudScenesSequence = noeudScenesSequence.nextSibling();
            }

            sequencesEnregistrees.push_back(sequenceAjoutee);
            noeudSequences = noeudSequences.nextSibling();
        }
        fichierSequences.close();

        return sequencesEnregistrees.size();
    }
}

/**
 * @brief Méthode de lecture d'une scene requise pour une sequence dans scenes.xml
 *
 * @fn XMLUtilitaire::lireSceneSequence
 *
 * @param &scene SceneSequence réference à une SceneSequence
 *
 * @return bool true si l'operation a été réalisée sinon false
 */
bool XMLUtilitaire::lireSceneSequence(SceneSequence &scene)
{
    if (!fichierScenes.open(QIODevice::ReadWrite))
    {
        qDebug() << Q_FUNC_INFO << "Erreur ouverture !" << fichierScenes.fileName();
        return false;
    }
    else
    {
        QDomDocument documentScenes;
        documentScenes.setContent(&fichierScenes, false);
        QDomElement root = documentScenes.documentElement();
        QDomNode noeudScenes = root.firstChild();
        while(!noeudScenes.isNull())
        {
            if(scene.scene.uuid == noeudScenes.toElement().attribute("uuid"))
            {
                scene.scene.nom = noeudScenes.toElement().attribute("nom");
                QDomNode canal = noeudScenes.firstChild();
                for( int i = 1; i <= 512; i++)
                {
                   scene.scene.valeursCanauxscene[i] = 0;
                }
                while(!canal.isNull())
                {
                    scene.scene.valeursCanauxscene[canal.toElement().attribute("id").toInt()] = canal.toElement().text().toInt() ;
                    canal = canal.nextSibling();
                }
                fichierScenes.close();
                return true;

            }
            noeudScenes = noeudScenes.nextSibling();
        }
    }
    fichierScenes.close();
    return false;
}


/**
 * @brief Méthode de suppression d'une séquence dans sequences.xml
 *
 * @fn XMLUtilitaire::supprimerSequence
 *
 * @param uuid QString uuid de la séquence concernée
 *
 * @return bool true si la modification a été réalisée sinon false
 */
bool XMLUtilitaire::supprimerSequence(QString uuid)
{
    qDebug() << Q_FUNC_INFO;
    if(!fichierSequences.open(QIODevice::ReadWrite))
    {
        qDebug() << Q_FUNC_INFO << "Erreur ouverture" << fichierSequences.fileName();
        return false;
    }
    else
    {
        QDomDocument documentSequences;
        documentSequences.setContent(&fichierSequences, false);
        QDomElement root = documentSequences.documentElement();
        QDomNode noeudSequences = root.firstChild();
        while(!noeudSequences.isNull())
        {
            QDomElement sequence = noeudSequences.toElement();
            qDebug() << "uuid XML:" << sequence.attribute("uuid") ;
            qDebug() << "uuid argument:" << uuid ;
            if(sequence.attribute("uuid") == uuid)
            {
                root.removeChild(noeudSequences);
                QTextStream out(&fichierSequences);
                fichierSequences.resize(0);
                documentSequences.save(out, 2);
                fichierSequences.close();
                return true;
            }
            noeudSequences = noeudSequences.nextSibling();
        }
        qDebug() << Q_FUNC_INFO << "Erreur supression !" ;
        fichierSequences.close();

        return false;
    }
}

/**
 * @brief Méthode de lecture des élements contenus dans spectacles.xml
 *
 * @fn XMLUtilitaire::lireSpectacles
 *
 * @param &spectaclesEnregistres QVector<Spectacle> réference à un conteneur d'objets Spectacle
 *
 * @return int donnant le nombre de spectacles recuperees
 */
int XMLUtilitaire::lireSpectacles(QVector<Spectacle> &spectaclesEnregistres)
{
    if(!fichierSpectacles.open(QIODevice::ReadWrite))
    {
        qDebug() << Q_FUNC_INFO << "Erreur ouverture" << fichierSpectacles.fileName();
        return 0;
    }
    else
    {
        spectaclesEnregistres.clear();
        QDomDocument documentSpectacles;
        documentSpectacles.setContent(&fichierSpectacles, false);
        QDomElement root = documentSpectacles.documentElement();
        QDomNode noeudSpectacles = root.firstChild();
        Spectacle spectacle;

        while(!noeudSpectacles.isNull())
        {
            QDomElement elementSpectacle = noeudSpectacles.toElement();
            spectacle.nom = elementSpectacle.attribute("nom");
            spectacle.uuid = elementSpectacle.attribute("uuid");

            QDomNode noeudSequences = elementSpectacle.firstChild();
            Sequence sequence;
            while(!noeudSequences.isNull())
            {
                sequence.nom = noeudSequences.toElement().attribute("nom");
                sequence.uuid = noeudSequences.toElement().attribute("uuid");
                lireSequenceSpectacle(sequence);
                spectacle.ensembleSequences.append(sequence);
                noeudSequences = noeudSequences.nextSibling();
            }


            spectaclesEnregistres.append(spectacle);
            noeudSpectacles = noeudSpectacles.nextSibling();
        }

        fichierSpectacles.close();
        return spectaclesEnregistres.size();
    }
}

/**
 * @brief Méthode de lecture d'une sequence requise pour un spectacle dans sequences.xml
 *
 * @fn XMLUtilitaire::lireSequenceSpectacle
 *
 * @param &sequence Sequence réference à une Sequence
 *
 * @return bool true si l'operation a été réalisée sinon false
 */
bool XMLUtilitaire::lireSequenceSpectacle(Sequence &sequence)
{
    qDebug() << Q_FUNC_INFO;
    if(!fichierSequences.open(QIODevice::ReadWrite))
    {
        qDebug() << Q_FUNC_INFO << "Erreur ouverture" << fichierSequences.fileName();
        return false;
    }
    else
    {
        QDomDocument documentSequences;
        documentSequences.setContent(&fichierSequences, false);
        QDomElement root = documentSequences.documentElement();
        QDomNode noeudSequences = root.firstChild();

        while(!noeudSequences.isNull())
        {
            if(sequence.uuid == noeudSequences.toElement().attribute("uuid"))
            {
                QDomNode noeudScenes = noeudSequences.toElement().firstChild();
                SceneSequence scene;
                while(!noeudScenes.isNull())
                {
                    scene.tempo = noeudScenes.toElement().attribute("temps").toInt();
                    scene.scene.uuid = noeudScenes.toElement().attribute("uuid");
                    lireSceneSequence(scene);
                    sequence.ensembleScenes.append(scene);
                    noeudScenes = noeudScenes.nextSibling();
                }
                fichierSequences.close();
                return true;
            }
            noeudSequences = noeudSequences.nextSibling();
        }
        fichierSequences.close();

        return false;

    }
}

/**
 * @brief Méthode d'écriture d'une sequence dans sequences.xml
 *
 * @fn XMLUtilitaire::lireSequenceSpectacle
 *
 * @param &sequences QVector<Spectacle> réference à un conteneur de Spectacles
 *
 * @return bool true si l'operation a été réalisée sinon false
 */
bool XMLUtilitaire::ecrireSpectacles(QVector<Spectacle> &spectacles)
{
    if(!fichierSpectacles.open(QIODevice::ReadWrite))
    {
        qDebug() << Q_FUNC_INFO << "Erreur ouverture" << fichierSpectacles.fileName();
        return 0;
    }
    else
    {
        QDomDocument documentSpectacles;

        fichierSpectacles.resize(0);
        //documentXML.setContent(&fichierSpectacles, false);
        QDomNode xmlNode = documentSpectacles.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
        documentSpectacles.insertBefore(xmlNode, documentSpectacles.firstChild());
        if(spectacles.size() > 0)
        {
            QDomElement root = documentSpectacles.createElement("spectacles");
            documentSpectacles.appendChild(root);

            for(int i = 0; i < spectacles.size(); i++)
            {
               QDomElement elementSpectacle = documentSpectacles.createElement("spectacle");
               root.appendChild(elementSpectacle);
               elementSpectacle.setAttribute("nom",spectacles[i].nom);
               elementSpectacle.setAttribute("uuid",spectacles[i].uuid);
               for(int j =0; j < spectacles[i].ensembleSequences.size(); j++)
               {
                   QDomElement elementSequence = documentSpectacles.createElement("sequence");
                   elementSpectacle.appendChild(elementSequence);
                   elementSequence.setAttribute("nom", spectacles[i].ensembleSequences[j].nom);
                   elementSequence.setAttribute("uuid", spectacles[i].ensembleSequences[j].uuid);
               }
            }

         }
        QTextStream out(&fichierSpectacles);
        fichierSpectacles.resize(0);
        documentSpectacles.save(out, 2);
    }
    fichierSpectacles.close();
    return true;

}
