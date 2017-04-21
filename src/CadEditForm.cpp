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
        if(CObject::hanged == nullptr){
            //マウス座標中心に拡大
            this->Zoom(next_scale,CObject::mouse_pos);
        }else{
            //選択点があればそれを中心に拡大
            this->Zoom(next_scale,CObject::hanged->GetNear(CObject::mouse_pos));
        }
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

    //生成点を更新
    if(hang_point != nullptr){
        *hang_point = CObject::mouse_pos;
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
    if(CObject::creating != nullptr){
        this->RemoveObject(make_obj);
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

void CadEditForm::CompleteObject(CObject* make_obj){
    /*
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
    }*/
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

void CadEditForm::MargePoints(){
    //原点が存在する場合は先頭に出す
    for(int i=0;i<CObject::selected.size();i++){
        if(dynamic_cast<CPoint*>(CObject::selected[i])->isControlPoint()){
            std::swap(CObject::selected[0],CObject::selected[i]);
            break;
        }
    }

    //先頭以外の点を破棄し、統合する。
    for(int j=1;j<CObject::selected.size();j++){
        for(int i=1;i<CObject::selected.size();i++){
            /*
            for(CObject* p : this->objects){
                for(int i=0;i<p->GetJointNum();i++){
                    //posが含まれる
                    if(p->GetJoint(i) == CObject::selected[j]){
                        p->SetJoint(i,dynamic_cast<CPoint*>(CObject::selected[0]));
                    }
                }
            }*/
        }
        this->objects.removeAll(CObject::selected[j]);
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
        if(eject_step == false){
            //新規作成CEdge
            if(state == Dot   )make_obj = new CPoint(this);
            if(state == Line  )make_obj = new CLine(this);
            if(state == Line  )make_obj = new CLine(this);
            if(state == Arc   )make_obj = new CArc(this);
            if(state == Spline)make_obj = new CSpline(this);

            //追加
            this->AddObject(make_obj);
            CObject::creating = make_obj;

            //新規作成
            CREATE_RESULT result = MakeJoint(make_obj);

            //一回だけ
            if(result == ONESHOT){
                //おわり
            }
            //二回必要
            if(result == TWOSHOT){
                //選択解除
                CObject::hanged = nullptr;
                //同一の点を作成
                MakeJoint(make_obj);
                //切り離し過程へ
                eject_step = true;

                //端点をオブジェクトリストに追加
                this->AddObject(dynamic_cast<CEdge*>(make_obj)->start);
                this->AddObject(dynamic_cast<CEdge*>(make_obj)->end);
                //終端を持つ
                this->hang_point = dynamic_cast<CEdge*>(make_obj)->end;
            }
        }else if(eject_step == true){

            //hangedが存在した場合
            if(CObject::hanged != nullptr && CObject::hanged->is<CPoint>()){
                dynamic_cast<CEdge*>(CObject::creating)->SetEndPos(dynamic_cast<CPoint*>(CObject::hanged));
                this->RemoveObject(this->hang_point);
            }
            CObject::creating = nullptr;
            this->hang_point = nullptr;
            eject_step = false;
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
        //始点を作成
        CPoint* new_point = new CPoint(CObject::mouse_pos);
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



