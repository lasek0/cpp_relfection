#pragma once
#include <map>
#include "IntReflection.h"
#include "Error.h"




class Reflection : virtual public IntReflection, virtual public Error {
private:
    std::map<const std::string, field_data> _fields;
    field_data _do_nothing;
    
public:
    Reflection();
    virtual ~Reflection();
    virtual void callFieldFunction(std::function<void(void*, ssize_t*)> f, void* v, ssize_t* l);
    
private:

    virtual void _addField(const std::string key, const std::type_index &type, std::function<void(void*, ssize_t*)> setter, std::function<void(void*, ssize_t*)> getter, ReflectionMode mode, std::shared_ptr<IntReflection> subobject);

    template<typename T>
    std::function<void(void*, ssize_t*)> defaultFieldSetter(T &field){
        return [&field](void *v, ssize_t*){ field = *(T*)v; };
    }
    
    template<typename T>
    std::function<void(void*, ssize_t*)> defaultFieldGetter(T &field){
        return [&field](void *v, ssize_t*){ *(T*)v = field; };
    }

private:
    virtual field_data getFieldDataByKey(const std::string key);
public:
    virtual void updateFieldDataByPath(const std::string key, const field_data& f);
    virtual field_data getFieldDataByPath(const std::string key);
    virtual std::vector<std::string> getReflectionList();
    virtual std::string getTypeNameFromPath(std::string key);

protected:

    // normal variables
    template<typename T>
    void reflectField(const std::string key, T &field, ReflectionMode mod=ReflectionMode::REFLECT_BOTH){
        _addField(
            key, 
            typeid(T), 
            defaultFieldSetter(field),
            defaultFieldGetter(field),
            mod,
            nullptr
        );

    }
    
    // sub objects
    template<typename T>
    void reflectField(const std::string key, std::shared_ptr<T> &field, ReflectionMode mod=ReflectionMode::REFLECT_BOTH){
        _addField(
            key, 
            typeid(std::shared_ptr<T>), 
            [this,&field,key](void *v, ssize_t*){ 
                defaultFieldSetter(field)(v, 0);//field is updated
                //NOTE: need to update subobject shared_ptr value
                //NOTE: this is called inside critical section
                _fields.find(key)->second.subobject = std::dynamic_pointer_cast<IntReflection>(field);
            }, 
            defaultFieldGetter(field),
            mod,
            std::static_pointer_cast<IntReflection>(field)
        );
    }
    
    
    //allows: std::function<int(int)> f; object->get("myfunction", f); int x = f(5);
    // member functions
    template<typename R, typename B, typename... T>
    void reflectField(const std::string key, B* base, R(B::*func)(T...)){
        //B* base = dynamic_cast<B*>(this); //works nice in g++ but not in msvc :( need to add parameter explicitelly            
        _addField(
            key,
            typeid(std::function<R(T...)>),
            [](void*,ssize_t*){},
            [base,func](void* v,ssize_t*){ 
                *(std::function<R(T...)>*)v = [base,func](T... args)->R{ 
                    return (base->*func)(args...); 
                };
            }, // return function that will call member function
            ReflectionMode::REFLECT_GETTER_ONLY,
            nullptr
        );
    }
    

    //TODO: check if this is still needed, probably not due to using of shared pointers and dynamic pointer cast is available
    /*
    template<typename T>
    void __reflectCast__hidden(const std::string key, T *field){
        reflect(
            key, 
            typeid(T*), 
            [](void *v, ssize_t*){}, 
            [o=const_cast<T*>(field)](void *v, ssize_t*){ *(T**)v = (T*)o; },
            ReflectionMode::REFLECT_GETTER_ONLY,
            nullptr
        );
    }
    */


};

