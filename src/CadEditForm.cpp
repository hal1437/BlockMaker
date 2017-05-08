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
    double delta = (event->angleDelta().y())/MOUSE_ZOOM_RATE;//差分値
    if(!this->shift_pressed){
        //拡大
        double next_scale = std::exp(std::log(this->scale) + delta);//次の拡大値

        //拡大値は負にならない
        if(next_scale > 0){
            //適応
            if(CObject::hanged == nullptr)this->Zoom(next_scale,CObject::mouse_pos); //マウス座標中心に拡大
            else                          this->Zoom(next_scale,CObject::hanged->GetNear(CObject::mouse_pos));//選択点があればそれを中心に拡大

            //シグナル発生
            emit ScaleChanged(next_scale);
            CObject::drawing_scale = next_scale;
        }
    }else{
        //深さ変更
        if(delta>0) this->depth++;
        if(delta<0) this->depth--;
        emit DepthChanged(this->depth);
    }
    repaint();
}
void CadEditForm::mouseMoveEvent   (QMouseEvent* event){
    //マウス移動を監視
    CObject::mouse_pos = ConvertLocalPos(Pos(event->pos().x(),event->pos().y())) + Pos(0,0,this->depth);
    if(this->hang_point != nullptr)this->hang_point->z = this->depth;
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
            this->RemoveEdge(make_obj);
        }
        this->hang_point = nullptr;
        this->make_result = COMPLETE;
    }
}

void CadEditForm::AddEdge(CEdge* obj){
    if(!exist(edges,obj)){
        edges.push_back(obj);
    }
}


void CadEditForm::RemoveObject(CObject* obj){
    //objを消す
    QVector<CEdge*>::Iterator it = std::find(this->edges.begin(),this->edges.end(),obj);
    if(it != this->edges.end()){
        this->edges.erase(it);
    }

    //点ならばその点を含むObjectを全てnullptrにする。
    if(obj->is<CPoint>()){
        for(CEdge*& it : this->edges){
            if(it != obj){
                if(it->start == obj ||
                   it->end   == obj ){
                    it = nullptr;
                    break;
                }
            }
        }
    }

    //nullptrを消す
    this->edges.removeAll(nullptr);
    repaint();
}
void CadEditForm::RemoveEdge(CEdge* obj){
    this->edges.removeAll(obj);
}

void CadEditForm::CompleteObject(CObject*){

}

/*double CadEditForm::GetScale()const{
    return scale;
}
Pos    CadEditForm::GetTranslate()const{
    return translate;
}*/

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

        this->SetTranslate(this->translate + diff);
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
    if(this->selecting_block >= 0 && this->selecting_block < this->blocks.size()){
        paint.setPen(QPen(Qt::cyan, CObject::DRAWING_LINE_SIZE/2 / this->scale,Qt::SolidLine,Qt::RoundCap));
        this->blocks[this->selecting_block].Draw(paint);
    }

    //寸法を描画
    paint.setPen(QPen(Qt::blue, 1,Qt::SolidLine,Qt::RoundCap));
    for(SmartDimension* dim:dimensions){
        dim->Draw(paint);
    }

    //エッジ描画
    for(CEdge* obj:this->edges){
        if(obj->start->z > this->depth){
            paint.setPen(QPen(QColor(200,200,200), CObject::DRAWING_LINE_SIZE / this->scale,Qt::SolidLine,Qt::RoundCap));
        }else if(obj->start->z < this->depth){
            paint.setPen(QPen(QColor(100,100,100), CObject::DRAWING_LINE_SIZE / this->scale,Qt::SolidLine,Qt::RoundCap));
        }else{
            paint.setPen(QPen(Qt::blue, CObject::DRAWING_LINE_SIZE / this->scale,Qt::SolidLine,Qt::RoundCap));
        }
        obj->Draw(paint);
        obj->start->Draw(paint);
        obj->end  ->Draw(paint);
        for(int i=0;i<dynamic_cast<CEdge*>(obj)->GetMiddleCount();i++){
            dynamic_cast<CEdge*>(obj)->GetMiddle(i)->Draw(paint);
        }
    }

    //原点
    if(this->origin->z > this->depth){
        paint.setPen(QPen(QColor(200,200,200), CObject::DRAWING_LINE_SIZE / this->scale,Qt::SolidLine,Qt::RoundCap));
    }else if(this->origin->z < this->depth){
        paint.setPen(QPen(QColor(100,100,100), CObject::DRAWING_LINE_SIZE / this->scale,Qt::SolidLine,Qt::RoundCap));
    }else{
        paint.setPen(QPen(Qt::blue, CObject::DRAWING_LINE_SIZE / this->scale,Qt::SolidLine,Qt::RoundCap));
    }
    this->origin->Draw(paint);

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
void CadEditForm::Translate(Pos local_diff){
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
        for(CEdge* p : this->edges){
            if(p->start == CObject::selected[i]){
                p->SetStartPos(dynamic_cast<CPoint*>(CObject::selected[0]));
            }
            if(p->end == CObject::selected[i]){
                p->SetEndPos(dynamic_cast<CPoint*>(CObject::selected[0]));
            }
            for(int j=0;j<p->GetMiddleCount();j++){
                if(p->GetMiddle(j) == CObject::selected[i]){
                    p->SetMiddle(dynamic_cast<CPoint*>(CObject::selected[0]),j);
                }
            }
        }
        //this->objects.removeAll(CObject::selected[i]);
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
    //this->AddObject(origin);
}

