#ifndef DMXPROJECTEUR__H
#define DMXPROJECTEUR__H

#include <QtCore>


/**
  *\file DMXProjecteur.cpp
  *
  *\class DMXProjecteur
  *
  *\brief Definition de la classe DMXProjecteur
  *
  *
  *\author Demont Thomas, Reynier Tony
  *
  *\version 1.0
  */

class EnttecDMXUSB;

class DMXProjecteur
{
    protected:
        EnttecDMXUSB *interfaceDMX; /**< \brief Association vers la classe EnttecDMXUSB */
        QString nom;
        int canalDebut;
        int nombreCanaux;
        QString uuid;
        QString type;
        QStringList nomCanaux;
		
	public:
		DMXProjecteur();
        DMXProjecteur(QString nom, int canalDebut, int nombreCanaux, QString uuid, QString type = "");
        virtual ~DMXProjecteur();

        void setEnttecDMXUSB(EnttecDMXUSB *interfaceDMX);
        QString getNom() const;
        int getCanalDebut() const;
        int getNombreCanaux() const;
        QString getUuid() const;
        QString getType() const;
        QStringList getNomCanaux() const;
        QString getNomCanal(int canal) const;

        void setCanalDebut(int debut);
        void setNomCanaux(QStringList nomCanaux);

        /** \todo définir les accesseurs set */

        void setNom(QString nom);
        void setNombreCanaux(int nombre);
        void setUuid(QString uuid);
        void setType(QString type);

};


#endif
