#ifndef DMXSPECIAL__H
#define DMXSPECIAL__H

#include "DMXProjecteur.h"

class DMXSpecial : public DMXProjecteur
{
	private:


	public:
		DMXSpecial();
        DMXSpecial(QString nom, int canalDebut, int nombreCanaux, QString uuid, QString type = "SPECIAL");
        virtual ~DMXSpecial();

        void setCanaux();
};

#endif
