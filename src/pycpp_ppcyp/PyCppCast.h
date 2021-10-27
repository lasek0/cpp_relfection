#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <tuple>
#include <limits>
#include <numeric>
#include <typeinfo>
#include <Python.h>
#include "IntError.h"
#include "IntPyCppReflection.h"
#include "PyCppType.h"
#include "cpp11helpers.h"
#include "cpp14helpers.h"


/** this class handle conversion between c++ stl types and python types

casting python types to c++ types with type checking mapping:
  python       c++                                 notes
  ----------------------------------------------------------------------------------------
  int          char, short, int, long, ...         all c++ integers up to 64bit
  double       float, double
  bool         bool
  str          std::string
  bytes        std::basic_string<unsigned char>
  list         std::vector<type>                   python list must contain only single type
  dict         std::map<K,V>                       python dict key must be only a single type, same for values
  tuple        std::tuple<T1,T2,...>
  function     PyCppReflection                     self class
  object       std::shared_ptr<BASE>               BASE type must same as one of bases an IntPyCppReflection class

WARINING [!] pointers are handled like size_t number
NOTE     [.] structures are not supported, but can be simmulated as class with attributes or tuple-s


CastToCpp -> casting type from python to cpp (mainly called when python invoke call to cpp, or when python function return to cpp)
CastToPy  -> casting type from cpp to python (called when cpp call python function or when cpp convert returned data for python)

NOTE: CastToCpp, CastToPy DO NOT modify life time of passed objects, and return aays new instance if should return an instance


NOTE: You can not return "const T" or "const T&"

*/



//need struct py_data from PyCppType
//need NewPyCppObject from PyCppType
//need struct attr_data from IntPyCppReflection
//need setError from IntError
//NOTE: full template
class PyCppCast : virtual public IntError {
public:
    virtual inline ~PyCppCast(){}
    
private:

    /** return python error string  */
    static const std::string _getPythonErrorString(){
        PyObject *ptype;
        PyObject *pvalue;
        PyObject *ptraceback;
        std::string s = "";
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
        PyErr_Clear();
        if(pvalue){
            const std::string type_name(pvalue->ob_type->tp_name);
            if(type_name == "str"){
                s = std::string(PyUnicode_AsUTF8(pvalue));
            }else if(type_name == "Exception"){
                s = std::string("Exception raised!");
            }else{
                s = type_name+" type error occour!";
            }
        }
        Py_XDECREF(ptype);
        Py_XDECREF(pvalue);
        Py_XDECREF(ptraceback);
        return s;
    }



    // TYPE CONVERSION SPECIFFICATIONS

    //-----------------------------------------------------------------------------------------------------

    // integers

    template<typename T, std::enable_if_t<std::numeric_limits<T>::is_integer && !std::is_same<T,bool>::value, bool> = true >
    PyObject* _cast(const std::string& type_name, T &value, PyObject* obj){
        if(type_name == "int") value = (T)PyLong_AsSsize_t(obj);
        else if(type_name == "float") value = (T)PyFloat_AsDouble(obj);//c type cast from float to int
        else return NULL;
        return obj;
    }
    
    template<typename T, std::enable_if_t<std::numeric_limits<T>::is_integer && !std::is_same<T,bool>::value, bool> = true >
    PyObject* _create(const std::string& type_name, T &value, PyObject* obj){
        return PyLong_FromSsize_t((size_t)value);
    }

    template<typename T, std::enable_if_t<std::numeric_limits<T>::is_integer && !std::is_same<T,bool>::value, bool> = true >
    PyObject* _update(const std::string& type_name, T &value, PyObject* obj){
        return obj;
    }


    //-----------------------------------------------------------------------------------------------------

    // support pointers as integers
    
    template<typename T, std::enable_if_t<std::is_pointer<T>::value, bool> = true >
    PyObject* _cast(const std::string& type_name, T &value, PyObject* obj){
        if(type_name == "int") value = (T)PyLong_AsSsize_t(obj);
        else return NULL;
        return obj;
    }
    
    template<typename T, std::enable_if_t<std::is_pointer<T>::value, bool> = true >
    PyObject* _create(const std::string, T &value, PyObject* obj){
        return PyLong_FromSsize_t((size_t)value);
    }

