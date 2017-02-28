#include "CadEditForm.h"
#include "ui_CadEditForm.h"

void CadEditForm::AddObject(CObject* obj){
    if(!exist(objects,obj)){
        objects.push_back(obj);
    }
    //CPoint→CLineの順で
    std::sort(objects.begin(),objects.end(),[](const CObject* lhs,const CObject* ){
        if(lhs->is<CPoint>())return true;
        else return false;
    });
}


void CadEditForm::RemoveObject(CObject* obj){
    if(obj == nullptr)return;

    //点ならば含むObjectを全て削除する。
    if(obj->is<CPoint>()){
        for(QVector<CObject*>::Iterator it = objects.begin();it != objects.end();it++){
            for(int i=0;i < (*it)->GetJointNum();i++){
                if((*it)->GetJointPos(i) == *dynamic_cast<CPoint*>(obj) && (*it) != obj){
                    *it = nullptr;
                    break;
                }
            }
        }
    }/*
    else{
        //点以外ならば端点の被参照数を確認して1なら消す

        //点を抽出
        QMap<Pos,int> map;//位置,個数
        for(QVector<CObject*>::Iterator it = objects.begin();it != objects.end();it++){
            if((*it)->is<CPoint>()){
                map[(*it)->GetJointPos(0)]++;//増える
            }
        }
        for(int i=0;i<obj->GetJointNum();i++){
            map.insert(obj->GetJointPos(i),1);
        }
    }*/

    //objを消す
    QVector<CObject*>::Iterator it = std::find(objects.begin(),objects.end(),obj);
    if(it != objects.end())objects.erase(it);

    //nullptrを消す
    it = std::find(objects.begin(),objects.end(),nullptr);
    while(it != objects.end()){
        delete *it;
        objects.erase(it);
        it = std::find(objects.begin(),objects.end(),nullptr);
    }
    repaint();
}

void CadEditForm::CompleteObject(CObject* make_obj){
    for(int i=0;i<make_obj->GetJointNum();i++){
        bool is_known_pos=false;
        //すでに点がなければ
        for(CObject* obj : objects){
            if(obj->is<CPoint>() && *dynamic_cast<CPoint*>(obj) == make_obj->GetJointPos(i)){
                is_known_pos=true;
                break;
            }
        }
        //追加
        if(!is_known_pos){
            CPoint* new_point = make_obj->GetJoint(i);
            new_point->Create(new_point,0);
        }
    }
}

double CadEditForm::GetScale()const{
    return scale;
}
Pos    CadEditForm::GetTranslate()const{
    return translate;
}


void CadEditForm::paintEvent(QPaintEvent*){
    QPainter paint(this);
    paint.fillRect(0,0,this->width(),this->height(),Qt::white); //背景を白塗りにする


    //状態を保存
    paint.save();

    //ペン設定
    paint.setPen(QPen(Qt::blue , (CObject::DRAWING_LINE_SIZE/2 / this->scale)));    //太さ設定

    //原点座標・マウス座標を描画
    paint.drawText(0,12,QString("(") + QString::number(this->translate.x   ) + "," + QString::number(this->translate.y   ) + ")");
    paint.drawText(0,24,QString("MousePosLocal(") + QString::number(CObject::mouse_pos.x) + "," + QString::number(CObject::mouse_pos.y) + ")");
    paint.drawText(0,36,QString("MousePosWorld(") + QString::number(this->ConvertWorldPos(CObject::mouse_pos).x) + "," + QString::number(this->ConvertWorldPos(CObject::mouse_pos).y) + ")");


    //変換行列を作成
    QTransform trans;
    trans.translate(-translate.x,-translate.y);
    trans.scale(scale,scale);
    paint.setTransform(trans); // 変換行列を以降の描画に適応

    //寸法を描画
    paint.setPen(QPen(Qt::blue, 1));
    for(SmartDimension* dim:dimensions){
        dim->Draw(paint);
    }

    //原点を描画
    paint.drawLine(-3,-3,+3,-3);
    paint.drawLine(+3,-3,+3,+3);
    paint.drawLine(+3,+3,-3,+3);
    paint.drawLine(-3,+3,-3,-3);

    //CBox描画
    paint.setBrush(QBrush(Qt::darkGray));                             //背景設定
    for(int i=0;i<this->blocks.size();i++){ //エリア描画
        this->blocks[i].Draw(paint,trans);
    }
    paint.setBrush(QBrush(Qt::white));      //ブラシ復元

    //普通のオブジェクト
    paint.setPen(QPen(Qt::blue, CObject::DRAWING_LINE_SIZE / this->scale));
    for(CObject* obj:objects){
        if(obj->Refresh())obj->Draw(paint);
    }
    //選択されたオブジェクト
    paint.setPen(QPen(Qt::cyan, CObject::DRAWING_LINE_SIZE / this->scale));
    for(CObject* obj:CObject::selected){
        if(obj->Refresh())obj->Draw(paint);
    }
    //メイン選択中
    paint.setPen(QPen(Qt::red , CObject::DRAWING_LINE_SIZE / this->scale));
    if(CObject::hanged!=nullptr){
        if(CObject::hanged->Refresh())CObject::hanged->Draw(paint);
    }

    //描画復元
    paint.restore();

    //競合確認
    bool is_conflict = false;
    for(Restraint* rest:this->restraints){
        if(rest->isComplete()==false){
            is_conflict = true;
        }
    }
    emit ToggleConflict(is_conflict);
}


