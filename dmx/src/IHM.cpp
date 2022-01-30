#include "IHM.h"

#include "enttecdmxusb.h"
#include "DMXProjecteur.h"
#include "xmlutilitaire.h"
#include "PlaybackWing.h"

#include <QDebug>

/**
  *
  * \file IHM.cpp
  *
  * \class IHM
  *
  * \brief Définition de la classe IHM
  *
  * \author Demont Thomas, Reynier Tony
  *
  * \version 1.0
  *
  */


/**
 * @brief Constructeur de la fenêtre principale
 *
 * @fn IHM::IHM
 *
 * @param parent QObject Adresse de l'objet Qt parent ici 0 car c'est la fenêtre principale
 */
IHM::IHM(QWidget *parent) : QWidget(parent)
{
    //qDebug() << Q_FUNC_INFO;

    interfaceDMX = NULL;
    wing = new PlaybackWing(this);
    utilitaireDocuments = new XMLUtilitaire(this);
    utilitaireDocuments->lireAppareils(projecteursDMX);
    for(int i = 0; i < projecteursDMX.count(); i++)
    {
        projecteursDMX.at(i)->setEnttecDMXUSB(interfaceDMX);
    }

    utilitaireDocuments->lireInterfaces(interfaces);
    canalDepartFaders = 1;
    pilotageIHMActif = true;
    pilotageDMXActif = false;

    //Mise en page
    creerIHM();
    genererFenetreNouvelleScene();
    genererFenetreNouvelleSequence();
    genererFenetreNouveauProjecteur();
    genererFenetreInformationsAppareil();
    // Sélectionne l'interface par défaut
    recupererDerniereInterfaceUtilisee();

    connect(wing, SIGNAL(envoyerEtatTouchesControle(EtatTouchesControle)), this, SLOT(afficherEtatTouchesControle(EtatTouchesControle)));
    connect(wing, SIGNAL(envoyerEtatFaders(EtatFaders)), this, SLOT(afficherEtatFaders(EtatFaders)));
    for (int i = 0; i < NB_SLIDERS ; ++i)
    {
        if(slidersCanaux[i] != NULL)
        {
            if(i == 0)
            {
                connect(slidersCanaux[0], SIGNAL(canalChange(int)), this, SLOT(changerCanalDepart(int)));
                connect(slidersCanaux[i], SIGNAL(sliderChange(int,int)), this, SLOT(envoyerTrameDMX(int,int)));
            }
            else
            {
                connect(slidersCanaux[i], SIGNAL(sliderChange(int,int)), this, SLOT(envoyerTrameDMX(int,int)));
            }
        }
    }
    connect(listeChoixPilotage, SIGNAL(activated(QString)), this, SLOT(changerPilotageIHM(QString)));
    connect(listeProjecteursPilotage, SIGNAL(currentIndexChanged(int)), this, SLOT(selectionnerProjecteursPilotage(int)));
    connect(boutonIncrementationCanal, SIGNAL(clicked()), this, SLOT(incrementerCanal()));
    connect(boutonDecrementationCanal, SIGNAL(clicked()), this, SLOT(decrementerCanal()));
    connect(boutonDetecterInterface, SIGNAL(clicked()), this, SLOT(detecterInterface()));
    connect(listeChoixInterface, SIGNAL(currentIndexChanged(int)), this, SLOT(selectionnerInterface(int)));
    connect(boutonNouveauProjecteur, SIGNAL(clicked()), this, SLOT(ouvrirFenetreNouveauProjecteur()));
    connect(boutonEnregistrer, SIGNAL(clicked()), this, SLOT(enregistrerProjecteur()));
    connect(boutonSupprimerTousProjecteurs, SIGNAL(clicked()), this, SLOT(supprimerTousProjecteurs()));
    connect(actionQuitter, SIGNAL(triggered()), this, SLOT(quitter()));
    connect(boutonAjouterInterface, SIGNAL(clicked()), this, SLOT(recupererDonneesNouvelleInterface()));

    connect(boutonSupprimerInterface, SIGNAL(clicked()), this, SLOT(supprimerInterface()));

    connect(boutonNouvelleScene, SIGNAL(clicked()), this, SLOT(ouvrirFenetreNouvelleScene()));
    connect(boutonParametresCanauxScene, SIGNAL(clicked()), this, SLOT(ouvrirFenetreCanauxScene()));

    connect(boutonexecuterScene, SIGNAL(clicked()),this,SLOT(executerSceneDefault()));
    connect(boutonSupprimerScene, SIGNAL(clicked()),this,SLOT(supprimerScene()));

    connect(boutonNouvelleSequence, SIGNAL(clicked()),this,SLOT(ouvrirFenetreNouvelleSequence()));
    connect(boutonSupprimmerSequence, SIGNAL(clicked()),this,SLOT(supprimerSequence()));
    connect(boutonExecuterSequence, SIGNAL(clicked()),this,SLOT(executerSequenceDefault()));

    connect(boutonchoixSequenceSpectacle, SIGNAL(clicked()),this,SLOT(ajouterSequenceSpectacle()));
    connect(boutonSauvegarderSpectacle, SIGNAL(clicked()),this,SLOT(enregistrerSpectacle()));
    connect(boutonExecuterSpectacle, SIGNAL(clicked()),this,SLOT(executerSpectacle()));
    connect(boutonSupprimerSpectacle, SIGNAL(clicked()),this,SLOT(supprimerSpectacle()));

    connect(boutonAjouterConsole, SIGNAL(clicked()), this, SLOT(ajouterConsole()));
    connect(boutonSupprimerConsole, SIGNAL(clicked()), this, SLOT(supprimerConsole()));
    connect(boutonModifierConsole, SIGNAL(clicked()), this ,SLOT(genererFenetreModifierConsole()));
    wing->demarrer();

    pilotageDMXActif = true;
}

/**
 * @brief Destructeur
 *

 * @fn IHM::~IHM
 */
IHM::~IHM()
{
    wing->envoyerCanalDepartWing(1);
    utilitaireDocuments->ecrireInterfaces(interfaces);
    for(int i = 0; i < projecteursDMX.count(); i++)
    {
        delete projecteursDMX.at(i);
    }
    delete interfaceDMX;
    //qDebug() << Q_FUNC_INFO;
}

/**
 * @brief Démarre la réception des données en provenance de la console WING
 *
 * @fn IHM::demarrer
 *
 * @return void
 */
void IHM::demarrer()
{
    //qDebug() << Q_FUNC_INFO;
    wing->demarrer();
}

/**
 * @brief Méthode régissant l'appui des touches de contrôles (Page Up, PAge Down, Back, Go) de la console wing
 *
 * @fn IHM::afficherEtatTouchesControle
 *
 * @param etatTouchesControle structure regroupant l'ensemble des états des touches de la console WING
 *
 * @return void
 */
void IHM::afficherEtatTouchesControle(EtatTouchesControle etatTouchesControle)
{
    QString etats = QString::fromUtf8("Etats touches : ") + QString::number(etatTouchesControle.touchePageUp) + QString::number(etatTouchesControle.touchePageDown) + QString::number(etatTouchesControle.toucheBack) + QString::number(etatTouchesControle.toucheGo);

    if(!etatTouchesControle.toucheBack && !etatTouchesControle.touchePageUp)
    {
        int index = listeProjecteursPilotage->currentIndex();
        if(index != listeProjecteursPilotage->count() - 1)
        {
            listeProjecteursPilotage->setCurrentIndex(index + 1);
        }
        else qDebug() << "Dernier projecteur atteint";

    }
    if(!etatTouchesControle.toucheBack && !etatTouchesControle.touchePageDown)
    {
        int index = listeProjecteursPilotage->currentIndex();
        if(index != 0)
        {
            listeProjecteursPilotage->setCurrentIndex(index - 1);
        }
        else qDebug() << "Premier projecteur atteint";

    }

    //qDebug() << Q_FUNC_INFO << etats;

    if(!pilotageIHMActif)
    {
        if(etatTouchesControle.touchePageUp == 0)
        {
            if(etatTouchesControle.toucheBack)
            incrementerCanal();
            //qDebug() << Q_FUNC_INFO << "canal depart = " << canalDepartFaders;
        }
        if(etatTouchesControle.touchePageDown == 0)
        {
            if(etatTouchesControle.toucheBack)
            decrementerCanal();
            //qDebug() << Q_FUNC_INFO << "canal depart = " << canalDepartFaders;
        }
    }
    if(etatTouchesControle.toucheBack == 0)
    {
        desactiverPilotageIHM();
        listeChoixPilotage->setCurrentIndex(1);
    }
    if(etatTouchesControle.toucheGo == 0)
    {
        activerPilotageIHM();
        listeChoixPilotage->setCurrentIndex(0);
    }

    setCanal();
}

/**
 * @brief Affiche les valeurs des faders de la console WING sur les sliders de l'IHM
 *
 * @fn IHM::afficherEtatFaders
 *
 * @param etatFaders EtatFaders structure regroupant l'ensemble des états des faders de la console WING
 *
 * @return void
 */
void IHM::afficherEtatFaders(EtatFaders etatFaders)
{
        setCanal();

        slidersCanaux[0]->setValeur(etatFaders.fader0);
        slidersCanaux[1]->setValeur(etatFaders.fader1);
        slidersCanaux[2]->setValeur(etatFaders.fader2);
        slidersCanaux[3]->setValeur(etatFaders.fader3);
        slidersCanaux[4]->setValeur(etatFaders.fader4);
        slidersCanaux[5]->setValeur(etatFaders.fader5);
        slidersCanaux[6]->setValeur(etatFaders.fader6);
        slidersCanaux[7]->setValeur(etatFaders.fader7);
        slidersCanaux[8]->setValeur(etatFaders.fader8);
        slidersCanaux[9]->setValeur(etatFaders.fader9);
}

/**
 * @brief Envoie une valeur sur un canal DMX
 *
 * @fn IHM::envoyerTrameDMX
 *
 * @param valeurCanal int le numéro de canal
 *
 * @param valeurFader int la valeur associée au canal sélectionné
 *
 * @return void
 */
