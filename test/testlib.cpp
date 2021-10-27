#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <tuple>
#include <functional>
#include <memory>

#include "IntError.h"
#include "Error.h"
#include "IntReflection.h"
#include "Reflection.h"
#include "IntPyCppReflection.h"
#include "PyCppReflection.h"
#include "PyCppModule.h"


class SubClass : virtual public PyCppReflection {
};


class TestClass : virtual public PyCppReflection {
    int number;
    std::string name;
    std::vector<float> points;
    std::basic_string<unsigned char> data;
    std::shared_ptr<SubClass> subclass;
private:

protected:

public:

    TestClass():number(1337),name("Hax"),points({(float)1.1}),subclass(new SubClass){
        
        reflectAttribute("number", number);
        reflectAttribute("name", name);
        reflectAttribute("points", points);
        reflectAttribute("data", data);
        reflectAttribute("subclass", subclass);
        
        reflectAttribute("test", this, &TestClass::test);
        reflectAttribute("test2", this, &TestClass::test2);
        reflectAttribute("test3", this, &TestClass::test3);
        reflectAttribute("test4", this, &TestClass::test4);
        reflectAttribute("test5", this, &TestClass::test5);
        reflectAttribute("test6", this, &TestClass::test6);
        reflectAttribute("test7", this, &TestClass::test7);
        reflectAttribute("test8", this, &TestClass::test8);
        reflectAttribute("test9", this, &TestClass::test9);
        reflectAttribute("test10", this, &TestClass::test10);
        reflectAttribute("test11", this, &TestClass::test11);
        reflectAttribute("test12", this, &TestClass::test12);
        reflectAttribute("test13", this, &TestClass::test13);
        reflectAttribute("test14", this, &TestClass::test14);
        reflectAttribute("test15", this, &TestClass::test15);
        reflectAttribute("test16", this, &TestClass::test16);
        reflectAttribute("test17", this, &TestClass::test17);
        reflectAttribute("test18", this, &TestClass::test18);
        reflectAttribute("test19", this, &TestClass::test19);
        reflectAttribute("test20", this, &TestClass::test20);
        reflectAttribute("test21", this, &TestClass::test21);
        reflectAttribute("test22", this, &TestClass::test22);
        reflectAttribute("add", this, &TestClass::add);
        reflectAttribute("sub", this, &TestClass::sub);
        reflectAttribute("mul", this, &TestClass::mul);
        reflectAttribute("div", this, &TestClass::div);
        reflectAttribute("say", this, &TestClass::say);
        reflectAttribute("mul2", this, &TestClass::mul2);
        reflectAttribute("mul1", this, &TestClass::mul1);
        reflectAttribute("poli", this, (void(TestClass::*)(int))&TestClass::pol);//add overloaded functions (need explicite cast and different name)
        reflectAttribute("pold", this, (void(TestClass::*)(double))&TestClass::pol);//add overloaded functions (need explicite cast and different name)
        reflectAttribute("array", this, &TestClass::array);
        reflectAttribute("array2", this, &TestClass::array2);
        reflectAttribute("array3", this, &TestClass::array3);
        reflectAttribute("array4", this, &TestClass::array4);//vectors of vector
        reflectAttribute("array5", this, &TestClass::array5);//vectors of vector
        reflectAttribute("func", this, &TestClass::func);
    }
    ~TestClass(){
    }
    

    void test(int a, int b, int c, double d){
        std::cout << " !!test!! "
            << a << " "
            << b << " "
            << c << " "
            << d << " " 
            <<std::endl;
    }
    
    
    void test2(float a,int b, unsigned short c){
        std::cout << " !!test2!! " 
            << a*2 << " "
            << b*2 << " "
            << c*2 << " "
            <<std::endl;
    }
    
    void test3(int a,double d){
        std::cout  << " !!test3!! "
            << a*3 << " "
            << d*3 << " "
            <<std::endl;
    }
    
    
    //void test4(const char* v){
    //    if(!v) v = "NULL!";
    //const char* as str is o longer used due to problems with dangling buffer 
    void test4(std::string v){
        std::cout  << " !!test4!! "
            << v 
            <<std::endl;
    }
    
    
    int* test5(int* x){
        if(!x) x = &number;
        std::cout  << " !!test5!! "
            << *x 
            <<std::endl;
        *x *= 2;
        return x;
    }
    
    void test6(std::string v){
        std::cout  << " !!test6!! "
            << v 
            << std::endl;
    }
    
    std::string test7(){
        return std::string("std::string to string");
    }
    
    std::string test8(std::function<std::string(int)> f){
        std::cout << "test8" << std::endl;
        std::string s = f(5);
        std::cout << "test8 errors:" << getErrorMsg() << " text:" << s << std::endl;
        return s;
    }
    
    void test9(std::function<void(int)> f){
        f(15);
        std::cout << "test8 errors:" << getErrorMsg() << std::endl;
    }
    
    void test10(std::map<std::string,double> m){
        for(auto it : m){
            std::cout << it.first << ": " << it.second << std::endl;
        }
    }
    