    template<typename T, std::enable_if_t<std::is_pointer<T>::value, bool> = true >
    PyObject* _update(const std::string& type_name, T &value, PyObject* obj){
        return obj;
    }
  

    //-----------------------------------------------------------------------------------------------------

    // floating point
    
    template<typename T, std::enable_if_t<std::numeric_limits<T>::is_iec559, bool> = true >
    PyObject* _cast(const std::string& type_name, T &value, PyObject* obj){
        if(type_name == "float") value = (T)PyFloat_AsDouble(obj);
        else if(type_name == "int") value = (T)PyLong_AsSsize_t(obj);//c type cast from int to float
        else return NULL;
        return obj;
    }
    
    template<typename T, std::enable_if_t<std::numeric_limits<T>::is_iec559, bool> = true >
    PyObject* _create(const std::string& type_name, T &value, PyObject* obj){
        return PyFloat_FromDouble((double)value);
    }

    template<typename T, std::enable_if_t<std::numeric_limits<T>::is_iec559, bool> = true >
    PyObject* _update(const std::string& type_name, T &value, PyObject* obj){
        return obj;
    }



    //-----------------------------------------------------------------------------------------------------

    // bool

    template<typename T, std::enable_if_t<std::is_same<T,bool>::value, bool> = true >
    PyObject* _cast(const std::string& type_name, T &value, PyObject* obj){
        if(type_name == "bool") value = (obj == Py_True);
        else return NULL;
        return obj;
    }
    
    template<typename T, std::enable_if_t<std::is_same<T,bool>::value, bool> = true >
    PyObject* _create(const std::string& type_name, T &value, PyObject* obj){
        return PyBool_FromLong((size_t)value);
    }

    template<typename T, std::enable_if_t<std::is_same<T,bool>::value, bool> = true >
    PyObject* _update(const std::string& type_name, T &value, PyObject* obj){
        return obj;
    }

 
 
    //-----------------------------------------------------------------------------------------------------

    // string

    template<typename T, std::enable_if_t<std::is_same<T,std::string>::value, bool> = true >
    PyObject* _cast(const std::string& type_name, T &value, PyObject* obj){
        if(type_name == "str"){
          Py_ssize_t len = 0;
          const char *buf = PyUnicode_AsUTF8AndSize(obj, &len);
          value = T(buf, (int)len);
        } 
        else return NULL;
        return obj;
    }
    
    template<typename T, std::enable_if_t<std::is_same<T,std::string>::value, bool> = true >
    PyObject* _create(const std::string& type_name, T &value, PyObject* obj){
        return PyUnicode_FromStringAndSize(value.c_str(), value.size());
    }
    
    template<typename T, std::enable_if_t<std::is_same<T,std::string>::value, bool> = true >
    PyObject* _update(const std::string& type_name, T &value, PyObject* obj){
        return obj;
    }


    //-----------------------------------------------------------------------------------------------------
 
    // bytes
    
    template<typename T, std::enable_if_t<std::is_same<T,std::basic_string<unsigned char>>::value, bool> = true >
    PyObject* _cast(const std::string& type_name, T &value, PyObject* obj){
        if(type_name == "bytes"){
            char* buf = NULL;
            Py_ssize_t len = 0;
            PyBytes_AsStringAndSize(obj, &buf, &len);
            value = T((unsigned char*)buf, (int)len);
        }else return NULL;
        return obj;
    }
    
    template<typename T, std::enable_if_t<std::is_same<T,std::basic_string<unsigned char>>::value, bool> = true >
    PyObject* _create(const std::string& type_name, T &value, PyObject* obj){
        return PyBytes_FromStringAndSize((const char*)value.data(), value.size());
    }
    
    template<typename T, std::enable_if_t<std::is_same<T,std::basic_string<unsigned char>>::value, bool> = true >
    PyObject* _update(const std::string& type_name, T &value, PyObject* obj){
        return obj;
    }

    
    //-----------------------------------------------------------------------------------------------------

    // list
    
