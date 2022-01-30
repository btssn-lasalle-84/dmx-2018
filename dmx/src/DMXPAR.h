#ifndef DMXPAR__H
#define DMXPAR__H

#include "DMXProjecteur.h"

class DMXPAR : public DMXProjecteur
{
	private:
        int canalCouleurRouge;
        int canalCouleurBleue;
        int canalCouleurVerte;

	public:
		DMXPAR();
        DMXPAR(QString nom, int canalDebut, int nombreCanaux, QString uuid, QString type = "PAR LED");
        virtual ~DMXPAR();

        void setCanaux(int rouge, int bleu, int vert);
};


#endif
