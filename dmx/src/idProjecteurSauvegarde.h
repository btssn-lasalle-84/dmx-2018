#ifndef IDPROJECTEURSAUVEGARDE_H
#define IDPROJECTEURSAUVEGARDE_H

#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets> /* tous les widgets de Qt5 */
#else
#include <QtGui> /* tous les widgets de Qt4 */
#endif

class IDProjecteurSauvegarde : public QWidget
{
    Q_OBJECT
    public:
      explicit IDProjecteurSauvegarde(QWidget *parent = 0);
      void setLabel(QString nom);
      void setUuid(QString uuid);
      QString getLabel();
      QString getUuid();

    private:
        QLabel *nomProjecteur;
        QPushButton *boutonModifier;
        QPushButton *boutonSupprimer;
        QPushButton *boutonInformations;
        QString uuid;
    
    signals:
        void modification(QString nomAppareilModifie, QString uuidAppareilModifie);
        void suppression(/*QString nomAppareilModifie,*/ QString uuidAppareilModifie);
        void informations(QString nomAppareilModifie, QString uuidAppareilModifie);
    
    public slots:

    private slots:
        void modifier();
        void supprimer();
        void informer();
    
};

#endif // IDPROJECTEURSAUVEGARDE_H
