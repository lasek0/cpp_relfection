#include "PyCppType.h"
#include "IntPyCppReflection.h"

#if DEBUG>0
#include <iostream>
#endif


void PyCppType::setObject(PyObject* obj, std::shared_ptr<IntPyCppReflection> ref){
    (*((py_data*)obj)->object) = ref;
}

std::shared_ptr<IntPyCppReflection> PyCppType::getObject(PyObject* obj){
    return *((py_data*)obj)->object;
}

void PyCppType::setCallback(PyObject* obj, std::function<PyObject*(PyObject*, PyObject*, PyObject*)> func){
    (*((py_data*)obj)->callback) = func;
}

std::function<PyObject*(PyObject*, PyObject*, PyObject*)> PyCppType::getCallback(PyObject* obj){
    return *((py_data*)obj)->callback;
}


/** create new PyCppReflection python object */

PyObject* PyCppType::NewPyCppObject(const std::string name){
    PyTypeObject* type = getPyTypeObject();
    if(type == NULL){
        return NULL;
    }

    PyObject* args = PyTuple_New(1);
    PyTuple_SetItem(args, 0, PyUnicode_FromString(name.c_str()));
    PyObject* obj = PyObject_CallObject((PyObject*)type, args);//call _new and _init
    if(obj == NULL){
        Py_DECREF(args);
        return NULL;
    }
    Py_DECREF(args);

    return obj;
}



void PyCppType::_initPyTypeObject(PyTypeObject* type){
    type->tp_name = "PyCppReflection";
    type->tp_doc = "TODO:";
    type->tp_basicsize = sizeof(py_data);
    //type->tp_dictoffset = offsetof(py_data, dict);
    type->tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_FINALIZE;
    type->tp_repr = _repr;
    type->tp_call = _call;
    type->tp_getattro = _getattro;
    type->tp_setattro = _setattro;
    type->tp_new = _new;
    type->tp_init = _init;
    type->tp_dealloc = _dealloc;
}


PyTypeObject* PyCppType::getPyTypeObject() {

    static PyTypeObject _type;

    PyTypeObject* type = &_type;
    if(type->tp_name == NULL || std::string(type->tp_name) != "PyCppReflection"){
        _initPyTypeObject(type);
        if(PyType_Ready(type) < 0){
            return NULL;
        }
    }

    #if DEBUG>0
    std::cout << "PyCppType::getPyTypeObject type created " << type << " refcnt:" << ((PyObject*)type)->ob_refcnt << std::endl;
    #endif


    return type;
}

/// python operators


PyObject* PyCppType::_repr(PyObject * self){
    return PyUnicode_FromFormat("<PyCppReflection '%s' type:%s, cpp_ptr:%p [cnt:%d], py_ptr:%p [cnt:%d]>",
        self->ob_type->tp_name,
        ((py_data*)self)->name->c_str(),
        ((py_data*)self)->object->get(), ((py_data*)self)->object->use_count(),
        self, self->ob_refcnt
    );
}



PyObject* PyCppType::_call(PyObject *self, PyObject *args, PyObject *other){

    #if DEUG>0
    std::cout << "PyCppType::_call " << self << " " << (*((py_data*)self)->object).get() << std::endl;
    #endif

    //get object
    std::shared_ptr<IntPyCppReflection> object = (*((py_data*)self)->object);
    if(object){
        //TODO: discuss, this probably makes impossible to call getErrorMsg because it clear whatever error is currently set
        object->clearError();
    }

    PyObject* ret = (*((py_data*)self)->callback)(self, args, other);
    if(object && object->getErrorCode()){
        PyErr_Format(PyExc_Exception, "%s", object->getErrorMsg().c_str());
        Py_XDECREF(ret);
        return NULL;
    }
    if(ret == NULL){
        PyErr_Format(PyExc_Exception, "an unspecified error occour");
        return NULL;
    }

    if(std::string(ret->ob_type->tp_name) == "PyCppReflection"){
        //allows errors to be forwarded if returned object does not have any cpp object set
        if((*((py_data*)ret)->object) == nullptr){
            (*((py_data*)ret)->object) = object;
        }
    }

    return ret;

}



PyObject* PyCppType::_getattro(PyObject* self, PyObject* attr){
    std::string name = PyUnicode_AsUTF8(attr);
    std::shared_ptr<IntPyCppReflection> object = *((py_data*)self)->object;

    #if DEBUG>0
    std::cout << "PyCppType::_getattro " << self << " " << name << " " << object.get() << std::endl;
    #endif

    if(name == "__dict__"){
        PyObject* item = ((py_data*)self)->dict; // local variable
        Py_INCREF(item);
        return item;
    }
    if(name == "__class__"){
        PyObject* item = (PyObject*)self->ob_type;
        Py_INCREF(item);
        return item;
    }

    PyObject* item = PyDict_GetItem(((py_data*)self)->dict, attr);
    if(item){
        Py_INCREF(item);
        return item;
    }


    //TODO: refactor this tower!

    // same as find_name_in_mro
    PyObject* mro = self->ob_type->tp_mro;
    Py_ssize_t n = PyTuple_GET_SIZE(mro);
    for(Py_ssize_t i=0; i<n; i++){
        PyTypeObject* type = (PyTypeObject*)PyTuple_GetItem(mro, i);
        if(std::string(type->tp_name) == "PyCppReflection"){
            std::shared_ptr<IntPyCppReflection> object = *((py_data*)self)->object;
            if(object){
                auto data = object->getAttributeDataByKey(name);
                if(data.mode != ReflectionMode::REFLECT_NONE){
                    PyObject* item = data.getter();
                    if(item){
                        if(std::string(item->ob_type->tp_name) == "PyCppReflection"){
                            if(*((py_data*)item)->object == nullptr){
                                *((py_data*)item)->object = object;
                            }
                        }
                        return item;
                    }else{
                        PyErr_Format(PyExc_Exception, "%s", object->getErrorMsg().c_str());
                        return NULL;
                    }
                }
            }else{
                PyErr_Format(PyExc_Exception, "Somethting went really wrong - no cpp object!");
                return NULL;
            }
        }else{
            PyObject* dict = type->tp_dict;
            PyObject* ret = PyDict_GetItem(dict, attr);
            if(ret){
                if(std::string(ret->ob_type->tp_name) == "function"){
                    return PyMethod_New(ret, self);
                }
                Py_INCREF(ret);//keep refcnt in dict
                return ret;
            }
        }
    }

    PyErr_Format(PyExc_Exception, "attribute '%s' not available for __get__.", name.c_str());
    return NULL;

}