CadEditForm::~CadEditForm()
{
    delete ui;
}

CObject* CadEditForm::getHanged(){
    CObject* final = nullptr;

    if(this->origin->isSelectable(CObject::mouse_pos)){
        if(this->origin->z == this->depth)return this->origin;
        else final = this->origin;
    }

    for(CEdge* obj:this->edges){
        if(obj->isCreating())continue;

        //端点
        if(obj->start->isSelectable(CObject::mouse_pos)){
            if(obj->start->z == this->depth)return obj->start;
            else final = obj->start;
        }
        if(obj->end  ->isSelectable(CObject::mouse_pos)){
            if(obj->end->z == this->depth)return obj->end;
            else final = obj->end;
        }
        for(int i=0;i<obj->GetMiddleCount();i++){
            if(obj->GetMiddle(i)->isSelectable(CObject::mouse_pos)){
                if(obj->GetMiddle(i)->z == this->depth)return obj->GetMiddle(i);
                else final = obj->GetMiddle(i);
            }
        }
        //エッジ自身
        if(obj->isSelectable(CObject::mouse_pos)){
            if(obj->start->z == this->depth && obj->end->z == this->depth)return obj;
            else if(final == nullptr) final = obj;
        }

    }

    return final;
}

void CadEditForm::SetDepth(double depth){
    this->depth = depth;
    repaint();
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
            CEdge* new_obj;
            //if(state == Dot   )new_obj = new CPoint(this);
            if(state == Line  )new_obj = new CLine(this);
            if(state == Arc   )new_obj = new CArc(this);
            if(state == Spline)new_obj = new CSpline(this);
            if(state == Rect  ){
                new_obj = new CLine(this);
            }

            //追加
            CObject::creating = new_obj;

            //新規作成
            this->AddEdge(new_obj);
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
                //this->AddObject(make_obj->start);
                //this->AddObject(make_obj->end);
                //中継点をオブジェクトリストに追加
                for(int i =0;i<make_obj->GetMiddleCount();i++){
                    //this->AddObject(make_obj->GetMiddle(i));
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
                //this->AddObject(make_obj->start);
                //this->AddObject(make_obj->end);
                //終端を持つ
                this->hang_point = make_obj->end;
            }
        }else if(make_result == TWOSHOT){
            //hangedが存在した場合
            if(CObject::hanged != nullptr ){
                if(CObject::hanged->is<CPoint>() && dynamic_cast<CPoint*>(CObject::hanged)->z == this->depth){
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
            CObject::hanged = this->getHanged();

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
                //this->AddObject(make_obj->end);
            }
        }
    }
    RefreshRestraints();
    repaint();
    emit RequireRefreshUI();
}
CREATE_RESULT CadEditForm::MakeJoint(CObject* obj){

    if(CObject::hanged == nullptr){
        //始点を作成
        CPoint* new_point = new CPoint(CObject::mouse_pos);
        return obj->Create(new_point);
    }else if(CObject::hanged->is<CPoint>() && dynamic_cast<CPoint*>(CObject::hanged)->z == this->depth){
        //既存の点を使用
        return obj->Create(dynamic_cast<CPoint*>(CObject::hanged));
    }else{
        //近接点を作成
        CPoint* new_point = new CPoint(CObject::hanged->GetNear(CObject::mouse_pos));
        new_point->z = this->depth;
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
        block.SetEdgeAll(edges);
        this->blocks.push_back(block);
        CObject::selected.clear();
    }
    return true;
}


