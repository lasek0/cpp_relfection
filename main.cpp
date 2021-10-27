#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <tuple>
#include <functional>
#include <memory>
#include <cstring>

#include "IntError.h"
#include "Error.h"
#include "IntReflection.h"
#include "Reflection.h"
#include "IntPyCppReflection.h"
#include "PyCppReflection.h"
#include "PyCppModule.h"





    
// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------


// EXAMPLE USAGE



class IntObject : virtual public IntError, virtual public IntReflection, virtual public IntPyCppReflection {
public:
    virtual std::string getObjectName() = 0;
    virtual inline ~IntObject(){}
};


class Object : virtual public IntObject, virtual public Error, virtual public Reflection, virtual public PyCppReflection {
private:
    std::string _name;
    std::shared_ptr<IntObject> _api;
public:
    Object(std::string name):_name(name){
        reflectAttribute("name", _name);
        reflectAttribute("getErrorMsg", static_cast<Error*>(this), &Error::getErrorMsg);//add attribute from base class
        reflectAttribute("getErrorCode", static_cast<Error*>(this), &Error::getErrorCode);//add attribute from base class
        reflectAttribute("getObjectName", this, &Object::getObjectName);
    }
    virtual inline ~Object(){
    }
    virtual std::string getObjectName(){
        return _name;
    }
    
};


// ----------------------------------------------------------------------------------------------------


class IIo : virtual public IntObject {
public:
    virtual inline ~IIo(){}
    virtual void open() = 0;
    virtual void close() = 0;
    virtual bool isOpened() = 0;
    virtual void write(std::basic_string<unsigned char> buf) = 0;
    virtual std::basic_string<unsigned char> read(const int len) = 0;
    
};


class Io : virtual public IIo, virtual public Object {
protected:
    bool _opened;
public:

        
    Io():Object("Io"),_opened(false){
        reflectAttribute("opened", _opened, ReflectionMode::REFLECT_GETTER_ONLY);
        reflectAttribute("isOpened", this, &Io::isOpened);
        reflectAttribute("open", this, &Io::open);
        reflectAttribute("close", this, &Io::close);
        reflectAttribute("write", this, &Io::write);
        reflectAttribute("read", this, &Io::read);
    }

    virtual inline ~Io(){
        
    }

    virtual void open(){
        
    }

    virtual void close(){
        
    }

    virtual bool isOpened(){
        return _opened;
    }

    
    virtual void write(std::basic_string<unsigned char> buf){
        
    }
    
    virtual std::basic_string<unsigned char> read(const int len){
        return {};
    }

    
};


// ----------------------------------------------------------------------------------------------------


class IntLog : virtual public IntObject {
public:
    virtual void operator<<(const std::string m) = 0;
    virtual inline ~IntLog(){}
    virtual std::string getMsg() = 0;
    virtual void setMsg(const std::string) = 0;
};


class Log : virtual public IntLog, virtual public Object {
private:
    std::string msg;
public:
    virtual void operator<<(const std::string m){
        msg += m;
    }
    
    Log():Object("Log"){
        reflectAttribute("msg", msg);
        reflectAttribute("getMsg", this, &Log::getMsg);
        reflectAttribute("setMsg", this, &Log::setMsg);
    }
    
    virtual inline ~Log(){
    }
    
    virtual std::string getMsg(){
        return msg;
    }
    
    virtual void setMsg(const std::string m){
        msg += m + "\n";
    }
    
    
};


// ----------------------------------------------------------------------------------------------------


class IntFile : virtual public IntObject, virtual public IIo {
public:
    virtual inline ~IntFile(){}
    virtual void setFielaName(std::string) = 0;
};


class File : virtual public IntFile, virtual public Object, public Io {
protected:
    std::string _file_name;
    std::shared_ptr<IntLog> _log;
public:    
    File():Object("File"),_log(new Log){
        reflectAttribute("filename", _file_name);
        reflectAttribute("log", _log);
        reflectAttribute("setFielaName", this, &File::setFielaName);
        reflectAttribute("write", this, &File::write);
        reflectAttribute("read", this, &File::read);
    }
    
