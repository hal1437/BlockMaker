#include "Restraint.h"

QList<Restraint*> Restraint::Restraintable(const QList<CObject*> nodes){
    //実際にオブジェクトを生成して投げつける
    QList<Restraint*> ans;
    if(EqualLengthRestraint::Restraintable(nodes))ans.push_back(new EqualLengthRestraint(nodes));
    if( ConcurrentRestraint::Restraintable(nodes))ans.push_back(new ConcurrentRestraint (nodes));
    if(   VerticalRestraint::Restraintable(nodes))ans.push_back(new VerticalRestraint   (nodes));
    if(       LockRestraint::Restraintable(nodes))ans.push_back(new LockRestraint       (nodes));
    if(     UnlockRestraint::Restraintable(nodes))ans.push_back(new UnlockRestraint     (nodes));
    if(      MatchRestraint::Restraintable(nodes))ans.push_back(new MatchRestraint     (nodes));
    return ans;
}
void Restraint::DrawGL(Pos camera,Pos center)const{
    int old_width;
    //色と線の太さを保存
    glGetIntegerv(GL_LINE_WIDTH   ,&old_width);
    //色と線の太さを設定
    glLineWidth(2);

    //カメラ変換行列
    Pos cc = camera - center;
    double theta1 = std::atan2(cc.y(),std::sqrt(cc.x()*cc.x()+cc.z()*cc.z()));
    double theta2 = std::atan2(-cc.x(),cc.z());
    Quat quat = Quat::getRotateXMatrix(theta1).Dot(Quat::getRotateYMatrix(theta2));

    QImage img(this->GetIconPath());
    QImage glimg = QGLWidget::convertToGLFormat(img);
    for(Pos pp:this->GetIconPoint()){
        //描画座標
        Pos cp   = pp + Pos(30*this->stack_level,0,0).Dot(quat);//画像描画座標
        glRasterPos3f(cp.x(),cp.y(),cp.z());
        glDrawPixels(img.width(), img.height(), GL_RGBA, GL_UNSIGNED_BYTE, glimg.bits());

        //無効なら色を変える
        if(this->isNotEffect()){
            glColor3f(1,0,0);
        }
        //枠線
        glBegin(GL_LINE_LOOP);
        const double length = cc.Length()*10*std::sqrt(2);
        Pos ofs = Pos(1/std::sqrt(2),1/std::sqrt(2),1);
        for(int i =0;i<4;i++){
            Pos p = ((Pos(std::sin((i+0.5)*M_PI/2),std::cos((i+0.5)*M_PI/2),0) + ofs)*length).Dot(quat);
            glVertex3f((cp+p).x(),(cp+p).y(),(cp+p).z());
        }
        glEnd();
        //無効なら／を入れる
        if(this->isNotEffect()){
            glBegin(GL_LINES);
            for(int i =0;i<2;i++){
                Pos p1 = ((Pos(std::sin((i+0.25)*M_PI),std::cos((i+0.25)*M_PI),0) + ofs)*length).Dot(quat);
                glVertex3f((cp+p1).x(),(cp+p1).y(),(cp+p1).z());
            }
            for(int i =0;i<2;i++){
                Pos p1 = ((Pos(std::sin((i+0.75)*M_PI),std::cos((i+0.75)*M_PI),0) + ofs)*length).Dot(quat);
                glVertex3f((cp+p1).x(),(cp+p1).y(),(cp+p1).z());
            }
            glEnd();
        }
    }
    //色と線の太さを復元
    glLineWidth(old_width);
}
void Restraint::Create(const QList<CObject*> nodes){
    this->children = nodes;
    for(CObject* edge:nodes)ObserveChild(edge);
}
CObject* Restraint::GetChild     (int index){
    return this->children[index];
}
void     Restraint::SetChild     (int index,CObject* obj){
    this->children[index] = obj;
}

int      Restraint::GetChildCount()const{
    return this->children.size();
}
CObject* Restraint::Clone()const{
    /*
    Restraint* c = new Restraint();
    c->children    = this->children;
    c->stack_level = this->stack_level;
    return c;*/
    return nullptr;
}



Restraint::Restraint(QList<CObject*> children):
    children(children){
    stack_level = 0;
}

QList<Pos> Restraint::GetIconPoint()const{
    QList<Pos> ans;
    for(CObject* obj:this->children){
        Pos p;
        if(obj->is<CPoint>())p = *dynamic_cast<CPoint*>(obj);//点自身
        else if(obj->is<CEdge>() )p = dynamic_cast<CEdge*>(obj)->GetMiddleDivide(0.5);//中点
        else {
            QList<CPoint*> children = obj->GetAllChildren();
            for(CPoint* pos:children){
                p += *pos;
            }
            p /= children.size();
        }
        ans.push_back(p);
    }
    return ans;
}

