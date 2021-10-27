#include "Error.h"

#if DEBUG>0
#include <iostream>
#endif

void Error::setError(ErrorType code, const std::string msg){
    #if DEBUG>0
    if(code>0) std::cout << "%%%%%%%%%%%%%%%%%%%%%% [" << code << "] " << msg << std::endl;
    #endif
    if(_error_msg.empty()){
        _error_msg = msg;
    }else{
        _error_msg += "; " + msg;
    }
    _error_code = code;
}

void Error::moveError(IntError* o){
    setError((ErrorType)o->getErrorCode(), o->getErrorMsg());
    o->clearError();
}

void Error::moveError(std::shared_ptr<IntError> o){
    moveError(o.get());
}


Error::Error():_error_msg(""),_error_code(0){

}

Error::~Error(){

}

std::string Error::getErrorMsg(){
    return _error_msg;
}

int Error::getErrorCode(){
    return _error_code;
}

void Error::clearError(){
    _error_msg = "";
    _error_code = 0;
}

