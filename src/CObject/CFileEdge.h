#ifndef CFILEEDGE_H
#define CFILEEDGE_H

#include <QFile>
#include <QException>
#include <QMessageBox>
#include <CObject/CSpline.h>

class CFileEdge : public CSpline
{
    Q_OBJECT
private:
    Pos start_base; //始点初期値
public:
    static CFileEdge* CreateFromFile(QString filepath);
public:
    virtual QString DefaultClassName(){return "CFileEdge";}

    CFileEdge();
};

#endif // CFILEEDGE_H
