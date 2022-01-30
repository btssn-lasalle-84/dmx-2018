#include "idProjecteurSauvegarde.h"
#include <QDebug>

/**
  *
  * \file idProjecteurSauvegarde.cpp
  *
  * \brief Définition de la classe PlaybackWing
  *
  * \author Reynier Tony
  *
  * \version 1.0
  *
  */

IDProjecteurSauvegarde::IDProjecteurSauvegarde(QWidget *parent) :
    QWidget(parent)
{
    QHBoxLayout *hLayout = new QHBoxLayout;
    nomProjecteur = new QLabel();
    nomProjecteur->setText("");
    boutonModifier = new QPushButton(QString::fromUtf8("Modifier"), this);
    boutonSupprimer = new QPushButton(QString::fromUtf8("Supprimer"), this);
    boutonInformations = new QPushButton(QString::fromUtf8("Informations"), this);



    hLayout->addWidget(nomProjecteur);
    hLayout->addWidget(boutonModifier);
    hLayout->addWidget(boutonSupprimer);
    hLayout->addWidget(boutonInformations);

    setLayout(hLayout);
    connect(boutonModifier, SIGNAL(clicked()), this, SLOT(modifier()));
    connect(boutonSupprimer, SIGNAL(clicked()), this, SLOT(supprimer()));
    connect(boutonInformations, SIGNAL(clicked()), this, SLOT(informer()));
}

/**
 * @brief Emet le signal modification
 *
 * @fn IDProjecteurSauvegarde::modifier
 *
 * @return void
 */
void IDProjecteurSauvegarde::modifier()
{
    emit modification(this->getLabel(), this->getUuid());
}

/**
 * @brief Emet le signal suppression
 *
 * @fn IDProjecteurSauvegarde::supprimer
 *
 * @return void
 */
void IDProjecteurSauvegarde::supprimer()
{
    emit suppression(/*this->getLabel(),*/ this->getUuid());
}

/**
 * @brief Emet le signal informations
 *
 * @fn IDProjecteurSauvegarde::informer
 *
 * @return void
 */
void IDProjecteurSauvegarde::informer()
{
    emit informations(this->getLabel(), this->getUuid());
}

/**
 * @brief Mutateur du contenu texte du Label nomProjecteur
 *
 * @fn IDProjecteurSauvegarde::setLabel
 *
 * @param nom
 *
 * @return void
 */
void IDProjecteurSauvegarde::setLabel(QString nom)
{
    nomProjecteur->setText(nom);
    //qDebug() << Q_FUNC_INFO << "nom donne:" << nom;
}

/**
 * @brief Mutateur de l'attribut uuid de la classe IDProjecteurSauvegarde
 *
 * @fn IDProjecteurSauvegarde::setUuid
 *
 * @param uuid
 *
 * @return void
 */
void IDProjecteurSauvegarde::setUuid(QString uuid)
{
    this->uuid = uuid;
}

/**
 * @brief Accesseur du contenu texte du Label nomProjecteur
 *
 * @fn IDProjecteurSauvegarde::getLabel
 *
 * @return QString
 */
QString IDProjecteurSauvegarde::getLabel()
{
    return nomProjecteur->text();
}

/**
 * @brief Accesseur de l'attribut uuid de la classe IDProjecteurSauvegarde
 *
 * @fn IDProjecteurSauvegarde::getUuid
 *
 * @return QString
 */
QString IDProjecteurSauvegarde::getUuid()
{
    return uuid;
}
