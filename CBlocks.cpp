#include "CBlocks.h"

bool CBlocks::Creatable(std::vector<CObject*> values){
    //まず点以外が4つ
    if(std::count_if(values.begin(),values.end(),[](CObject* p){return !p->is<CPoint>();}) == 4){
        //構成点カウント
        std::map<Pos,int> map;
        for(CObject* p:values){
            map[p->GetJointPos(0)]++;
            map[p->GetJointPos(p->GetJointNum()-1)]++;
        }
        //構成点合計が2であれば
        return std::all_of(map.begin(),map.end(),[](std::pair<Pos,int> v){
            return v.second == 2;
        });
    }
    return false;
}

void CBlocks::SetNodeAll(std::vector<CObject*> lines){
    this->lines = lines;
}
void CBlocks::SetNode(int index,CObject* line){
    this->lines[index] = line;
}

CObject* CBlocks::GetNode(int index)const{
    return this->lines[index];
}



CBlocks::CBlocks()
{
}
CBlocks::CBlocks(std::vector<CObject*> lines):
    lines(lines)
{
}

CBlocks::~CBlocks()
{

}