void IHM::envoyerTrameDMX(int valeurCanal,int valeurFader)
{
    if(pilotageDMXActif)
    {
        if(interfaceDMX != NULL)
        {
            interfaceDMX->SetCanalDMX(valeurCanal, valeurFader);
            interfaceDMX->SendDMX();
        }
    }        
}

/**
 * @brief Active ou désactive le pilotage des projecteurs depuis l'IHM par rapport à l'état du QComboBox listeChoixPilotage
 *
 * @fn IHM::changerPilotageIHM
 *
 * @param choix
 *
 * @return void
 */
void IHM::changerPilotageIHM(QString choix)
{
    //qDebug() << Q_FUNC_INFO << choix;

    if(choix == QString::fromUtf8("IHM"))
    {        
        activerPilotageIHM();
    }
    else if(choix == QString::fromUtf8("Console"))
    {
        desactiverPilotageIHM();
    }
}

/**
 * @brief Méthode permettant d'incrémenter la valeur de départ du canal
 *
 * @fn IHM::incrementerCanal
 *
 * @return void
 */
void IHM::incrementerCanal()
{
    pilotageDMXActif = false;
    //canalDepartFaders++;
    canalDepartFaders = (canalDepartFaders + NB_SLIDERS) % NB_CANAUX;
    qDebug() << "Canal depart =" << canalDepartFaders;
    setCanal();
    pilotageDMXActif = true;
    envoyerCanalDepartWing();
}

/**
 * @brief Méthode permettant de décrémenter la valeur de départ du canal
 *
 * @fn IHM::decrementerCanal
 *
 * @return void
 */
void IHM::decrementerCanal()
{
    pilotageDMXActif = false;
    //canalDepartFaders--;
    canalDepartFaders = ((canalDepartFaders + NB_CANAUX) - NB_SLIDERS) % NB_CANAUX;
    qDebug() << "Canal depart =" << canalDepartFaders;
    setCanal();
    pilotageDMXActif = true;
    envoyerCanalDepartWing();
}

/**
 * @brief
 *
 * @fn IHM::changerCanalDepart
 *
 * @param canal
 */
void IHM::changerCanalDepart(int canal)
{
    pilotageDMXActif = false;
    canalDepartFaders = canal % NB_CANAUX;
    setCanal();
    pilotageDMXActif = true;
    envoyerCanalDepartWing();

}

/**
  *@brief Méthode gérant le type de projecteurs séléctionné (sur la liste proposée), l'affichage ou non des sliders selon le nombre de canaux
 du projecteur séléctionné et l'incrémentation du canal de départ par rapport au numéro indiqué dans le type de projecteur.
  *
  *@fn IHM::selectionnerProjecteursPilotage
  *
  *@param index
  *
  *@return void
  */   // //
void IHM::selectionnerProjecteursPilotage(int index)
{
    if(index == 0)
    {
        //qDebug() << Q_FUNC_INFO << index << "Aucun";
        pilotageDMXActif = false;
        boutonIncrementationCanal->setEnabled(true);
        boutonDecrementationCanal->setEnabled(true);
        canalDepartFaders = 1;
        setCanal();
        pilotageDMXActif = true;
        return;
    }

    DMXProjecteur* projecteurDMX = projecteursDMX.at(index-1);
    //qDebug() << Q_FUNC_INFO << index << projecteurDMX->getNom() << projecteurDMX->getType();
    pilotageDMXActif = false;
    for(int i = 0; i < NB_SLIDERS; i ++)
    {
        if(i >= projecteurDMX->getNomCanaux().size())
        {
            slidersCanaux[i]->hide();
        }
        else
        {
            slidersCanaux[i]->setConnecte(false);
            slidersCanaux[i]->setCanalEnabled(false);
            slidersCanaux[i]->setCanal(projecteurDMX->getCanalDebut() + i);
            slidersCanaux[i]->setNomCanal(projecteurDMX->getNomCanal(projecteurDMX->getCanalDebut() + i));
            slidersCanaux[i]->show();
            slidersCanaux[i]->setConnecte(true);
        }
    }
    canalDepartFaders = projecteurDMX->getCanalDebut();
    boutonIncrementationCanal->setEnabled(false);
    boutonDecrementationCanal->setEnabled(false);
    pilotageDMXActif = true;
    envoyerCanalDepartWing();
}


/**
  *@brief Execute la commande Syteme passée en paramètre
  *
  *@fn IHM::executerCommandeSysteme
  *
  *@param commande QString
  *
  *@return QStringList
  */
QStringList IHM::executerCommandeSysteme(QString commande)
{
#ifdef Q_OS_WIN
    // Windows
    //wchar_t wNom[256];
    //wchar_t wLigne[1024];
    char wLigne[1024];
    FILE *wResultat;
    QString wReponse;

    QTextCodec *codec = QTextCodec::codecForName("850");
    wResultat = popen(commande.toLocal8Bit().constData(), "r");
    fgets(wLigne, 1024, wResultat);
    while(! feof(wResultat))
    {
        wReponse += codec->toUnicode(wLigne);
        fgets(wLigne, 1024, wResultat);
    }
    pclose(wResultat);

    //qDebug() << Q_FUNC_INFO << wReponse;

    wReponse.chop(1);
    QStringList wReponses = wReponse.split("\n");

    return wReponses;

#elif defined(Q_OS_UNIX)
    // Unix
    FILE *resultat;
    char ligne[1024];
    QString reponse;

    // lit l'état de la connexion
    resultat = popen(commande.toAscii().constData(), "r");
    fgets(ligne, 1024, resultat);
    while (! feof(resultat))
    {
        reponse += ligne;
        fgets(ligne, 1024, resultat);
    }
    pclose(resultat);

    // enlève le dernier \n
    reponse.chop(1);
    QStringList reponses = reponse.split("\n");

    //qDebug() << Q_FUNC_INFO << reponses;

    return reponses;
#  endif    
}

/**
  *@brief Detecte les interfaces séries connectées au PC, les affiches sur l'IHM, et si l'interface sélectionnée (dans la liste du choix de l'interface) est une interface Enttec Pro, affiche ses caractéristiques.
  *
  *@fn IHM::detecterInterface
  *
  *@return void
  */
void IHM::detecterInterface()
{
    int numeroInterface = listeChoixInterface->currentIndex();
    EnttecInterfaces type = (EnttecInterfaces)listeTypeInterface->currentIndex();
    EnttecDMXUSB *interfaceEnttec;
    bool statut = false;

    if(interfaces.count() == 0)
        return;

    #ifdef Q_OS_WIN
        if(interfaceDMX != NULL && interfaceDMX->IsOpen())
        {
            interfaceDMX->closePort();
            statut = true;
        }
        //resultatDetectionInterface->append(executerCommandeSysteme("devcon find *").join("\n"));
        resultatDetectionInterface->append("Interfaces disponibles : ");
        resultatDetectionInterface->append(executerCommandeSysteme("mode").join("\n"));
        if(statut)
            interfaceDMX->openPort(interfaceDMX->GetPortInterface());
    #elif defined(Q_OS_UNIX)
        resultatDetectionInterface->append(executerCommandeSysteme("lsusb | grep -i serial").join("\n"));
        resultatDetectionInterface->append("Interfaces disponibles : ");
        resultatDetectionInterface->append(executerCommandeSysteme("ls /dev/ttyUSB*").join("\n"));
    #endif

    interfaceEnttec = new EnttecDMXUSB(type, interfaces.at(numeroInterface).portInterface.toLocal8Bit().data());
    if(interfaceDMX != NULL && interfaceDMX->IsAvailable())
    {
        if((EnttecInterfaces)listeTypeInterface->currentIndex() == DMX_USB_PRO)
        {
            resultatDetectionInterface->append(QString::fromUtf8("Interface Enttec DMX USB PRO séléctionnée détectée\n") + QString::fromUtf8("Version : ") + QString::number(interfaceEnttec->GetFirmware_H()) + "." + QString::number(interfaceEnttec->GetFirmware_L()) + "\n");
        }
        else
        {
            resultatDetectionInterface->append(QString::fromUtf8("Interface Enttec OPEN DMX USB séléctionnée détectée \n"));
        }
        Interface intf = interfaces.at(numeroInterface);
        intf.typeInterface = type;
        interfaces.replace(numeroInterface, intf);
    }
    else
    {
        resultatDetectionInterface->append(QString::fromUtf8("Interface Enttec non détectée \n"));
    }

    delete interfaceEnttec;
}

void IHM::selectionnerInterface(int index)
{
    if(interfaces.count() == 0)
    {
        if(interfaceDMX != NULL)
            delete interfaceDMX;
        interfaceDMX = NULL;
        return;
    }

    if(interfaceDMX != NULL)
        delete interfaceDMX;

    for(int i = 0; i < interfaces.count(); i++)
    {
        interfaces[i].utilisee = 0;
    }

    interfaceDMX = new EnttecDMXUSB(interfaces.at(index).typeInterface, interfaces.at(index).portInterface.toLocal8Bit().data());
    interfaces[index].utilisee = 1;

    if(interfaceDMX->IsOpen())
    {
        for(int i = 0; i < projecteursDMX.count(); i++)
        {
            projecteursDMX.at(i)->setEnttecDMXUSB(interfaceDMX);
        }
    }
    else
    {
        QMessageBox::critical(0, QString::fromUtf8("DMX 2018"), QString::fromUtf8("Impossible d'ouvrir l'interface DMX !"));
        delete interfaceDMX;
        interfaceDMX = NULL;
    }
    resultatDetectionInterface->setText(QString::fromUtf8(""));
}

/**
 * @brief Ferme l'application
 *
 * @fn IHM::quitter
 *
 * @return void
 */
void IHM::quitter()
{
    close();
}

// Méthodes privées

/**
 * @brief Crée la fenêtre principale
 *
 * @fn IHM::creerIHM
 */
