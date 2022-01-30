#ifndef DMXLASER__H
#define DMXLASER__H

#include "DMXProjecteur.h"

class DMXLaser : public DMXProjecteur
{
	private:
        int canalControle;
        int canalExtinction;
        int canalPictures;
        int canalDeplacement;
        int canalRotation;
        int canalSens;
        int canalStretch;
        int canalSpeed;
        int canalSlowDrawSpeed;
        int canalTaille;

	public:
		DMXLaser();
        DMXLaser(QString nom, int canalDebut, int nombreCanaux, QString uuid, QString type = "LASER");
        virtual ~DMXLaser();

        void setCanaux(int controle, int extinction, int pictures, int deplacement, int rotation, int sens, int stretch, int speed, int slowDrawSpeed, int taille);
};

#endif