    std::map<std::string,double> test11(){
        return {
            {"A", 10.10},            
            {"B", 11.11},            
            {"C", 12.12},            
        };
    }
    
    std::function<std::string(std::vector<int>)> test12(){
        std::cout << "test12" << std::endl;
        return [](std::vector<int> v)->std::string {
            std::cout << "test12 lambda" << std::endl;
            std::string s = "";
            for(auto it : v){
                s += std::to_string(it)+"+";
            }
            return s;
        };
    }
    
    void test13(std::tuple<int,double,std::string> tuple){
        std::cout 
            << std::get<0>(tuple) << " "
            << std::get<1>(tuple) << " "
            << std::get<2>(tuple) << " "
            << std::endl;
    }
    
    std::tuple<int,double,std::string> test14(){
        std::tuple<int,double,std::string> tuple;
        std::get<0>(tuple) = 0x41;
        std::get<1>(tuple) = 3.1415;
        std::get<2>(tuple) = "Hello World in tuple";
        return tuple;
    }
    

    void test15(std::basic_string<unsigned char> bytes){
        const char* h = "0123456789ABCDEF";
        for(auto it : bytes){
            std::cout << h[it>>4] << h[it&15] << " ";
        }
        std::cout << std::endl;
    }
    
    std::basic_string<unsigned char> test16(){
        std::basic_string<unsigned char> bytes((const unsigned char*)"\x41\x42\x04\xfe\x00\xff", 6);
        bytes += (const unsigned char*)"\x00";
        bytes += (const unsigned char*)"\xff";
        return bytes;
    }
    
    void test17(size_t l){
        std::cout << std::hex << l << std::endl;
    }
    
    size_t test18(){
        return 0x4343434346464646;
    }
    
    void test19(std::function<std::string(std::vector<int>)> f){
        std::cout << "test19 " << f({5,4,3,2,1,0}) << std::endl;
        std::cout << "error message set by function: " << getErrorMsg() << std::endl;
    }
    
    
    std::map<double,std::string> test20(){
        return {
            {10.10, "A"},
            {11.11, "B"},
            {12.12, "C"},
        };
    }
    
    std::map<std::vector<int>,std::string> test21(){
        return {
            {{1,1}, "A"},
            {{1,2}, "B"},
            {{2,1}, "C"},
        };
    }

    std::string func(int n){
        return std::to_string(n*100);
    }    

    void test22(std::shared_ptr<TestClass> obj){
// function "std::string func(int){}"
        std::cout << "calling function from obj "
            << obj->get<std::function<std::string(int)>>("func")(5) 
            << std::endl;
    }
    
    int add(int a, int b){
        return a+b;
    }
    int sub(int a, int b){
        return a-b;
    }
    int mul(int a, int b){
        return a*b;
    }
    double div(int a, int b){
        if(b==0.0) return std::numeric_limits<double>::quiet_NaN();
        return a/b;
    }

    const char* say(){
        return "Hello World!";
    }
    
    virtual void mul1(int a){
        a *= 2;
    }
    
    //NOTE int in python is not muttable, can not return via reference
    virtual void mul2(int& a){
        a *= 2;
    }

    void pol(int a){
        std::cout << "pol int " << a << std::endl;
    }
    void pol(double a){
        std::cout << "pol double " << a << std::endl;
    }
    
    
    void array(std::vector<int>& x){
        for(size_t i=0; i<x.size(); i++){
            std::cout << x[i] << ", ";
            x[i] *= 5;
        }
        std::cout << std::endl;
    }
    
    //void array2(std::vector<const char*>& x){//const char* as str no longer used due to problem with dangling pointer
    void array2(std::vector<std::string>& x){
        for(size_t i=0; i<x.size(); i++){
            std::cout << x[i] << ", ";
            x[i] = "Hello World!";
        }
        std::cout << std::endl;
    }
    
    
    std::vector<int> array3(int len){
        std::vector<int> x;
        x.resize(len);
        for(size_t i=0; i<x.size(); i++){
            x[i] = i*10;
        }
        return x;
    }

    void array4( std::vector< std::vector<int> >& x ){
        std::vector< std::vector<int> > y;
        std::cout << "array4" << std::endl;
        for(size_t i=0; i<x.size(); i++){
            y.push_back( {} );
            for(size_t j=0; j<x[i].size();j++){
                std::cout << "  " << x[i][j];
                y[i].push_back(x[i][j]);
                y[i].push_back(x[i][j]*2);
            }
            std::cout << " " << std::endl;
        }
        
        x = y;
    }
    
    
    std::vector< std::vector<int> > array5(){
        
        std::vector< std::vector<int> > x;
        
        x.resize(3);
        for(int i=0; i<3; i++){
            x[i].resize(3);
            for(int j=0; j<3;j++){
                x[i][j] = (i+1)*(j+1);
            }
        }
        return x;
    }

};




PyMODINIT_FUNC
PyInit_test(void)
{

    return PyCppModule::createModule("test", "testclass", [](PyObject*, PyObject*)->PyObject*{
        std::shared_ptr<TestClass> o(new TestClass);
        return o->createPyObjectFromPyCppReflection(o);
    });

}


