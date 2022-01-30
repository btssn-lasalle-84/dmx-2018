#ifndef DMXSCANNER__H
#define DMXSCANNER__H

#include "DMXProjecteur.h"

class DMXScanner : public DMXProjecteur
{
	private:
        int canalPan;
        int canalTilt;
        int canalCouleur;

	public:
		DMXScanner();
        DMXScanner(QString nom, int canalDebut, int nombreCanaux, QString uuid, QString type = "SCANNER");
        virtual ~DMXScanner();

        void setCanaux(int pan, int tilt, int couleur);
};


#endif