void IHM::creerIHM()
{
    // Crée les onglets
    ongletsPrincipaux = new QTabWidget(this);
    optionsCreations= new QTabWidget(this);
    optionsParametres= new QTabWidget(this);
    optionsSpectacles= new QTabWidget(this);

    // Crée les pages
    pageAssembler = new QWidget(this);
    pageCreer = new QWidget(this);
    pageJouer = new QWidget(this); // 3
    pagePiloter = new QWidget(this); // 2
    parametresInterface = new QWidget(this);
    parametresProjecteurs = new QWidget(this);
    parametresConsoles = new QWidget(this);
    pagesSpectacles[0] = new QWidget(this);
    pagesSpectacles[1] = new QWidget(this);

    // Place les pages dans les onglets
    ongletsPrincipaux->addTab(optionsCreations, QString::fromUtf8("Créer un spectacle"));
    ongletsPrincipaux->addTab(optionsSpectacles, QString::fromUtf8("Jouer un spectacle"));
    ongletsPrincipaux->addTab(pagePiloter, QString::fromUtf8("Piloter les projecteurs"));
    ongletsPrincipaux->addTab(optionsParametres, QString::fromUtf8("Configurer le système"));
    optionsCreations->addTab(pageCreer, QString::fromUtf8("Créer les scènes"));
    optionsCreations->addTab(pageAssembler, QString::fromUtf8("Assembler les scènes/Créer des Séquences"));
    optionsParametres->addTab(parametresInterface, QString::fromUtf8("Paramétrer l'interface"));
    optionsParametres->addTab(parametresProjecteurs, QString::fromUtf8("Paramétrer les projecteurs"));
    optionsParametres->addTab(parametresConsoles, QString::fromUtf8("Paramétrer les consoles"));
    optionsSpectacles->addTab(pagesSpectacles[0], QString::fromUtf8("Création de Spectacles"));
    optionsSpectacles->addTab(pagesSpectacles[1], QString::fromUtf8("Jouer un Spectacle"));

    // Crée les widgets
    listeProjecteursEnregistres = new QScrollArea(this);
    boutonNouveauProjecteur = new QPushButton(QString::fromUtf8("Nouveau Projecteur"), this);
    boutonSupprimerTousProjecteurs = new QPushButton(QString::fromUtf8("Tout supprimer"), this);
    for (int i = 0; i < NB_SLIDERS ; ++i)
    {
       slidersCanaux[i] = new MySlider(this);
       if(i > 0)
           slidersCanaux[i]->setCanalEnabled(false);
    }
    slidersCanaux[0]->setCanal(canalDepartFaders);
    listeChoixPilotage = new QComboBox(this);
    listeChoixPilotage->addItem(QString::fromUtf8("IHM"));
    listeChoixPilotage->addItem(QString::fromUtf8("Console"));
    labelInterface = new QLabel(this);
    labelInterface->setText("Interface : ");
    listeProjecteursPilotage = new QComboBox(this);
    listeProjecteursPilotage->addItem(QString::fromUtf8(""));
    for(int i = 0; i < projecteursDMX.count(); i++)
    {
        listeProjecteursPilotage->addItem(projecteursDMX.at(i)->getNom() + " (" + projecteursDMX.at(i)->getType() + ")");
    }
    boutonIncrementationCanal = new QPushButton(QString::fromUtf8(">>"), this);
    boutonDecrementationCanal = new QPushButton(QString::fromUtf8("<<"), this);
    setCanal();
    listeChoixInterface = new QComboBox(this);
    //listeChoixInterface->addItem(QString::fromUtf8(""));
    for(int i = 0; i < interfaces.count(); i++)
    {
        if(interfaces.at(i).typeInterface == OPEN_DMX_USB)
            listeChoixInterface->addItem(interfaces.at(i).portInterface);
        else if(interfaces.at(i).typeInterface == DMX_USB_PRO)
            listeChoixInterface->addItem(interfaces.at(i).portInterface);
        else
            listeChoixInterface->addItem(interfaces.at(i).portInterface);
    }
    listeTypeInterface = new QComboBox(this);
    listeTypeInterface->addItem(QString::fromUtf8("OPEN DMX USB"));
    listeTypeInterface->addItem(QString::fromUtf8("DMX USB PRO"));
    if(interfaces.count() > 0)
    {
        if(interfaces.at(0).typeInterface == OPEN_DMX_USB)
            listeTypeInterface->setCurrentIndex((int)OPEN_DMX_USB);
        else if(interfaces.at(0).typeInterface == DMX_USB_PRO)
            listeTypeInterface->setCurrentIndex((int)DMX_USB_PRO);
        else
            listeTypeInterface->setCurrentIndex((int)DMX_USB_PRO);
    }
    boutonDetecterInterface = new QPushButton(QString::fromUtf8("Detecter Interfaces"), this);
    boutonDetecterInterface->setFixedSize(boutonDetecterInterface->sizeHint());
    resultatDetectionInterface = new QTextEdit(this);
    resultatDetectionInterface->setReadOnly(true);

    portInterfaceAjouterInterface = new QLineEdit(this);
    portInterfaceAjouterInterface->setPlaceholderText("port Interface");
    listeTypeInterfaceAjouterInterface = new QComboBox(this);
    listeTypeInterfaceAjouterInterface->addItem(QString::fromUtf8("OPEN DMX USB"));
    listeTypeInterfaceAjouterInterface->addItem(QString::fromUtf8("DMX USB PRO"));
    boutonAjouterInterface = new QPushButton(QString::fromUtf8("Ajouter Interface"), this);
    listeInterfacesSupprimables = new QComboBox(this);
    for(int i = 0; i < interfaces.count(); i++)
    {
        listeInterfacesSupprimables->addItem(interfaces[i].portInterface);
    }
    boutonSupprimerInterface = new QPushButton(QString::fromUtf8("Supprimer Interface"), this);


    labelScenesDisponibles = new QLabel(QString::fromUtf8("Scènes disponibles :"), this);
    choixScenesDisponibles = new QComboBox;
    int nbScenes = utilitaireDocuments->lireScenes(scenes);
    for(int i = 0; i < nbScenes; i++)
    {
        choixScenesDisponibles->addItem(scenes.at(i).nom);
    }
    // TODO : remettre les boutons en enabled quand les methodes seront fonctionnelles
    boutonexecuterScene = new QPushButton(QString::fromUtf8("Tester"), this);

    boutonModifierScene = new QPushButton(QString::fromUtf8("Modifier"), this);
    boutonModifierScene->setEnabled(false);
    boutonSupprimerScene = new QPushButton(QString::fromUtf8("Supprimer"), this);
    boutonNouvelleScene = new QPushButton(QString::fromUtf8("Nouvelle scène"), this);
    boutonSupprimerToutesScenes = new QPushButton(QString::fromUtf8("Supprimer toutes les scènes"), this);
    boutonSupprimerToutesScenes->setEnabled(false);

    labelSequencesDisponibles = new QLabel(QString::fromUtf8("Séquences disponibles :"), this);
    choixSequencesDisponibles = new QComboBox;
    int nbSequences = utilitaireDocuments->lireSequences(sequences);
    for(int i = 0; i < nbSequences; i++)
    {
        choixSequencesDisponibles->addItem(sequences.at(i).nom);
    }
    boutonExecuterSequence = new QPushButton(QString::fromUtf8("Tester"), this);
    boutonNouvelleSequence = new QPushButton(QString::fromUtf8("Nouvelle Séquence"), this);
    boutonSupprimmerSequence = new QPushButton(QString::fromUtf8("Supprimer"), this);

    labelnomSpectacle = new QLabel(QString::fromUtf8("Nom du nouveau Spectacle :"), this);
    choixNomSpectacle = new QLineEdit;
    labelSequencesSpectacle = new QLabel(QString::fromUtf8("Séquences Disponibles :"), this);
    choixSequencesSpectacle = new QComboBox;
    for(int i = 0; i < nbSequences; i++)
    {
        choixSequencesSpectacle->addItem(sequences.at(i).nom);
    }
    boutonchoixSequenceSpectacle = new QPushButton(QString::fromUtf8("Choisir cette Séquence"), this);
    listeSequencesSpectacle = new QScrollArea;
    boutonSauvegarderSpectacle = new QPushButton(QString::fromUtf8("Sauvegarder"), this);


    labelSpectaclesDisponibles = new QLabel(QString::fromUtf8("Spectacles disponibles :"), this);
    choixSpectaclesDisponibles = new QComboBox();
    espace = new QSpacerItem(0,150);
    int nbSpectacles = utilitaireDocuments->lireSpectacles(spectacles);
    for(int i = 0; i < nbSpectacles; i++)
    {
        choixSpectaclesDisponibles->addItem(spectacles.at(i).nom);
    }
    boutonExecuterSpectacle = new QPushButton(QString::fromUtf8("Jouer"), this);
    boutonSupprimerSpectacle = new QPushButton(QString::fromUtf8("Supprimer"), this);


    listeChoixConsole = new QComboBox(this);
    for(int i = 0; i < wing->getNombreConsoles(); i++)
    {
        listeChoixConsole->addItem(wing->getAdresseIPConsole(i));
    }

    champAdresseIPConsole = new QLineEdit(this);
    champAdresseIPConsole->setPlaceholderText("Adresse IP");
    champPortConsole = new QLineEdit(this);
    champPortConsole->setPlaceholderText("Port");
    boutonAjouterConsole = new QPushButton(QString::fromUtf8("Ajouter Console"), this);
    boutonSupprimerConsole = new QPushButton(QString::fromUtf8("Supprimer Console"), this);
    boutonModifierConsole = new QPushButton(QString::fromUtf8("Modifier Console"), this);

    // Crée le positionnement
    QHBoxLayout *mainLayout = new QHBoxLayout;
    QVBoxLayout *mainLayoutParametres = new QVBoxLayout;
    QVBoxLayout *mainLayoutPilotage = new QVBoxLayout;
    layoutListeProjecteursParametres = new QVBoxLayout;

    QHBoxLayout *layoutOptionsParametres = new QHBoxLayout;

    QHBoxLayout *layoutListePilotage = new QHBoxLayout;
    QHBoxLayout *layoutListeProjecteursPilotage = new QHBoxLayout;

    QHBoxLayout *layoutFadersPilotage = new QHBoxLayout;
    QHBoxLayout *layoutBoutonsPilotage = new QHBoxLayout;

    QVBoxLayout *layoutParametrerInterface = new QVBoxLayout;
    QHBoxLayout *layoutChoixInterface = new QHBoxLayout;
    QVBoxLayout *layoutDetectionInterface = new QVBoxLayout;

    QHBoxLayout *layoutAjouterInterface = new QHBoxLayout;

    QHBoxLayout *layoutSupprimerInterface = new QHBoxLayout;

    QVBoxLayout *mainLayoutScenes = new QVBoxLayout;
    QHBoxLayout *layoutScenesDisponibles = new QHBoxLayout;
    QHBoxLayout *layoutBoutonsOptionsScenes = new QHBoxLayout;
    QHBoxLayout *layoutBoutonsNouvelleScene = new QHBoxLayout;

    QVBoxLayout *layoutParametrerConsoles = new QVBoxLayout;
    QHBoxLayout *layoutModificationConsole = new QHBoxLayout;
    QHBoxLayout *layoutAjoutConsole = new QHBoxLayout;

    QVBoxLayout *layoutSequences = new QVBoxLayout;
    QHBoxLayout *layoutchoixSequencesDisponibles = new QHBoxLayout;
    QHBoxLayout *layoutoptionsSequencesDisponibles = new QHBoxLayout;

    //QHBoxLayout *mainLayoutSpectacles = new QHBoxLayout;
    QVBoxLayout *layoutCreationSpectacles = new QVBoxLayout;
    QVBoxLayout *layoutJeuSpectacles = new QVBoxLayout;
    QHBoxLayout *layoutOptionsSpectacles = new QHBoxLayout;
    layoutListeSequenceSpectacle = new QVBoxLayout;

    listeProjecteursEnregistres->setLayout(layoutListeProjecteursParametres);
    layoutOptionsParametres->addWidget(boutonNouveauProjecteur);
    layoutOptionsParametres->addWidget(boutonSupprimerTousProjecteurs);
    mainLayoutParametres->addWidget(listeProjecteursEnregistres);
    mainLayoutParametres->addLayout(layoutOptionsParametres);
    parametresProjecteurs->setLayout(mainLayoutParametres);

    utilitaireDocuments->afficherProjecteursEnregistres(projecteursEnregistres);

    mettreAJourProjecteursEnregistres();

    QLabel *labelListePilotage = new QLabel(QString::fromUtf8("Pilotage :"), this);
    layoutListePilotage->addWidget(labelListePilotage);
    layoutListePilotage->addWidget(listeChoixPilotage);
    layoutListePilotage->addStretch();
    QLabel *labelListeProjecteursPilotage = new QLabel(QString::fromUtf8("Projecteur :"), this);
    layoutListeProjecteursPilotage->addWidget(labelListeProjecteursPilotage);
    layoutListeProjecteursPilotage->addWidget(listeProjecteursPilotage);
    layoutListeProjecteursPilotage->addStretch();
    mainLayoutPilotage->addLayout(layoutListePilotage);
    mainLayoutPilotage->addLayout(layoutListeProjecteursPilotage);
    for (int i = 0; i < NB_SLIDERS ; ++i)
    {
       layoutFadersPilotage->addWidget(slidersCanaux[i]);
    }
    mainLayoutPilotage->addLayout(layoutFadersPilotage);
    layoutBoutonsPilotage->addWidget(boutonDecrementationCanal);
    layoutBoutonsPilotage->addWidget(boutonIncrementationCanal);
    mainLayoutPilotage->addLayout(layoutBoutonsPilotage);
    pagePiloter->setLayout(mainLayoutPilotage);

    mainLayout->addWidget(ongletsPrincipaux);

    parametresInterface->setLayout(layoutParametrerInterface);
    layoutParametrerInterface->addLayout(layoutChoixInterface);
    layoutChoixInterface->addWidget(labelInterface);
    layoutChoixInterface->addWidget(listeChoixInterface);
    layoutChoixInterface->addSpacing(20);
    layoutChoixInterface->addWidget(listeTypeInterface);
    labelInterface->setFixedSize(70, 15);
    layoutDetectionInterface->addWidget(boutonDetecterInterface);
    layoutDetectionInterface->addWidget(resultatDetectionInterface);
    layoutDetectionInterface->addStretch();
    layoutParametrerInterface->addLayout(layoutDetectionInterface);

    layoutParametrerInterface->addLayout(layoutAjouterInterface);
    layoutAjouterInterface->addWidget(portInterfaceAjouterInterface);
    layoutAjouterInterface->addWidget(listeTypeInterfaceAjouterInterface);
    layoutAjouterInterface->addWidget(boutonAjouterInterface);

    layoutParametrerInterface->addLayout(layoutSupprimerInterface);
    layoutSupprimerInterface->addWidget(listeInterfacesSupprimables);
    layoutSupprimerInterface->addWidget(boutonSupprimerInterface);
    pageCreer->setLayout(mainLayoutScenes);
    mainLayoutScenes->addLayout(layoutScenesDisponibles);
    mainLayoutScenes->addLayout(layoutBoutonsOptionsScenes);
    mainLayoutScenes->addLayout(layoutBoutonsNouvelleScene);
    layoutScenesDisponibles->addWidget(labelScenesDisponibles);
    layoutScenesDisponibles->addWidget(choixScenesDisponibles);
    layoutBoutonsOptionsScenes->addWidget(boutonexecuterScene);
    layoutBoutonsOptionsScenes->addWidget(boutonModifierScene);
    layoutBoutonsOptionsScenes->addWidget(boutonSupprimerScene);
    layoutBoutonsNouvelleScene->addWidget(boutonNouvelleScene);
    layoutBoutonsNouvelleScene->addWidget(boutonSupprimerToutesScenes);

    parametresConsoles->setLayout(layoutParametrerConsoles);
    layoutParametrerConsoles->addLayout(layoutAjoutConsole);
    layoutParametrerConsoles->addLayout(layoutModificationConsole);
    layoutAjoutConsole->addWidget(champAdresseIPConsole);
    layoutAjoutConsole->addWidget(champPortConsole);
    layoutAjoutConsole->addWidget(boutonAjouterConsole);
    layoutModificationConsole->addWidget(listeChoixConsole);
    layoutModificationConsole->addWidget(boutonModifierConsole);
    layoutModificationConsole->addWidget(boutonSupprimerConsole);

    pageAssembler->setLayout(layoutSequences);
    layoutSequences->addLayout(layoutchoixSequencesDisponibles);
    layoutSequences->addLayout(layoutoptionsSequencesDisponibles);
    layoutchoixSequencesDisponibles->addWidget(labelSequencesDisponibles);
    layoutchoixSequencesDisponibles->addWidget(choixSequencesDisponibles);
    layoutoptionsSequencesDisponibles->addWidget(boutonExecuterSequence);
    layoutoptionsSequencesDisponibles->addWidget(boutonNouvelleSequence);
    layoutoptionsSequencesDisponibles->addWidget(boutonSupprimmerSequence);

    pagesSpectacles[0]->setLayout(layoutCreationSpectacles);
    pagesSpectacles[1]->setLayout(layoutJeuSpectacles);
    layoutCreationSpectacles->addWidget(labelnomSpectacle);
    layoutCreationSpectacles->addWidget(choixNomSpectacle);
    layoutCreationSpectacles->addWidget(labelSequencesSpectacle);
    layoutCreationSpectacles->addWidget(choixSequencesSpectacle);
    layoutCreationSpectacles->addWidget(boutonchoixSequenceSpectacle);
    layoutCreationSpectacles->addWidget(listeSequencesSpectacle);
    layoutCreationSpectacles->addWidget(boutonSauvegarderSpectacle);

    layoutJeuSpectacles->addWidget(labelSpectaclesDisponibles);
    layoutJeuSpectacles->addWidget(choixSpectaclesDisponibles);
    layoutJeuSpectacles->addSpacerItem(espace);
    layoutJeuSpectacles->addLayout(layoutOptionsSpectacles);

    layoutOptionsSpectacles->addWidget(boutonExecuterSpectacle);
    layoutOptionsSpectacles->addWidget(boutonSupprimerSpectacle);

    listeSequencesSpectacle->setLayout(layoutListeSequenceSpectacle);

    setLayout(mainLayout);
    this->setFixedSize(QSize(800,500));
    this->setWindowTitle(QString::fromUtf8("DMX 2018"));

    actionQuitter = new QAction("&Quitter", this);
    actionQuitter->setShortcut(QKeySequence(QKeySequence::Quit)); // Ctrl+Q
    addAction(actionQuitter);
}

