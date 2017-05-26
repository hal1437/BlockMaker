#include "CadEditForm.h"
#include "ui_CadEditForm.h"

void CadEditForm::setModel(CadModelCore* model){
    this->model = model;
}

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
            CObject* hanged = this->getHanged();
            if(hanged == nullptr)this->Zoom(next_scale,this->mouse_pos); //マウス座標中心に拡大
            else                 this->Zoom(next_scale,hanged->GetNearPos(this->mouse_pos));//選択点があればそれを中心に拡大

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
    this->mouse_pos = ConvertLocalPos(Pos(event->pos().x(),event->pos().y())) + Pos(0,0,this->depth);

    //選択オブジェクトの選定
    CObject* answer = this->getHanged();
    //フィルター適用
    this->mouse_pos = filter.Filtering(this->mouse_pos);
    //Z座標設定
    if(this->hang_point != nullptr)this->hang_point->z() = this->depth;

    //ズーム支点リセット
    zoom_piv = Pos(0,0);
    //生成点座標を更新
    if(hang_point != nullptr){
        hang_point->Move(this->mouse_pos-*hang_point);
    }
    //UI更新
    repaint();
    emit MovedMouse(event,answer);
}
void CadEditForm::resizeEvent(QResizeEvent*){
    //原点を中心に来るように上書き
    this->translate.x() = -this->width()  / 2;
    this->translate.y() = -this->height() / 2;
}

void CadEditForm::Escape(){
    //作成解除    
    if(this->creating != nullptr){
        if(make_count > 0){
            this->model->RemoveEdges(this->creating);
        }
        this->creating   = nullptr;
        this->hang_point = nullptr;
        this->make_count = COMPLETE;
    }
}


void CadEditForm::MovedMouse(QMouseEvent *event, CObject *under_object){

    static Pos past;
    static Pos piv; //画面移動支点
    const  Pos null_pos = Pos(std::numeric_limits<double>::lowest(),std::numeric_limits<double>::lowest()); //無効な点

    //選択
    if(!(event->buttons() & Qt::LeftButton) || !move_flag){
        piv = null_pos;//移動支点を解除
    }
    //画面移動
    if((event->buttons() & Qt::LeftButton) && this->selected.isEmpty() && this->state == Edit){
        //支点登録
        if(piv == null_pos){
            piv = this->ConvertWorldPos(this->mouse_pos);
        }
        Pos hand = this->ConvertWorldPos(this->mouse_pos);
        Pos diff = (piv - hand);

        this->SetTranslate(this->translate + diff);
        piv = hand;
    }

    //編集
    if(move_flag == true){
        for(CObject* p : this->selected){
            //フィルタリング
            if(p->is<CPoint>()){
                p->Move(this->filter.Filtering(*dynamic_cast<CPoint*>(p)) - *dynamic_cast<CPoint*>(p));
            }
            //移動量適用
            p->Move(this->mouse_pos - past);
        }
    }

    //拘束更新
    this->RefreshRestraints();

    emit MouseMoved(this->mouse_pos);

    past = this->mouse_pos;
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
    trans.translate(-translate.x(),-translate.y());
    trans.scale(scale,-scale);
    paint.setTransform(trans); // 変換行列を以降の描画に適応

    //グリッド描画
    this->filter.DrawGrid(paint,this->translate.x()/this->scale ,(-this->height()-this->translate.y())/this->scale,
                                this->width()      /this->scale ,this->height()                     /this->scale);

    //CBox描画
    for(int i=0;i<this->model->GetBlocks().size();i++){ //エリア描画
        if(i == this->selecting_block){
            paint.setPen(QPen(Qt::cyan, CObject::DRAWING_LINE_SIZE/2 / this->scale,Qt::SolidLine,Qt::RoundCap));
        }else{
            paint.setPen(QPen(Qt::darkGray, CObject::DRAWING_LINE_SIZE/2 / this->scale,Qt::SolidLine,Qt::RoundCap));
        }
        paint.setBrush(QBrush(Qt::lightGray));   //背景設定
        this->model->GetBlocks()[i]->Draw(paint);
    }

    //寸法を描画
    paint.setPen(QPen(Qt::blue, 1,Qt::SolidLine,Qt::RoundCap));
    for(SmartDimension* dim:this->model->GetDimensions()){
        dim->Draw(paint);
    }

    //エッジ描画
    for(CEdge* obj:this->model->GetEdges()){
        if     (obj->start->z() > this->depth)paint.setPen(QPen(QColor(200,200,200), CObject::DRAWING_LINE_SIZE / this->scale,Qt::SolidLine,Qt::RoundCap));
        else if(obj->start->z() < this->depth)paint.setPen(QPen(QColor(100,100,100), CObject::DRAWING_LINE_SIZE / this->scale,Qt::SolidLine,Qt::RoundCap));
        else                                  paint.setPen(QPen(Qt::blue           , CObject::DRAWING_LINE_SIZE / this->scale,Qt::SolidLine,Qt::RoundCap));

        obj->Draw(paint);
        for(int i=0;i<obj->GetPosSequenceCount();i++){
            if(obj->GetPosSequence(i)!=nullptr){
                obj->GetPosSequence(i)->Draw(paint);
            }
        }
    }

    //原点
    if     (this->model->origin->z() > this->depth)paint.setPen(QPen(QColor(200,200,200), CObject::DRAWING_LINE_SIZE / this->scale,Qt::SolidLine,Qt::RoundCap));
    else if(this->model->origin->z() < this->depth)paint.setPen(QPen(QColor(100,100,100), CObject::DRAWING_LINE_SIZE / this->scale,Qt::SolidLine,Qt::RoundCap));
    else                                           paint.setPen(QPen(Qt::blue           , CObject::DRAWING_LINE_SIZE / this->scale,Qt::SolidLine,Qt::RoundCap));
    this->model->origin->Draw(paint);

    //選択されたオブジェクト
    paint.setPen(QPen(Qt::cyan, CObject::DRAWING_LINE_SIZE / this->scale,Qt::SolidLine,Qt::RoundCap));
    for(CObject* obj:this->selected){
        obj->Draw(paint);
    }
    //メイン選択中
    paint.setPen(QPen(Qt::red , CObject::DRAWING_LINE_SIZE / this->scale,Qt::SolidLine,Qt::RoundCap));
    if(this->getHanged() != nullptr){
        this->getHanged()->Draw(paint);
    }

    //描画復元
    paint.restore();

    //競合確認
    bool is_conflict = false;
    for(Restraint* rest:this->model->GetRestraints()){
        if(rest->isComplete()==false){
            is_conflict = true;
        }
    }
    emit ToggleConflict(is_conflict);
}



