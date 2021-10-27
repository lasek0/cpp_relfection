#pragma once

#include <string>

enum ErrorType : int {
    NO_ERROR=0, FATAL_ERROR, KEY_ERROR, TYPE_ERROR, VALUE_ERROR, SUPPORT_ERROR, PYTHON_ERROR, CAST_ERROR,
    SIZE_ERROR, STATUS_ERROR, ACCESS_ERROR
};

class IntError {
public:
protected:
    virtual void setError(ErrorType code, const std::string msg) = 0;
    virtual void moveError(IntError* o) = 0;
public:
    virtual ~IntError(){}
    virtual std::string getErrorMsg() = 0;
    virtual int getErrorCode() = 0;
    virtual void clearError() = 0;
};