/**
 * @brief Méthode permettant de fixer la valeur des canaux
 *
 * @fn IHM::setCanal
 *
 * @return void
 */
void IHM::setCanal()
{
    if(listeProjecteursPilotage->currentIndex() == 0)
    {
        for(int i = 0; i < NB_SLIDERS; i ++)
        {
            slidersCanaux[i]->setConnecte(false);
            if(i == 0)
                slidersCanaux[i]->setCanalEnabled(true);
            slidersCanaux[i]->setNomCanal("");
            slidersCanaux[i]->setCanal(canalDepartFaders + i);
            if(canalDepartFaders + i > NB_CANAUX)
                slidersCanaux[i]->hide();
            else
                slidersCanaux[i]->show();
            slidersCanaux[i]->setConnecte(true);
        }
    }
}

/**
 * @brief Connecte l'envoi de l'etat des faders depuis la console avec leur affichage dans l'éventualité où la console est désactivée
 *
 * @fn IHM::activerPilotageConsole
 *
 * @return void
 */
void IHM::activerPilotageConsole()
{
    connect(wing, SIGNAL(envoyerEtatFaders(EtatFaders)), this, SLOT(afficherEtatFaders(EtatFaders)));
}

/**
 * @brief déconnecte l'envoi de l'etat des faders depuis la console avec leur affichage
 *
 * @fn IHM::desactiverPilotageConsole
 *
 * @return void
 */
void IHM::desactiverPilotageConsole()
{
    disconnect(wing, SIGNAL(envoyerEtatFaders(EtatFaders)), this, SLOT(afficherEtatFaders(EtatFaders)));
}