Pos CadEditForm::ConvertLocalPos(Pos pos)const{
    QTransform trans;
    trans.translate(-translate.x(),-translate.y());
    trans.scale(scale,-scale);
    trans = trans.inverted();//逆行列化
    QPointF ans = trans.map(QPointF(pos.x(),pos.y()));
    return Pos(ans.x(),ans.y());
}
Pos CadEditForm::ConvertWorldPos(Pos pos)const{
    QTransform trans;
    trans.translate(-translate.x(),-translate.y());
    trans.scale(scale,-scale);//逆行列化
    QPointF ans = trans.map(QPointF(pos.x(),pos.y()));
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
    this->mouse_pos = this->ConvertLocalPos(zoom_piv);
}
void CadEditForm::Translate(Pos local_diff){
    this->translate += local_diff;
}

void CadEditForm::MergePoints(){
    //原点が存在する場合は先頭に出す
    for(int i=0;i<this->selected.size();i++){
        if(dynamic_cast<CPoint*>(this->selected[i])->isControlPoint()){
            std::swap(this->selected[0],this->selected[i]);
            break;
        }
    }

    //先頭以外の点を破棄し、統合する。
    for(int i=1;i<this->selected.size();i++){
        for(CEdge* p : this->model->GetEdges()){
            if(p->start == this->selected[i]){
                p->SetStartPos(dynamic_cast<CPoint*>(this->selected[0]));
            }
            if(p->end == this->selected[i]){
                p->SetEndPos(dynamic_cast<CPoint*>(this->selected[0]));
            }
            for(int j=0;j<p->GetMiddleCount();j++){
                if(p->GetMiddle(j) == this->selected[i]){
                    p->SetMiddle(dynamic_cast<CPoint*>(this->selected[0]),j);
                }
            }
        }
        //this->objects.removeAll(this->selected[i]);
    }
    this->selected.clear();
    repaint();
}


CadEditForm::CadEditForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CadEditForm)
{
    ui->setupUi(this);

    //マウストラッキング対象
    setMouseTracking(true);
}

CadEditForm::~CadEditForm()
{
    delete ui;
}

