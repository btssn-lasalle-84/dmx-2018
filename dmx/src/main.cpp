#include <QApplication>
#include "IHM.h"

/**
 * @brief Programme principal
 *
 * @details Crée et affiche la fenêtre principale de l'application
 *
 * @file main.cpp
 */

/**
 * @brief Programme principal
 *
 * @fn main(int argc, char *argv[])
 * @param argc
 * @param argv[]
 * @return int
 */

int main(int argc, char *argv[])
{
    //QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    //QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    //QTextCodec::setCodecForLocale(QTextCodec::codecForName("ISO 8859-1"));
    //QTextCodec::setCodecForLocale(QTextCodec::codecForName("Windows-1250"));

	QApplication a(argc, argv);
    IHM w;
    w.show();

    
    return a.exec();	
}