void EqualLengthRestraint::Create(const QList<CObject*> nodes){
    CEdge* ee = dynamic_cast<CEdge*>(nodes.first());
    this->length = (*ee->end - *ee->start).Length();
    Restraint::Create(nodes);
}
bool EqualLengthRestraint::isComplete(){
    //全てのEdgeの長さが同じ
    double dd = (*dynamic_cast<CEdge*>(this->children[0])->end - *dynamic_cast<CEdge*>(this->children[0])->start).Length();
    bool equal = std::all_of(this->children.begin()+1,this->children.end(),[dd](CObject* obj){
        CEdge* edge = dynamic_cast<CEdge*>(obj);
        return (*edge->end - *edge->start).Length() == dd;
    });

    if(!equal){
        //競合発生
        Conflict conf;
        conf.error = "長さが等しくありません";
        double dd = (*dynamic_cast<CEdge*>(this->children[0])->end - *dynamic_cast<CEdge*>(this->children[0])->start).Length();
        for(int i =1;i<this->children.size();i++){
            conf.solvers.push_back(new ConflictSolver(QString("線%1を線1の長さに変更").arg(i+1),[&](){
                CEdge* edge = dynamic_cast<CEdge*>(this->children[i]);
                //中心へ移動させる
                Pos center = (*edge->end + *edge->start)/2;
                *edge->start = (*edge->start - center).GetNormalize() * dd/2 + center;
                *edge->end   = (*edge->end   - center).GetNormalize() * dd/2 + center;
            }));
        }
        emit Conflicted(this,conf);
    }else{
        emit Solved(this);
    }
    this->SetNotEffect(!equal);
    return equal;
}
void EqualLengthRestraint::Calc(){
    //先頭のCEdgeの長さに統一する。
    bool changed = false;
    double dd = (*dynamic_cast<CEdge*>(this->children[0])->end - *dynamic_cast<CEdge*>(this->children[0])->start).Length();
    for(int i=1;i<this->children.size();i++){
        CEdge* edge = dynamic_cast<CEdge*>(this->children[i]);
        //中心へ移動させる
        Pos center = (*edge->end + *edge->start)/2;
        *edge->start = (*edge->start - center).GetNormalize() * dd/2 + center;
        *edge->end   = (*edge->end   - center).GetNormalize() * dd/2 + center;
        changed = true;
    }
    if(changed){
        //emit Changed();
    }
}



bool ConcurrentRestraint::isComplete(){
    //全てのEdgeの向きが同じ
    Pos pos = (*dynamic_cast<CEdge*>(this->children[0])->end - *dynamic_cast<CEdge*>(this->children[0])->start).GetNormalize();
    return std::all_of(this->children.begin()+1,this->children.end(),[pos](CObject* obj){
        CEdge* edge = dynamic_cast<CEdge*>(obj);
        return (*edge->end - *edge->start).GetNormalize() == pos;
    });
}
void ConcurrentRestraint::Calc(){
    //先頭のCEdgeと同じ向きに変更する。
    bool changed = false;
    Pos base = (*dynamic_cast<CEdge*>(this->children[0])->end - *dynamic_cast<CEdge*>(this->children[0])->start).GetNormalize();
    for(int i=1;i<this->children.size();i++){
        CEdge* edge = dynamic_cast<CEdge*>(this->children[i]);
        //endを移動させる
        double l = (*edge->end  - *edge->start).Length();
        if((*edge->end - *edge->start).DotPos(base) < 0) l = -l;//反転
        *edge->end = base * l + *edge->start;
        changed = true;
    }
    if(changed){
        //emit Changed();
    }
}

