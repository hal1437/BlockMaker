#include "CadModelSearch.h"

CPoint* CadModelSearch::GetUnionPoint(CEdge* e1,CEdge* e2){
    if(e1->start == e2->start)return e1->start;
    if(e1->start == e2->end  )return e1->start;
    if(e1->end   == e2->start)return e1->end;
    if(e1->end   == e2->end  )return e1->end;
    return nullptr;
}
bool CadModelSearch::isContinuity(CEdge* e1,CEdge* e2){
    return GetUnionPoint(e1,e2) != nullptr;
}

//オブジェクト生成補完関数
QVector<CEdge*> CadModelSearch::SearchEdgeMakeFace (QVector<CEdge*> select){
    QVector<QVector<CEdge*>> ans;
    //対象外
    if(select.size() <= 1)return QVector<CEdge*>();
    if(select.size() >= 4){
        QVector<CObject*> objects;
        //CObjectに変換
        for(CEdge* edge:select)objects.push_back(edge);
        if(CFace::Creatable(objects))return select;
        else return QVector<CEdge*>();
    }

    //選択されたオブジェクトは排除
    QVector<CEdge*> edges = this->model->GetEdges();
    for(CEdge* edge:select){
        edges.removeAll(edge);
    }

    if(select.size() == 2){
        CPoint* union_pos = GetUnionPoint(select.first(),select.last());//共有点
        if(union_pos == nullptr){
            //2線は非連続
            //組み合わせ探索
            for(int i=0;i<edges.size();i++){
                //両方の辺に連続な線の探索
                if(!isContinuity(edges[i],select.first()) ||
                   !isContinuity(edges[i],select.last ()))continue;
                for(int j=i+1;j<edges.size();j++){
                    if(isContinuity(edges[j],select.first()) &&
                       isContinuity(edges[j],select.last() ) &&
                       !isContinuity(edges[i],edges[j])){
                        //適合パターン検出
                        ans.push_back({select.first(),select.last(),edges[i],edges[j]});
                    }
                }
            }
        }else{
            //2線は連続
            //三角形
            for(int i=0;i<edges.size();i++){
                if(isContinuity(edges[i],select.first()) &&
                   isContinuity(edges[i],select.last())){
                    //適合パターン検出
                    ans.push_back({select.first(),select.last(),edges[i]});
                }
            }
            //四角形
            for(int i=0;i<edges.size();i++){
                //両方の辺に連続な線の探索
                if((!isContinuity(edges[i],select.first()) && !isContinuity(edges[i],select.last ())) ||
                   ( isContinuity(edges[i],select.first()) &&  isContinuity(edges[i],select.last ())))continue;
                for(int j=i+1;j<edges.size();j++){
                    if(isContinuity(edges[j],edges[i]) &&
                      ((isContinuity(edges[j],select.first ()) && isContinuity(edges[i],select.last ())) ||
                       (isContinuity(edges[i],select.first ()) && isContinuity(edges[j],select.last ())))){
                        //適合パターン検出
                        ans.push_back({select.first(),select.last(),edges[i],edges[j]});
                    }
                }
            }
        }
    }else{
        //3線
        CEdge *e1,*e3;//e1とe3は非連続
        e1 = e3 = nullptr;
        for(int i =0;i<3;i++){
            for(int j =i+1;j<3;j++){
                if(!this->isContinuity(select[i],select[j])){
                    if(e1 == nullptr){
                        e1 = select[i];
                        e3 = select[j];
                    }else{
                        //二回目以降は失敗
                        return QVector<CEdge*>();
                    }
                }
            }
        }
        if(e1 == nullptr)return QVector<CEdge*>();

        //のこり1辺を探す
        for(int i=0;i<edges.size();i++){
            if(isContinuity(edges[i],e1) &&
               isContinuity(edges[i],e3)){
                //適合パターン検出
                ans.push_back({select[0],select[1],select[2],edges[i]});
            }
        }
    }
    if(ans.size() == 1)return ans.first();
    else return QVector<CEdge*>();
}
QVector<CEdge*> CadModelSearch::SearchFaceMakeBlock(QVector<CEdge*> select){
    return QVector<CEdge*>();
}


CadModelSearch::CadModelSearch()
{

}
