#ifndef IHM__H
#define IHM__H

/**
  *
  * \file IHM.h
  *
  * \brief Déclaration de la classe IHM
  *
  * \author Demont Thomas, Reynier Tony
  *
  * \version 1.1.2
  *
  */

#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets> /* tous les widgets de Qt5 */
#else
#include <QtGui> /* tous les widgets de Qt4 */
#endif
#include <QTimer>
#include "interface.h"
#include "PlaybackWing.h"
#include "myslider.h"
#include "idProjecteurSauvegarde.h"
#include "scene.h"
#include "sequence.h"
#include "spectacle.h"
#include "ThreadAttente.h"

//#define NB_PAGES        10
#define NB_TAB_MAIN     4
#define NB_SLIDERS      10
#define NB_CANAUX       512
#define NB_PROJECTEURS  32


class EnttecDMXUSB;
class DMXProjecteur;
class XMLUtilitaire;

/**
  *
  * \class IHM
  *
  * \brief La fenêtre principale de l'application
  *
  * \author Demont Thomas, Reynier Tony
  *
  * \version 1.1
  *
  */

class IHM : public QWidget
{
	Q_OBJECT
	
    private:
        int incrementTemporisation;
        QList<Interface> interfaces; /**< \brief Liste d'interfaces Enttex USB */
        QVector<Scene> scenes; /**< \brief Liste des scenes enregistrées depuis le fichier scenes.xml */
        QVector<Sequence> sequences; /**< \brief Liste des sequences enregistrées depuis le fichier sequences.xml */
        QVector<Spectacle> spectacles; /**< \brief Liste des spectacles enregistrés depuis le fichier spectacles.xml */
        Scene cacheScene; /**< \brief cache d'un objet Scene retenu dans cet espace mémoire avant d'etre implémenté dans un conteneur */
        Sequence cacheSequence; /**< \brief cache d'un objet Sequence retenu dans cet espace mémoire avant d'etre implémenté dans un conteneur */
        Spectacle cacheSpectacle; /**< \brief cache d'un objet Spectacle retenu dans cet espace mémoire avant d'etre implémenté dans un conteneur */
        EnttecDMXUSB *interfaceDMX; /**< \brief Association vers la classe EnttecDMXUSB */
        PlaybackWing *wing; /**< \brief Association vers la classe PlaybackWing */
        QVector<DMXProjecteur*> projecteursDMX; /**< \brief rampe de projecteurs DMX */
        XMLUtilitaire *utilitaireDocuments; /**< \brief Utilitaire de gestion des fichiers xml */
        int nbAppareils;
        int canalDepartFaders;
        bool pilotageIHMActif;
        bool pilotageDMXActif;

        // Les onglets
        QTabWidget *ongletsPrincipaux;
        QTabWidget *optionsCreations;
        QTabWidget *optionsParametres;
        QTabWidget *optionsSpectacles;
        // Les pages
        QWidget *pageAssembler;
        QWidget *pageCreer;
        QWidget *pageJouer;
        QWidget *pagePiloter;
        QWidget *parametresInterface;
        QWidget *parametresProjecteurs;
        QWidget *parametresConsoles;
        QWidget *pagesSpectacles[2];
        //Les fenetres
        QDialog* fenetreNouveauProjecteur;
        QDialog* fenetreNouvelleScene;
        QDialog* fenetreNouvelleSequence;
        QDialog* fenetreInformationsAppareil;
        QDialog* fenetreCanauxScene;
        QDialog *fenetreModifierConsole;
        // Les widgets
        MySlider *slidersCanaux[NB_SLIDERS];
        QScrollArea *listeProjecteursEnregistres;
        QPushButton *boutonNouveauProjecteur;
        QPushButton *boutonSupprimerTousProjecteurs;
        QComboBox *listeChoixPilotage;
        QComboBox *listeProjecteursPilotage;
        QPushButton *boutonIncrementationCanal;
        QPushButton *boutonDecrementationCanal;
        QLabel *labelNom;
        QLineEdit *choixNom;
        QLabel *labelType;
        QComboBox *choixTypes;
        QLabel *labelNBCanaux;
        QSpinBox *choixNBCanaux;
        QLabel *labelCanalDepart;
        QSpinBox *choixCanalDepart;
        QLabel *labelNomCanaux;
        QLineEdit *nomsCanaux[10];
        QPushButton *boutonEnregistrer;

        QLabel *labelInformationsCanalDepart;
        QLabel *labelInformationsCanaux[10];
        QScrollArea *listeInformationsCanaux;

        QLabel *labelInterface;
        QComboBox *listeChoixInterface;
        QComboBox *listeTypeInterface;
        QPushButton *boutonDetecterInterface;
        QTextEdit *resultatDetectionInterface;        

        QLabel *labelScenesDisponibles;
        QComboBox *choixScenesDisponibles;
        QPushButton *boutonexecuterScene;
        QPushButton *boutonModifierScene;
        QPushButton *boutonSupprimerScene;
        QPushButton *boutonNouvelleScene;
        QPushButton *boutonSupprimerToutesScenes;

        QLabel *labelNomScene;
        QLineEdit *choixNomScene;
        QLabel *labelAppareilScene;
        QComboBox *choixAppareilScene;
        QPushButton *boutonParametresCanauxScene;
        QScrollArea *listeAppareilsScene;
        QList<QLabel*> labelsAppareilsScene;
        QPushButton *boutonSauvegarderScene;

        MySlider *slidersCanauxScene[NB_SLIDERS];
        QPushButton *boutonSauvegarderCanauxScene;

        QLineEdit *numeroIdAjouterInterface;

