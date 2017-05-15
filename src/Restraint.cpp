#include "Restraint.h"

QVector<RestraintType> Restraint::Restraintable(const QVector<CObject*> values){
    QVector<RestraintType> answer;
    //等値
    if(values.size()>=2 && !values[0]->is<CPoint>() && std::all_of(values.begin(),values.end(),[&](CObject* v){return typeid(values[0])==typeid(v);}))answer.push_back(EQUAL);
    //マージ
    if(values.size()>=2 && std::count_if(values.begin(),values.end(),[](CObject* obj){return obj->is<CPoint>();}) == values.size())answer.push_back(MARGE);
    //一致
    if(values.size()==2 && std::count_if(values.begin(),values.end(),[](CObject* obj){return obj->is<CPoint>();}) >= 1)answer.push_back(MATCH);
    //正接
    if(values.size()==2 && std::count_if(values.begin(),values.end(),[](CObject* obj){return obj->is<CArc>();}) >= 1)answer.push_back(TANGENT);
    //並行
    if(std::count_if(values.begin(),values.end(),[](CObject* obj){return obj->is<CLine>();}) == 2)answer.push_back(CONCURRENT);
    if(values.size()>=1){
        if(std::any_of(values.begin(),values.end(),[](CObject* obj){return !obj->isLock();}))answer.push_back(LOCK);  //固定
        if(std::any_of(values.begin(),values.end(),[](CObject* obj){return  obj->isLock();}))answer.push_back(UNLOCK);//固定解除
        answer.push_back(VERTICAL);//垂直
        answer.push_back(HORIZONTAL);//水平
    }

    return answer;
}


bool EqualRestraint::Complete(){
    /*
    std::sort(nodes.begin(),nodes.end(),[](CObject* lhs,CObject* rhs){
        return lhs->GetJointNum() > rhs->GetJointNum();
    });

    double length=0;
    if(nodes[0]->is<CLine>())length = (nodes[0]->GetJointPos(0) - nodes[0]->GetJointPos(1)).Length();
    if(nodes[0]->is<CArc> ())length = (nodes[0]->GetJointPos(0) - nodes[0]->GetJointPos(2)).Length();
    for(int i=1;i<nodes.size();i++){
        if(nodes[i]->is<CLine>()){
            Pos next = (nodes[i]->GetJointPos(1) - nodes[i]->GetJointPos(0)).GetNormalize() * length;
            nodes[i]->GetJoint(1)->Move(next-nodes[i]->GetJointPos(1));
        }
        if(nodes[i]->is<CArc>()){
            Pos next1 = (nodes[i]->GetJointPos(0) - nodes[i]->GetJointPos(2)).GetNormalize() * length;
            Pos next2 = (nodes[i]->GetJointPos(1) - nodes[i]->GetJointPos(2)).GetNormalize() * length;
            nodes[i]->GetJoint(0)->Move(next1-nodes[i]->GetJointPos(0));
            nodes[i]->GetJoint(1)->Move(next2-nodes[i]->GetJointPos(1));
        }
    }
*/
    return true;
}
bool EqualRestraint::isComplete(){
    return true;
}
bool VerticalRestraint::Complete(){
    /*
    //矛盾な点が存在
    if(std::count_if(nodes.begin(),nodes.end(),[](CObject* obj){
        for(int i=0;i<obj->GetJointNum();i++){
            if(obj->GetJoint(i)->isLock())return true;
        }
        return false;
    })){
        double x = 0;
        for(CObject* obj:nodes){
            for(int i=0;i<obj->GetJointNum();i++){
                if(obj->GetJoint(i)->isLock()){
                    if(x == 0.0)x = obj->GetJointPos(i).x;
                    else{
                        if(x != obj->GetJointPos(i).x){
                            return false;
                        }
                    }
                }
            }
        }
    }
    int index = 0;
    QVector<CObject*>::iterator it = std::find_if(nodes.begin(),nodes.end(),[&](CObject* obj){
        for(int i=0;i<obj->GetJointNum();i++){
            if(obj->GetJoint(i)->isLock()){
                index = 0;
                return true;
            }
        }
        return false;
    });
    if(it != nodes.end()){
        std::swap(nodes[0],*it);
    }

    for(int i=0;i<nodes.size();i++){
        for(int j=0;j<nodes[i]->GetJointNum();j++){
            if(!(i==0 && j==index)){
                nodes[i]->GetJoint(j)->Move(Pos((*nodes[0]->GetJoint(index)).x - (*nodes[i]->GetJoint(j)).x,0));
            }
        }
    }*/
    return true;
}
bool VerticalRestraint::isComplete(){
    return true;
}

