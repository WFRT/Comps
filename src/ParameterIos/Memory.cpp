#include "Memory.h"

ParameterIoMemory::ParameterIoMemory(const Options& iOptions, std::string iConfiguration, const Data& iData) : ParameterIo(iOptions, iConfiguration, iData) {
   iOptions.check();
}
void ParameterIoMemory::writeCore(const std::map<Key::Par,Parameters>& iParametersWrite) {

}
bool ParameterIoMemory::readCore(const Key::Par& iKey, Parameters& iParameters) const {
   return false;
}
