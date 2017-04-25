#include "CadEditForm.h"
#include "ui_CadEditForm.h"

void CadEditForm::mousePressEvent  (QMouseEvent*){
    release_flag = false;
    if(state==Edit)move_flag = true;
    MakeObject();
}

void CadEditForm::mouseReleaseEvent(QMouseEvent*){
    //編集
    move_flag = false;
    if(release_flag==true)MakeObject();
}

void CadEditForm::keyPressEvent(QKeyEvent* event){
    shift_pressed = event->modifiers() & Qt::ShiftModifier;
    ctrl_pressed  = event->modifiers() & Qt::ControlModifier;
}
void CadEditForm::keyReleaseEvent(QKeyEvent* event){
    shift_pressed = event->modifiers() & Qt::ShiftModifier;
    ctrl_pressed  = event->modifiers() & Qt::ControlModifier;
}

void CadEditForm::wheelEvent(QWheelEvent * event){
    //拡大
    double delta = (event->angleDelta().y())/MOUSE_ZOOM_RATE;//差分値
    double next_scale = std::exp(std::log(this->GetScale()) + delta);//次の拡大値

    //拡大値は負にならない
    if(next_scale > 0){
        //適応
        if(CObject::hanged == nullptr)this->Zoom(next_scale,CObject::mouse_pos); //マウス座標中心に拡大
        else                          this->Zoom(next_scale,CObject::hanged->GetNear(CObject::mouse_pos));//選択点があればそれを中心に拡大

        //シグナル発生
        emit ScaleChanged(next_scale);
        CObject::drawing_scale = next_scale;
    }
    repaint();
}
void CadEditForm::mouseMoveEvent   (QMouseEvent* event){
    //マウス移動を監視
    CObject::mouse_pos = ConvertLocalPos(Pos(event->pos().x(),event->pos().y()));
    //選択オブジェクトの選定
    CObject* answer = this->getHanged();
    //ズーム支点リセット
    zoom_piv = Pos(0,0);
    //生成点座標を更新
    if(hang_point != nullptr){
        hang_point->Move(CObject::mouse_pos-*hang_point);
    }
    //UI更新
    repaint();
    emit MovedMouse(event,answer);
}
void CadEditForm::resizeEvent(QResizeEvent*){
    //原点を中心に来るように上書き
    this->translate.x = -this->width()  / 2;
    this->translate.y = -this->height() / 2;
}

void CadEditForm::Escape(){
    //作成解除
    if(CObject::creating != nullptr){
        if(make_result == TWOSHOT){
            this->RemoveObject(make_obj->end);
            this->RemoveObject(make_obj);
        }
        this->hang_point = nullptr;
        this->make_result = COMPLETE;
    }
}

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
    //objを消す
    QVector<CObject*>::Iterator it = std::find(objects.begin(),objects.end(),obj);
    if(it != objects.end()){
        objects.erase(it);
    }

    //点ならばその点を含むObjectを全てnullptrにする。
    if(obj->is<CPoint>()){
        for(CObject*& it : objects){
            if(it->is<CEdge>() && it != obj){
                if(dynamic_cast<CEdge*>(it)->start == obj ||
                   dynamic_cast<CEdge*>(it)->end   == obj ){
                    it = nullptr;
                    break;
                }
            }
        }
    }

    //nullptrを消す
    it = std::find(objects.begin(),objects.end(),nullptr);
    while(it != objects.end()){
        objects.erase(it);
        it = std::find(objects.begin(),objects.end(),nullptr);
    }
    repaint();
}

void CadEditForm::CompleteObject(CObject*){

}

double CadEditForm::GetScale()const{
    return scale;
}
Pos    CadEditForm::GetTranslate()const{
    return translate;
}