/**
 * @brief Désactive les sliders de l'IHM
 *
 * @fn IHM::desactiverPilotageIHM
 *
 * @return void
 */
void IHM::desactiverPilotageIHM()
{
    pilotageIHMActif = false;

    for(int i = 0; i < NB_SLIDERS ; i++)
    {
        slidersCanaux[i]->setEnabled(false);
    }
    boutonIncrementationCanal->setEnabled(false);
    boutonDecrementationCanal->setEnabled(false);
    listeProjecteursPilotage->setEnabled(false);

    activerPilotageConsole();

    pilotageDMXActif = true;
}

/**
 * @brief Active les sliders de l'IHM
 *
 * @fn IHM::activerPilotageIHM
 *
 * @return void
 */
void IHM::activerPilotageIHM()
{
    pilotageIHMActif = true;

    for(int i = 0; i < NB_SLIDERS; i++)
    {
        slidersCanaux[i]->setEnabled(true);
    }
    boutonIncrementationCanal->setEnabled(true);
    boutonDecrementationCanal->setEnabled(true);
    listeProjecteursPilotage->setEnabled(true);

    desactiverPilotageConsole();

    pilotageDMXActif = true;
}
/**
 * @brief Méthode permettant l'enregistrement de projecteurs à partir de l'utilitaire XML
 *
 * @fn IHM::enregistrerProjecteur
 *
 * @return void
 */
void IHM::enregistrerProjecteur(/*QString uuid*/)
{
    QString nomsCanauxEnregistres[10];
    qDebug() << Q_FUNC_INFO;
    for(int i = 0; i < 10; i++ )
    {
        nomsCanauxEnregistres[i] = nomsCanaux[i]->text();
    }
    effacerAffichageProjecteursPilotage();
    effacerAffichageProjecteursEnregistres();

    if(fenetreNouveauProjecteur->windowTitle() == "Nouvel Appareil DMX" )
        utilitaireDocuments->enregistrerAppareil(choixNom->text(), choixTypes->currentText(), choixNBCanaux->value(), choixCanalDepart->value(), nomsCanauxEnregistres);

    else if(fenetreNouveauProjecteur->windowTitle() == "Modifier Appareil DMX" )
        utilitaireDocuments->modifierAppareil(cacheUuid ,choixNom->text(), choixTypes->currentText(), choixNBCanaux->value(), choixCanalDepart->value(), nomsCanauxEnregistres);

    else
    {
        qDebug() << Q_FUNC_INFO << "erreur enregistrement!!";
    }
    utilitaireDocuments->lireAppareils(projecteursDMX);
    utilitaireDocuments->afficherProjecteursEnregistres(projecteursEnregistres);

    mettreAJourProjecteursEnregistres();
    mettreAJourProjecteursPilotage();
    utilitaireDocuments->lireAppareils(projecteursDMX);
    fenetreNouveauProjecteur->close();
}

/**
 * @brief Création de la fenetre d'interface pour créer un nouveau Projecteur
 *
 * @fn IHM::genererFenetreNouveauProjecteur
 *
 * @return void
 */
void IHM::genererFenetreNouveauProjecteur()
{
    fenetreNouveauProjecteur = new QDialog;
    fenetreNouveauProjecteur->setWindowTitle("Nouvel Appareil DMX");

    labelNom = new QLabel(QString::fromUtf8("Nom :"), this);
    choixNom = new QLineEdit();

    labelType = new QLabel(QString::fromUtf8("Type :"), this);
    choixTypes = new QComboBox();
    choixTypes->addItem("PAR LED");
    choixTypes->addItem("LYRE");
    choixTypes->addItem("SCANNER");
    choixTypes->addItem("LASER");
    choixTypes->addItem("AUTRES");

    labelNBCanaux = new QLabel(QString::fromUtf8("Nombre de Canaux :"), this);
    choixNBCanaux = new QSpinBox;
    choixNBCanaux->setMinimum(1);
    choixNBCanaux->setMaximum(10);

    labelCanalDepart = new QLabel(QString::fromUtf8("Canal de Depart :"), this);
    choixCanalDepart = new QSpinBox;
    choixCanalDepart->setMinimum(1);
    choixCanalDepart->setMaximum(512);

    labelNomCanaux = new QLabel(QString::fromUtf8("Noms des Canaux :"), this);
    for(int i = 0; i < 10; i++)
    {
        nomsCanaux[i] = new QLineEdit;
    }

    boutonEnregistrer = new QPushButton(QString::fromUtf8("Enregistrer"), this);

    QVBoxLayout *mainLayoutFenetre = new QVBoxLayout;
    QHBoxLayout *layoutEnregistrementNom = new QHBoxLayout;
    QHBoxLayout *layoutEnregistrementType = new QHBoxLayout;
    QHBoxLayout *layoutEnregistrementParametresCanaux = new QHBoxLayout;
    QVBoxLayout *layoutEnregistrementNomsCanaux = new QVBoxLayout;

    layoutEnregistrementNom->addWidget(labelNom);
    layoutEnregistrementNom->addWidget(choixNom);

    layoutEnregistrementType->addWidget(labelType);
    layoutEnregistrementType->addWidget(choixTypes);

    layoutEnregistrementParametresCanaux->addWidget(labelNBCanaux);
    layoutEnregistrementParametresCanaux->addWidget(choixNBCanaux);
    layoutEnregistrementParametresCanaux->addWidget(labelCanalDepart);
    layoutEnregistrementParametresCanaux->addWidget(choixCanalDepart);

    layoutEnregistrementNomsCanaux->addWidget(labelNomCanaux);
    for(int i = 0; i < 10; i++)
    {
        layoutEnregistrementNomsCanaux->addWidget(nomsCanaux[i]);
        connect(choixNBCanaux, SIGNAL(valueChanged(int)),this, SLOT(bloquerChoixNomCanaux(int)));
    }

    mainLayoutFenetre->addLayout(layoutEnregistrementNom);
    mainLayoutFenetre->addLayout(layoutEnregistrementType);
    mainLayoutFenetre->addLayout(layoutEnregistrementParametresCanaux);
    mainLayoutFenetre->addLayout(layoutEnregistrementNomsCanaux);
    mainLayoutFenetre->addWidget(boutonEnregistrer);

    fenetreNouveauProjecteur->setLayout(mainLayoutFenetre);


}

/**
 * @brief Bloquage des LineEdit de noms de canaux en trop
 *
 * @fn IHM::bloquerChoixNomCanaux
 *
 * @param valeur int
 *
 * @return void
 */
void IHM::bloquerChoixNomCanaux(int valeur)
{
    for(int i = 0; i < 10; i++)
    {
        nomsCanaux[i]->setEnabled(false);
    }
    for(int i = 0; i < valeur; i++)
    {
        nomsCanaux[i]->setEnabled(true);
    }
}

/**
 * @brief Ouverture de la fenetre permettant l'ajout d'un projecteur dans le fichier XML
 *
 * @fn IHM::ouvrirFenetreNouveauProjecteur
 *
 * @return void
 */
void IHM::ouvrirFenetreNouveauProjecteur()
{
    fenetreNouveauProjecteur->setWindowTitle("Nouvel Appareil DMX");
    choixNom->setText("");
    choixTypes->setCurrentIndex(0);
    choixNBCanaux->setValue(0);
    choixCanalDepart->setValue(0);
    for(int i = 0; i < 10; i++)
    {
        nomsCanaux[i]->setText("");
    }
    bloquerChoixNomCanaux(choixNBCanaux->value());
    fenetreNouveauProjecteur->show();
}

/**
 * @brief Supression simultanée de tous les projecteurs depuis l'IHM
 *
 * @fn IHM::supprimerTousProjecteurs
 *
 * @return void
 */
void IHM::supprimerTousProjecteurs()
{
    qDebug() << Q_FUNC_INFO;
    effacerAffichageProjecteursEnregistres();
    effacerAffichageProjecteursPilotage();
    utilitaireDocuments->supprimerProjecteursEnregistres();
    utilitaireDocuments->lireAppareils(projecteursDMX);

}


/**
 * @brief Supression d'un projecteur du fichier XML depuis l'IHM
 *
 * @fn IHM::supprimerProjecteur
 *
 * @param uuid
 *
 * @return void
 */
void IHM::supprimerProjecteur(QString uuid)
{
    qDebug() << Q_FUNC_INFO;

    effacerAffichageProjecteursEnregistres();
    effacerAffichageProjecteursPilotage();
    utilitaireDocuments->supprimerAppareil(uuid);

    utilitaireDocuments->afficherProjecteursEnregistres(projecteursEnregistres);


    utilitaireDocuments->lireAppareils(projecteursDMX);
    mettreAJourProjecteursPilotage();
    mettreAJourProjecteursEnregistres();

}

/**
 * @brief Ouverture de la fenetre permettant la modification d'un projecteur dans le fichier XML
 *
 * @fn IHM::ouvrirFenetreModifierProjecteur
 *
 * @param projecteur QString
 * @param uuid Qstring
 *
 * @return void
 *
 */
void IHM::ouvrirFenetreModifierProjecteur(QString projecteur, QString uuid)
{

    fenetreNouveauProjecteur->setWindowTitle("Modifier Appareil DMX");
    cacheUuid = uuid;
    choixNom->setText(projecteur);
    for(int i = 0; i < projecteursDMX.size(); i++)
    {
        if(projecteursDMX[i]->getUuid() == uuid)
        {

            if(projecteursDMX[i]->getType() == "PAR LED")
                choixTypes->setCurrentIndex(0);
            else if(projecteursDMX[i]->getType() == "LYRE")
                choixTypes->setCurrentIndex(1);
            else if(projecteursDMX[i]->getType() == "SCANNER")
                choixTypes->setCurrentIndex(2);
            else if(projecteursDMX[i]->getType() == "LASER")
                choixTypes->setCurrentIndex(3);
            else
                choixTypes->setCurrentIndex(4);

            choixNBCanaux->setValue(projecteursDMX[i]->getNomCanaux().size());
            choixCanalDepart->setValue(projecteursDMX[i]->getCanalDebut());

            DMXProjecteur *projecteurDmx = projecteursDMX[i];

            for(int i = 0; i < projecteurDmx->getNomCanaux().size() ; i++)
            {
                nomsCanaux[i]->setText(projecteurDmx->getNomCanaux().at(i));
            }
            bloquerChoixNomCanaux(choixNBCanaux->value());

        }
    }
    fenetreNouveauProjecteur->show();

}


