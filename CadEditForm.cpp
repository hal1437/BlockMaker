#include "CadEditForm.h"
#include "ui_CadEditForm.h"

void CadEditForm::AddObject(CObject* obj){
    objects.push_back(obj);

    //CPoint→CLineの順で
    std::sort(objects.begin(),objects.end(),[](const CObject* lhs,const CObject* ){
        if(lhs->is<CPoint>())return true;
        else return false;
    });
}


void CadEditForm::RemoveObject(CObject* obj){
    //点ならば含むObjectを全て削除する。
    if(obj->is<CPoint>()){
        for(QVector<CObject*>::Iterator it = objects.begin();it != objects.end();it++){
            for(int i=0;i < (*it)->GetJointNum();i++){
                if((*it)->GetJointPos(i) == dynamic_cast<CPoint*>(obj)->getRelative() && (*it) != obj){
                    *it = nullptr;
                    break;
                }
            }
        }
    }else{
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
    }

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
            if(obj->is<CPoint>() && (*dynamic_cast<CPoint*>(obj))() == make_obj->GetJointPos(i)){
                is_known_pos=true;
                break;
            }
        }
        //追加
        if(!is_known_pos){
            CPoint* new_point = make_obj->GetJoint(i);
            new_point->Make(new_point);
            AddObject(new_point);
        }
    }
}

double CadEditForm::GetScale()const{
    return scale;
}
Pos    CadEditForm::GetTransform()const{
    return transform;
}


void CadEditForm::paintEvent(QPaintEvent*){
    QPainter paint(this);
    paint.save();
    paint.fillRect(0,0,this->width(),this->height(),Qt::white);
    if(scale == 0) paint.scale(0.00001f,0.00001f);
    else paint.scale(scale,scale);

    paint.setPen(QPen(Qt::blue, 1));
    for(SmartDimension* dim:dimensions){
        dim->Draw(paint);
    }
    for(CObject* obj:objects){
        if     (exist(CObject::selected,obj))paint.setPen(QPen(Qt::cyan, CObject::DRAWING_LINE_SIZE));
        else if(obj == CObject::selecting)   paint.setPen(QPen(Qt::red , CObject::DRAWING_LINE_SIZE));
        else                                 paint.setPen(QPen(Qt::blue, CObject::DRAWING_LINE_SIZE));
        if(obj->Refresh())obj->Draw(paint);
    }

    paint.restore();
}


void CadEditForm::mouseMoveEvent   (QMouseEvent* event){
    CObject::mouse_over = Pos(event->pos().x(),event->pos().y());
    CObject* answer = Selecting();
    repaint();
    emit MovedMouse(event,answer);
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

CObject* CadEditForm::Selecting(){
    bool selected = false;
    CObject* select_obj = nullptr;
    for(CObject* obj:objects){
        if(!selected){
            if(obj->Selecting() && !obj->isCreateing()){
                selected = true;
                select_obj = obj;
            }
        }
    }
    return select_obj;
}

void CadEditForm::SetScale(double scale){
    this->scale = scale;
    repaint();
}

void CadEditForm::SetTransform(Pos trans){
    this->transform = trans;
    repaint();
}

void CadEditForm::MakeSmartDimension(){
    if(CObject::selected.size()!=0){
        //スマート寸法生成
        SmartDimensionDialog* diag = new SmartDimensionDialog(this);
        if(diag->exec()){
            double value = diag->GetValue();
            SmartDimension* dim = new SmartDimension();

            //有効寸法であれば
            CObject* sel[2];
            sel[0] = CObject::selected[0];
            if(CObject::selected.size()==1)sel[1] = nullptr;
            else sel[1] = CObject::selected[1];

            if(dim->SetTarget(sel[0],sel[1])){
                dim->SetValue(value);
                this->dimensions.push_back(dim);
            }else{
                delete dim;
            }
            restraints.clear();
            for(SmartDimension* dim:dimensions){

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
    Restraint* rest;
    /*
    RestraintType
            EQUAL      ,//等しい値
            VERTICAL   ,//垂直拘束 c:[-.]
            HORIZONTAL ,//水平拘束 c:[-.]
            MATCH      ,//一致拘束 c:[p.-]
            CONCURRENT ,//並行拘束 c:[l,l]
            ANGLE      ,//角度拘束 c:[l,l]
            TANGENT    ,//正接拘束 c:[l]
            FIX        ,//固定拘束 c:[]
            Paradox    ,//矛盾拘束 c:[]
            */
    if(type == EQUAL)     rest = new EqualRestraint({CObject::selected[0],CObject::selected[1]});
    if(type == VERTICAL)  rest = new VerticalRestraint({CObject::selected[0]});
    if(type == HORIZONTAL)rest = new HorizontalRestraint({CObject::selected[0]});
    if(type == MATCH)     rest = new MatchRestraint({CObject::selected[0],CObject::selected[1]});
    if(type == CONCURRENT)rest = new ConcurrentRestraint({CObject::selected[0],CObject::selected[1]});
    //if(type == TANGENT)   rest = new TangentRestraint(CObject::selected[0],CObject::selected[1]);


    restraints.push_back(rest);
    RefreshRestraints();
}

void CadEditForm::RefreshRestraints(){
    for(Restraint* rest:restraints){
        if(rest!=nullptr)rest->Complete();
    }
}
