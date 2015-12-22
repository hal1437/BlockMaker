#ifndef RELATIVE_H
#define RELATIVE_H
#include <memory>

//相対値
template<class Base,class Diff = Base>
class Relative{
public:
    Base* ref=nullptr;
    Relative<Base,Diff>* ref_same=nullptr;
    Diff diff;

public:

    void setRelative(Base* base){
        ref = base;
        ref_same = nullptr;
    }
    void setRelative(Relative<Base,Diff>* r){
        ref = nullptr;
        ref_same = r;
    }
    void setDifferent(const Diff& diff){
        this->diff = diff;
    }
    Base getRelative()const{
        if     (ref == nullptr && ref_same == nullptr)return diff;
        else if(ref == nullptr && ref_same != nullptr)return ref_same->getRelative() + diff;
        else if(ref != nullptr && ref_same == nullptr)return *ref + diff;
    }
    Base* getReference()const{
        return ref;
    }
    Relative<Base,Diff>* getReferenceSame()const{
        return ref_same;
    }
    Diff getDifference()const{
        return diff;
    }

    Base operator()()const{
        return getRelative();
    }
    //参照先を変更
    Relative<Base,Diff> operator=(Base& rhs){
        setRelative(&rhs);
        setDifferent(Diff());
        return (*this);
    }
    //相対値を変更
    Relative<Base,Diff> operator=(Diff&& rhs){
        setDifferent(rhs);
        return (*this);
    }
    //ふつうのコピー
    Relative<Base,Diff> operator=(const Relative<Base,Diff>& rhs){
        this->ref = rhs.ref;
        this->ref_same = rhs.ref_same;
        this->diff = rhs.diff;
        return (*this);
    }
    Relative(){
        setDifferent(Diff());
    }
    Relative(const Diff& diff){
        setRelative(nullptr);
        setDifferent(diff);
    }
    Relative(std::shared_ptr<Base> base,const Diff& diff){
        setRelative(base);
        setDifferent(diff);
    }
};

#endif // RELATIVE_H

