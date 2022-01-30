#include "DMXProjecteur.h"
#include <QDebug>

/**
  *\file DMXProjecteur.cpp
  *
  *\brief Definition de la classe DMXProjecteur
  *
  *
  *\author Demont Thomas, Reynier Tony
  *
  *\version 1.0
  */

DMXProjecteur::DMXProjecteur() : interfaceDMX(NULL), nom(""), canalDebut(1), nombreCanaux(0), uuid(""), type("")
{
}

DMXProjecteur::DMXProjecteur(QString nom, int canalDebut, int nombreCanaux, QString uuid, QString type) : interfaceDMX(NULL), nom(nom), canalDebut(canalDebut), nombreCanaux(nombreCanaux), uuid(uuid), type(type)
{

}

DMXProjecteur::~DMXProjecteur()
{
    //qDebug() << Q_FUNC_INFO;
}

/**
 * @brief Mutateur de l'attribut interfaceDMX de la classe DMXProjecteur
 *
 * @fn DMXProjecteur::setEnttecDMXUSB
 *
 * @param *interfaceDMX
 *
 * @return void
 */
void DMXProjecteur::setEnttecDMXUSB(EnttecDMXUSB *interfaceDMX)
{
    this->interfaceDMX = interfaceDMX;
}

/**
 * @brief Accesseur de l'attribut nom de la classe DMXProjecteur
 *
 * @fn DMXProjecteur::getNom
 *
 * @return QString
 */
QString DMXProjecteur::getNom() const
{
    return nom;
}

/**
 * @brief Accesseur de l'attribut type de la classe DMXProjecteur
 *
 * @fn DMXProjecteur::getType
 *
 * @return QString
 */
QString DMXProjecteur::getType() const
{
    return type;
}

/**
 * @brief Accesseur de l'attribut canalDebut de la classe DMXProjecteur
 *
 * @fn DMXProjecteur::getCanalDebut
 *
 * @return int
 */
int DMXProjecteur::getCanalDebut() const
{
    return canalDebut;
}

/**
 * @brief Accesseur de l'attribut nombreCanaux de la classe DMXProjecteur
 *
 * @fn DMXProjecteur::getNombreCanaux
 *
 * @return int
 */
int DMXProjecteur::getNombreCanaux() const
{
    return nombreCanaux;
}

/**
 * @brief Accesseur de l'attribut uuid de la classe DMXProjecteur
 *
 * @fn DMXProjecteur::getUuid
 *
 * @return QString
 */
QString DMXProjecteur::getUuid() const
{
    return uuid;
}

/**
 * @brief Accesseur de l'attribut nomCanaux[] de la classe DMXProjecteur
 *
 * @fn DMXProjecteur::getNomCanaux
 *
 * @return QStringList
 */
QStringList DMXProjecteur::getNomCanaux() const
{
    return nomCanaux;
}

/**
 * @brief Accesseur de la case canal-canalDebut de l'attribut nomCanaux[] de la classe DMXProjecteur
 *
 * @fn DMXProjecteur::getNomCanal
 *
 * @param canal
 *
 * @return QString
 */
QString DMXProjecteur::getNomCanal(int canal) const
{
    return nomCanaux.at(canal - canalDebut);
}

/**
 * @brief Mutateur de l'attribut canalDebut de la classe DMXProjecteur
 *
 * @fn DMXProjecteur::setCanalDebut
 *
 * @param debut
 *
 * @return void
 */
void DMXProjecteur::setCanalDebut(int debut)
{
    this->canalDebut = debut;
}

/**
 * @brief Mutateur de l'attribut nomCanaux[] de la classe DMXProjecteur
 *
 * @fn DMXProjecteur::setNomCanaux
 *
 * @param nomCanaux
 *
 * @return void
 */
void DMXProjecteur::setNomCanaux(QStringList nomCanaux)
{
    this->nomCanaux = nomCanaux;
}

/**
 * @brief Mutateur de l'attribut nom de la classe DMXProjecteur
 *
 * @fn DMXProjecteur::setNom
 *
 * @param nom
 *
 * @return void
 */
void DMXProjecteur::setNom(QString nom)
{
    this->nom = nom;
}

/**
 * @brief Mutateur de l'attribut nombreCanaux de la classe DMXProjecteur
 *
 * @fn DMXProjecteur::setNombreCanaux
 *
 * @param nombre
 *
 * @return void
 */
void DMXProjecteur::setNombreCanaux(int nombre)
{
    this->nombreCanaux = nombre;
}

/**
 * @brief Mutateur de l'attribut uuid de la classe DMXProjecteur
 *
 * @fn DMXProjecteur::setUuid
 *
 * @param uuid
 *
 * @return void
 */
void DMXProjecteur::setUuid(QString uuid)
{
    this->uuid = uuid;
}

/**
 * @brief Mutateur de l'attribut type de la classe DMXProjecteur
 *
 * @fn DMXProjecteur::setType
 *
 * @param type
 *
 * @return void
 */
void DMXProjecteur::setType(QString type)
{
    this->type = type;
}
