#include "Reflection.h"




Reflection::Reflection():_fields({}), _do_nothing({typeid(void),[](void*,ssize_t*){},[](void*,ssize_t*){},ReflectionMode::REFLECT_NONE,nullptr}){
    
}

Reflection::~Reflection(){
    
}

void Reflection::callFieldFunction(std::function<void(void*, ssize_t*)> f, void* v, ssize_t* l){
    //TODO: add mutex for multi-threading!
    f(v, l);
}


void Reflection::_addField(const std::string key, const std::type_index &type, std::function<void(void*, ssize_t*)> setter, std::function<void(void*, ssize_t*)> getter, ReflectionMode mode = ReflectionMode::REFLECT_BOTH, std::shared_ptr<IntReflection> subobject = nullptr){
    
    if((mode & ReflectionMode::REFLECT_SETTER_ONLY) == 0){
        setter = _do_nothing.setter;
    }
    if((mode & ReflectionMode::REFLECT_GETTER_ONLY) == 0){
        getter = _do_nothing.getter;
    }
    
    //prevent self reference
    if(subobject.get() == static_cast<IntReflection*>(this)){
        setError(ErrorType::FATAL_ERROR, "self reference disallow!");
        subobject = nullptr;
    }
    
    //remove before add - no duplicates = overritable
    auto it = _fields.find(key);
    if(it != _fields.end()){
        _fields.erase(it);
    }

    _fields.insert( {key, {type, setter, getter, mode, subobject}} );

}

Reflection::field_data Reflection::getFieldDataByKey(const std::string key){
    
    //access to map guarded by mutes
    //TODO: add mutex for multi-threading!
    
    auto it = _fields.find(key);
    if(it == _fields.end()){
        setError(ErrorType::KEY_ERROR, std::string()+"key '"+key+"' does not exist.");
        return _do_nothing;
    }else{
        return it->second;
    }
}


void Reflection::updateFieldDataByPath(const std::string key, const Reflection::field_data& f){
    
    //access to map guarded by mutes
    //TODO: add mutex for multi-threading!
    
    auto it = _fields.find(key);
    if(it != _fields.end()){
        it->second.setter = f.setter;
        it->second.getter = f.getter;
        it->second.subobject = f.subobject;
    }
    
}

Reflection::field_data Reflection::getFieldDataByPath(const std::string key){ //unfortunatelly this must be public

    // split path by space
    std::size_t pos = key.find(".");
    if(pos != std::string::npos){
        std::string obj = key.substr(0, pos);
        std::string subkey = key.substr(pos+1);
        
        //read root in path
        field_data v = getFieldDataByKey(obj);
        
        //check if it is subobject
        if(v.subobject == nullptr){
            setError(ErrorType::KEY_ERROR, std::string()+"not an object in a path '" + key + "'");
            return _do_nothing;
        }
        
        //read reflection from suboject
        v.subobject->clearError();
        field_data w = v.subobject->getFieldDataByPath(subkey);
        moveError(v.subobject);
        
        return w;
    }

    return getFieldDataByKey(key);
}


std::vector<std::string> Reflection::getReflectionList(){
    //TODO: add mutex for multi-threading!
    
    std::vector<std::string> out;
    
    for(auto it : _fields){
        out.push_back(it.first);
        
        if(it.second.subobject){
            for(auto i : it.second.subobject->getReflectionList()){
                out.push_back(it.first+"."+i);        
            }
        }
        
    }
    
    return out;
}

std::string Reflection::getTypeNameFromPath(std::string key){
    return getFieldDataByPath(key).type.name();
}