void CadEditForm::RefreshRestraints(){
    //拘束を解決
    if(this->edges.size()!=0){
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


void CadEditForm::ImportObjectList(QTreeWidget* list){
    //CObject::selectedを更新する。
    //ポインタを保持していないため、添字でカウント
    CObject::selected.clear();

    for(int i=0;i<list->topLevelItemCount();i++){
        QTreeWidgetItem* item = list->topLevelItem(i);
        QString          text = item->text(0);

        if(i > this->edges.size())continue;

        if(text == "Origin"){
            if(item->isSelected()){
                CObject::selected.push_back(this->origin);
            }
        }else{
            //CObject::selected内をループ
            if(item->isSelected()){
                CObject::selected.push_back(this->edges[i-1]);
            }
            //子が選択されているか
            for(int k=0;k<item->childCount();k++){
                if(item->child(k)->isSelected()){
                    if(k==0){
                        CObject::selected.push_back(this->edges[i-1]->start);
                    }
                    else if(k == 1){
                        CObject::selected.push_back(this->edges[i-1]->end);
                    }
                    else{
                        CObject::selected.push_back(this->edges[i-1]->GetMiddle(k-2));
                    }
                }
            }
        }
    }
}
void CadEditForm::ExportObjectList(QTreeWidget* list){
    if(list->topLevelItemCount() != this->edges.size()+1){
        list->clear();

        //原点を追加
        list->addTopLevelItem(new QTreeWidgetItem());
        list->topLevelItem(list->topLevelItemCount()-1)->setText(0,"Origin");
        list->topLevelItem(list->topLevelItemCount()-1)->setIcon(0,QIcon(":/ToolImages/Dot.png"));

        for(int i=0;i<this->edges.size();i++){
            std::pair<std::string,std::string> p;
            if(this->edges[i]->is<CLine>  ())p = std::make_pair("CLine"  ,":/ToolImages/Line.png");
            if(this->edges[i]->is<CArc>   ())p = std::make_pair("CArc"   ,":/ToolImages/Arc.png");
            if(this->edges[i]->is<CSpline>())p = std::make_pair("CSpline",":/ToolImages/Spline.png");

            //オブジェクト追加
            list->addTopLevelItem(new QTreeWidgetItem());
            list->topLevelItem(list->topLevelItemCount()-1)->setText(0,p.first.c_str());
            list->topLevelItem(list->topLevelItemCount()-1)->setIcon(0,QIcon(p.second.c_str()));

            if(!list->topLevelItem(list->topLevelItemCount()-1)->isSelected()){
                list->topLevelItem(list->topLevelItemCount()-1)->setSelected(this->edges[i]->isSelected());
            }
        }
    }

    //子の設定
    for(int i=0;i<this->edges.size();i++){
        if(this->edges[i]->GetMiddleCount()+2 != list->topLevelItem(i+1)->childCount()){
            for(int j=0;j<this->edges[i]->GetMiddleCount()+2;j++){
                QTreeWidgetItem* child;
                if(j < list->topLevelItem(i+1)->childCount()){
                    child = list->topLevelItem(i+1)->child(j);
                }else{
                    child = new QTreeWidgetItem();
                }

                child->setIcon(0,QIcon(":/ToolImages/Dot.png"));
                if(j == 0){
                    child->setText(0,"Start");
                }
                else if(j == 1){
                    child->setText(0,"End");
                }
                else{
                    child->setText(0,QString("Middle_") + QString::number(j) + ")");
                }

                if(j >= list->topLevelItem(i+1)->childCount()){
                    if(j==0 || j == this->edges[i]->GetMiddleCount()+1){
                        list->topLevelItem(i+1)->addChild(child);
                    }else{
                        list->topLevelItem(i+1)->insertChild(1,child);
                    }
                }
            }
        }
    }

    //選択状態の反映
    for(int i=0;i<list->topLevelItemCount();i++){
        if(i == 0){
            //原点
            list->topLevelItem(0)->setSelected(this->origin->isSelected());
        }else{
            //Edge自身
            list->topLevelItem(i)->setSelected(this->edges[i-1]->isSelected());

            //端点
            list->topLevelItem(i)->child(0)->setSelected(this->edges[i-1]->start->isSelected());
            list->topLevelItem(i)->child(1)->setSelected(this->edges[i-1]->end->isSelected());
            for(int j=0;j<this->edges[i-1]->GetMiddleCount() && j<list->topLevelItem(i)->childCount()-2;j++){
                list->topLevelItem(i)->child(j+2)->setSelected(this->edges[i-1]->GetMiddle(j)->isSelected());
            }
        }
    }


}
void CadEditForm::ImportCBoxList  (QListWidget *list){
    //selecting_blockを更新する
    this->selecting_block = -1;
    for(int i=0;i<list->count();i++){
        //選択していればselecting_blockを更新する
        if(list->item(i)->isSelected()){
            this->selecting_block = i;
        }
    }
}
void CadEditForm::ExportCBoxList   (QListWidget *list){
    //数が一致しなければ、全て削除し再度代入する
    if(list->count() != this->blocks.size()){
        list->clear();
        for(int i=0;i<this->blocks.size();i++) {
            list->addItem(new QListWidgetItem("CBox"));
            list->item(list->count()-1)->setIcon(QIcon(":/ToolImages/Blocks.png"));
            list->item(list->count()-1)->setSelected(selecting_block == i);
        }
    }
}
void CadEditForm::ConfigureBlock(QListWidgetItem*){
    if(this->selecting_block == -1)return;

    CBoxDefineDialog* diag = new CBoxDefineDialog(this);
    diag->ImportCBlock(this->blocks[selecting_block]);
    if(diag->exec()){
        QVector<CEdge*> ll;
        for(int i =0;i<4;i++)ll.push_back(dynamic_cast<CEdge*>(this->blocks[selecting_block].GetEdge(i)));
        this->blocks[selecting_block] = diag->ExportCBlock();
        this->blocks[selecting_block].SetEdgeAll(ll);
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
    for(int i =0;i<this->edges.size();i++){
        if(this->edges[i]->is<CPoint>()){//CPointなら
            points.push_back(*dynamic_cast<CPoint*>(this->edges[i]));
        }
        if(this->edges[i]->is<CEdge>()){//CEdgeなら
            points.push_back(*this->edges[i]->start);
            points.push_back(*this->edges[i]->end);
            for(int j=0;j<this->edges[i]->GetMiddleCount();j++){
                points.push_back(*this->edges[i]->GetMiddle(j));
            }
        }
    }
    //並び替え&同一排除
    std::sort(points.begin(),points.end());
    points.erase(std::unique(points.begin(),points.end()),points.end());


    //オブジェクト追加
    QVector<std::pair<QString,QVector<int>>> pairs;//(名前,頂点番号)
    for(int i =0;i<this->edges.size();i++){
        QString name = "Unknown";
        QVector<int> vertex;
        if(this->edges[i]->is<CPoint>()){
            vertex.push_back(IndexOf(points,*dynamic_cast<CPoint*>(this->edges[i])));
            name = "CPoint";
        }
        if(this->edges[i]->is<CEdge>()){
            if(this->edges[i]->is<CLine>  ())name = "CLine";
            if(this->edges[i]->is<CArc>   ())name = "CArc";
            if(this->edges[i]->is<CSpline>())name = "CSPline";

            //始点終点を追加
            vertex.push_back(IndexOf(points,*this->edges[i]->start));
            vertex.push_back(IndexOf(points,*this->edges[i]->end));
            //中継点を追加
            for(int j=0;j<this->edges[i]->GetMiddleCount();j++){
                vertex.push_back(IndexOf(points,*this->edges[i]->GetMiddle(j)));
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

    QString filename = QFileDialog::getOpenFileName(this, "Load");
    std::ifstream in(filename.toStdString().c_str());
    if(!in)return ;
    //オブジェクトをクリア
    this->edges.clear();

    //頂点数取得
    int vertex_num;
    in >> vertex_num;

    //頂点取得
    QVector<CPoint*> points;
    for(int i=0;i<vertex_num;i++){
        Pos p;
        in >> p;
        if(p != Pos(0,0)){
            points.push_back(new CPoint(p));
            //this->edges.push_back(points.back());
        }else{
            points.push_back(this->origin);
            //this->edges.push_back(this->origin);
        }
    }

    //オブジェクト数取得
    int object_num;
    in >> object_num;

    //オブジェクト復元
    for(int i=0;i<object_num;i++){
        std::string str;
        in >> str;

        //オブジェクト判定
        CEdge* make = nullptr;
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
            this->edges.push_back(make);
        }
    }
    emit RequireRefreshUI();

}















