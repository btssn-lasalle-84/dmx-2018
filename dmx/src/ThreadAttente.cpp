#include "ThreadAttente.h"

/**
 * @brief Méthode de génération d'un temps d'attente
 *
 * @fn ThreadAttente::attendre
 *
 * @param temps int
 *
 */
void ThreadAttente::attendre(int temps)
{
    this->sleep(temps);
}