CObject* CadEditForm::getHanged()const{
    CObject* final = nullptr;

    //原点
    if(this->model->origin->isSelectable(this->mouse_pos)){
        if(this->model->origin->z() == this->depth)return this->model->origin;
        else final = this->model->origin;
    }

    //エッジ
    for(CEdge* obj:this->model->GetEdges()){
        //選択済みは除外
        if(exist(this->selected,obj))continue;
        //作成中も除外
        if(obj == this->creating)continue;

        //構成点
        bool exit = false;
        for(int i=0;i<obj->GetPosSequenceCount();i++){
            if(obj->GetPosSequence(i) == this->hang_point){
                exit = true;
                break;
            }
            if(obj->GetPosSequence(i)->isSelectable(this->mouse_pos)){
                if(obj->GetPosSequence(i)->z() == this->depth)return obj->GetPosSequence(i);
                else final = obj->GetPosSequence(i);
            }
        }
        if(exit == true)continue;

        //エッジ自身
        if(obj->isSelectable(this->mouse_pos)){
            if(obj->start->z() == this->depth && obj->end->z() == this->depth)return obj;
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
void CadEditForm::SetGridFilterStatus(double x,double y){
    this->filter.setXGrid(x);
    this->filter.setYGrid(y);
    repaint();
}
void CadEditForm::MakeObject(){

    Pos local_pos = this->mouse_pos;
    CObject* hanged = this->getHanged();
    if(hanged != nullptr)local_pos = hanged->GetNearPos(local_pos);

    release_flag=false;

    //編集
    if(state == Edit){
        //シフト状態
        if(!shift_pressed)this->selected.clear();

        //選択状態をトグル
        if(exist(this->selected,hanged))erase(this->selected,hanged);
        else if(hanged != nullptr)this->selected.push_back(hanged);

    }else{
        //新規オブジェクト
        if(make_count == COMPLETE){
            //新規作成CEdge
            if(state == Line  )this->creating = new CLine(this);
            if(state == Arc   )this->creating = new CArc(this);
            if(state == Spline)this->creating = new CSpline(this);
            if(state == Rect  ){
                this->creating = new CLine(this);
            }
            //startを作成する。
            make_count = MakeJoint(this->creating);

            //持ち点を作成
            this->hang_point = new CPoint(this->mouse_pos,this->creating);
            this->creating->Create(this->hang_point);

            //モデルに追加
            this->model->AddEdges(this->creating);
        }else{
            //継続
            if(this->make_count != ENDLESS)this->make_count--;//作成過程を進める

            //終了処理
            if(this->make_count == COMPLETE){
                if(hanged != nullptr){
                    if(hanged->is<CPoint>()){
                        //すり替え
                        this->creating->SetEndPos(dynamic_cast<CPoint*>(hanged));
                    }else{
                        //近似移動
                        this->hang_point->Move(this->hang_point->GetNearPos(*this->hang_point) - *this->hang_point);
                    }
                }
                this->hang_point = nullptr; //手放す
                this->creating = nullptr;   //作成完了
            }else{
                //ジョイントを作成しつつ継続
                MakeJoint(this->creating);
            }


        }
    }
    RefreshRestraints();
    repaint();
}
CREATE_RESULT CadEditForm::MakeJoint(CObject* obj){

    CObject* hanged = this->getHanged();

    if(hanged == nullptr){
        //始点を作成
        this->hang_point = new CPoint(this->mouse_pos,obj);
    }else if(hanged->is<CPoint>() && dynamic_cast<CPoint*>(hanged)->z() == this->depth){
        //既存の点を使用
        this->hang_point = dynamic_cast<CPoint*>(hanged);
    }else{
        //近接点を作成
        CPoint* new_point = new CPoint(hanged->GetNearPos(this->mouse_pos));
        new_point->z() = this->depth;
        this->hang_point = new_point;
    }

    //実行
    CREATE_RESULT result = obj->Create(this->hang_point);
    return result;
}

void CadEditForm::MakeSmartDimension(){
    if(this->selected.size() > 0 && this->selected.size() < 3){
        //スマート寸法ダイアログ生成
        SmartDimensionDialog* diag = new SmartDimensionDialog(this);
        SmartDimension* dim (new SmartDimension());

        //ターゲット設定
        CObject* target[2];
        target[0] = this->selected[0];
        if(this->selected.size() == 1)target[1] = nullptr;
        else                          target[1] = this->selected[1];

        if(dim->SetTarget(target[0],target[1])){//寸法定義可能ならば

            //ダイアログ起動
            diag->SetValue(dim->currentValue());//ダイアログ初期値設定
            if(diag->exec()){
                //登録
                dim->SetValue(diag->GetValue());
                this->model->AddDimensions(dim);
                //スマート寸法の拘束も追加
                std::vector<Restraint*> rs = dim->MakeRestraint();
                for(Restraint* r : rs){
                    this->model->AddRestraints(r);
                }
            }
        }
    }
    RefreshRestraints();
}

void CadEditForm::MakeRestraint(RestraintType type){
    Restraint* rest = nullptr;
    if(type == EQUAL)     rest = new EqualRestraint({this->selected[0],this->selected[1]});
    if(type == VERTICAL)  rest = new VerticalRestraint(this->selected);
    if(type == HORIZONTAL)rest = new HorizontalRestraint(this->selected);
    if(type == MATCH)     rest = new MatchRestraint({this->selected[0],this->selected[1]});
    if(type == CONCURRENT)rest = new ConcurrentRestraint({this->selected[0],this->selected[1]});
    //if(type == TANGENT)   rest = new TangentRestraint(this->selected[0],this->selected[1]);

    //固定
    if(type == LOCK | type == UNLOCK){
        for(CObject* obj : this->selected){
            obj->Lock(type == LOCK);
        }
    }

    if(rest != nullptr){
        this->model->AddRestraints(rest);
        RefreshRestraints();
    }
}
bool CadEditForm::MakeBlock(){
    CBoxDefineDialog* diag = new CBoxDefineDialog();

    if(diag->exec()){
        //QVector<CObject*>からQVector<CEdge*>に変換
        QVector<CEdge*> edges;
        for(CObject* obj:this->selected)edges.push_back(dynamic_cast<CEdge*>(obj));

        CBlock* block = new CBlock(diag->ExportCBlock());
        block->SetEdgeAll(edges);
        this->model->AddBlocks(block);
        this->selected.clear();
    }
    return true;
}


void CadEditForm::RefreshRestraints(){
    //拘束を解決
    if(this->model->GetEdges().size()!=0){
        CObject* hanged = this->getHanged();
        //持ち手が存在すれば
        if(hanged != nullptr){
            std::vector<std::pair<int,CObject*>> rank;

            //過去の選択位置を保持する
            static CObject* hand = nullptr;

            //持ち手は0番に
            if(hanged != nullptr && hanged->is<CPoint>()){
                hand = this->getHanged();
            }
            rank.push_back(std::make_pair(0,hand));
            //ランク分けダイクストラ
            int max_rank=0;
            std::queue<std::pair<int,CObject*>> queue;
            queue.push(std::make_pair(0,hand));//初期ノード
            while(!queue.empty()){
                //全ての拘束リストから
                for(Restraint* rest:this->model->GetRestraints()){
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
                for(Restraint* rest:this->model->GetRestraints()){
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
            for(Restraint* rest:this->model->GetRestraints()){
                if(!rest->Complete()){
                    qDebug() << "CONFLICT" ;
                }
            }
       }
    }
}


void CadEditForm::ImportObjectList(QTreeWidget* list){
    //this->selectedを更新する。
    //ポインタを保持していないため、添字でカウント
    this->selected.clear();

    for(int i=0;i<list->topLevelItemCount();i++){
        QTreeWidgetItem* item = list->topLevelItem(i);
        QString          text = item->text(0);

        if(i > this->model->GetEdges().size())continue;

        if(text == "Origin"){
            if(item->isSelected()){
                this->selected.push_back(this->model->origin);
            }
        }else{
            //this->selected内をループ
            if(item->isSelected()){
                this->selected.push_back(this->model->GetEdges()[i-1]);
            }
            //子が選択されているか
            for(int k=0;k<item->childCount();k++){
                if(item->child(k)->isSelected()){
                    if(k==0){
                        this->selected.push_back(this->model->GetEdges()[i-1]->start);
                    }
                    else if(k == 1){
                        this->selected.push_back(this->model->GetEdges()[i-1]->end);
                    }
                    else{
                        this->selected.push_back(this->model->GetEdges()[i-1]->GetMiddle(k-2));
                    }
                }
            }
        }
    }
}
void CadEditForm::ExportObjectList(QTreeWidget* list){
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
}
void CadEditForm::ConfigureBlock(QListWidgetItem*){
    if(this->selecting_block == -1)return;

    CBoxDefineDialog* diag = new CBoxDefineDialog(this);
    diag->ImportCBlock(*this->model->GetBlocks()[selecting_block]);
    if(diag->exec()){
        QVector<CEdge*> ll;
        for(int i =0;i<4;i++)ll.push_back(dynamic_cast<CEdge*>(this->model->GetBlocks()[selecting_block]->GetEdge(i)));
        *this->model->GetBlocks()[selecting_block] = diag->ExportCBlock();
        this->model->GetBlocks()[selecting_block]->SetEdgeAll(ll);
    }
}


void CadEditForm::SetState(CEnum state){
    this->state = state;
}

void CadEditForm::ResetAllExpantion(){
    //原点を中心に
    this->translate.x() = -this->width()  / 2;
    this->translate.y() = -this->height() / 2;
    this->scale = 1.0;
}

void CadEditForm::Export(){
    ExportDialog* diag = new ExportDialog(this);
    diag->SetBlocks(this->model->GetBlocks());
    diag->exec();
}

void CadEditForm::Save(){
    QString filename = QFileDialog::getSaveFileName(this, "Save");
    this->model->ExportFoamFile(filename);
}

void CadEditForm::Load(){
    QString filename = QFileDialog::getOpenFileName(this, "Load");
    this->model->ImportFoamFile(filename);
}
