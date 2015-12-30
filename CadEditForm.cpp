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

    for(CObject*        obj:objects)   obj->Draw(paint);

    paint.setPen(QPen(Qt::blue, 1));
    for(SmartDimension* dim:dimensions)dim->Draw(paint);
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
        if (!selected){
            if(obj->Selecting()){
                selected=true;
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
    if(CObject::selected.size()==2){

        //スマート寸法生成
        SmartDimensionDialog* diag = new SmartDimensionDialog(this);
        if(diag->exec()){
            double value = diag->GetValue();
            SmartDimension* dim = new SmartDimension();

            //有効寸法であれば
            if(dim->SetTarget(CObject::selected[0],CObject::selected[1])){
                dim->SetValue(value);
                this->dimensions.push_back(dim);
            }else{
                delete dim;
            }
        }
    }
    RefreshRestraints();
}


void CadEditForm::RefreshRestraints(){
    restraints.clear();
    for(SmartDimension* dim:dimensions){
        if(restraints.find(dim->GetTarget(1)) != restraints.end()){
            restraints[dim->GetTarget(1)] = *restraints[dim->GetTarget(1)] & *dim->MakeRestraint();
        }else{
            restraints[dim->GetTarget(1)] = dim->MakeRestraint();
        }
    }
    //拘束関係更新
    for(QMap<CObject*,Restraint*>::iterator rest = restraints.begin();rest != restraints.end();rest++){
        Pos p1 = rest.key()->GetJointPos(0);
        Pos diff =  p1-rest.value()->GetNearPoint(p1);
        rest.key()->Move(-diff);
    }
}