    //NOTE: PyList_GetItem returned borrowed reference (do not decremt it!)
    template<typename T>
    PyObject* _cast(const std::string& type_name, std::vector<T> &value, PyObject* obj){
        if(type_name == "list"){
            std::vector<T> r(PyList_Size(obj));
            for(size_t i=0; i<r.size(); i++){
                if(CastToCpp(r[i], PyList_GetItem(obj, i)) == NULL){
                    setError(ErrorType::CAST_ERROR, "an error during processing list element"+std::to_string(i));
                    return NULL;
                }
            }
            value = r;
        }else return NULL;
        return obj;
    }

    template<typename T>
    PyObject* _create(const std::string& type_name, std::vector<T> &value, PyObject* obj){
       return _update("list", value, PyList_New(value.size()));
    }
    
    template<typename T>
    PyObject* _update(const std::string& type_name, std::vector<T> &value, PyObject* obj){
        if(type_name == "list"){
            PyList_SetSlice(obj, 0, PyList_Size(obj), NULL);
            for(size_t i=0; i<value.size(); i++){
                PyObject* v = CastToPy(value[i], NULL);
                if(v == NULL){
                    setError(ErrorType::CAST_ERROR, "an error during processing list element"+std::to_string(i));
                    return NULL;
                }
                PyList_Append(obj, v);
                Py_DECREF(v);
            }
        }
        return obj;
    }
    

    //-----------------------------------------------------------------------------------------------------

    // dict
    
    template<typename K, typename V>
    PyObject* _cast(const std::string& type_name, std::map<K,V> &value, PyObject* obj){
        if(type_name == "dict"){
            Py_ssize_t pos = 0;
            PyObject* k;
            PyObject* v;
            K key;
            V val;
            std::map<K,V> tmp;
            while (PyDict_Next(obj, &pos, &k, &v)){ // Any references returned through them are borrowed
                if(CastToCpp(key, k) == NULL){
                    setError(ErrorType::PYTHON_ERROR, std::to_string(2)+"an error during parsing "+std::to_string(pos)+"-th key");
                    return NULL;
                }
                if(CastToCpp(val, v) == NULL){
                    setError(ErrorType::PYTHON_ERROR, std::to_string(3)+"an error during parsing "+std::to_string(pos)+"-th value");
                    return NULL;
                }
                tmp[key] = val; 
            }
            value = tmp;
        }else return NULL;
        return obj;
    }
    
    template<typename K, typename V>
    PyObject* _create(const std::string& type_name, std::map<K,V> &value, PyObject* obj){
        return _update("dict", value, PyDict_New());
    }
    
    template<typename K, typename V>
    PyObject* _update(const std::string& type_name, std::map<K,V> &value, PyObject* obj){
        PyDict_Clear(obj);
        for(auto it : value){
            K _k = it.first;
            V _v = it.second;
            PyObject* k = CastToPy(_k, NULL);
            if(k == NULL){
                return NULL;
            }
            PyObject* v = CastToPy(_v, NULL);
            if(v == NULL){
                Py_DECREF(k);
                return NULL;
            }
            int ret = PyDict_SetItem(obj, k, v); // inc refcount only on success, but in case of fail it must be deletet any way
            Py_DECREF(k);
            Py_DECREF(v);
            if(ret < 0){
                setError(ErrorType::PYTHON_ERROR, std::to_string(6)+": "+_getPythonErrorString());
                return NULL;
            }

        }
        return obj;
    }
    

    //-----------------------------------------------------------------------------------------------------

    // tuple

    template<typename... T>
    PyObject* _cast(const std::string& type_name, std::tuple<T...> &value, PyObject* obj){
        if(type_name == "tuple"){
            
            const size_t py_size = PyTuple_GET_SIZE(obj);
            const size_t cpp_size = std::tuple_size<std::tuple<T...>>::value;

            if(py_size != cpp_size){
                setError(ErrorType::PYTHON_ERROR, std::to_string(7)+": invalid number of tuple elements. got: "+std::to_string(py_size)+", expected: "+std::to_string(cpp_size)+", type: "+typeid(std::tuple<T...>).name());
                return NULL;
            }
            
            size_t i=cpp_size-1;
            int err = 0;
            //for each in tuple - reverse order
            std::apply([this,&i,&err,obj](auto&... x){ std::make_tuple([this,&i,&err,obj](auto& t)->bool{
               if(!err){
                   err |= (CastToCpp(t, PyTuple_GetItem(obj, i--)) == NULL);
               }
               return true;
            }(x)...);  }, value);
            if(err){
                setError(ErrorType::PYTHON_ERROR, std::to_string(8)+": an error during parsing tuple element "+std::to_string(i+1));
                return NULL;
            } 
            
        }else return NULL;
        return obj;
    }

