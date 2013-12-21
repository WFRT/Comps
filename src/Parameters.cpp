#include "Parameters.h"

Parameters::Parameters(std::vector<float> iValues, bool iIsDefault) :
      mValues(iValues),
      mIsDefault(iIsDefault) {

}
Parameters::Parameters() : mIsDefault(true) {

}
float& Parameters::operator[](unsigned int i) {
   if(i >= Parameters::maxSize) {
      std::stringstream ss;
      ss << "Maximum parameters size is " << Parameters::maxSize << std::endl;
      Global::logger->write(ss.str(), Logger::error);
   }
   // Resize
   if(i >= mValues.size()) {
      mValues.resize(i+1, Global::MV);
   }
   return mValues[i];
}
float const& Parameters::operator[](unsigned int i) const {
   if(i >= mValues.size()) {
      return Global::MV;
   }
   return mValues[i];
}
std::vector<float> Parameters::getAllParameters() const {
   return mValues;
}
void Parameters::setAllParameters(std::vector<float> iValues) {
   mValues = iValues;
}
int Parameters::size() const {
   return mValues.size();
}

void Parameters::getSubset(int iStart, int iEnd, Parameters& iParameters) const {
   assert(iEnd < iStart || iStart < mValues.size());
   if(iEnd >= mValues.size())
      std::cout << "iEnd = " << iEnd << " mValues.size() = " << mValues.size() << std::endl;
   assert(iEnd   < mValues.size());
   //assert(iStart <= iEnd); // Doesn't have to be true since we might get an empty set
   std::vector<float> parameters;
   for(int i = iStart; i <= iEnd; i++) {
      parameters.push_back(mValues[i]);
   }
   iParameters.setAllParameters(parameters);
}

bool Parameters::getIsDefault() const {
   return mIsDefault;
}
void Parameters::setIsDefault(bool iIsDefault) {
   mIsDefault = iIsDefault;
}