void CadEditForm::MovedMouse(QMouseEvent *event, CObject *under_object){

    static Pos past;
    static Pos piv; //画面移動支点
    const  Pos null_pos = Pos(std::numeric_limits<double>::lowest(),std::numeric_limits<double>::lowest()); //無効な点

    //選択
    if(!(event->buttons() & Qt::LeftButton) || !move_flag){
        CObject::hanged = under_object;
        piv = null_pos;//移動支点を解除
    }
    //画面移動
    if((event->buttons() & Qt::LeftButton) && CObject::hanged == nullptr && this->state == Edit){
        //支点登録
        if(piv == null_pos){
            piv = this->ConvertWorldPos(CObject::mouse_pos);
        }
        Pos hand = this->ConvertWorldPos(CObject::mouse_pos);
        Pos diff = (piv - hand);

        this->SetTranslate(this->GetTranslate() + diff);
        piv = hand;
    }

    //編集
    if(move_flag == true){
        for(CObject* p : CObject::selected){
            p->Move(CObject::mouse_pos - past);
        }
    }

    //拘束更新
    this->RefreshRestraints();

    emit MouseMoved(CObject::mouse_pos);

    past = CObject::mouse_pos;
    release_flag=true;
}

void CadEditForm::paintEvent(QPaintEvent*){
    QPainter paint(this);
    paint.setRenderHint(QPainter::Antialiasing, true);//アンチエイリアスセット
    paint.fillRect(0,0,this->width(),this->height(),Qt::white); //背景を白塗りにする

    //状態を保存
    paint.save();

    //変換行列を作成
    QTransform trans;
    trans.translate(-translate.x,-translate.y);
    trans.scale(scale,-scale);
    paint.setTransform(trans); // 変換行列を以降の描画に適応

    //CBox描画
    paint.setPen(QPen(Qt::darkGray, CObject::DRAWING_LINE_SIZE/2 / this->scale,Qt::SolidLine,Qt::RoundCap));
    paint.setBrush(QBrush(Qt::lightGray));   //背景設定
    for(int i=0;i<this->blocks.size();i++){ //エリア描画
        this->blocks[i].Draw(paint);
    }

    //寸法を描画
    paint.setPen(QPen(Qt::blue, 1,Qt::SolidLine,Qt::RoundCap));
    for(SmartDimension* dim:dimensions){
        dim->Draw(paint);
    }
/*
    //原点を描画
    paint.drawLine(-5,-5,+5,-5);
    paint.drawLine(+5,-5,+5,+5);
    paint.drawLine(+5,+5,-5,+5);
    paint.drawLine(-5,+5,-5,-5);
*/
    //普通のオブジェクト
    paint.setPen(QPen(Qt::blue, CObject::DRAWING_LINE_SIZE / this->scale,Qt::SolidLine,Qt::RoundCap));
    for(CObject* obj:objects){
        obj->Draw(paint);
    }
    //選択されたオブジェクト
    paint.setPen(QPen(Qt::cyan, CObject::DRAWING_LINE_SIZE / this->scale,Qt::SolidLine,Qt::RoundCap));
    for(CObject* obj:CObject::selected){
        obj->Draw(paint);
    }
    //メイン選択中
    paint.setPen(QPen(Qt::red , CObject::DRAWING_LINE_SIZE / this->scale,Qt::SolidLine,Qt::RoundCap));
    if(CObject::hanged!=nullptr){
        CObject::hanged->Draw(paint);
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



Pos CadEditForm::ConvertLocalPos(Pos pos)const{
    QTransform trans;
    trans.translate(-translate.x,-translate.y);
    trans.scale(scale,-scale);
    trans = trans.inverted();//逆行列化
    QPointF ans = trans.map(QPointF(pos.x,pos.y));
    return Pos(ans.x(),ans.y());
}
Pos CadEditForm::ConvertWorldPos(Pos pos)const{
    QTransform trans;
    trans.translate(-translate.x,-translate.y);
    trans.scale(scale,-scale);//逆行列化
    QPointF ans = trans.map(QPointF(pos.x,pos.y));
    return Pos(ans.x(),ans.y());
}


void CadEditForm::Zoom(double scale,Pos local_piv){
    //ズーム支点保存
    if(zoom_piv == Pos(0,0)){
          zoom_piv = this->ConvertWorldPos(local_piv);
    }
    //ズーム適用
    this->translate += (zoom_piv + this->translate) * ((scale / this->scale) - 1);
    this->scale = scale;
    //マウス座標復元
    CObject::mouse_pos = this->ConvertLocalPos(zoom_piv);
}
void CadEditForm::Move(Pos local_diff){
    this->translate += local_diff;
}

void CadEditForm::MergePoints(){
    //原点が存在する場合は先頭に出す
    for(int i=0;i<CObject::selected.size();i++){
        if(dynamic_cast<CPoint*>(CObject::selected[i])->isControlPoint()){
            std::swap(CObject::selected[0],CObject::selected[i]);
            break;
        }
    }

    //先頭以外の点を破棄し、統合する。
    for(int i=1;i<CObject::selected.size();i++){
        for(CObject* p : this->objects){
            if(p->is<CEdge>()){
                if(dynamic_cast<CEdge*>(p)->start == CObject::selected[i]){
                    dynamic_cast<CEdge*>(p)->SetStartPos(dynamic_cast<CPoint*>(CObject::selected[0]));
                }
                if(dynamic_cast<CEdge*>(p)->end == CObject::selected[i]){
                    dynamic_cast<CEdge*>(p)->SetEndPos(dynamic_cast<CPoint*>(CObject::selected[0]));
                }
                for(int j=0;j<dynamic_cast<CEdge*>(p)->GetMiddleCount();j++){
                    if(dynamic_cast<CEdge*>(p)->GetMiddle(j) == CObject::selected[i]){
                        dynamic_cast<CEdge*>(p)->SetMiddle(dynamic_cast<CPoint*>(CObject::selected[0]),j);
                    }
                }
            }
        }
        this->objects.removeAll(CObject::selected[i]);
    }
    CObject::selected.clear();
    repaint();
}


CadEditForm::CadEditForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CadEditForm)
{
    ui->setupUi(this);

    //マウストラッキング対象
    setMouseTracking(true);

    //原点追加
    origin = new CPoint();
    origin->ControlPoint(true);
    this->AddObject(origin);
}

CadEditForm::~CadEditForm()
{
    delete ui;
}

CObject* CadEditForm::getHanged(){
    for(CObject* obj:objects){
        if(obj->isSelectable(CObject::mouse_pos) && obj != this->hang_point){
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
    this->translate = trans;
    repaint();
}
void CadEditForm::MakeObject(){

    Pos local_pos = CObject::mouse_pos;
    if(CObject::hanged != nullptr)local_pos = CObject::hanged->GetNear(local_pos);

    release_flag=false;

    //編集
    if(state == Edit){
        //シフト状態
        if(!shift_pressed)CObject::selected.clear();

        //選択状態をトグル
        if(exist(CObject::selected,CObject::hanged))erase(CObject::selected,CObject::hanged);
        else if(CObject::hanged != nullptr)CObject::selected.push_back(CObject::hanged);

    }else{
        //新規オブジェクト
        if(make_result == COMPLETE){
            //新規作成CEdge
            CObject* new_obj;
            if(state == Dot   )new_obj = new CPoint(this);
            if(state == Line  )new_obj = new CLine(this);
            if(state == Arc   )new_obj = new CArc(this);
            if(state == Spline)new_obj = new CSpline(this);
            if(state == Rect  ){
                new_obj = new CLine(this);
            }

            //追加
            CObject::creating = new_obj;

            //新規作成
            this->AddObject(new_obj);
            make_result = MakeJoint(new_obj);
            //選択解除
            CObject::hanged = nullptr;

            //一回だけ
            if(make_result == COMPLETE){
                //おわり
            }
            //二回必要
            if(make_result == TWOSHOT){
                //保持
                make_obj = dynamic_cast<CEdge*>(new_obj);
                //同一の点を作成
                MakeJoint(make_obj);

                //端点をオブジェクトリストに追加
                this->AddObject(make_obj->start);
                this->AddObject(make_obj->end);
                //中継点をオブジェクトリストに追加
                for(int i =0;i<make_obj->GetMiddleCount();i++){
                    this->AddObject(make_obj->GetMiddle(i));
                }
                //終端を持つ
                this->hang_point = make_obj->end;
            }
            //無限回指定
            if(make_result == ENDLESS){
                //保持
                make_obj = dynamic_cast<CEdge*>(new_obj);
                //同一の点を作成
                MakeJoint(make_obj);
                //端点をオブジェクトリストに追加
                this->AddObject(make_obj->start);
                this->AddObject(make_obj->end);
                //終端を持つ
                this->hang_point = make_obj->end;
            }
        }else if(make_result == TWOSHOT){
            //hangedが存在した場合
            if(CObject::hanged != nullptr ){
                if(CObject::hanged->is<CPoint>()){
                    //すり替え
                    dynamic_cast<CEdge*>(CObject::creating)->SetEndPos(dynamic_cast<CPoint*>(CObject::hanged));
                    this->RemoveObject(this->hang_point);
                }else{
                    //近接点へ移動
                    *this->hang_point = CObject::hanged->GetNear(*this->hang_point);
                }
            }
            CObject::creating = nullptr;
            this->hang_point = nullptr;
            make_result = COMPLETE;

        }else if(make_result == ENDLESS){
            //hangedが存在した場合
            if(CObject::hanged != nullptr){
                if(CObject::hanged->is<CPoint>()){
                    //すり替えて終了
                    dynamic_cast<CEdge*>(CObject::creating)->SetEndPos(dynamic_cast<CPoint*>(CObject::hanged));
                    this->RemoveObject(this->hang_point);
                }else{
                    //近接点へ移動して終了
                    *this->hang_point = CObject::hanged->GetNear(*this->hang_point);
                }
                CObject::creating = nullptr;
                this->hang_point = nullptr;
                make_result = COMPLETE;
            }else{
                //同一の点を作成
                MakeJoint(make_obj);

                //終端を持つ
                this->hang_point = make_obj->end;
                this->AddObject(make_obj->end);
            }
        }
    }
    RefreshRestraints();
    repaint();
    emit RquireRefreshUI();
}
CREATE_RESULT CadEditForm::MakeJoint(CObject* obj){
    Pos local_pos = CObject::mouse_pos;
    if(CObject::hanged != nullptr)local_pos = CObject::hanged->GetNear(local_pos);

    if(CObject::hanged == nullptr){
        //始点を作成
        CPoint* new_point = new CPoint(CObject::mouse_pos);
        return obj->Create(new_point);
    }else if(CObject::hanged->is<CPoint>()){
        //既存の点を使用
        return obj->Create(dynamic_cast<CPoint*>(CObject::hanged));
    }else{
        //近接点を作成
        CPoint* new_point = new CPoint(CObject::hanged->GetNear(CObject::mouse_pos));
        return obj->Create(new_point);
    }
}

void CadEditForm::MakeSmartDimension(){
    if(CObject::selected.size() > 0 && CObject::selected.size() < 3){
        //スマート寸法ダイアログ生成
        SmartDimensionDialog* diag = new SmartDimensionDialog(this);
        SmartDimension* dim (new SmartDimension());

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
        //QVector<CObject*>からQVector<CEdge*>に変換
        QVector<CEdge*> edges;
        for(CObject* obj:CObject::selected)edges.push_back(dynamic_cast<CEdge*>(obj));

        CBlock block = diag->ExportCBlock();
        block.SetNodeAll(edges);
        this->blocks.push_back(block);
        CObject::selected.clear();
    }
    return true;
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
                            for(int j=0;j<static_cast<int>(rank.size());j++){
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


void CadEditForm::ApplyObjectList(QTreeWidget* list){
    //CObject::selectedを更新する。
    //ポインタを保持していないため、添字でカウント
    CObject::selected.clear();
    QMap<QString,int>map;
    for(int i=0;i<list->topLevelItemCount();i++){
        QTreeWidgetItem* item = list->topLevelItem(i);
        QString text = item->text(0);
        if(text == "Origin") text = "CPoint";

        //カウント
        if(map.find(text) != map.end()){
            map[text]++;
        }else map.insert(text,1);


        if(item->isSelected()){
            //CObject::selected内をループ
            int count = 0;
            int j;
            for(j = 0;count < map[text];j++){
                if(this->objects[j]->is<CPoint >() && text=="CPoint" )count++;
                if(this->objects[j]->is<CLine  >() && text=="CLine"  )count++;
                //if(this->objects[j]->is<CRect  >() && text=="CRect"  )count++;
                if(this->objects[j]->is<CArc   >() && text=="CArc"   )count++;
                if(this->objects[j]->is<CSpline>() && text=="CSpline")count++;
            }
            CObject::selected.push_back(this->objects[j-1]);
        }
    }
}
void CadEditForm::DrawObjectList(QTreeWidget* list){
    if(list->topLevelItemCount() != this->objects.size()){
        list->clear();
        for(int i=0;i<this->objects.size();i++){
            std::pair<std::string,std::string> p;
            if(objects[i]->is<CPoint> () && !dynamic_cast<CPoint*>(this->objects[i])->isControlPoint())p = std::make_pair("CPoint" ,":/ToolImages/Dot.png");
            if(objects[i]->is<CPoint> () &&  dynamic_cast<CPoint*>(this->objects[i])->isControlPoint())p = std::make_pair("Origin" ,":/ToolImages/Dot.png");
            if(objects[i]->is<CLine>  ())p = std::make_pair("CLine"  ,":/ToolImages/Line.png");
            //if(objects[i]->is<CRect>  ())p = std::make_pair("CRect"  ,":/ToolImages/Rect.png");
            if(objects[i]->is<CArc>   ())p = std::make_pair("CArc"   ,":/ToolImages/Arc.png");
            if(objects[i]->is<CSpline>())p = std::make_pair("CSpline",":/ToolImages/Spline.png");
            list->addTopLevelItem(new QTreeWidgetItem());
            list->topLevelItem(list->topLevelItemCount()-1)->setText(0,p.first.c_str());
            list->topLevelItem(list->topLevelItemCount()-1)->setIcon(0,QIcon(p.second.c_str()));
            list->topLevelItem(i)->setSelected(exist(CObject::selected,objects[i]));
        }
    }
}
void CadEditForm::ApplyCBoxList  (QListWidget *list){
    //selecting_blockを更新する
    this->selecting_block = -1;
    for(int i=0;i<list->count();i++){
        //選択していればselecting_blockを更新する
        if(list->item(i)->isSelected()){
            this->selecting_block = i;
        }
    }
}
void CadEditForm::DrawCBoxList   (QListWidget *list){
    //数が一致しなければ、全て削除し再度代入する
    if(list->count() != this->blocks.size()){
        list->clear();
        for(int i=0;i<this->blocks.size();i++) {
            list->addItem(new QListWidgetItem("CBox"));
            list->item(list->count()-1)->setIcon(QIcon(":/ToolImages/Blocks.png"));
            list->item(list->count()-1)->setSelected(selecting_block == i);
        }
    }
    //選択情報を更新
    for(int i=0;i<this->blocks.size();i++){
        //list->item(i)->setSelected(exist(this->selecting_block,&blocks[i]));
    }
}
void CadEditForm::ConfigureBlock(QListWidgetItem*){
    if(this->selecting_block == -1)return;

    CBoxDefineDialog* diag = new CBoxDefineDialog(this);
    diag->ImportCBlock(this->blocks[selecting_block]);
    if(diag->exec()){
        QVector<CEdge*> ll;
        for(int i =0;i<4;i++)ll.push_back(dynamic_cast<CEdge*>(this->blocks[selecting_block].GetNode(i)));
        this->blocks[selecting_block] = diag->ExportCBlock();
        this->blocks[selecting_block].SetNodeAll(ll);
    }
}


void CadEditForm::SetState(CEnum state){
    this->state = state;
}

void CadEditForm::ResetAllExpantion(){
    //原点を中心に
    this->translate.x = -this->width()  / 2;
    this->translate.y = -this->height() / 2;
    this->scale = 1.0;
}

void CadEditForm::Export(){
    ExportDialog* diag = new ExportDialog(this);
    diag->SetBlocks(this->blocks);
    diag->exec();
}

void CadEditForm::Save(){

    QString filename = QFileDialog::getSaveFileName( this, "Save");
    if(filename == "")return;

    QVector<Pos> points;
    //全ての頂点を保存
    for(int i =0;i<this->objects.size();i++){
        if(this->objects[i]->is<CPoint>()){//CPointなら
            points.push_back(*dynamic_cast<CPoint*>(this->objects[i]));
        }
        if(this->objects[i]->is<CEdge>()){//CEdgeなら
            points.push_back(*dynamic_cast<CEdge*>(this->objects[i])->start);
            points.push_back(*dynamic_cast<CEdge*>(this->objects[i])->end);
            for(int j=0;j<dynamic_cast<CEdge*>(this->objects[i])->GetMiddleCount();j++){
                points.push_back(*dynamic_cast<CEdge*>(this->objects[i])->GetMiddle(j));
            }
        }
    }
    //並び替え&同一排除
    std::sort(points.begin(),points.end());
    points.erase(std::unique(points.begin(),points.end()),points.end());


    //オブジェクト追加
    QVector<std::pair<QString,QVector<int>>> pairs;//(名前,頂点番号)
    for(int i =0;i<this->objects.size();i++){
        QString name = "Unknown";
        QVector<int> vertex;
        if(this->objects[i]->is<CPoint>()){
            vertex.push_back(IndexOf(points,*dynamic_cast<CPoint*>(this->objects[i])));
            name = "CPoint";
        }
        if(this->objects[i]->is<CEdge>()){
            if(this->objects[i]->is<CLine>  ())name = "CLine";
            if(this->objects[i]->is<CArc>   ())name = "CArc";
            if(this->objects[i]->is<CSpline>())name = "CSPline";

            //始点終点を追加
            vertex.push_back(IndexOf(points,*dynamic_cast<CEdge*>(this->objects[i])->start));
            vertex.push_back(IndexOf(points,*dynamic_cast<CEdge*>(this->objects[i])->end));
            //中継点を追加
            for(int j=0;j<dynamic_cast<CEdge*>(this->objects[i])->GetMiddleCount();j++){
                vertex.push_back(IndexOf(points,*dynamic_cast<CEdge*>(this->objects[i])->GetMiddle(j)));
            }
        }
        //登録
        pairs.push_back(std::make_pair(name,vertex));
    }

    //出力
    std::ofstream out(filename.toStdString().c_str());
    if(!out)return;

    //頂点数
    out << points.size() << std::endl;
    for(Pos p : points){
        out << p << std::endl;
    }
    //オブジェクト数
    out << pairs.size() << std::endl;
    for(std::pair<QString,QVector<int>> p : pairs){
        out << p.first.toStdString();
        for(int i=0;i<p.second.size();i++){
            out << "," << p.second[i];
        }
        out << std::endl;
    }
}

void CadEditForm::Load(){
    //オブジェクトをクリア
    this->objects.clear();

    QString filename = QFileDialog::getOpenFileName(this, "Load");
    std::ifstream in(filename.toStdString().c_str());
    if(!in)return ;

    //頂点数取得
    int vertex_num;
    in >> vertex_num;

    //頂点取得
    QVector<CPoint*> points;
    for(int i=0;i<vertex_num;i++){
        Pos p;
        in >> p;
        points.push_back(new CPoint(p));
        this->objects.push_back(points.back());
    }

    //オブジェクト数取得
    int object_num;
    in >> object_num;

    //オブジェクト復元
    for(int i=0;i<object_num;i++){
        std::string str;
        in >> str;

        //オブジェクト判定
        CObject* make = nullptr;
        QStringList sl = QString(str.c_str()).split(',');

        //if(sl[0] == "CPoint" )make = new CPoint();
        if(sl[0] == "CLine"  )make = new CLine();
        if(sl[0] == "CArc"   )make = new CArc();
        if(sl[0] == "CSpline")make = new CSpline();
        //オブジェクト生成
        if(make != nullptr){
            for(int j=1;j<sl.size();j++){
                make->Create(points[sl[j].toInt()]);
            }
            this->objects.push_back(make);
        }
    }

}















