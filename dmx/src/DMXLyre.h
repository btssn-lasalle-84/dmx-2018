#ifndef DMXLYRE__H
#define DMXLYRE__H

#include "DMXProjecteur.h"

class DMXLyre : public DMXProjecteur
{
	private:
        int canalPan;
        int canalTilt;
        int canalGlobos;
        int canalCouleur;

	public:
		DMXLyre();
        DMXLyre(QString nom, int canalDebut, int nombreCanaux, QString uuid, QString type = "LYRE");
        virtual ~DMXLyre();

        void setCanaux(int pan, int tilt, int globos, int couleur);
};


#endif
