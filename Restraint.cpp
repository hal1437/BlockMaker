#include "Restraint.h"


std::vector<RestraintType> Restraint::Restraintable(const std::vector<CObject *> &values){
    std::vector<RestraintType> answer;
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
    if(values.size()==1){
        answer.push_back(FIX);//固定
        if(values[0]->is<CLine>()){
            answer.push_back(VERTICAL);//垂直
            answer.push_back(HORIZONTAL);//水平
        }
    }

    return answer;
}


EqualRestraint::EqualRestraint(){
    type = EQUAL;
}
VerticalRestraint::VerticalRestraint(){
    type = VERTICAL;
}
HorizontalRestraint::HorizontalRestraint(){
    type = HORIZONTAL;
}
MatchRestraint::MatchRestraint(){
    type = MATCH;
}
ConcurrentRestraint::ConcurrentRestraint(){
    type = CONCURRENT;
}
bool EqualRestraint::Complete(){

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

    return true;
}
bool VerticalRestraint::Complete(){


    for(int i=0;i<nodes.size();i++){
        for(int j=0;j<nodes[i]->GetJointNum();j++){
            if(!(i==0 && j==0))nodes[i]->GetJoint(j)->Move(Pos((*nodes[0]->GetJoint(0))().x - (*nodes[i]->GetJoint(j))().x,0));
        }
    }
    return true;
}
bool HorizontalRestraint::Complete(){

    for(int i=0;i<nodes.size();i++){
        for(int j=0;j<nodes[i]->GetJointNum();j++){
            if(!(i==0 && j==0))nodes[i]->GetJoint(j)->Move(Pos(0,(*nodes[0]->GetJoint(0))().y-(*nodes[i]->GetJoint(j))().y));
        }
    }

    return true;
}
bool MatchRestraint::Complete(){

    std::sort(nodes.begin(),nodes.end(),[](CObject* lhs,CObject* rhs){
        return lhs->GetJointNum() > rhs->GetJointNum();
    });

    for(int i=1;i<nodes.size();i++){
        CObject* ptr = nodes[i];
        Pos near = nodes[0]->GetNear(ptr->GetJointPos(0));
        Pos current_pos = ptr->GetJointPos(0);
        Pos next_pos = (current_pos - near).GetNormalize() * value + near;
        if(!(current_pos == near)){
            ptr->Move(next_pos - current_pos);
        }
    }
    return true;
}
bool ConcurrentRestraint::Complete(){

    if(exist(this->nodes,CObject::selecting)){
        std::swap(*nodes.begin(),*std::find(nodes.begin(),nodes.end(),CObject::selecting));
    }else{
        std::swap(*nodes.begin(),*std::find_if(nodes.begin(),nodes.end(),[](CObject* obj){return obj->is<CLine>();}));
    }
    Relative<Pos> line_pos[2] = {nodes[0]->GetJointPos(0),nodes[0]->GetJointPos(1)};

    Pos base = (line_pos[1]()-line_pos[0]());
    QPointF diff_ = QPointF(base.x,base.y)*QTransform().rotate(90);
    Pos diff = Pos(diff_.x(),diff_.y()).GetNormalize();
    //全てを並行に
    for(int i=1;i<nodes.size();i++){
        if(nodes[i]->is<CLine>()){
            Pos line_near1 = Pos::LineNearPoint(line_pos[0]()+diff*value,line_pos[1]()+diff*value,nodes[i]->GetJointPos(0));
            Pos line_near2 = Pos::LineNearPoint(line_pos[0]()+diff*value,line_pos[1]()+diff*value,nodes[i]->GetJointPos(1));

            nodes[i]->GetJoint(0)->setDifferent(line_near1);
            nodes[i]->GetJoint(0)->setDifferent(line_near2);
        }
    }
    return true;
}

