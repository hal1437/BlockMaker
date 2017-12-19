#include "Conflict.h"

ConflictSolver::ConflictSolver(QObject* parent){
}
ConflictSolver::ConflictSolver(QString explain,ConflictFunc solver , QObject* parent):
    QObject(parent),explain(explain),solver(solver)
{
}

void ConflictSolver::Run(){
    this->solver();
}
