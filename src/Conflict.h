#ifndef CONFLICT_H
#define CONFLICT_H

#include <QString>
#include <QVector>
#include <QPushButton>
#include <functional>

//競合
class ConflictSolver : public QObject
{
    Q_OBJECT
public:
    typedef std::function<void()> ConflictFunc;
    QString explain;
    ConflictFunc solver;

    ConflictSolver(QObject* parent=nullptr);
    ConflictSolver(QString exprain,ConflictFunc solver , QObject* parent=nullptr);
    virtual ~ConflictSolver(){}
public slots:
    void Run();
};

struct Conflict{
    QString error; //原因
    QVector<ConflictSolver*> solvers; //解決関数

    bool operator<(const Conflict& rhs)const{
        return this->error < rhs.error;
    }
};


#endif // CONFLICT_H
