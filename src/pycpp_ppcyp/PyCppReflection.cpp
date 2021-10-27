#include "PyCppReflection.h"

#if DEBUG>0
#include <iostream>
#endif


PyCppReflection::PyCppReflection(): _attributes({}), _do_nothing({[](PyObject*)->int{return -1;},[]()->PyObject*{return NULL;},ReflectionMode::REFLECT_NONE}){
    #if DEBUG>0
    std::cout << "PyCppReflection::PyCppReflection  " << static_cast<IntPyCppReflection*>(this) << std::endl;
    #endif
}
    
PyCppReflection::~PyCppReflection(){
    #if DEBUG>0
    std::cout << "PyCppReflection::~PyCppReflection " << static_cast<IntPyCppReflection*>(this) << std::endl;
    #endif
}

void PyCppReflection::_addAttribute(const std::string key, std::function<int(PyObject*)> setter, std::function<PyObject*(void)> getter, ReflectionMode mode = ReflectionMode::REFLECT_BOTH){
    if((mode & ReflectionMode::REFLECT_SETTER_ONLY) == 0){
        setter = [this](PyObject*)->int{setError(ErrorType::ACCESS_ERROR,"write forbidden."); return -1;};
    }
    if((mode & ReflectionMode::REFLECT_GETTER_ONLY) == 0){
        getter = [this]()->PyObject*{setError(ErrorType::ACCESS_ERROR,"read forbidden.");return NULL;};
    }
    
    auto it = _attributes.find(key);
    if(it != _attributes.end()){
        _attributes.erase(it);
    }
    
    _attributes.insert( {key, {setter, getter, mode}} );
}

PyCppReflection::attr_data PyCppReflection::getAttributeDataByKey(const std::string key){
    //TODO: add mutex here!
    auto it = _attributes.find(key);
    if(it != _attributes.end()){
        return it->second;
    }
    return _do_nothing;
}


/** create python object for c++ related base object with reflection configures */

PyObject* PyCppReflection::createPyObjectFromPyCppReflection(std::shared_ptr<IntPyCppReflection> ref){
    return CastToPy(ref, NULL);
}

/** template to create c++ object from python object. it must be an specific python PyCppReflection type object */

std::shared_ptr<IntPyCppReflection> PyCppReflection::createPyCppReflectionFromPyObject(PyObject* obj){
    std::shared_ptr<IntPyCppReflection> ret;
    CastToCpp(ret, obj); // in case of error ret i set to nullptr
    return ret;
}