int PyCppType::_setattro(PyObject* self, PyObject* attr, PyObject* value){
    std::string name = PyUnicode_AsUTF8(attr);
    std::shared_ptr<IntPyCppReflection> object = *((py_data*)self)->object;

    #if DEBUG>0
    std::cout << "PyCppReflection::_setattro " << self << " " << name << std::endl;
    #endif


    //TODO: refactor this tower!

    PyObject* mro = self->ob_type->tp_mro;
    Py_ssize_t n = PyTuple_GET_SIZE(mro);
    for(Py_ssize_t i=0; i<n; i++){
        PyTypeObject* type = (PyTypeObject*)PyTuple_GetItem(mro, i);
        if(std::string(type->tp_name) == "PyCppReflection"){
            std::shared_ptr<IntPyCppReflection> object = *((py_data*)self)->object;
            if(object){
                auto data = object->getAttributeDataByKey(name);
                if(data.mode != ReflectionMode::REFLECT_NONE){
                    int ret = data.setter(value);
                    if(ret < 0){
                        PyErr_Format(PyExc_Exception, "%s", object->getErrorMsg().c_str());
                    }
                    return ret;
                }
            }else{
                PyErr_Format(PyExc_Exception, "Somethting went really wrong - no cpp object!");
                return -1;
            }
        }else{
            PyObject* dict = type->tp_dict;
            PyObject* ret = PyDict_GetItem(dict, attr);
            if(ret){
                return PyDict_SetItem(dict, attr, value);
            }
        }
    }


    return PyDict_SetItem(((py_data*)self)->dict, attr, value);

}


PyObject* PyCppType::_new(PyTypeObject* type, PyObject* args, PyObject* kwds){

    //generate python object
    PyObject* self = PyType_GenericNew(type, args, kwds);
    if(self == NULL){
        PyErr_Format(PyExc_Exception, "generic python type generator error");
        return NULL;
    }

    py_data* obj = (py_data*)self;

    //allocate 
    obj->object = new std::shared_ptr<IntPyCppReflection>();
    obj->callback = new std::function<PyObject*(PyObject* _self, PyObject* _args, PyObject *_other)>();
    obj->name = new std::string;
    obj->dict = PyDict_New();

    //set default data
    *(obj->object) = nullptr;
    *(obj->callback) = [](PyObject*,PyObject*,PyObject*)->PyObject*{ Py_RETURN_NONE; };
    *(obj->name) = "";

    #if DEBUG>0
    std::cout << "PyCppType::_new " << obj << " " << *(obj->name) << std::endl;
    #endif

    return self;
}


int PyCppType::_init(PyObject* self, PyObject* args, PyObject* kwds){

    py_data* _self = (py_data*)self;

    #if DEBUG>0
    std::cout << "PyCppType::_init " << self << std::endl;
    #endif

    Py_ssize_t size = PyTuple_GET_SIZE(args);

    if(size <= 0){
        PyErr_Format(PyExc_Exception, "%s", "missing arguments" );
        Py_DECREF(self);
        return -1;
    }

    //assign name to pyobject
    if(size > 0){
        PyObject* item = PyTuple_GetItem(args, 0);
        if(std::string(item->ob_type->tp_name) == "str"){
            *(_self->name) = PyUnicode_AsUTF8(item);
        }
    }

    //TODO: use MRO here!

    //assign object shared_ptr to pyobject
    if(size > 1){
        PyObject* item = PyTuple_GetItem(args, 1);
        for(PyTypeObject* type = item->ob_type; type; type = type->tp_base){
            if(std::string(type->tp_name) == "PyCppReflection"){
                py_data* _item = (py_data*)item;
                if(*(_self->object) == nullptr){
                    *(_self->object) = *(_item->object);
                }
                break;
            }
        }
    }

    return 0;
}

void PyCppType::_dealloc(PyObject* self){

    #if DEBUG>0
    std::cout << "PyCppType::_dealloc " << self << " " << *((py_data*)self)->name << std::endl;
    #endif

    py_data* obj = (py_data*)self;

    //free objects
    if(obj->object) delete obj->object;
    if(obj->callback) delete obj->callback;
    if(obj->name) delete obj->name;
    Py_XDECREF(obj->dict);

    //run free for sub types
    self->ob_type->tp_free(self);

}




