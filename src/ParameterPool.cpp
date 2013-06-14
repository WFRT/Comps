#include "ParameterPool.h"

ParameterPool::ParameterPool(std::string iRunName, std::string iConfName) :
      mRunName(iRunName), mConfName(iConfName) {
   init();
}
void ParameterPool::init() {
   std::stringstream ss;
   ss << Namelist::getDefaultLocation() << "configuration.nl";
   Namelist nl(ss.str());
   std::string line = nl.findLine(mConfName);
   Options opt(line);
   LocationPool* LocationPool::getScheme(Options(location
}

