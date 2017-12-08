#ifndef CONFLICT_H
#define CONFLICT_H

#include <QString>
#include <QVector>
#include <functional>

//競合
struct ConflictSolver{
    QString explain; //解決手段
    std::function<void()> solver; //解決関数
};
struct Conflict{
    QString error; //原因
    QVector<ConflictSolver> solvers; //解決関数
};


#endif // CONFLICT_H