        QLineEdit *portInterfaceAjouterInterface;
        QComboBox *listeTypeInterfaceAjouterInterface;
        QPushButton *boutonAjouterInterface;

        QComboBox *listeInterfacesSupprimables;
        QPushButton *boutonSupprimerInterface;

        QComboBox *listeChoixConsole;
        QLineEdit *champAdresseIPConsole;
        QLineEdit *champPortConsole;
        QPushButton *boutonAjouterConsole;
        QPushButton *boutonSupprimerConsole;
        QPushButton *boutonModifierConsole;

        QLineEdit *champAdresseIPModifierConsole;
        QLineEdit *champPortModifierConsole;
        QPushButton *boutonEnregistrerModificationConsole;

        QLabel *labelSequencesDisponibles;
        QComboBox *choixSequencesDisponibles;
        QPushButton *boutonExecuterSequence;
        QPushButton *boutonNouvelleSequence;
        QPushButton *boutonSupprimmerSequence;

        QLabel *labelnomSequence;
        QLineEdit *choixnomSequence;
        QLabel *labelScenesSequence;
        QComboBox *choixScenesSequence;
        QLabel *labelTemporisation;
        QSpinBox *choixHeures;
        QLabel *labelHeures;
        QSpinBox *choixMinutes;
        QLabel *labelMinutes;
        QSpinBox *choixSecondes;
        QLabel *labelSecondes;
        QPushButton *boutonAjouterScene;
        QScrollArea *listeScenesSequenceChoisies;
        QList <QLabel*> labelsScenesSequences;
        QPushButton *boutonEnregistrerSequence;

        QLabel *labelCreationSpectacle;
        QLabel *labelnomSpectacle;
        QLineEdit *choixNomSpectacle;
        QLabel *labelSequencesSpectacle;
        QComboBox *choixSequencesSpectacle;
        QPushButton *boutonchoixSequenceSpectacle;
        QScrollArea *listeSequencesSpectacle;
        QList<QLabel*> labelsSequencesSpectacle;
        QPushButton *boutonSauvegarderSpectacle;

        QLabel *labelJeuSpectacle;
        QLabel *labelSpectaclesDisponibles;
        QComboBox *choixSpectaclesDisponibles;
        QSpacerItem *espace;
        QPushButton *boutonExecuterSpectacle;
        QPushButton *boutonSupprimerSpectacle;

        QAction *actionQuitter;
        // Les layouts
        QVBoxLayout *layoutListeProjecteursParametres;
        QVBoxLayout *layoutlisteAppareilsScene;
        QVBoxLayout *layoutListeScenesSequences;
        QVBoxLayout *layoutListeSequenceSpectacle;
        IDProjecteurSauvegarde *projecteursEnregistres[NB_PROJECTEURS];

        void creerIHM();
        void setCanal();
        void activerPilotageConsole();
        void desactiverPilotageConsole();
        void desactiverPilotageIHM();
        void activerPilotageIHM();
        QStringList executerCommandeSysteme(QString commande);

        QString cacheUuid;

    public:
        explicit IHM(QWidget *parent = 0);
        ~IHM();
        bool afficherProjecteursEnregistres();
        void recupererDerniereInterfaceUtilisee();
        void verifierAjoutZero(QString &nombre);
        void executerScene(Scene scene);
        void executerSequence(Sequence sequence);
    public slots:
        void demarrer();
        void afficherEtatTouchesControle(EtatTouchesControle etatTouchesControle);
        void afficherEtatFaders(EtatFaders etatFaders);
        void envoyerTrameDMX(int valeurFader,int valeurCanal);
        void changerPilotageIHM(QString choix);
        void incrementerCanal();
        void decrementerCanal();
        void changerCanalDepart(int canal);
        void selectionnerProjecteursPilotage(int index);
        void detecterInterface();
        void selectionnerInterface(int index);
        void enregistrerProjecteur(/*QString uuid*/);
        void genererFenetreNouveauProjecteur();
        void bloquerChoixNomCanaux(int valeur);
        void genererFenetreNouvelleScene();
        void genererFenetreNouvelleSequence();
        void genererFenetreInformationsAppareil();
        void genererFenetreCanauxScene();
        void ouvrirFenetreNouveauProjecteur();
        void ouvrirFenetreModifierProjecteur(QString projecteur, QString uuid);
        void ouvrirFenetreInformationsProjecteur(QString projecteur, QString uuid);
        void ouvrirFenetreNouvelleScene();
        void ouvrirFenetreCanauxScene();
        void ouvrirFenetreNouvelleSequence();

        void enregistrerScene();
        void ajouterAppareilScene();

        void enregistrerSequence();
        void ajouterSceneSequence();
        //void modifierScene();

        void executerSceneDefault();
        void supprimerScene();
        void executerSequenceDefault();
        void supprimerSequence();
        //void supprimerToutesScenes();
        void supprimerProjecteur(/*QString nom,*/ QString uuid);
        void supprimerTousProjecteurs();
        void recupererDonneesNouvelleInterface();
        void quitter();
        void supprimerInterface();

        void ajouterConsole();
        void genererFenetreModifierConsole();
        void envoyerModificationConsole();
        void supprimerConsole();

        void ajouterSequenceSpectacle();
        void enregistrerSpectacle();
        void executerSpectacle();
        void supprimerSpectacle();

        void effacerAffichageProjecteursEnregistres();
        void mettreAJourProjecteursEnregistres();
        void effacerAffichageProjecteursPilotage();
        void mettreAJourProjecteursPilotage();

        void envoyerCanalDepartWing();

    signals :

};

#endif
