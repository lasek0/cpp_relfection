#pragma once
#include <string>

//TODO: support doc!

class PyCppModule {
public:
    template<typename F>
    static PyObject* createModule(const std::string _mod_name, const std::string _func_name, F func){

        static std::string mod_name(_mod_name);
        static std::string func_name = _func_name;
        static PyModuleDef examplemodule = {
            PyModuleDef_HEAD_INIT,
            mod_name.c_str(),
            "Example module that creates an extension type.",
            -1,
            NULL, NULL, NULL, NULL, NULL
        };
 
        PyObject* m = PyModule_Create(&examplemodule);
        if (m == NULL){
            return NULL;
        }
    
        static PyMethodDef method_def[] = {
            {_func_name.c_str(),
            (PyObject*(*)(PyObject*,PyObject*))func,
            METH_VARARGS,
            ""},//TODO: doc
            {NULL,NULL,0,NULL}
        };
        PyModule_AddFunctions(m, method_def);

        return m;

    }
};