bool HorizontalRestraint::Complete(){
/*
    //矛盾な点が存在
    if(std::count_if(nodes.begin(),nodes.end(),[](CObject* obj){
        for(int i=0;i<obj->GetJointNum();i++){
            if(obj->GetJoint(i)->isLock())return true;
        }
        return false;
    })){
        double y = 0;
        for(CObject* obj:nodes){
            for(int i=0;i<obj->GetJointNum();i++){
                if(obj->GetJoint(i)->isLock()){
                    if(y == 0.0)y = obj->GetJointPos(i).y;
                    else{
                        if(y != obj->GetJointPos(i).y){
                            return false;
                        }
                    }
                }
            }
        }
    }
    int index = 0;
    QVector<CObject*>::iterator it = std::find_if(nodes.begin(),nodes.end(),[&](CObject* obj){
        for(int i=0;i<obj->GetJointNum();i++){
            if(obj->GetJoint(i)->isLock()){
                index = i;
                return true;
            }
        }
        return false;
    });
    if(it != nodes.end()){
        std::swap(nodes[0],*it);
    }

    for(int i=0;i<nodes.size();i++){
        for(int j=0;j<nodes[i]->GetJointNum();j++){
            if(!(i==0 && j==index))nodes[i]->GetJoint(j)->Move(Pos(0,nodes[0]->GetJoint(index)->y-nodes[i]->GetJoint(j)->y));
        }
    }*/
    return true;
}
bool HorizontalRestraint::isComplete(){
    return true;
}


bool MatchRestraint::Complete(){
    /*
    if(nodes[0]->isLock() && nodes[1]->isLock())return false;
    QVector<CObject*> cc = CObject::selected;

    if(nodes[1]->isLock() ||
       exist(CObject::selected,nodes[1])){
        std::swap(nodes[0],nodes[1]);
    }

    //0へ1を近づける
    for(int i=1;i<nodes.size();i++){
        Pos pp,near;
        CObject *pos,*line;

        //点を移動
        if(nodes[0]->is<CPoint>()){
            pos  = nodes[i];
            line = nodes[0];
            pp   = pos ->GetJointPos(0);
            near = line->GetNear(pos->GetJointPos(0));
            pos->Move((near-pp)-(near-pp).GetNormalize()*value);
        }
        //線を移動
        if(nodes[i]->is<CPoint>()){
            pos  = nodes[0];
            line = nodes[i];
            pp   = pos ->GetJointPos(0);
            near = line->GetNear(pos->GetJointPos(0));
            line->Move((pp-near)-(pp-near).GetNormalize()*value);
        }
    }*/
    return true;
}
bool MatchRestraint::isComplete(){
/*
    const float EPS = 0.0001;
    //点と線の距離
    float length = std::abs((this->nodes[0]->GetJointPos(0) - this->nodes[1]->GetJointPos(0)).Length() - this->value);
    if(length < EPS){
        return true;
    }else{
        return false;
    }
    */
}

bool MatchHRestraint::Complete(){
    /*
    if(nodes[0]->isLock() && nodes[1]->isLock())return false;
    if(nodes[1]->isLock() || exist(CObject::selected,nodes[1]))std::swap(nodes[0],nodes[1]);
    //0へ1を近づける(CPoint,CPoint)

    Pos near = nodes[0]->GetNear(nodes[1]->GetJointPos(0));
    Pos current_pos = nodes[1]->GetJointPos(0);
    Pos rot = (current_pos-near);
    rot.x = 0;
    rot = rot.GetNormalize();

    Pos next_pos = Pos(current_pos.x,  near.y + rot.y*value);
    if(!(current_pos == near)){
        nodes[1]->Move(next_pos - current_pos);
    }*/
    return true;
}
bool MatchHRestraint::isComplete(){
    return true;
}


bool MatchVRestraint::Complete(){/*
    if(nodes[0]->isLock() && nodes[1]->isLock())return false;
    if(nodes[1]->isLock() || exist(CObject::selected,nodes[1]))std::swap(nodes[0],nodes[1]);
    //0へ1を近づける(CPoint,CPoint)

    Pos near = nodes[0]->GetNear(nodes[1]->GetJointPos(0));
    Pos current_pos = nodes[1]->GetJointPos(0);
    Pos rot = (current_pos-near);
    rot.y = 0;
    rot = rot.GetNormalize();
    Pos next_pos = Pos(near.x + rot.x*value , current_pos.y);


    if(!(current_pos == near)){
        nodes[1]->Move(next_pos - current_pos);
    }*/
    return true;
}
bool MatchVRestraint::isComplete(){
    return true;
}

