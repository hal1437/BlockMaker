#include "Restraint.h"

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