void CadEditForm::mouseMoveEvent   (QMouseEvent* event){
    //マウス移動を監視
    CObject::mouse_pos = this->ConvertLocalPos(Pos(event->pos().x(),event->pos().y()));
/*    //平行移動量を適用
    CObject::mouse_pos += translate;
    //拡大移動量を適用
    CObject::mouse_pos /= scale;
*/
    //選択オブジェクトの選定
    CObject* answer = this->getHanged();

    //UI更新
    repaint();
    emit MovedMouse(event,answer);
}
void CadEditForm::resizeEvent(QResizeEvent* event){
    //原点を中心に
    this->translate.x = -this->width()  / 2;
    this->translate.y = -this->height() / 2;
}

Pos CadEditForm::ConvertLocalPos(Pos pos)const{
    QTransform trans;
    trans.translate(translate.x,translate.y);
    trans.scale(scale,scale);
    QPoint ans = trans.map(QPoint(pos.x,pos.y));
    return Pos(ans.x(),ans.y());
}
Pos CadEditForm::ConvertWorldPos(Pos pos)const{
    QTransform trans;
    trans.translate(translate.x,translate.y);
    trans.scale(scale,scale);
    trans = trans.inverted();
    QPoint ans = trans.map(QPoint(pos.x,pos.y));
    return Pos(ans.x(),ans.y());
}
void CadEditForm::Zoom(double scale,Pos local_piv){
    //qDebug() << (1 - (scale / this->scale));
    this->translate = this->translate + (this->translate - local_piv) * (1 - (scale / this->scale));
    this->scale = scale;
}


CadEditForm::CadEditForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CadEditForm)
{
    ui->setupUi(this);
    setMouseTracking(true);

}

CadEditForm::~CadEditForm()
{
    delete ui;
}

CObject* CadEditForm::getHanged(){
    for(CObject* obj:objects){
        if(obj->isSelectable() && !obj->isCreating()){
            return obj;
        }
    }
    return nullptr;
}

void CadEditForm::SetScale(double scale){
    this->scale = scale;
    repaint();
}

void CadEditForm::SetTranslate(Pos trans){
    this->translate = trans*this->scale;
    repaint();
}

void CadEditForm::MakeSmartDimension(){
    if(CObject::selected.size() > 0 && CObject::selected.size() < 3){
        //スマート寸法ダイアログ生成
        SmartDimensionDialog* diag = new SmartDimensionDialog(this);
        SmartDimension* dim = new SmartDimension();

        //ターゲット設定
        CObject* target[2];
        target[0] = CObject::selected[0];
        if(CObject::selected.size() == 1)target[1] = nullptr;
        else                             target[1] = CObject::selected[1];

        if(dim->SetTarget(target[0],target[1])){//寸法定義可能ならば

            //ダイアログ起動
            diag->SetValue(dim->currentValue());//ダイアログ初期値設定
            if(diag->exec()){
                //登録
                dim->SetValue(diag->GetValue());
                this->dimensions.push_back(dim);
                //スマート寸法の拘束も追加
                std::vector<Restraint*> rs = dim->MakeRestraint();
                for(Restraint* r : rs){
                    restraints.push_back(r);
                }
            }
        }
/*
        //XY軸成分指定
        if(CObject::selected.size()==2 && CObject::selected[0]->is<CPoint>() && CObject::selected[1]->is<CPoint>()){
            diag->UseRadioLayout(true);
            if(diag->exec()){
                double value = diag->GetValue();
                SmartDimension* dim = new SmartDimension();
                dim->SetXYType(diag->GetCurrentRadio()==2,diag->GetCurrentRadio()==1);
                if(dim->SetTarget(CObject::selected[0],CObject::selected[1])){
                    dim->SetValue(value);
                    this->dimensions.push_back(dim);
                }else{
                    delete dim;
                }
                for(SmartDimension* dim:dimensions){
                    Restraint* rs;
                    if(diag->GetCurrentRadio()==0)rs = new MatchRestraint();
                    if(diag->GetCurrentRadio()==1)rs = new MatchHRestraint();
                    if(diag->GetCurrentRadio()==2)rs = new MatchVRestraint();
                    rs->value = value;
                    rs->nodes.push_back(CObject::selected[0]);
                    rs->nodes.push_back(CObject::selected[1]);
                    restraints.push_back(rs);
                }

            }
        }else{
            //直線距離指定
            diag->UseRadioLayout(false);
            if(diag->exec()){
                double value = diag->GetValue();
                SmartDimension* dim = new SmartDimension();

                //有効寸法であれば
                CObject* sel[2];
                sel[0] = CObject::selected[0];
                if(CObject::selected.size()==1)sel[1] = nullptr;
                else                           sel[1] = CObject::selected[1];

                //ターゲット設定
                if(dim->SetTarget(sel[0],sel[1])){
                    dim->SetValue(value);
                    this->dimensions.push_back(dim);
                }else{
                    //ターゲット生成不可
                    delete dim;
                }
                //restraints.clear();
                std::vector<Restraint*> rs = dim->MakeRestraint();
                for(Restraint* r : rs){
                    restraints.push_back(r);
                }
            }
        }
        */
    }
    RefreshRestraints();
}

