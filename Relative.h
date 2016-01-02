#ifndef RELATIVE_H
#define RELATIVE_H
#include <memory>

//相対値
template<class Base,class Diff = Base>
class Relative{
public:
    Relative<Base,Diff>* ref=nullptr;
    Diff diff;

public:

    void setReference(Relative<Base,Diff>* r){
        ref = r;
    }
    void setDifferent(const Diff& diff){
        this->diff = diff;
    }
    Base getRelative()const{
        if(ref == nullptr)return diff;
        else return (*ref)() + diff;
    }
    Relative<Base,Diff>* getReference()const{
        return ref;
    }
    Diff getDifference()const{
        return diff;
    }

    Base operator()()const{
        return getRelative();
    }
    Relative(){
        setDifferent(Diff());
    }
    Relative(const Diff& diff){
        setReference(nullptr);
        setDifferent(diff);
    }
    Relative(Relative<Base,Diff>* base){
        setRelative(base);
        setDifferent(Diff());
    }
    Relative(Relative<Base,Diff>* base,const Diff& diff){
        setRelative(base);
        setDifferent(diff);
    }
};

#endif // RELATIVE_H

