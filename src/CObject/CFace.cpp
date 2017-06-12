#include "CFace.h"

bool CFace::Creatable(QVector<CObject*> lines){
    if(std::any_of(lines.begin(),lines.end(),[](CObject* p){return !p->is<CEdge>();}))return false;
    if(lines.size() < 3)return false;

    //閉じた図形テスト
    std::map<CPoint*,int> point_maps;
    QVector<CPoint*> points;
    for(CObject* line: lines){
        for(int i=0;i<dynamic_cast<CEdge*>(line)->GetPosSequenceCount();i++){
            point_maps[dynamic_cast<CEdge*>(line)->GetPosSequence(i)]++;
            points.push_back(dynamic_cast<CEdge*>(line)->GetPosSequence(i));
        }
    }
    //全ての点の数が2である
    if(!std::all_of(point_maps.begin(),point_maps.end(),[](std::pair<CObject*,int> c){return c.second==2;}))return false;

    //重複削除
    std::sort(points.begin(),points.end());
    points.erase(std::unique(points.begin(),points.end()),points.end());

    //同一平面テスト
    Pos cross = (*points[1] - *points[0]).Cross
                (*points[2] - *points[0]);
    for(int i=3;i<point_maps.size();i++){
        double d = (*points[i] - *points[0]).DotPos(cross);
        if(!NearlyEqual(d,0)){
            return false;
        }
    }
    return true;
}

bool CFace::isParadox()const{
    if(this->edges.size() < 3)return false;
    else {
        //矛盾がないか確認する。
        for(int i=2;i<this->GetPoint().size();i++){
            if(this->isComprehension(*this->GetPoint()[i]) == false)return false;
        }
    }
    return true;
}

bool CFace::isComprehension(Pos pos)const{
    if(this->edges.size()<3)return true;
    else {
        //法線ベクトルとの内積が0であれば平面上に存在する。
        return this->GetNorm().DotPos(pos) == 0;
    }
}

Pos CFace::GetNorm()const{
    if(this->edges.size()<2)return Pos();
    else{
        return Pos((*this->GetPoint()[                   1] - *this->GetPoint()[0]).Cross
                   (*this->GetPoint()[this->edges.size()-1] - *this->GetPoint()[0])).GetNormalize();
    }
}
QVector<CPoint*> CFace::GetPoint(){
    //点に変換
    QVector<CPoint*> points;
    for(CEdge* line : this->edges){
        for(int i=0;i<line->GetPosSequenceCount();i++){
            points.push_back(line->GetPosSequence(i));
        }
    }
    std::sort(points.begin(),points.end());
    points.erase(std::unique(points.begin(),points.end()),points.end());
    return points;
}
QVector<CPoint*> CFace::GetPoint()const{
    //点に変換
    QVector<CPoint*> points;
    for(CEdge* line : this->edges){
        for(int i=0;i<line->GetPosSequenceCount();i++){
            points.push_back(line->GetPosSequence(i));
        }
    }
    std::sort(points.begin(),points.end());
    points.erase(std::unique(points.begin(),points.end()),points.end());
    return points;
}


bool CFace::Draw(QPainter& painter)const{
    return true;
}
bool CFace::DrawGL(Pos,Pos)const{
    if(this->isPolygon()){
        //薄い色に変更
        float currentColor[4];
        glGetFloatv(GL_CURRENT_COLOR,currentColor);
        glColor4f(currentColor[0],currentColor[1],currentColor[2], 0.1);

        //中を塗る
        glBegin(GL_TRIANGLE_FAN);

        for(CPoint* c : this->GetPoint()){
            glVertex3f(c->x(),c->y(), c->z());
        }
        glEnd();

        //色を復元
        glColor4f(currentColor[0],currentColor[1],currentColor[2], currentColor[3]);
    }

    //外側
    glBegin(GL_LINE_LOOP);
    for(CPoint* c : this->GetPoint()){
        glVertex3f(c->x(),c->y(), c->z());
    }
    glEnd();

    return true;
}

bool CFace::Move(const Pos& diff){
    return true;
}


//近接点
Pos CFace::GetNearPos (const Pos& hand)const{
    return Pos();
}
Pos CFace::GetNearLine(const Pos& pos1,const Pos& pos2)const{
    return Pos();
}



CFace::CFace(QObject* parent):
    CObject(parent)
{
}

CFace::~CFace()
{
}