    template<typename... T>
    PyObject* _create(const std::string& type_name, std::tuple<T...> &value, PyObject* obj){
        const size_t cpp_size = std::tuple_size<std::tuple<T...>>::value;

        obj = PyTuple_New(cpp_size);
        
        size_t i=cpp_size-1;
        int err = 0;
        std::apply([this,&i,&err,obj](auto&... x){ std::make_tuple([this,&i,&err,obj](auto& t)->bool{//for each in tuple
            if(!err){
                PyObject* item = CastToPy(t, NULL);
                if(item) PyTuple_SetItem(obj, i--, item);
                err |= (item == NULL);
            }
            return true;
        }(x)...); }, value);
        if(err){
            setError(ErrorType::CAST_ERROR, std::to_string(28)+": fail to create tuple element "+std::to_string(i+1));
            return NULL;
        }

        return obj;
    }   

    template<typename... T>
    PyObject* _update(const std::string& type_name, std::tuple<T...> &value, PyObject* obj){
        const size_t cpp_size = std::tuple_size<std::tuple<T...>>::value;

        size_t i=cpp_size-1;
        int err = 0;
        std::apply([this,&i,&err,obj](auto&... x){ std::make_tuple([this,&i,&err,obj](auto& t)->bool{//for each in tuple
            if(!err){
                PyObject* item = CastToPy(t, PyTuple_GetItem(obj, i));
                //if(item) PyTuple_SetItem(obj, i, item); // tuple does not allow to re assign value!, when You do that it Will crash!
                err |= (item == NULL); i--;
            }
            return true;
        }(x)...); }, value);
        if(err){
            setError(ErrorType::CAST_ERROR, std::to_string(28)+": fail to update tuple element "+std::to_string(i+1));
            return NULL;
        }

        return obj;
    }

    //c++11 helper - not used right now

    template<typename T>
    int _tuple_helper_cast(int &err, size_t& i, PyObject* obj, T& t){
        if(!err){
            err |= (CastToCpp(t, PyTuple_GetItem(obj, i--)) == NULL);
        }
        return 1;
    }

    template<typename T>
    int _tuple_helper_create(int &err, size_t& i, PyObject* obj, T& t){
        if(!err){
            PyObject* item = CastToPy(t, NULL);
            if(item) PyTuple_SetItem(obj, i--, item);
            err |= (item == NULL);
        }
        return 1;
    }

    template<typename T>
    int _tuple_helper_update(int &err, size_t& i, PyObject* obj, T& t){
        if(!err){
            PyObject* item = CastToPy(t, PyTuple_GetItem(obj, i));
            err |= (item == NULL); i--;
        }
        return 1;
    }


    //-----------------------------------------------------------------------------------------------------


    // function

    template<typename R, typename... T>
    PyObject* _cast(const std::string& type_name, std::function<R(T...)> &value, PyObject* obj){
        if(type_name == "method" || type_name == "function" || type_name == "PyCppReflection"){
            Py_INCREF(obj);//keep that function untill cpp needs it
            std::shared_ptr<PyObject> _obj = std::shared_ptr<PyObject>(obj, [](void* o){Py_DECREF((PyObject*)o);});
            value = [this,_obj](T... args)->R{
                
                //create tuple with cpp args
                auto tup = tuple_with_removed_refs_and_consts<T...>(args...);
    
                //initialize py tuple with cpp tuple full with values
                PyObject* _args = _create("tuple", tup, NULL);
                if(_args == NULL){
                    setError(ErrorType::PYTHON_ERROR, std::to_string(27)+": fail to convert argument ");
                    return R();
                }
    
                //call python function
                PyObject* x = PyObject_CallObject(_obj.get(), _args);
                Py_DECREF(_args);
                
                //return with casting to cpp type
                if(x == NULL){
                    setError(ErrorType::PYTHON_ERROR, std::to_string(10)+": python error: "+_getPythonErrorString());
                    return R();
                }
                return _returnAsCppObject<R>(x);
            };

        }else return NULL;
        return obj;
    }
    
