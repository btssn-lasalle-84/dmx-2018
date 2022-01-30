#include "myslider.h"
#include <QDebug>

/**
  *\file myslider.cpp
  *
  *\brief Définition de la classe myslider permettant d'instancier des widgets de type slider personnalisés
  *
  *\version 1.0
  */

MySlider::MySlider( QWidget *parent, int canal, int valeur, QString nomCanal ) : QWidget( parent )
{
   lcd = new QLCDNumber(NB_DIGIT, this );
   s = new QSlider( Qt::Vertical, this );
   s->setTickPosition( QSlider::TicksBothSides );
   s->setTickInterval(VALEUR_MAX);
   s->setMinimum(VALEUR_MIN);
   s->setMaximum(VALEUR_MAX);

   sb = new QSpinBox(this);
   sb->setMaximum(NB_CANAUX);

   nom = new QLabel(nomCanal, this);

   QVBoxLayout *mainLayout = new QVBoxLayout;
   QHBoxLayout *hLayout1 = new QHBoxLayout;
   QHBoxLayout *hLayout2 = new QHBoxLayout;
   QHBoxLayout *hLayout3 = new QHBoxLayout;
   QHBoxLayout *hLayout4 = new QHBoxLayout;
   hLayout1->addWidget(lcd);
   hLayout2->addWidget(s);
   hLayout3->addWidget(sb);
   hLayout4->addWidget(nom);
   hLayout4->setAlignment(Qt::AlignHCenter);
   mainLayout->addLayout(hLayout1);
   mainLayout->addLayout(hLayout2);
   mainLayout->addLayout(hLayout3);
   mainLayout->addLayout(hLayout4);
   setLayout(mainLayout);

   connect( s, SIGNAL(valueChanged(int)), lcd, SLOT(display(int)) );
   connect( s, SIGNAL(valueChanged(int)), this, SLOT(changerValeur(int)) );
   connect( sb, SIGNAL(valueChanged(int)), this, SLOT(changerCanal(int)) );
   connect( this, SIGNAL(valeurChange(int)), lcd, SLOT(display(int)) );
   connect( this, SIGNAL(canalChange(int)), sb, SLOT(setValue(int)) );
   etat = true;

   lcd->setPalette(Qt::black);
   s->setValue(valeur);
   sb->setValue(canal);
}

MySlider::~MySlider()
{
    //qDebug() << Q_FUNC_INFO;
}

/**
 * @brief active le ScrollBar sb
 *
 * @fn MySlider::setCanalEnabled
 *
 * @param etat bool true active le scrollbar sb et false le désactive
 *
 * @return void
 */
void MySlider::setCanalEnabled(bool etat)
{
    sb->setEnabled(etat);
}

/**
 * @brief Assesseur de la valeur du SpinBox s
 *
 * @return int le numéro du canal
 */
int MySlider::getCanal()
{
    return sb->value();
}

/**
 * @brief Assesseur de la valeur du SpinBox s
 *
 * @return int la valeur dans le canal
 */
int MySlider::getValeur()
{
    return s->value();
}

/**
 * @brief Emmeteur des signaux valeurChange et sliderChange
 *
 * @fn MySlider::changerValeur
 *
 * @param valeur int la valeur dans le canal
 *
 * @return void
 */
void MySlider::changerValeur(int valeur)
{
    //qDebug() << "valeur = " << valeur;
    emit valeurChange(valeur);
    emit sliderChange(sb->value(), valeur);
}

/**
 * @brief Emmeteur des signaux canalChange et sliderChange
 *
 * @fn MySlider::changerCanal
 *
 * @param canal int le numéro du canal
 *
 * @return void
 */
void MySlider::changerCanal(int canal)
{
    //qDebug() << "canal = " << canal;
    emit canalChange(canal);
    emit sliderChange(canal, s->value());
}

/**
 * @brief Mutateur de la valeur du SpinBox s
 *
 * @fn MySlider::setValeur
 *
 * @param valeur int la valeur dans le canal
 *
 * @return void
 */
void MySlider::setValeur(int valeur)
{
    s->setValue(valeur);
}

/**
 * @brief Mutateur de la valeur du ScrollBar sb
 *
 * @fn MySlider::setCanal
 *
 * @param canal int le numéro du canal
 *
 * @return void
 */
void MySlider::setCanal(int canal)
{
    sb->setValue(canal);
}

/**
 * @brief Mutateur de la valeur du Label nom
 *
 * @fn MySlider::setNomCanal
 *
 * @param nomCanal QString le libellé du canal
 *
 * @return void
 */
void MySlider::setNomCanal(QString nomCanal)
{
    nom->setText(nomCanal);
}

/**
 * @brief Non utilisé
 *
 * @fn MySlider::updateDMX
 *
 * @param canal int le numéro du canal
 * @param valeur int la valeur dans le canal
 *
 * @return void
 */
void MySlider::updateDMX(int canal, int valeur)
{
    Q_UNUSED(canal)
    Q_UNUSED(valeur)
}

/**
 * @brief Connexion/Déconnexion des slots et signaux de la classe MySlider
 *
 * @fn MySlider::setConnecte
 *
 * @param etat bool true permet de connecter les signaux/slos et false les déconnecte
 *
 * @return void
 */
void MySlider::setConnecte(bool etat)
{
    if(etat == this->etat)
        return;

    if(etat)
    {
        connect( s, SIGNAL(valueChanged(int)), lcd, SLOT(display(int)) );
        connect( s, SIGNAL(valueChanged(int)), this, SLOT(changerValeur(int)) );
        connect( sb, SIGNAL(valueChanged(int)), this, SLOT(changerCanal(int)) );
        connect( this, SIGNAL(valeurChange(int)), lcd, SLOT(display(int)) );
        connect( this, SIGNAL(canalChange(int)), sb, SLOT(setValue(int)) );
    }
    else
    {
        disconnect( s, SIGNAL(valueChanged(int)), lcd, SLOT(display(int)) );
        disconnect( s, SIGNAL(valueChanged(int)), this, SLOT(changerValeur(int)) );
        disconnect( sb, SIGNAL(valueChanged(int)), this, SLOT(changerCanal(int)) );
        disconnect( this, SIGNAL(valeurChange(int)), lcd, SLOT(display(int)) );
        disconnect( this, SIGNAL(canalChange(int)), sb, SLOT(setValue(int)) );
    }
    this->etat = etat;
}