bool VerticalRestraint::Restraintable(const QList<CObject *> nodes){
    if(nodes.size() < 2)return false;
    if(std::any_of(nodes.begin(),nodes.end(),[](CObject* obj){return !obj->is<CEdge>();}))return false;

    CEdge* base_edge = dynamic_cast<CEdge*>(nodes[0]);
    if(std::any_of(nodes.begin()+1,nodes.end(),[&](CObject* obj){
        CEdge* edge = dynamic_cast<CEdge*>(obj);
        return (!base_edge->isOnEdge(*edge->start)) &&
               (!base_edge->isOnEdge(*edge->end  ));
    }))return false;

    return true;
}
bool VerticalRestraint::isComplete(){
    CEdge* base_edge = dynamic_cast<CEdge*>(this->children[0]);
    //すべてのEdgeの端点が初めのEdgeと一致している
    if(std::any_of(this->children.begin()+1,this->children.end(),[&](CObject* obj){
        CEdge* edge = dynamic_cast<CEdge*>(obj);
        return (!base_edge->isOnEdge(*edge->start))  &&
               (!base_edge->isOnEdge(*edge->end  ));
    }))return false;

    //すべてのEdgeの端点が水平方向に成分を持たない
    if(std::any_of(this->children.begin()+1,this->children.end(),[&](CObject* obj){
        CEdge* edge = dynamic_cast<CEdge*>(obj);
        CPoint *base,*move;//起点,移動点
        if(base_edge->isOnEdge(*edge->start)){
            move = edge->end;
            base = edge->start;
        }else{
            move = edge->start;
            base = edge->end;
        }
        qDebug() << base_edge->GetDifferentialVec(*base).DotPos(*move-*base);
        return std::abs(base_edge->GetDifferentialVec(*base).DotPos(*move-*base)) > CObject::SAME_POINT_EPS;
    }))return false;

    return true;
}
void VerticalRestraint::Calc(){
    //先頭のCEdgeと同じ向きに変更する。
    for(int i = 1;i<this->children.size();i++){
        CEdge* base_edge = dynamic_cast<CEdge*>(this->children[0]);
        CEdge* move_edge = dynamic_cast<CEdge*>(this->children[i]);
        CPoint *base,*move;//起点,移動点
        if(base_edge->isOnEdge(*move_edge->start)){
            move = move_edge->end;
            base = move_edge->start;
        }else{
            move = move_edge->start;
            base = move_edge->end;
        }
        double length = (*move_edge->end - *move_edge->start).Length();//ベクトルの長さ
        Pos diff = base_edge->GetDifferentialVec(*base);//Edge方向のベクトル
        Pos moved = *move - diff * (diff.DotPos((*move-*base)));//水平方向成分を取り去った点
        move->MoveAbsolute((moved - *base).GetNormalize() * length + *base);
    }
}


bool LockRestraint::isComplete(){
    bool unlocked = false;
    for(CObject* child :this->children){
        for(CPoint* pp:child->GetAllChildren()){
            //一つでもロックが解除されていれば
            if(!pp->isLock())unlocked = true;
        }
    }
    //ロック解除
    return !unlocked;
}
void LockRestraint::Calc(){
    for(CObject* child :this->children){
        for(CPoint* pp:child->GetAllChildren()){
            //全てロック
            pp->SetLock(true);
        }
    }
}
void LockRestraint::ChangeObjectCallback(CObject*){
    if(!this->isComplete()){
        //全てのロックの解除
        for(CObject* child :this->children){
            for(CPoint* pp:child->GetAllChildren()){
                //全てのロックを解除
                pp->SetLock(false);
            }
        }
        //自壊
        emit Destroy(this);
    }
}
bool LockRestraint::Restraintable(const QList<CObject *> nodes){
    for(CObject* child :nodes){
        for(CPoint* pp:child->GetAllChildren()){
            if(!pp->isLock())return true;
        }
    }
    return false;
}

void UnlockRestraint::Calc(){
    for(CObject* child :this->children){
        for(CPoint* pp:child->GetAllChildren()){
            //全てロック解除
            pp->SetLock(false);
            pp->ChangedEmittor();//
        }
    }
    //自壊
    emit Destroy(this);
}
bool UnlockRestraint::Restraintable(const QList<CObject *> nodes){
    for(CObject* child :nodes){
        for(CPoint* pp:child->GetAllChildren()){
            if(pp->isLock())return true;
        }
    }
    return false;
}


QList<Pos> MatchRestraint::GetIconPoint()const{
    QList<Pos> ans;
    for(int i=0;i<this->children.size();i++){
        if(this->children[i]->is<CPoint>()){
            ans.push_back(*dynamic_cast<CPoint*>(this->children[i]));
        }
    }
    return ans;
}
void MatchRestraint::Calc(){
    //CPointでないCObjectにその他のCPoint一致させる。
    for(int i=0;i<this->children.size();i++){
        if(!this->children[i]->is<CPoint>()){
            for(int j=0;j<this->children.size();j++){
                if(i==j)continue;
                CPoint* pos = dynamic_cast<CPoint*>(this->children[j]);
                pos->MoveAbsolute(this->children[i]->GetNearPos(*pos));

            }
        }
    }
}
bool MatchRestraint::isComplete(){
    for(int i=0;i<this->children.size();i++){
        if(!this->children[i]->is<CPoint>()){
            for(int j=0;j<this->children.size();j++){
                if(i==j)continue;
                CPoint* pos = dynamic_cast<CPoint*>(this->children[j]);
                if((*pos - this->children[i]->GetNearPos(*pos)).Length() > 1.0e-10){
                    return false;
                }
            }
        }
    }
    return true;
}
