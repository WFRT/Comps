#include "Memory.h"

ParameterIoMemory::ParameterIoMemory(const Options& iOptions, const Data& iData) : ParameterIo(iOptions, iData) {

}
void ParameterIoMemory::writeCore(const std::map<Key::Par,Parameters>& iParametersWrite) {

}
bool ParameterIoMemory::readCore(const Key::Par& iKey, Parameters& iParameters) const {
   return false;
}
