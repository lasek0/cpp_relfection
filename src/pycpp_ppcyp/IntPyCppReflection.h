#pragma once
#include <string>
#include <memory>
#include <functional>
#include <string>
#include <Python.h>
#include "IntReflection.h"

/** base class for python-c++ class */

class IntPyCppReflection : virtual public IntReflection {
protected:
    struct attr_data {
        std::function<int(PyObject*)> setter;
        std::function<PyObject*(void)> getter;
        const ReflectionMode mode;
    };
    
public:
    virtual inline ~IntPyCppReflection(){}
    virtual attr_data getAttributeDataByKey(const std::string) = 0;
    virtual PyObject* createPyObjectFromPyCppReflection(std::shared_ptr<IntPyCppReflection>) = 0;
    virtual std::shared_ptr<IntPyCppReflection> createPyCppReflectionFromPyObject(PyObject*) = 0;

    template <typename T>
    std::shared_ptr<T> createPyCppReflectionFromPyObject(PyObject* obj){
        std::shared_ptr<IntPyCppReflection> r = createPyCppReflectionFromPyObject(obj);
        //TODO: check r == nullptr and try chatch
        return std::dynamic_pointer_cast<T>(r);
    }
protected:
    virtual void _addAttribute(const std::string key, std::function<int(PyObject*)> setter, std::function<PyObject*(void)> getter, ReflectionMode mode) = 0;
    
};