bool FixRestraint::Complete(){
    for(int i=0;i<nodes.size();i++){
        /*
         * for(int j=0;j<nodes[i]->GetJointNum();j++){
            nodes[i]->Lock(true);
        }*/
    }
    return true;
}
bool FixRestraint::isComplete(){
    return true;
}


bool ConcurrentRestraint::Complete(){/*
    Pos base_line[2] = {nodes[0]->GetJointPos(0),nodes[0]->GetJointPos(1)};
    //矛盾な点が存在
    if(nodes[0]->GetJoint(0)->isLock()&&
       nodes[0]->GetJoint(1)->isLock()&&
       nodes[1]->GetJoint(0)->isLock()&&
       nodes[1]->GetJoint(1)->isLock()){
            Pos dir1 = nodes[0]->GetJointPos(1)-nodes[0]->GetJointPos(0);
            Pos dir2 = nodes[0]->GetJointPos(1)-nodes[0]->GetJointPos(0);

            if(!Pos::DirComp(dir1.GetNormalize(),dir2.GetNormalize())){
                return false;
            }
    }
    //全てを並行に
    for(int i=1;i<nodes.size();i++){
        if(nodes[i]->is<CLine>()){
            Pos line_near1 = Pos::LineNearPoint(base_line[0],base_line[1],nodes[i]->GetJointPos(0));
            //Pos line_near2 = Pos::LineNearPoint(base_line[0],base_line[1],nodes[i]->GetJointPos(1));

            //double length = (line_near1 - nodes[i]->GetJointPos(0)).Length();
            //nodes[i]->GetJoint(0)->setDifferent(line_near1 + (nodes[i]->GetJointPos(0) - line_near1).GetNormalize()*length);
            if(Pos::MoreThan(base_line[0],base_line[1],nodes[i]->GetJointPos(0)) == Pos::MoreThan(base_line[0],base_line[1],nodes[i]->GetJointPos(1))){
                //nodes[i]->GetJoint(1)->setDifferent(line_near2 + (nodes[i]->GetJointPos(1) - line_near2).GetNormalize()*length);
            }else{
                //nodes[i]->GetJoint(1)->setDifferent(line_near2 - (nodes[i]->GetJointPos(1) - line_near2).GetNormalize()*length);
            }
        }
    }*/
    return true;
}
bool ConcurrentRestraint::isComplete(){
    return true;
}
bool CrossRestraint::Complete(){
/*
    //矛盾な点が存在
    if(nodes[0]->GetJoint(0)->isLock()&&
       nodes[0]->GetJoint(1)->isLock()&&
       nodes[1]->GetJoint(0)->isLock()&&
       nodes[1]->GetJoint(1)->isLock()){
            //Pos dir = nodes[0]->GetJointPos(1)-nodes[0]->GetJointPos(0);

            if((nodes[0]->GetJointPos(1)-nodes[0]->GetJointPos(0)).GetNormalize().Dot(nodes[1]->GetJointPos(1)-nodes[1]->GetJointPos(0)) != 0){
                return false;
            }
    }

    Pos base_line[2] = {nodes[0]->GetJointPos(0),nodes[0]->GetJointPos(1)};
    for(int i=1;i<nodes.size();i++){
        if(nodes[i]->is<CLine>()){
            Pos line_near1 = Pos::LineNearPoint(base_line[0],base_line[1],nodes[i]->GetJointPos(0));
            //Pos line_near2 = Pos::LineNearPoint(base_line[0],base_line[1],nodes[i]->GetJointPos(1));

            //double length = (line_near1 - nodes[i]->GetJointPos(0)).Length();
            //nodes[i]->GetJoint(0)->setDifferent(line_near1 + (nodes[i]->GetJointPos(0) - line_near1).GetNormalize()*length);
            if(Pos::MoreThan(base_line[0],base_line[1],nodes[i]->GetJointPos(0)) == Pos::MoreThan(base_line[0],base_line[1],nodes[i]->GetJointPos(1))){
                //nodes[i]->GetJoint(1)->setDifferent(line_near2 + (nodes[i]->GetJointPos(1) - line_near2).GetNormalize()*length);
            }else{
                //nodes[i]->GetJoint(1)->setDifferent(line_near2 - (nodes[i]->GetJointPos(1) - line_near2).GetNormalize()*length);
            }
        }
    }*/
    return true;
}
bool CrossRestraint::isComplete(){
    return true;
}