/**
 * @brief Affichage des informations d'un projecteur depuis l'IHM
 *
 * @fn IHM::ouvrirFenetreInformationsProjecteur
 *
 * @param projecteur QString
 * @param uuid Qstring
 *
 * @return void
 *
 */
void IHM::ouvrirFenetreInformationsProjecteur(QString projecteur, QString uuid)
{
    for(int i = 0; i < projecteursDMX.size(); i++)
    {
        if(projecteursDMX[i]->getUuid() == uuid)
        {
            fenetreInformationsAppareil->setWindowTitle(projecteur +" ("+ projecteursDMX[i]->getType() +")");
            labelInformationsCanalDepart->setText(QString::fromUtf8("Canal DMX de départ :") + QString::number(projecteursDMX[i]->getCanalDebut()));
            DMXProjecteur *projecteurDmx = projecteursDMX[i];

            for(int i = 0; i < 10; i++)
            {
                labelInformationsCanaux[i]->setText("");
            }

            for(int i = 0; i < projecteurDmx->getNomCanaux().size(); i++)
            {
                int canal = projecteurDmx->getCanalDebut() + i;
                labelInformationsCanaux[i]->setText(QString::fromUtf8("Canal N° ") + QString::number(canal) + " : " + projecteurDmx->getNomCanaux().at(i));
            }
        }

    }

    fenetreInformationsAppareil->show();
}


/**
 * @brief Ouverture de la fenetre permettant l'ajout d'une sequence dans le fichier XML
 *
 * @fn IHM::ouvrirFenetreNouvelleSequence
 *
 * @return void
 */
void IHM::ouvrirFenetreNouvelleSequence()
{

    for(int i = 0 ; i < labelsScenesSequences.count() ; i ++)
    {
         delete labelsScenesSequences.at(i);
    }
    labelsScenesSequences.clear();
    choixnomSequence->setText("");
    choixHeures->setValue(0);
    choixMinutes->setValue(0);
    choixSecondes->setValue(0);

    choixScenesSequence->clear();
    for(int i = 0; i < scenes.count(); i++)
    {
        choixScenesSequence->addItem(scenes.at(i).nom);
    }


    fenetreNouvelleSequence->show();
}

/**
  *@brief Récupere les données entrées dans les LineEdit de l'ajout d'interface et les place dans la liste interfaces
  *
  *@fn IHM::recupererDonneesNouvelleInterface
  *
  *@return void
  **/
void IHM::recupererDonneesNouvelleInterface()
{
    Interface interfaceAjoutee;

    interfaceAjoutee.id = interfaces.count() + 1;
    interfaceAjoutee.portInterface = portInterfaceAjouterInterface->text();
    interfaceAjoutee.typeInterface = (EnttecInterfaces)listeTypeInterfaceAjouterInterface->currentIndex();
    interfaceAjoutee.utilisee = 0;

    interfaces.push_back(interfaceAjoutee);

    utilitaireDocuments->ecrireInterfaces(interfaces);
    listeChoixInterface->addItem(interfaces.back().portInterface);
    listeInterfacesSupprimables->addItem(interfaces.back().portInterface);
}


/**
  *@brief Supprime l'interface DMX du fichier adaptateurs.xml et de la QListe interfaces
  *
  *@fn IHM::supprimerInterface
  *
  *@return void
  */
void IHM::supprimerInterface()
{
    QList<Interface>::iterator it;
    it = interfaces.begin();
    for(int i = 0; i < interfaces.count(); i++)
    {
        if(interfaces[i].portInterface == listeInterfacesSupprimables->currentText())
        {
            interfaces.erase(it);
            listeInterfacesSupprimables->removeItem(i);
            listeChoixInterface->removeItem(i);
        }
        it++;
    }

    utilitaireDocuments->ecrireInterfaces(interfaces);

}

/**
 * @brief Création de la fenetre d'interface pour créer une nouvelle Scène
 *
 * @fn IHM::genererFenetreNouvelleScene()
 *
 * @return void
 */
void IHM::genererFenetreNouvelleScene()
{
    //qDebug() << Q_FUNC_INFO;

    fenetreNouvelleScene = new QDialog;
    fenetreNouvelleScene->setWindowTitle(QString::fromUtf8("Paramètres de la nouvelle Scène"));

    labelNomScene = new QLabel(QString::fromUtf8("Nom de la scène:"), this);
    choixNomScene = new QLineEdit(this);
    labelAppareilScene = new QLabel(QString::fromUtf8("Appareils disponibles:"), this);
    choixAppareilScene = new QComboBox();
    for(int i = 0; i < projecteursDMX.count(); i++)
    {
    choixAppareilScene->addItem(projecteursDMX.at(i)->getNom() + " (" + projecteursDMX.at(i)->getType() + ")");
    }
    boutonParametresCanauxScene = new QPushButton(QString::fromUtf8("Choisir ce projecteur"), this);
    listeAppareilsScene = new QScrollArea();
    boutonSauvegarderScene = new QPushButton(QString::fromUtf8("Sauvegarder cette scène"), this);

    QVBoxLayout *mainLayoutNouvelleScene = new QVBoxLayout;
    QHBoxLayout *layoutNomScene = new QHBoxLayout;
    QHBoxLayout *layoutChoixAppareilsScene = new QHBoxLayout;
    layoutlisteAppareilsScene = new QVBoxLayout;

    fenetreNouvelleScene->setLayout(mainLayoutNouvelleScene);
    mainLayoutNouvelleScene->addLayout(layoutNomScene);
    mainLayoutNouvelleScene->addLayout(layoutChoixAppareilsScene);
    mainLayoutNouvelleScene->addWidget(boutonParametresCanauxScene);
    mainLayoutNouvelleScene->addWidget(listeAppareilsScene);
    mainLayoutNouvelleScene->addWidget(boutonSauvegarderScene);

    layoutNomScene->addWidget(labelNomScene);
    layoutNomScene->addWidget(choixNomScene);
    layoutChoixAppareilsScene->addWidget(labelAppareilScene);
    layoutChoixAppareilsScene->addWidget(choixAppareilScene);

    listeAppareilsScene->setLayout(layoutlisteAppareilsScene);
    connect(boutonSauvegarderScene,SIGNAL(clicked()),this,SLOT(enregistrerScene()));

   // qDebug() << Q_FUNC_INFO << "fenetre generee" ;

}

/**
 * @brief Création de la fenetre d'interface pour créer une nouvelle Séquence
 *
 * @fn IHM::genererFenetreNouvelleSequence()
 *
 * @return void
 */

void IHM::genererFenetreNouvelleSequence()
{
    fenetreNouvelleSequence = new QDialog;
    fenetreNouvelleSequence->setWindowTitle(QString::fromUtf8("Assemblage d'une séquence"));

    labelnomSequence = new QLabel(QString::fromUtf8("Nom de la séquence :"), this);
    choixnomSequence = new QLineEdit();
    labelScenesSequence = new QLabel(QString::fromUtf8("Scènes disponibles :"), this);
    choixScenesSequence = new QComboBox;
    for(int i = 0; i < scenes.count(); i++)
    {
        choixScenesSequence->addItem(scenes.at(i).nom);
    }

    labelTemporisation = new QLabel(QString::fromUtf8("Durée de la scène :"), this);
    choixHeures = new QSpinBox();
    labelHeures = new QLabel(QString::fromUtf8("h"), this);
    choixMinutes = new QSpinBox();
    choixMinutes->setMaximum(60);
    labelMinutes = new QLabel(QString::fromUtf8("min"), this);
    choixSecondes = new QSpinBox();
    choixSecondes->setMaximum(60);
    labelSecondes = new QLabel(QString::fromUtf8("sec"), this);
    boutonAjouterScene = new QPushButton(QString::fromUtf8("Ajouter cette Scène"), this);
    listeScenesSequenceChoisies = new QScrollArea;
    boutonEnregistrerSequence = new QPushButton(QString::fromUtf8("Enregistrer"), this);

    QVBoxLayout *mainLayoutNouvelleSequence = new QVBoxLayout;
    QHBoxLayout *layoutNomSequence = new QHBoxLayout;
    QHBoxLayout *layoutScenesSequences = new QHBoxLayout;
    QHBoxLayout *layoutTempsScene = new QHBoxLayout;
    layoutListeScenesSequences = new QVBoxLayout;

    fenetreNouvelleSequence->setLayout(mainLayoutNouvelleSequence);
    mainLayoutNouvelleSequence->addLayout(layoutNomSequence);
    layoutNomSequence->addWidget(labelnomSequence);
    layoutNomSequence->addWidget(choixnomSequence);
    mainLayoutNouvelleSequence->addLayout(layoutScenesSequences);
    layoutScenesSequences->addWidget(labelScenesSequence);
    layoutScenesSequences->addWidget(choixScenesSequence);
    mainLayoutNouvelleSequence->addLayout(layoutTempsScene);
    layoutTempsScene->addWidget(labelTemporisation);
    layoutTempsScene->addWidget(choixHeures);
    layoutTempsScene->addWidget(labelHeures);
    layoutTempsScene->addWidget(choixMinutes);
    layoutTempsScene->addWidget(labelMinutes);
    layoutTempsScene->addWidget(choixSecondes);
    layoutTempsScene->addWidget(labelSecondes);
    mainLayoutNouvelleSequence->addWidget(boutonAjouterScene);
    mainLayoutNouvelleSequence->addWidget(listeScenesSequenceChoisies);
    listeScenesSequenceChoisies->setLayout(layoutListeScenesSequences);
    mainLayoutNouvelleSequence->addWidget(boutonEnregistrerSequence);

    connect(boutonAjouterScene,SIGNAL(clicked()),this,SLOT(ajouterSceneSequence()));
    connect(boutonEnregistrerSequence,SIGNAL(clicked()),this,SLOT(enregistrerSequence()));
}

