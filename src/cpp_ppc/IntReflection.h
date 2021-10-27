#pragma once
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <typeinfo>
#include <typeindex>
#include "IntError.h"

//set/get support with auto registration

///NOTE: class template, it will be generated in derived class
///NOTE: field_data is returned by copny on purpose because I want to avoid situation when 
///      in multithreading, one thread read data from map, and second modifyis it!

enum ReflectionMode : int {
    REFLECT_NONE=0, REFLECT_SETTER_ONLY=1, REFLECT_GETTER_ONLY=2, REFLECT_BOTH=3
};

/** support set and get methods to achive c++ reflections */

class IntReflection : virtual public IntError {
protected:

    //internal use only struct
    struct field_data {
        const std::type_index type;
        std::function<void(void*, ssize_t*)> setter;
        std::function<void(void*, ssize_t*)> getter;
        const ReflectionMode mode;
        std::shared_ptr<IntReflection> subobject;
    };

public:
    virtual inline ~IntReflection(){}
    virtual field_data getFieldDataByPath(const std::string key) = 0;
    virtual void updateFieldDataByPath(const std::string, const field_data&) = 0;
    virtual std::vector<std::string> getReflectionList() = 0;
    virtual std::string getTypeNameFromPath(std::string key) = 0;

protected:
    virtual void callFieldFunction(std::function<void(void*, ssize_t*)> f, void*, ssize_t*) = 0;

private:

    template<typename T>
    field_data _find_and_check_type(const std::string key){
        clearError();//TODO: this might cause problem with getting previous error via property :(

        field_data v = getFieldDataByPath(key);

        const std::type_index type = typeid(T);
        if(v.type != type){
            setError(ErrorType::TYPE_ERROR, std::string()+"invalid type cast, got:"+type.name()+" expected: "+v.type.name());
            return getFieldDataByPath(""); //return _do_nothing
        }

        return v;
    }
public:

    //allows: double value; int len=4; get("key", value, len);
    template<typename T>
    void get(const std::string key, T& val, ssize_t &len){
        callFieldFunction(_find_and_check_type<T>(key).getter, const_cast<void*>(reinterpret_cast<const void*>(&val)), &len);
    }

    //allows: double value; get("key", value, 10); // constat number as length
    //allows: double value; get("key", value); // deduce len as sizeof(T)
    template<typename T>
    void get(const std::string key, T& val, const ssize_t &len = sizeof(T)){
        ssize_t _len = len;
        get(key, val, _len);
    }

    //allows: char buf[128]; get("key", buf, 128);
    //allows: char buf[128]; get("key", buf); // NOTE len is equal to S
    template<typename T, size_t S>
    void get(const std::string key, T (&val)[S], const int &len = S){
        T *_tmp = &val[0]; // fix error: cannot bind non-const lvalue reference of type 'char*&' to an rvalue of type 'char*'
        ssize_t _len = len;
        get(key, _tmp, _len);
    }

    //allows: char buf[128]; int len=128; get("key", buf, len);
    template<typename T, size_t S>
    void get(const std::string key, T (&val)[S], ssize_t &len){
        T *_tmp = &val[0]; // fix error: cannot bind non-const lvalue reference of type 'char*&' to an rvalue of type 'char*'
        get(key, _tmp, len);
    }

    //allows: int len=0; double value = get<double>("key", len); //NOTE: less preffered
    template<typename T>
    T get(const std::string key, ssize_t &len){
        T val;
        get(key, val, len);
        return val;
    }

    //allows: double value = get<double>("key"); //NOTE: less preffered
    template<typename T>
    T get(const std::string key){
        T val;
        get(key, val, sizeof(T));
        return val;
    }

    //allows: char* buffer = new char[20]; set("name", buffer, 128);
    //allows: const char* buffer = "text"; set("name", buffer, 4);
    //allows: set("port", 8080);
    //allows: set("pi", 3.1415);
    template<typename T>
    void set(const std::string key, const T &val, const ssize_t &_len = sizeof(T)){
        ssize_t len = _len;
        callFieldFunction(_find_and_check_type<T>(key).setter, const_cast<void*>(reinterpret_cast<const void*>(&val)), &len);
    }

    //allows: set("msg", "asdf", 4);
    //allows: char buf[5]; set("msg", buf, sizeof(buf));
    //allows: char buf[5]; set("name", buf);//deduce size
    template<typename T, size_t S>
    void set(const std::string key, T (&val)[S], const ssize_t &len = S){
        set(key, (T*)(&val[0]), len);
    }

};