    template<typename R, typename... T>
    PyObject* _create(const std::string& type_name, std::function<R(T...)> &value, PyObject* obj){
        obj = PyCppType::NewPyCppObject(std::string("function_")+typeid(std::function<R(T...)>).name());//its type as name for function
        if(obj == NULL){
            setError(ErrorType::PYTHON_ERROR, "ERROR: can not create PyCppReflection for function ");
            return NULL;
        }
        PyCppType::setCallback(obj, [this,func=value](PyObject*, PyObject* args, PyObject*)->PyObject*{

            //prepare tuple type with types for function call
            auto tup = tuple_with_removed_refs_and_consts<T...>();
            
            //fill cpp tuple with values from py tuple
            //it will also validate number of parameters
            if(_cast("tuple", tup, args) == NULL){
                setError(ErrorType::PYTHON_ERROR, std::to_string(12)+": an error during convertion of arguemts");
                return NULL;
            }

            //call cpp function and return PyObject
            PyObject* ret = _callCppFunctionAndCastToPyObject<R>(func, tup);
            if(ret == NULL){
                setError(ErrorType::FATAL_ERROR, std::to_string(28)+": cpp function fail ");
                return NULL;
            }
 
            //update PyTuple elemts value (aka pass via reference) 
            //NOTE: only mutable PyObject-s (like list/dict) can be changed via reference !
            _update("tuple", tup, args);

            //return result PyObject
            return ret;

        });
        return obj;
    }
    
    template<typename R, typename... T>
    PyObject* _update(const std::string& type_name, std::function<R(T...)> &value, PyObject* obj){
        return obj;
    }


    /** allow to pass const and reference arguments */
    template <typename... T>
    using tuple_with_removed_refs_and_consts = std::tuple<typename std::remove_const<typename std::remove_reference<T>::type>::type... >;


    /** allows to handle void and any type as return for c++. assume obj is NOT NULL */
    template<typename R, std::enable_if_t<std::is_same<R, void>::value, bool> = true>
    R _returnAsCppObject(PyObject* obj){
        Py_DECREF(obj);
    }

    /** allows to handle void and any type as return for c++. assume obj is NOT NULL */
    template<typename R, std::enable_if_t<!std::is_same<R, void>::value, bool> = true>
    R _returnAsCppObject(PyObject* obj){
        R r;
        CastToCpp(r, obj);
        Py_DECREF(obj);
        return r;
    }

    /** allows to handle void and any type as return for python */
    template<typename R, typename F, typename T, std::enable_if_t<std::is_same<R, void>::value, bool> = true>
    PyObject* _callCppFunctionAndCastToPyObject(F f, T& t){
        std::apply(f, t);
        Py_RETURN_NONE;
    }

    /** allows to handle void and any type as return for python */
    template<typename R, typename F, typename T, std::enable_if_t<!std::is_same<R, void>::value, bool> = true>
    PyObject* _callCppFunctionAndCastToPyObject(F f, T& t){
        R r = std::apply(f, t);
        return CastToPy(r, NULL);
    }


    

    
    //-----------------------------------------------------------------------------------------------------
    
    // object
    
