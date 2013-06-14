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

void Parameters::setRecords(const std::vector<int>& iRecords) {
   mRecords = iRecords;
   /*
   for(int i = 0; i < (int) iRecords.size(); i++) {
      if(iRecords[i] >= mValues.size() || iRecords[i] <= 0) {
         Global::logger->write("Parameters: invalid record edge", Logger::error);
      }
   }  */
}

void Parameters::addRecord(int iRecord) {
   mRecords.push_back(iRecord);
   if(iRecord >= (int) mValues.size() || iRecord <= 0) {
      Global::logger->write("Parameters: invalid record edge", Logger::error);
   }
}

void Parameters::getSubset(int iRecord, Parameters& iParameters) const {
   if(iRecord >= (int) mRecords.size()) {
      Global::logger->write("Parameters: record cannot be read: does not exist", Logger::error);
   }
   int start = mRecords[iRecord]; 
   int end;
   if(iRecord == mRecords.size()-1) {
      end = (int) mValues.size() - 1;
   }
   else {
      end = mRecords[iRecord+1]-1;
   }
   getSubset(start, end, iParameters);
}

void Parameters::setSubset(int iRecord, const Parameters& iParameters) {
   if(iRecord >= (int) mRecords.size()) {
      Global::logger->write("Parameters: record cannot be set: does not exist", Logger::error);
   }
   int start = mRecords[iRecord]; 
   mValues.resize(start + iParameters.size());
   //std::cout << "Parameters: " << iRecord << " " << start << " " << mValues.size() << " " << iParameters.size() << std::endl;
   if(start + (int) iParameters.size() > (int) mValues.size()) {
      Global::logger->write("Parameters: parameters size + record edge is too large", Logger::error);
   }

   for(int i = 0; i < (int) iParameters.size(); i++) {
      mValues[start + i] = iParameters[i];
   }
}