void CadEditForm::MakeRestraint(RestraintType type){
    Restraint* rest = nullptr;
    if(type == EQUAL)     rest = new EqualRestraint({CObject::selected[0],CObject::selected[1]});
    if(type == VERTICAL)  rest = new VerticalRestraint(CObject::selected);
    if(type == HORIZONTAL)rest = new HorizontalRestraint(CObject::selected);
    if(type == MATCH)     rest = new MatchRestraint({CObject::selected[0],CObject::selected[1]});
    if(type == CONCURRENT)rest = new ConcurrentRestraint({CObject::selected[0],CObject::selected[1]});
    //if(type == TANGENT)   rest = new TangentRestraint(CObject::selected[0],CObject::selected[1]);

    //固定
    if(type == LOCK | type == UNLOCK){
        for(CObject* obj : CObject::selected){
            obj->Lock(type == LOCK);
        }
    }

    if(rest != nullptr){
        restraints.push_back(rest);
        RefreshRestraints();
    }
}
bool CadEditForm::MakeBlock(){
    CBoxDefineDialog* diag = new CBoxDefineDialog();
    if(diag->exec()){
        CBlock block = diag->ExportCBlock();
        block.SetNodeAll(CObject::selected);
        this->blocks.push_back(block);
        CObject::selected.clear();
    }
}


void CadEditForm::RefreshRestraints(){
    //拘束を解決
    if(objects.size()!=0){
        //持ち手が存在すれば
        if(CObject::hanged != nullptr){
            std::vector<std::pair<int,CObject*>> rank;

            //過去の選択位置を保持する
            static CObject* hand = nullptr;

            //持ち手は0番に
            if(CObject::hanged != nullptr && CObject::hanged->is<CPoint>()){
                hand = CObject::hanged;
            }
            rank.push_back(std::make_pair(0,hand));
            //ランク分けダイクストラ
            int max_rank=0;
            std::queue<std::pair<int,CObject*>> queue;
            queue.push(std::make_pair(0,hand));//初期ノード
            while(!queue.empty()){
                //全ての拘束リストから
                for(Restraint* rest:restraints){
                    QVector<CObject*> child = rest->nodes;
                    //childにqueue.frontが含まれる
                    if(exist(child,queue.front().second)){
                        //未探索ならばを次の探索点に追加
                        for(int i=0;i<child.size();i++){
                            bool not_alive = true;
                            for(int j=0;j<rank.size();j++){
                                if(child[i] == rank[j].second){
                                    not_alive = false;
                                }
                            }
                            if(not_alive==true){
                                queue.push    (std::make_pair(queue.front().first+1,child[i]));
                                rank.push_back(std::make_pair(queue.front().first+1,child[i]));
                                max_rank = queue.front().first+1;
                            }
                        }
                    }
                }
                queue.pop();
            }

            std::vector<std::pair<int,Restraint*>> solver;
            if(max_rank > 0){
                //拘束を捜査
                for(Restraint* rest:restraints){
                    if(rest->nodes.size()==2){
                        std::vector<std::pair<int,CObject*>>::iterator it1,it2;
                        int score1,score2,current;

                        //拘束ランクごとに拘束の親関係を修正
                        score1 = score2 = -1;
                        it1 = std::find_if(rank.begin(),rank.end(),[&](std::pair<int,CObject*>obj){
                            return (obj.second == rest->nodes[0]);
                        });
                        it2 = std::find_if(rank.begin(),rank.end(),[&](std::pair<int,CObject*>obj){
                            return (obj.second == rest->nodes[1]);
                        });
                        if(it1 != rank.end())score1 = it1->first;
                        if(it2 != rank.end())score2 = it2->first;
                        if(score1 > score2){
                            std::swap(rest->nodes[0],rest->nodes[1]);
                        }

                        //解決順序を決定
                        current = std::min(score1,score2);

                        //解決順と拘束を組み合わせて保存
                        solver.push_back(std::make_pair(current,rest));
                    }else{
                        //解決順と拘束を組み合わせて保存
                        solver.push_back(std::make_pair(0,rest));
                    }
                }
            }
            //解決順が小さい順にソート
            std::sort(solver.begin(),solver.end());


            //拘束を解決
            for(std::pair<int,Restraint *> rest:solver){
                if(!rest.second->Complete()){
                    qDebug() << "CONFLICT" ;
                }
            }
        }else{
            //持ち手が存在しなければ
            for(Restraint* rest:restraints){
                if(!rest->Complete()){
                    qDebug() << "CONFLICT" ;
                }
            }
       }
    }
}