    template<typename B>
    PyObject* _cast(const std::string& type_name, std::shared_ptr<B> &value, PyObject* obj){
        if(type_name == "PyCppReflection"){
            std::shared_ptr<B> r = nullptr;
            try{
                r = std::dynamic_pointer_cast<B>( PyCppType::getObject(obj) );
            }catch(std::bad_cast& e){
                r = nullptr;
            }
            if(r == nullptr){//dynamic_pointer_cast can also return nullptr without throwing exception
                setError(ErrorType::CAST_ERROR, std::to_string(25)+": fail to cast types or empty object assign");
                return NULL;
            }
            value = r;
        }else{

            //TODO: use MRO here!

            // find PyCppReflection is in MRO list
            std::string type_name = "";
            for(PyTypeObject* type = obj->ob_type; type; type = type->tp_base){
                type_name = type->tp_name;
                if(type_name == "PyCppReflection"){
                    break;
                }
            }

            if(type_name != "PyCppReflection"){
                setError(ErrorType::CAST_ERROR, std::to_string(30)+": object is not an PyCppReflection or it does not inherit from it!");
                return NULL;
            }

            
            if(_cast("PyCppReflection", value, obj) == NULL){
                setError(ErrorType::CAST_ERROR, std::to_string(31)+": object PyCppReflection does not have cpp object assign or its object can not be dynamically casted to desired type");
                return NULL;
            }

            //object can not be nullptr here, otherwise _cast must fail
            //value should be same as *(data->object) here since _cast should convert it
            
            //for each class in MRO find functions that overrite cpp object functions - and update 
            // reflection to point to taht python function
            PyObject* mro = obj->ob_type->tp_mro;
            Py_ssize_t n = PyTuple_GET_SIZE(mro);
            for(Py_ssize_t i=0; i<n; i++){
                PyTypeObject* type = (PyTypeObject*)PyTuple_GetItem(mro, i);
                if(std::string(type->tp_name) == "PyCppReflection"){
                    //TODO: is here somethting to do !? getter for function sould be already set correctly
                }else{

                    Py_ssize_t pos = 0;
                    PyObject* k; PyObject* v; 
                    while (PyDict_Next(type->tp_dict, &pos, &k, &v)){ 
                        if(std::string(v->ob_type->tp_name) == "function"){
                            std::string key = PyUnicode_AsUTF8(k);
                            auto meth = std::shared_ptr<PyObject>(PyMethod_New(v, obj), [](void* o){Py_DECREF((PyObject*)o);});
                            auto f = value->getFieldDataByPath(key);
                            f.getter = [getter=f.setter, obj=meth](void* v, ssize_t*){ getter(v, (ssize_t*)obj.get()); };
                            value->updateFieldDataByPath(key, f);
                        }
                        //TODO: think about other types (maybe Method ?)
                    }

                    value->clearError();
                }
            }
            
        }

        return obj;
    }
    
    template<typename B>
    PyObject* _create(const std::string& type_name, std::shared_ptr<B> &value, PyObject* obj){
        if(!value){
            setError(ErrorType::PYTHON_ERROR, std::to_string(26)+": can not create PyObject from nullptr shared_ptr. ");
            return NULL;
        }
        
        //create object
        const std::string name = typeid(B).name();
        obj = PyCppType::NewPyCppObject(std::string("object_")+ name);
        if(obj == NULL){
            setError(ErrorType::PYTHON_ERROR, std::to_string(19)+": object not created. ");
            return NULL;
        }
        
        //assign cpp shared_ptr to py object
        PyCppType::setObject(obj, value);

        return obj;
    }
    
    template<typename B>
    PyObject* _update(const std::string& type_name, std::shared_ptr<B> &value, PyObject* obj){
        return obj;
    }
    


    //-----------------------------------------------------------------------------------------------------
    
    
    
        
protected:

    /** convert python object to c++ type via reference,
        it should pass through python object, or reutrn NULL on fail during conversion
    */

    template<typename T>
    PyObject* CastToCpp(T &value, PyObject* obj){

        if(obj == NULL){
            setError(ErrorType::PYTHON_ERROR, std::to_string(14)+": NULL object. expected: "+typeid(T).name());
            return NULL;
        }
        
        const std::string type_name(obj->ob_type->tp_name);
        obj = _cast(type_name, value, obj);
        if(obj == NULL){
            setError(ErrorType::PYTHON_ERROR, std::to_string(15)+": can not convert types: got: "+type_name+", expected: "+typeid(T).name());
            return NULL;
        }
        
        return obj;
    };
    
    /** convert cpp type passed via reference and convert it to python object
         when obj is NULL then it creates new object or return NULL on creation fail
         when obj is NOT NULL then it updates mutable object and pass through obj unless 
         there is an error, then is should return NULL
    */
    
    template<typename T>
    PyObject* CastToPy(T &value, PyObject* obj){
        
        if(obj == NULL){
            obj = _create("", value, obj);
        }else{
            obj = _update(obj->ob_type->tp_name, value, obj);
        }

        if(obj == NULL){
            setError(ErrorType::PYTHON_ERROR, std::to_string(34)+": can not convert types: expected: "+typeid(T).name());
            return NULL;
        }

        return obj;        
    };



};