/**
 * @brief Création de la fenetre contenant les information d'un projecteur au choix
 *
 * @fn IHM::genererFenetreInformationsAppareil()
 *
 * @return void
 */
void IHM::genererFenetreInformationsAppareil()
{
    fenetreInformationsAppareil = new QDialog;
    labelInformationsCanalDepart = new QLabel;
    for(int i=0; i < 10; i++ )
    {
        labelInformationsCanaux[i] = new QLabel;
    }
    listeInformationsCanaux = new QScrollArea;

    QVBoxLayout *mainLayoutInformations = new QVBoxLayout;
    //QVBoxLayout *layoutListeInformationsCanaux = new QVBoxLayout;

    fenetreInformationsAppareil->setLayout(mainLayoutInformations);

    mainLayoutInformations->addWidget(labelInformationsCanalDepart);
    //mainLayoutInformations->addWidget(listeInformationsCanaux);

    //listeInformationsCanaux->setLayout(layoutListeInformationsCanaux);
    for(int i=0; i < 10; i++ )
    {
         mainLayoutInformations->addWidget(labelInformationsCanaux[i]);
    }
    fenetreInformationsAppareil->setFixedWidth(300);
}

/**
 * @brief Création de la fenetre d'options des canaux d'un apparail à ajouter pour une scène
 *
 * @fn IHM::genererFenetreCanauxScene()
 *
 * @return void
 */
void IHM::genererFenetreCanauxScene()
{
   // qDebug() << Q_FUNC_INFO;
    fenetreCanauxScene = new QDialog;
    fenetreCanauxScene->setWindowTitle(QString::fromUtf8("Paramètres des canaux DMX du projecteur à ajouter"));



    boutonSauvegarderCanauxScene = new QPushButton(QString::fromUtf8("Confirmer cette Configuration de canaux"), this);

    QVBoxLayout *mainLayoutCanauxScene = new QVBoxLayout;
    QHBoxLayout *layoutSlidersCanauxScene = new QHBoxLayout;

    fenetreCanauxScene->setLayout(mainLayoutCanauxScene);
    mainLayoutCanauxScene->addLayout(layoutSlidersCanauxScene);
    mainLayoutCanauxScene->addWidget(boutonSauvegarderCanauxScene);

    for(int i = 0; i < NB_SLIDERS; i++)
    {
        slidersCanauxScene[i] = new MySlider();
        layoutSlidersCanauxScene->addWidget(slidersCanauxScene[i]);

    }

    DMXProjecteur *projecteur = projecteursDMX.at(choixAppareilScene->currentIndex());
    pilotageDMXActif = false;
    for(int i = 0; i < NB_SLIDERS; i ++)
    {
        if(i >= projecteur->getNomCanaux().size())
        {
            slidersCanauxScene[i]->hide();
        }
        else
        {
            slidersCanauxScene[i]->setConnecte(false);
            slidersCanauxScene[i]->setCanalEnabled(false);
            slidersCanauxScene[i]->setCanal(projecteur->getCanalDebut() + i);
            slidersCanauxScene[i]->setNomCanal(projecteur->getNomCanal(projecteur->getCanalDebut() + i));
            slidersCanauxScene[i]->show();
            slidersCanauxScene[i]->setConnecte(true);
        }
    }
    connect(boutonSauvegarderCanauxScene, SIGNAL(clicked()),this,SLOT(ajouterAppareilScene()));
    //qDebug() << Q_FUNC_INFO << "fenetre generee" ;
}

/**
 * @brief Affichage de la fenêtre d'options de creation d'une scène
 *
 * @fn IHM::ouvrirFenetreNouvelleScene()
 *
 * @return void
 *
 */
void IHM::ouvrirFenetreNouvelleScene()
{
    qDebug() << Q_FUNC_INFO << labelsAppareilsScene.count();
    for(int i = 0 ; i < labelsAppareilsScene.count() ; i ++)
    {
            qDebug() << Q_FUNC_INFO << " labelsAppareilsScene.at(" << i <<")";
            delete labelsAppareilsScene.at(i);

    }
    labelsAppareilsScene.clear();
    choixAppareilScene->clear();
    choixNomScene->setText("");
    for(int i = 0; i < projecteursDMX.count(); i++)
    {
    choixAppareilScene->addItem(projecteursDMX.at(i)->getNom() + " (" + projecteursDMX.at(i)->getType() + ")");
    }
    fenetreNouvelleScene->show();
}

/**
 * @brief Affichage de la fenêtre d'options d'ajustements des canaux de l'appareil à implémenter
 *
 * @fn IHM::ouvrirFenetreCanauxScene()
 *
 * @return void
 *
 */
void IHM::ouvrirFenetreCanauxScene()
{
    bool estDejaEnregistre = false;
    for(int i = 0; i < labelsAppareilsScene.size(); i++)
    {
        if(choixAppareilScene->currentText() == labelsAppareilsScene[i]->text())
        {
            estDejaEnregistre = true;
        }
    }
    if(!estDejaEnregistre)
    {
        genererFenetreCanauxScene();
        fenetreCanauxScene->show();
    }
    else
    {
        QMessageBox::critical(0, QString::fromUtf8("DMX 2018"), QString::fromUtf8("L'appareil selectionné est déja présent dans la scène"));
    }
}


/**
 * @brief Enregistrement d'une scène depuis l'IHM
 *
 * @fn IHM::enregistrerScene()
 *
 * @return void
 *
 */
void IHM::enregistrerScene()
{
    qDebug() << Q_FUNC_INFO;
    cacheScene.nom = choixNomScene->text();
    cacheScene.uuid = QUuid::createUuid().toString();
    utilitaireDocuments->enregistrerScene(cacheScene);
    choixScenesDisponibles->addItem(cacheScene.nom);
    cacheScene.nom = "";
    cacheScene.uuid = "";
    cacheScene.valeursCanauxscene.clear();
    utilitaireDocuments->lireScenes(scenes);

    fenetreNouvelleScene->close();
}

/**
 * @brief Ajout d'un appareil à la scène en cours de création
 *
 * @fn IHM::ajouterAppareilScene()
 *
 * @return void
 *
 */
void IHM::ajouterAppareilScene()
{
    QLabel* appareil = new QLabel(choixAppareilScene->currentText(), this);
    labelsAppareilsScene.append(appareil);
    for(int i=0; i < NB_SLIDERS; i++)
    {
        cacheScene.valeursCanauxscene[slidersCanauxScene[i]->getCanal()] = slidersCanauxScene[i]->getValeur();
    }

    layoutlisteAppareilsScene->addWidget(labelsAppareilsScene.last());

    fenetreCanauxScene->close();
}

/**
 * @brief Exécution d'une scène sur la chaîne DMX
 *
 * @fn IHM::executerScene()
 *
 * @return void
 *
 */
void IHM::executerScene(Scene scene)
{
    qDebug() << Q_FUNC_INFO;
    for(int i = 1; i <= 512; i++)
    {

        if(interfaceDMX != NULL)
        {
            interfaceDMX->SetCanalDMX(i, scene.valeursCanauxscene[i]);
        }

    }
    if(interfaceDMX != NULL)
    {
        interfaceDMX->SendDMX();
    }
}

void IHM::executerSceneDefault()
{
    qDebug() << Q_FUNC_INFO;
    executerScene(scenes[choixScenesDisponibles->currentIndex()]);
}

/**
 * @brief Suppression d'une scène depuis l'IHM
 *
 * @fn IHM::supprimerScene()
 *
 * @return void
 *
 */
void IHM::supprimerScene()
{
    utilitaireDocuments->supprimerScene(scenes[choixScenesDisponibles->currentIndex()].uuid);
    utilitaireDocuments->lireScenes(scenes);
    choixScenesDisponibles->removeItem(choixScenesDisponibles->currentIndex());
    qDebug() << Q_FUNC_INFO;
}


void IHM::recupererDerniereInterfaceUtilisee()
{
    for(int i = 0; i < interfaces.count(); i ++)
    {
        //qDebug() << "Interface numero " << i << ", Port : " << interfaces.at(i).portInterface;
        //qDebug() << "Interface numero " << i << ", Utilisee : " << interfaces.at(i).utilisee;
        if(interfaces.at(i).utilisee)
        {
            //qDebug() << "Interface numero" << i << ", Utilisee : " << interfaces[i].utilisee;
            selectionnerInterface(i);
            listeChoixInterface->setCurrentIndex(i);
            listeTypeInterface->setCurrentIndex((int)interfaces[i].typeInterface);
        }
    }
}

/**
  *@brief Ajoute une console à la QList consoles dont les paramètres seront définis par les QLineEdit champAdresseIPConsole, champPortConsole
  *
  *@fn IHM::ajouterConsole
  *
  *@return void
  */
void IHM::ajouterConsole()
{
    wing->ajouterConsoleListe(champAdresseIPConsole->text(), champPortConsole->text());

    listeChoixConsole->addItem(wing->getConsole(wing->getNombreConsoles()-1).adresseIP);
}

/**
  *@brief Genere une nouvelle fenetre de dialogue qui contient les informations de la console à modifier
  *
  *@fn IHM::genererFenetreModifierConsole
  *
  *@return void
  */
void IHM::genererFenetreModifierConsole()
{
    fenetreModifierConsole = new QDialog;
    fenetreModifierConsole->setWindowTitle("Modifier Console");

    champAdresseIPModifierConsole = new QLineEdit(this);
    champPortModifierConsole = new QLineEdit(this);
    boutonEnregistrerModificationConsole = new QPushButton("Enregistrer", this);
    champAdresseIPModifierConsole->setPlaceholderText("Adresse IP");
    champPortModifierConsole->setPlaceholderText("Port");

    QHBoxLayout *layoutModificationConsole = new QHBoxLayout;

    fenetreModifierConsole->setLayout(layoutModificationConsole);
    layoutModificationConsole->addWidget(champAdresseIPModifierConsole);
    layoutModificationConsole->addWidget(champPortModifierConsole);
    layoutModificationConsole->addWidget(boutonEnregistrerModificationConsole);

    connect(boutonEnregistrerModificationConsole,SIGNAL(clicked()), this, SLOT(envoyerModificationConsole()));
    fenetreModifierConsole->show();
}