    virtual inline ~File(){
        
    }

    void setFielaName(std::string name){
        _file_name = name;
    }
    

    virtual void open(){
        _opened = true;
    }
    
    virtual void close(){
        _opened = false;
    }

    virtual void write(std::basic_string<unsigned char>& buf) {
        static const char* nibles = "0123456789ABCDEF";
        for(auto it : buf)
            *_log << std::string()+nibles[it>>4]+nibles[it&15];
        *_log << std::string("\n");
    }
    
    virtual std::basic_string<unsigned char> read(const int len) {
        std::basic_string<unsigned char> buf;
        for(int i=0; i<len; i++)
            buf += (unsigned char)i;
        return buf;
    }
  
};


// ----------------------------------------------------------------------------------------------------


class IntTcp : virtual public IntObject, virtual public IIo {
public:
    virtual inline ~IntTcp(){}
    virtual void setIp(std::string) = 0;
    virtual void setPort(unsigned short) = 0;
};


class Tcp : virtual public IntTcp, virtual public Object, public Io {
protected:
    std::string _ip;
    unsigned short _port;
    std::shared_ptr<IntLog> _log;
public:
    Tcp():Object("Tcp"),_log(new Log),_ip(""),_port(0){
        reflectAttribute("ip", _ip);
        reflectAttribute("port", _port);
        reflectAttribute("log", _log);
        reflectAttribute("setIp", this, &Tcp::setIp);
        reflectAttribute("setPort", this, &Tcp::setPort);
        reflectAttribute("open", this, &Tcp::open);
        reflectAttribute("close", this, &Tcp::close);
        reflectAttribute("write", this, &Tcp::write);
        reflectAttribute("read", this, &Tcp::read);
        reflectAttribute("resetLogObject", this, &Tcp::resetLogObject);
    }
    
    virtual inline ~Tcp(){
        
    }
    
    virtual void resetLogObject(){
        _log = std::shared_ptr<IntLog>(new Log);
    }

    virtual void setIp(std::string ip){
        _ip = ip;
    }
    virtual void setPort(unsigned short port){
        _port = port;
    }


    virtual void open(){
        _opened = true;
    }
    
    virtual void close(){
        _opened = false;
    }

    virtual void write(std::basic_string<unsigned char>& buf) {
        if(isOpened()==false){
            setError(ErrorType::STATUS_ERROR, "status not opened!");
            return;
        }
        static const char* nibles = "0123456789ABCDEF";
        std::stringstream ss;
        for(auto it : buf)
            ss << nibles[it>>4] << nibles[it&15];
        ss << "\n";
        
        //_log->setMsg(ss.str());

        //std::function<void(const std::string)> setMsg;
        //_log->get("setMsg", setMsg);
        //setMsg(ss.str());
        _log->get<std::function<void(const std::string)>>("setMsg")(ss.str()+" write called\n"); // same as 3 above
    }
    
    virtual std::basic_string<unsigned char> read(const int len) {
        std::basic_string<unsigned char> buf;
        
        
        if(isOpened()==false){
            setError(ErrorType::STATUS_ERROR, "status not opened!");
            return buf;
        }
        for(int i=0; i<len; i++)
            buf += (unsigned char)(_log->getMsg().size() + i);
        
        _log->get<std::function<void(const std::string)>>("setMsg")("read called\n"); // same as 3 above
        
        return buf;
    }
  
};


// ----------------------------------------------------------------------------------------------------


class IntTestClass : virtual public IntObject {
public:
    virtual ~IntTestClass(){}
};


class TestClass : virtual public IntTestClass, virtual public Object {
    int number;
    std::string name;
    std::vector<float> points;
    std::basic_string<unsigned char> data;
private:

protected:

public:

    TestClass(): Object("TestClass"),number(1337),name("Yea!"),points({(float)1.1}){
        
        reflectAttribute("number", number);
        reflectAttribute("name", name);
        reflectAttribute("points", points);
        reflectAttribute("data", data);
        
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
    
    void test22(std::shared_ptr<IntObject> obj){
        std::cout << "calling function from obj "
            << obj->get<std::function<std::string(int)>>("func")(5) // function "std::string func(int){}"
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


// ----------------------------------------------------------------------------------------------------


class IntEmpty : virtual public IntObject {
public:
    virtual ~IntEmpty(){}
};


class Empty : virtual public IntEmpty, virtual public Object {
public:
    Empty(): Object("Empty"){
    }
    virtual ~Empty(){
    }
};


// ----------------------------------------------------------------------------------------------------


class IntApi : virtual public IntObject {
public:
    virtual std::shared_ptr<IntFile> createFileInstance() = 0;
    virtual std::shared_ptr<IntTcp> createTcpInstance() = 0;
    virtual std::shared_ptr<IntLog> createLogInstance() = 0;
    virtual std::shared_ptr<IntEmpty> createEmptyInstance() = 0;
    virtual std::shared_ptr<IntTestClass> createTestInstance() = 0;
    virtual inline ~IntApi(){}
};


class Api : virtual public IntApi, virtual public Object {
private:

    template<typename T>
    std::shared_ptr<T> registerObject(T* _obj){
        return std::shared_ptr<T>(_obj);
    }
    
    
public:
    Api():Object("Api"){
        reflectAttribute("file", this, &Api::createFileInstance);
        reflectAttribute("tcp", this, &Api::createTcpInstance);
        reflectAttribute("log", this, &Api::createLogInstance);
        reflectAttribute("test", this, &Api::createTestInstance);
        reflectAttribute("empty", this, &Api::createEmptyInstance);
    }

    virtual ~Api(){
    }
    
    virtual std::shared_ptr<IntFile> createFileInstance(){
        return registerObject(new File); 
    };
    
    virtual std::shared_ptr<IntTcp> createTcpInstance(){
        return registerObject(new Tcp); 
    };

    virtual std::shared_ptr<IntTestClass> createTestInstance(){
        return registerObject(new TestClass); 
    };
    
    virtual std::shared_ptr<IntLog> createLogInstance(){
        return registerObject(new Log);
    }
    
    virtual std::shared_ptr<IntEmpty> createEmptyInstance(){
        return registerObject(new Empty);
    }
};


// ----------------------------------------------------------------------------------------------------





PyMODINIT_FUNC
PyInit_example(void)
{
 
    return PyCppModule::createModule("example", "api", [](PyObject*, PyObject*)->PyObject*{ 
        std::shared_ptr<IntApi> o(new Api); 
        return o->createPyObjectFromPyCppReflection(o);
    });

}


// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------



// USAGE TEST


void printPropertyList(std::shared_ptr<IntObject> o, int width = 40){
    for(auto s : o->getReflectionList()){
        if(s.size() == 0) continue;
        std::cout.width(width);
        std::cout << std::left << s << " ";
        std::cout << o->getTypeNameFromPath(s) << std::endl;
    }
}


void test_cpp(){
    
    //testing garbage collector 
    std::shared_ptr<IntApi> api(new Api);
    
    std::cout << "----------------------------\n";
    auto tcp = api->createTcpInstance();// create instance, You can use auto type to do magic to create std::shared_ptr<IntTcp>
    
    std::cout << "----------------------------\n";
    std::cout << api->getObjectName() << std::endl; // call like normal IntObject*, but with std::shared_ptr giard keeping!!!
    std::cout << "----------------------------\n";
    printPropertyList(api);
    
    std::cout << "----------------------------\n";
    std::cout << tcp->getObjectName() << std::endl;
    std::cout << "----------------------------\n";
    printPropertyList(tcp);
    
    std::cout << "----------------------------\n";
    
    std::cout << "----------------------------\n";
    std::shared_ptr<IntLog> log;
    tcp->get("log", log);
    std::cout << tcp->getErrorMsg() << std::endl;
    std::cout << "----------------------------\n";
    
    std::cout << log->getObjectName() << std::endl;
    
    std::cout << "----------------------------\n";
    printPropertyList(log);
    
    std::cout << "----------------------------\n";
    
    //call registred function via property!!! it return baunded function so no "this" pointer is needed
    std::function<std::string()> getLogObjectName;
    tcp->get("log.getObjectName", getLogObjectName);
    std::cout << tcp->getErrorMsg() << std::endl;
    std::cout << getLogObjectName() << std::endl;//call baunded function
    
    std::cout << "----------------------------\n";
    
    //direct return type via template specialization
    std::cout << tcp->get<std::string>("log.name") << std::endl;
    
    
    
    std::cout << "----------------------------\n";
    log = nullptr;
    tcp = nullptr;
    api = nullptr;
    std::cout << "----------------------------\n";
    
    
    api = std::shared_ptr<IntApi>(new Api);
    
    std::cout << "----------------------------\n";
    tcp = api->createTcpInstance();// create instance, You can use auto type to do magic to create std::shared_ptr<IntTcp>
    
    std::cout << "----------------------------\n";

    log = tcp->get<std::shared_ptr<IntLog>>("log");
    log->set<std::string>("msg", "ABCDEFGHIJK");
    std::cout << log->get<std::string>("msg") << std::endl;
    std::cout << tcp->get<std::string>("log.msg") << std::endl;
    
    
    std::cout << "----------------------------\n";
    
    printPropertyList(tcp);
    
    std::cout << "----------------------------\n";
    
    std::cout << "create new log instance " << std::endl;
    log = api->createLogInstance();
    log->set<std::string>("msg", "0123456789");

    std::cout << "----------------------------\n";
    std::cout << "assign new log to tcp" << std::endl;
    
    tcp->set("log", log);
    
    std::cout << "----------------------------\n";
    
    std::cout << log->get<std::string>("msg") << std::endl;
    std::cout << tcp->get<std::string>("log.msg") << std::endl;

    std::cout << "----------------------------\n";
    
    printPropertyList(tcp);

    std::cout << "----------------------------\n";
    
    
    api = nullptr;
    tcp = nullptr;
    log = nullptr;
    
    std::cout << "----------------------------\n";
    
    
}
/*
    printPropertyList(api);

    std::cout << _tcp.use_count() << std::endl;//check refcount

    _tcp = nullptr; // free object
    
    std::cout << "----------------------------\n";

    //test create object from get
    _tcp = api->get<std::shared_ptr<IPort>>("createTcpInstance");
    _tcp = nullptr;
    
    
    std::cout << "----------------------------\n";
    
    api = nullptr;
    
    api = std::shared_ptr<IntApi>(new Api);

    std::cout << "----------------------------\n";
    
    
    //test objects
    _tcp = api->createTcpInstance();
    IPort& tcp = *_tcp->get<IPort*>("IPort");//cast from base to child// NOTE You have reference!!! Yea!
    tcp.connect();
    tcp.disconnect();
    tcp.isConnected();
    std::cout << tcp.getObjectName() << std::endl;

    auto _port = api->createPortInstance();
    IPort& port = *_port->get<IPort*>("IPort");//cast from base to child
    port.connect();
    port.disconnect();
    port.isConnected();
    std::cout << port.getObjectName() << std::endl;
    
    std::cout << "----------------------------\n";
    
    printPropertyList(api);
    
    std::cout << "----------------------------\n";
    
    printPropertyList(_tcp);
    
    std::cout << "----------------------------\n";

    //test set decuce template type from arguemnt
    tcp.set("name", "ASDF");//char [5]
    std::cout << tcp.get<std::string>("name") << std::endl;
    std::cout << tcp.getErrorMsg() << std::endl;

    //auto __name = tcp.get("name");

    std::cout << "----------------------------\n";
    
    //test force template type - handy for std::string
    tcp.set("name", "FunnyTCP");//const char*
    std::cout << tcp.get<std::string>("name") << std::endl;
    std::cout << tcp.getErrorMsg() << std::endl;

    std::cout << "----------------------------\n";

    //test dedicated lambda function as setter and getter
    tcp.set("magicPort", 123);
    std::cout << tcp.getErrorMsg() << std::endl;
    std::cout << tcp.get<int>("port") << std::endl;
    std::cout << tcp.getErrorMsg() << std::endl;
    
    std::cout << "----------------------------\n";
    
    //test path access
    std::cout << tcp.get<std::shared_ptr<IntLog>>("log") << std::endl;
    tcp.set("log.msg", std::string("my log info"));
    std::cout << "msg:" << tcp.get<std::string>("log.msg") << std::endl;
    std::cout << tcp.getErrorMsg() << std::endl;
    tcp.set<std::string>("log.msg", "my log info 2!");
    std::cout << "msg:" << tcp.get<std::string>("log.msg") << std::endl;
    std::cout << tcp.getErrorMsg() << std::endl;


    std::cout << "----------------------------\n";
    
    //test get and replace pointer
    std::shared_ptr<IntLog> mylogger(new Log);
    mylogger->set("name", "MyLoGGer");
    std::cout << "err: " << tcp.getErrorMsg() << std::endl;
    std::cout << tcp.get<std::shared_ptr<IntLog>>("log") << std::endl;
    std::cout << "err: " << tcp.getErrorMsg() << std::endl;
    std::cout << tcp.get<std::string>("log.name") << std::endl;
    std::cout << "err: " << tcp.getErrorMsg() << std::endl;
    tcp.set("log", mylogger);
    std::cout << "err: " << tcp.getErrorMsg() << std::endl;
    std::cout << tcp.get<std::shared_ptr<IntLog>>("log") << std::endl;
    std::cout << tcp.get<std::string>("log.name") << std::endl;
    std::cout << "err: " << tcp.getErrorMsg() << std::endl;
    tcp.recv(NULL, 0);
    std::cout << mylogger->get<std::string>("msg") << std::endl;
    std::cout << "err: " << tcp.getErrorMsg() << std::endl;
    
    mylogger = nullptr;
    
    
    std::cout << "----------------------------\n";
    
    //test interface
    std::vector<IntObject*> objs = {&tcp, &port};
    for(auto o : objs){
        std::cout << "name: " << o->get<std::string>("name") << std::endl;
        std::cout << "err: " << o->getErrorMsg() << std::endl;
        o->clearError();
        std::cout << "IntObject " << o->get<IntObject*>("IntObject") << std::endl;
        std::cout << "IPort " << o->get<IPort*>("IPort") << std::endl;
        std::cout << "ErrorString " << o->get<const char*>("ErrorString") << std::endl;
        std::cout << "----------------------------\n";
    }
    
    
    std::cout << "----------------------------\n";

    //test reference to pointer set and get
    std::cout << tcp.get<const char*>("ip") << std::endl;
    tcp.set("ip", "127.0.0.1");
    std::cout << tcp.get<const char*>("ip") << std::endl;
    
    
    std::cout << "----------------------------\n";
    
    //test property with len parameter
    char buf[128] = "Hello";
    int len=sizeof(buf);
    tcp.set("buffer", buf, len);
    tcp.set("buffer", buf, 128);
    tcp.set("buffer", buf, sizeof(buf));
    std::cout << tcp.get<char*>("buffer", len) << std::endl;
    std::cout << tcp.getErrorMsg() << std::endl;
    tcp.send((const Byte*)":)", 3);
    tcp.clearError();
    std::cout << tcp.get<char*>("buffer", len) << std::endl;
    std::cout << tcp.getErrorMsg() << std::endl;
    std::cout << buf << " " << len << std::endl;

    tcp.send((const Byte*)"HelloWOrld", 11);
    char *buf3 = NULL;
    tcp.get("buffer", buf3, len);
    std::cout << (void*)buf3 << " " << (void*)&buf << std::endl;

    tcp.clearError();
    tcp.send((const Byte*)"1234", 5);
    char buf2[128] = "";
    tcp.get("bufferCopy", buf2);//should deduce size from size of array
    buf2[127] = 0;
    std::cout << buf2 << " | " << buf << std::endl;
    std::cout << tcp.getErrorMsg() << std::endl;


    std::cout << "----------------------------\n";
    
    //test casting to base and back to child
    std::vector<IntObject*> objs2 = { tcp.get<IntObject*>("IntObject"), port.get<IntObject*>("IntObject") };
    for(auto ptr : objs2){
        std::cout << ptr << " " << ptr->get<std::string>("name") << std::endl;
        IPort* p = ptr->get<IPort*>("IPort");
        p->connect();
        p->disconnect();
        p->isConnected();//should be aays Port becase it is virtual not overriten in FileBin
    }
    
    std::cout << "----------------------------\n";
    
    
    //test invalid usecase - should set lastError
    
    tcp.clearError();
    int val;
    tcp.get("asdads", val);
    std::cout << tcp.getErrorMsg() << std::endl;
    
    tcp.clearError();
    tcp.set("asdasd", "aadsad");
    std::cout << tcp.getErrorMsg() << std::endl;

    tcp.clearError();
    tcp.set("log.asda", "aadsad");
    std::cout << tcp.getErrorMsg() << std::endl;
    
    tcp.clearError();
    tcp.set("name.asda", "aadsad");
    std::cout << tcp.getErrorMsg() << std::endl;

    tcp.clearError();
    tcp.set("", "aadsad");
    std::cout << tcp.getErrorMsg() << std::endl;

    tcp.clearError();
    tcp.set("log.", "aadsad");
    std::cout << tcp.getErrorMsg() << std::endl;
    
    tcp.clearError();
    tcp.set(".", "aadsad");
    std::cout << tcp.getErrorMsg() << std::endl;
    
    tcp.clearError();
    tcp.get("name", val);
    std::cout << tcp.getErrorMsg() << std::endl;
    
    std::cout << "----------------------------\n";

    tcp.clearError();
    //IPort* _tport = tcp.get<IntObject*>("IntObject");//not compile - OK
    std::cout << tcp.getErrorMsg() << std::endl;

    std::cout << "----------------------------\n";
    
    tcp.clearError();
    std::string key = "IntObject";
    tcp.get<IntObject*>(key);
    std::cout << tcp.getErrorMsg() << std::endl;
    
    std::cout << "----------------------------\n";
    
    tcp.clearError();
    int _tport = 0;
    std::cout << tcp.get<int>("port", _tport) << std::endl;// this use T set(key, len)
    std::cout << _tport << std::endl;
    std::cout << tcp.getErrorMsg() << std::endl;

    std::cout << "----------------------------\n";

    tcp.clearError();
    tcp.get("port", _tport);// this use set(key, T&)
    std::cout << _tport << std::endl;
    std::cout << tcp.getErrorMsg() << std::endl;
    
    std::cout << "----------------------------\n";
    
    tcp.clearError();
    int err_code = 0;
    tcp.set(".", "aadsad");//force error
    const char* err_str = tcp.get<const char*>("lastError", err_code);
    std::cout << tcp.getErrorMsg() << std::endl;//two errors no aadsad and lastError
    //NOTE: be careful to use value when error is set! it may be unpredicted
    if(tcp.getErrorCode() == ErrorType::NO_ERROR){
        std::cout << err_str << " " << err_code << std::endl;
    }
    
    std::cout << "----------------------------\n";
    
    tcp.clearError();
    tcp.set("lastError", "asdf");// this use set(key, T&)
    std::cout << tcp.getErrorMsg() << std::endl;
    
    std::cout << "----------------------------\n";
    
    std::cout << tcp.getTypeNameFromPath("log") << std::endl;
    std::cout << tcp.getTypeNameFromPath("buffer") << std::endl;
    
    std::cout << "----------------------------\n";
    
    //two options to remove objects
    _tcp = nullptr;
    _port = nullptr;
    
    std::cout << "----------------------------\n";
*/

void test_py(){
    
    Py_Initialize();
    std::cout << "----------------------------------" << std::endl;

    PyObject* example = PyImport_ImportModule("example");
    
    Py_DECREF(example);
    
    std::cout << "----------------------------------" << std::endl;
    std::cout << "finalize" << std::endl;
    Py_Finalize();
    
    std::cout << "----------------------------------" << std::endl;
    std::cout << "end" << std::endl;
    std::cout << "----------------------------------" << std::endl;
    
        
}



int main(){

    test_cpp();
    //test_py();
    
    return 0;

}


