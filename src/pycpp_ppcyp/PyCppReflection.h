#pragma once
#include "IntPyCppReflection.h"
#include "Reflection.h"
#include "PyCppCast.h"



class PyCppReflection : virtual public IntPyCppReflection, virtual public Reflection, virtual public PyCppCast {
private:  
    std::map< const std::string, attr_data > _attributes;
    attr_data _do_nothing;

public:
    PyCppReflection();
    virtual ~PyCppReflection();

    virtual attr_data getAttributeDataByKey(const std::string key);
    virtual PyObject* createPyObjectFromPyCppReflection(std::shared_ptr<IntPyCppReflection> ref);
    virtual std::shared_ptr<IntPyCppReflection> createPyCppReflectionFromPyObject(PyObject* obj);

protected:
    virtual void _addAttribute(const std::string key, std::function<int(PyObject*)> setter, std::function<PyObject*(void)> getter, ReflectionMode mode);


private:
    
    template <typename T>
    const std::function<int(PyObject*)> defaultAttribSetter(const std::string key){
        return [this,key](PyObject* obj)->int{ 
            T r; 
            if(CastToCpp(r, obj) == NULL){return -1;} 
            callFieldFunction(getFieldDataByPath(key).setter, (void*)&r, 0);
            return 0;
        };
    }
    
    template <typename T>
    const std::function<PyObject*(void)> defaultAttribGetter(const std::string key){
        return [this,key]()->PyObject*{ 
            T r; 
            callFieldFunction(getFieldDataByPath(key).getter, (void*)&r, 0);
            return CastToPy(r, NULL);
        };
    }

protected:

    // normal variables
    template<typename T>
    void reflectAttribute(const std::string key, T& value, ReflectionMode mod=ReflectionMode::REFLECT_BOTH){
        reflectField(key, value, mod);
        _addAttribute(key, 
            defaultAttribSetter<T>(key), 
            defaultAttribGetter<T>(key), 
            mod);
    }
    
    // sub objects 
    template<typename T>
    void reflectAttribute(const std::string key, std::shared_ptr<T>& value, ReflectionMode mod=ReflectionMode::REFLECT_BOTH){
        reflectField(key, value, mod);
        _addAttribute(key, 
            defaultAttribSetter<std::shared_ptr<T>>(key),
            defaultAttribGetter<std::shared_ptr<T>>(key),
            mod);
    }

    // member functions
    template<typename R, typename B, typename... T> //order of template typenames does not matter! :)
    void reflectAttribute(const std::string key, B* base, R(B::*func)(T...)){
        //B* base = dynamic_cast<B*>(this); //works nice in g++ but not in msvc :( need to add parameter explicitelly 
        reflectField(key, base, func);
        _addAttribute(key,
            [](PyObject*)->int{return -1;},
            defaultAttribGetter< std::function<R(T...)> >(key),
            ReflectionMode::REFLECT_GETTER_ONLY
        );
        
        
        
        //very very haxy trick which allow to cast python object methods back to cpp function reflection functions!!!
        //NOTE: it use setter as slot to store properly generated function gnerator from python obj
        //      and exploit that get and set realy most case it use len as sizeof so it is quite safe to block accigent set
        //      
        field_data f = getFieldDataByPath(key);
        f.setter = [this](void* v, ssize_t* obj){
            //very very haxy code!!!
            if(*obj == sizeof(std::function<R(T...)>))return; 
            //NOTE: prevent accident assigmnemt, since this function works now like getter, 
            // and int len is mimicked to be pointer to python object, only internal 
            // function knows how to use it :)
            std::function<R(T...)> tmp;
            CastToCpp(tmp, (PyObject*)obj);
            *(std::function<R(T...)>*)v = tmp;
        };
        updateFieldDataByPath(key, f);
        

    }



};



