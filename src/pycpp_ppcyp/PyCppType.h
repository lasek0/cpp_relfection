#pragma once
#include "IntPyCppReflection.h"

//need IntPyCppReflection class
//need getAttributeDataByKey in IntPyCppReflection
//NOTE: full static
class PyCppType {
private:
    //disallow creation of this class
    inline PyCppType(){}
    inline PyCppType(const PyCppType&){}
    inline void operator=(const PyCppType&){}
    inline ~PyCppType(){}

    struct py_data {
        PyObject_HEAD // WARNING: do not type ';' after PyObject_HEAD, it can cause compilation error
        std::shared_ptr<IntPyCppReflection> *object; // IT MUST have getAttributeDataByKey method!
        std::function<PyObject*(PyObject*, PyObject*, PyObject*)> *callback;
        std::string *name;
        PyObject* dict;
    };

public:

    static void setObject(PyObject* obj, std::shared_ptr<IntPyCppReflection> ref);
    static std::shared_ptr<IntPyCppReflection> getObject(PyObject* obj);
    static void setCallback(PyObject* obj, std::function<PyObject*(PyObject*, PyObject*, PyObject*)> func);
    static std::function<PyObject*(PyObject*, PyObject*, PyObject*)> getCallback(PyObject* obj);
    static PyObject* NewPyCppObject(const std::string name);

private:

    static void _initPyTypeObject(PyTypeObject* type);
    static PyTypeObject* getPyTypeObject();


    /// python operators

    static PyObject* _repr(PyObject * self);
    static PyObject* _call(PyObject *self, PyObject *args, PyObject *other);
    static PyObject* _getattro(PyObject* self, PyObject* attr);
    static int _setattro(PyObject* self, PyObject* attr, PyObject* value);
    static PyObject* _new(PyTypeObject* type, PyObject* args, PyObject* kwds);
    static int _init(PyObject* self, PyObject* args, PyObject* kwds);
    static void _dealloc(PyObject* self);

};