void CadEditForm::ApplyObjectList(QListWidget* list){
    //ポインタを保持していないため、添字でカウント
    CObject::selected.clear();
    QMap<QString,int>map;
    for(int i=0;i<list->count();i++){
        QListWidgetItem* item = list->item(i);

        //カウント
        if(map.find(item->text()) != map.end())map[item->text()]++;
        else map.insert(item->text(),1);


        if(item->isSelected()){
            //CObject::selected内をループ
            int count = 0;
            int j;
            for(j = 0;count < map[item->text()];j++){
                if(this->objects[j]->is<CPoint >() && item->text()=="CPoint" )count++;
                if(this->objects[j]->is<CLine  >() && item->text()=="CLine"  )count++;
                if(this->objects[j]->is<CRect  >() && item->text()=="CRect"  )count++;
                if(this->objects[j]->is<CArc   >() && item->text()=="CArc"  )count++;
                if(this->objects[j]->is<CSpline>() && item->text()=="CSpline")count++;
            }
            CObject::selected.push_back(this->objects[j-1]);
        }
    }
}
void CadEditForm::DrawObjectList(QListWidget* list){
    bool clear = false;
    if(list->count() != this->objects.size()){
        clear=true;
        list->clear();
    }
    for(int i=0;i<this->objects.size();i++){
        if(clear){
            std::pair<std::string,std::string> p;
            if(objects[i]->is<CPoint> ())p = std::make_pair("CPoint" ,":/ToolImages/Dot.png");
            if(objects[i]->is<CLine>  ())p = std::make_pair("CLine"  ,":/ToolImages/Line.png");
            if(objects[i]->is<CRect>  ())p = std::make_pair("CRect"  ,":/ToolImages/Rect.png");
            if(objects[i]->is<CArc>   ())p = std::make_pair("CArc"   ,":/ToolImages/Arc.png");
            if(objects[i]->is<CSpline>())p = std::make_pair("CSpline",":/ToolImages/Spline.png");
            list->addItem(new QListWidgetItem(p.first.c_str()));
            list->item(list->count()-1)->setIcon(QIcon(p.second.c_str()));
        }
        list->item(i)->setSelected(exist(CObject::selected,objects[i]));
    }
}
void CadEditForm::ApplyCBoxList  (QListWidget *list){
    this->selecting_block.clear();
    for(int i=0;i<list->count();i++){
        //選択していればリストに追加
        if(list->item(i)->isSelected()){
            this->selecting_block.push_back(&this->blocks[i]);
        }
    }
}
void CadEditForm::DrawCBoxList   (QListWidget *list){
    bool clear = false;
    if(list->count() != this->blocks.size()){
        clear=true;
        list->clear();
    }
    for(int i=0;i<this->blocks.size();i++){
        if(clear){
            list->addItem(new QListWidgetItem("CBox"));
            list->item(list->count()-1)->setIcon(QIcon(":/ToolImages/Blocks.png"));
            list->item(list->count()-1)->setSelected(exist(selecting_block,&blocks[i]));
        }
        list->item(i)->setSelected(exist(this->selecting_block,&blocks[i]));
    }
}
void CadEditForm::ConfigureBlock(QListWidgetItem*){
    CBoxDefineDialog* diag = new CBoxDefineDialog(this);
    diag->ImportCBlock(*this->selecting_block[0]);
    if(diag->exec()){
        QVector<CObject*> ll;
        for(int i =0;i<4;i++)ll.push_back(this->selecting_block[0]->GetNode(i));
        *this->selecting_block[0] = diag->ExportCBlock();
        this->selecting_block[0]->SetNodeAll(ll);
    }
}

