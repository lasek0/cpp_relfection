#pragma once
#include <memory>
#include "IntError.h"

class Error : virtual public IntError {
private:
    std::string _error_msg;
    int _error_code;
protected:
    virtual void setError(ErrorType code, const std::string msg);
    virtual void moveError(IntError* o);
    virtual void moveError(std::shared_ptr<IntError> o);
public:
    Error();
    virtual ~Error();
    virtual std::string getErrorMsg();
    virtual int getErrorCode();
    virtual void clearError();
};



