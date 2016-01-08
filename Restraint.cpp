#include "Restraint.h"

bool MatchRestraint::Complete(){

    //選択中を優先的に
    if(exist(this->nodes,CObject::selecting)){
        std::swap(*nodes.begin(),*std::find(nodes.begin(),nodes.end(),CObject::selecting));
    }

    for(int i=0;i<nodes.size();i++){
        CObject* ptr = nodes[i];
        Pos near = nodes[i]->GetNear(ptr->GetJointPos(0));
        ptr->Move((near-ptr->GetJointPos(0)).GetNormalize() * value - ptr->GetJointPos(0));
    }
    return true;
}
bool ConcurrentRestraint::Complete(){

    if(exist(this->nodes,CObject::selecting)){
        std::swap(*nodes.begin(),*std::find(nodes.begin(),nodes.end(),CObject::selecting));
    }else{
        std::swap(*nodes.begin(),*std::find(nodes.begin(),nodes.end(),[](CObject* obj){return obj->is<CLine>();}));
    }
    Relative<Pos> line_pos[2] = {nodes[0]->GetJointPos(0),nodes[0]->GetJointPos(1)};

    Pos base = (line_pos[1]()-line_pos[0]());
    QPointF diff_ = QPointF(base.x,base.y)*QTransform().rotate(90);
    Pos diff = Pos(diff_.x(),diff_.y()).GetNormalize();
    //全てを並行に
    for(int i=1;i<nodes.size();i++){
        if(nodes[i]->is<CLine>()){
            Pos line2_near1 = Pos::LineNearPoint(line_pos[0]()+diff*value,line_pos[1]()+diff*value,nodes[i]->GetJointPos(0)());
            Pos line2_near2 = Pos::LineNearPoint(line_pos[0]()+diff*value,line_pos[1]()+diff*value,nodes[i]->GetJointPos(1)());

            line2_pos[0] = line2_near1;
            line2_pos[1] = line2_near2;
        }
    }
    return true;
}

