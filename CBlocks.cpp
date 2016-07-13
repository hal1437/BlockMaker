#include "CBlocks.h"

bool CBlocks::Creatable(std::vector<CObject*> values){
    std::map<Pos,int> map;
    if(std::count_if(values.begin(),values.end(),[](CObject* p){return !p->is<CPoint>();}) == 4){
        //カウント
        for(CObject* p:values){
            for(int i=0;i<p->GetJointNum();i++){
                map[p->GetJointPos(i)]++;
            }
        }
        //合計が2であれば
        return std::all_of(map.begin(),map.end(),[](std::pair<Pos,int> v){
            return v.second == 2;
        });
    }
    return false;
}

void CBlocks::SetNode(std::vector<CObject*> lines){
    this->lines = lines;
}

Pos CBlocks::GetPos(int index)const{
    if(0 <= index &&  index <= 3){
        //32
        //01
        Pos pp[4];


    }else if(4 <= index &&  index <= 7){


    }else  Pos();
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

