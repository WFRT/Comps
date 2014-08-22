#include "Log.h"

TransformLog::TransformLog(const Options& iOptions) : Transform(iOptions),
      mBase(exp(1)) {
   iOptions.getValue("base", mBase);
   if(mBase <= 0) {
      std::stringstream ss;
      ss << "TransformLog: 'base' must be greater than 0";
      Global::logger->write(ss.str(), Logger::error);
   }
   iOptions.check();
}
float TransformLog::transformCore(float iValue) const {
   if(iValue <= 0)
      return Global::MV;
   return log(iValue)/log(mBase);
}

float TransformLog::inverseCore(float iValue) const {
   return pow(mBase, iValue);
}

float TransformLog::derivativeCore(float iValue) const {
   return 1/(iValue*log(mBase));
}