/**
  *@brief Enregistre les modifications apportées a la console
  *
  *@fn IHM::enregistrerModificationConsole
  *
  *@return void
  */
void IHM::envoyerModificationConsole()
{
    Console console;
    if(champAdresseIPModifierConsole->text() == "")
    {
        console.adresseIP = wing->getAdresseIPConsole(listeChoixConsole->currentIndex());
        //qDebug() << "adresse IP console = " << console.adresseIP;
    }
    else console.adresseIP = champAdresseIPModifierConsole->text();
    if(champPortModifierConsole->text() == "")
    {
        console.port = wing->getPort(listeChoixConsole->currentIndex());
        //qDebug() << "port console = " << console.port;
    }
    else console.port = champPortModifierConsole->text().toInt();

    wing->enregistrerModificationConsole(console, listeChoixConsole->currentIndex());

    fenetreModifierConsole->close();
    listeChoixConsole->clear();
    for(int i = 0; i < wing->getNombreConsoles(); i++)
    {
        //qDebug()<< wing->getNombreConsoles();
        listeChoixConsole->addItem(wing->getAdresseIPConsole(i));
    }
}

/**
  *@brief Supprime la console activement séléctionnée dans le QComboBox listeChoixConsole de la QList consoles
  *
  *@fn IHM::supprimerConsole
  *
  *@return void
  */
void IHM::supprimerConsole()
{
    QList<Console>::iterator it;
    it = wing->getDebutListeConsoles();
    for(int i = 0; i < wing->getNombreConsoles(); i++)
    {
        if(wing->getAdresseIPConsole(i) == listeChoixConsole->currentText())
        {
            wing->supprimerConsoleListe(it);
            listeChoixConsole->removeItem(i);
        }
        it++;
    }
    utilitaireDocuments->ecrireConsole(wing->getListeConsoles());
}

/**
 * @brief Efface l'affichage des projecteurs enregistrés dans le menu d'ajout de projecteurs
 *
 * @fn IHM::effacerAffichageProjecteursEnregistres()
 *
 * @return void
 *
 */
void IHM::effacerAffichageProjecteursEnregistres()
{
    qDebug() << Q_FUNC_INFO;
    for(int i = 0; i < utilitaireDocuments->getNbAppareils(); i++)
    {
        delete projecteursEnregistres[i];
    }

}

/**
 * @brief Réaffiche la liste mise à jour des projecteurs enregistrés dans le menu d'ajout de projecteur
 *
 * @fn IHM::mettreAJourProjecteursEnregistres()
 *
 * @return void
 *
 */
void IHM::mettreAJourProjecteursEnregistres()
{
    for(int i = 0; i < utilitaireDocuments->getNbAppareils(); i++)
    {
        layoutListeProjecteursParametres->addWidget(projecteursEnregistres[i]);
        connect(projecteursEnregistres[i], SIGNAL(suppression(QString)), this, SLOT(supprimerProjecteur(QString)));
        connect(projecteursEnregistres[i], SIGNAL(modification(QString,QString)), this, SLOT(ouvrirFenetreModifierProjecteur(QString,QString)));
        connect(projecteursEnregistres[i], SIGNAL(informations(QString,QString)), this, SLOT(ouvrirFenetreInformationsProjecteur(QString,QString)));
    }
}

void IHM::envoyerCanalDepartWing()
{
    //qDebug() << Q_FUNC_INFO;
    wing->envoyerCanalDepartWing(slidersCanaux[0]->getCanal());
}

/**
 * @brief Efface les projecteurs en index dans listeProjecteursPilotage
 *
 * @fn IHM::effacerAffichageProjecteursEnregistres()
 *
 * @return void
 *
 */
void IHM::effacerAffichageProjecteursPilotage()
{
    for(int i = projecteursDMX.count(); i > 0 ; i-- )
    {
        listeProjecteursPilotage->removeItem(i);
    }
}

/**
 * @brief Réaffiche les projecteurs en index mis à jour  dans listeProjecteursPilotage
 *
 * @fn IHM::mettreAJourProjecteursPilotage()
 *
 * @return void
 *
 */
void IHM::mettreAJourProjecteursPilotage()
{

    for(int i = 0; i < projecteursDMX.count(); i++)
    {
        listeProjecteursPilotage->addItem(projecteursDMX.at(i)->getNom() + " (" + projecteursDMX.at(i)->getType() + ")");
    }
}

/**
 * @brief ajoute un 0 à coté d'un nombre si ce dernier est inferieur à 10
 *
 * @fn IHM::verifierAjoutZero()
 *
 * @return void
 *
 */
void IHM::verifierAjoutZero(QString &nombre)
{
    if(nombre.toInt() < 10)
    {
        nombre = "0" + nombre;
    }
}

/**
 * @brief ajoute une scène dans le cache de la séquence à enregistrer
 *
 * @fn IHM::ajouterSceneSequence()
 *
 * @return void
 *
 */
void IHM::ajouterSceneSequence()
{


    QString h = QString::number(choixHeures->value());
    QString m = QString::number(choixMinutes->value());
    QString s = QString::number(choixSecondes->value());

    verifierAjoutZero(h);
    verifierAjoutZero(m);
    verifierAjoutZero(s);


    QString texteSceneSequence = choixScenesSequence->currentText() + " " + h + ":" + m + ":" + s;
    QLabel* labelscene = new QLabel(texteSceneSequence, this);
    labelsScenesSequences.append(labelscene);

    layoutListeScenesSequences->addWidget(labelsScenesSequences.last());

    SceneSequence sceneSequence;
    sceneSequence.tempo = choixHeures->value() * 3600 + choixMinutes->value() * 60 + choixSecondes->value();
    sceneSequence.scene = scenes[choixScenesSequence->currentIndex()];
    cacheSequence.ensembleScenes.append(sceneSequence);

}

void IHM::enregistrerSequence()
{
    cacheSequence.nom = choixnomSequence->text();
    cacheSequence.uuid = QUuid::createUuid().toString();
    utilitaireDocuments->enregistrerSequence(cacheSequence);
    choixSequencesDisponibles->addItem(cacheSequence.nom);
    cacheSequence.nom = "";
    cacheSequence.uuid = "";

    cacheSequence.ensembleScenes.clear();

    utilitaireDocuments->lireSequences(sequences);

    fenetreNouvelleSequence->close();

}

void IHM::executerSequence(Sequence sequence)
{
    ThreadAttente thread;
    qDebug() << Q_FUNC_INFO << "nombre de scenes :" << sequence.ensembleScenes.count();
    for(int i = 0; i <sequence.ensembleScenes.count(); i++)
    {
        executerScene(sequence.ensembleScenes[i].scene);
        thread.attendre(sequence.ensembleScenes[i].tempo);
    }


}

void IHM::executerSequenceDefault()
{
    qDebug() << Q_FUNC_INFO;
    executerSequence(sequences[choixSequencesDisponibles->currentIndex()]);
}

void IHM::supprimerSequence()
{
    utilitaireDocuments->supprimerSequence(sequences[choixSequencesDisponibles->currentIndex()].uuid);
    utilitaireDocuments->lireSequences(sequences);
    choixSequencesDisponibles->removeItem(choixSequencesDisponibles->currentIndex());
    qDebug() << Q_FUNC_INFO;
}

/**
 * @brief ajoute une séquence dans le cache du spectacle à enregistrer
 *
 * @fn IHM::ajouterSequenceSpectacle()
 *
 * @return void
 *
 */
void IHM::ajouterSequenceSpectacle()
{

    QLabel* labelsequence = new QLabel(choixSequencesSpectacle->currentText(), this);
    labelsSequencesSpectacle.append(labelsequence);
    layoutListeSequenceSpectacle->addWidget(labelsSequencesSpectacle.last());
    Sequence sequence;
    sequence.nom = choixSequencesSpectacle->currentText();
    sequence.uuid = sequences[choixSequencesSpectacle->currentIndex()].uuid;
    cacheSpectacle.ensembleSequences.append(sequence);

}

void IHM::enregistrerSpectacle()
{
   qDebug() << Q_FUNC_INFO;
   cacheSpectacle.nom = choixNomSpectacle->text();
   cacheSpectacle.uuid = QUuid::createUuid().toString();
   choixSpectaclesDisponibles->addItem(cacheSpectacle.nom);
   spectacles.append(cacheSpectacle);

   utilitaireDocuments->ecrireSpectacles(spectacles);
   utilitaireDocuments->lireSpectacles(spectacles);
   choixNomSpectacle->setText("");
   for(int i =0; i < cacheSpectacle.ensembleSequences.size(); i++)
   {
      delete labelsSequencesSpectacle.at(i);
   }
   labelsSequencesSpectacle.clear();
   cacheSpectacle.ensembleSequences.clear();
   cacheSpectacle.nom = "";
   cacheSpectacle.uuid = "";

}

void IHM::executerSpectacle()
{
    qDebug() << Q_FUNC_INFO << "spectacle :" << spectacles[choixSpectaclesDisponibles->currentIndex()].nom ;
    qDebug() << Q_FUNC_INFO << spectacles[choixSpectaclesDisponibles->currentIndex()].uuid;
    qDebug() << Q_FUNC_INFO << "nombre sequences :" << spectacles[choixSpectaclesDisponibles->currentIndex()].ensembleSequences.size();
    for(int i = 0; i < spectacles[choixSpectaclesDisponibles->currentIndex()].ensembleSequences.size(); i++)
    {
        executerSequence(spectacles[choixSpectaclesDisponibles->currentIndex()].ensembleSequences[i]);
    }
}

void IHM::supprimerSpectacle()
{

    spectacles.remove(choixSpectaclesDisponibles->currentIndex());
    choixSpectaclesDisponibles->removeItem(choixSpectaclesDisponibles->currentIndex());
    utilitaireDocuments->ecrireSpectacles(spectacles);
}
