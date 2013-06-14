#ifndef PARAMETER_POOL_H
#define PARAMETER_POOL_H
#include "Global.h"
#include "Component.h"
#include "Location.h"
#include "Parameters.h"

class ParameterPool {
   public:
      ParameterPool(std::string iRunName, std::string iConfName);
      void getParameters(int iDate,
                         int iInit,
                         int iOffset,
                         const Location& iLocation,
                         const std::string& iVariable,
                         Component::Type iType,
                         Parameters& iParameters);
      void setParameters(int iDate,
                         int iInit,
                         int iOffset,
                         const Location& iLocation,
                         const std::string& iVariable,
                         Component::Type iType,
                         const Parameters& iParameters);
      void write() const;
   private:
      void init();
      int getIndex(int iDate, int iInit, int iOffset);
      std::map<int,std::string> mParameterMap;
      std::string mConfName;
      std::string mRunName;
};
#endif

