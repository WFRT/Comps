#ifndef PARAMETER_IO_MEMORY_H
#define PARAMETER_IO_MEMORY_H
#include "ParameterIo.h"

//! Store parameters in memory
class ParameterIoMemory : public ParameterIo {
   public:
      ParameterIoMemory(const Options& iOptions, std::string iConfiguration, const Data& iData);
   protected:
      void writeCore(const std::map<Key::Par,Parameters>& iParametersWrite);
      bool readCore(const Key::Par& iKey, Parameters& iParameters) const;
};
#endif

