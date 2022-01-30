#ifndef MYSLIDER_H
#define MYSLIDER_H

#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets> /* tous les widgets de Qt5 */
#else
#include <QtGui> /* tous les widgets de Qt4 */
#endif

/**
  *\file myslider.h
  *
  *\class MySlider
  *
  *\brief Déclaration de la classe myslider permettant d'instancier des widgets de type slider personnalisés
  *
  *\version 1.0
  */

#define VALEUR_MIN   0
#define VALEUR_MAX   255
#define NB_CANAUX    512

#define NB_DIGIT     3

class MySlider : public QWidget
{
	Q_OBJECT
    
private:
        QLCDNumber  *lcd;
        QSlider     *s;
        QSpinBox    *sb;
        QLabel      *nom;
        bool        etat;
        
public:
        MySlider( QWidget *parent = 0, int canal = 0, int valeur = 0, QString nomCanal = QString("") );
        ~MySlider();

        void setCanalEnabled(bool etat);
        int getCanal();
        int getValeur();

signals:
        void sliderChange(int canal, int valeur);
        void valeurChange(int valeur);
        void canalChange(int canal);

private slots:
        void changerValeur(int valeur);
        void changerCanal(int canal);

public slots:
        void setValeur(int valeur);
        void setCanal(int canal);
        void setNomCanal(QString nomCanal);
        void updateDMX(int canal, int valeur);
        void setConnecte(bool etat);
};

#endif //MYSLIDER_H
