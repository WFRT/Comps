#include "SchemesHeader.inc"
#include "Output.h"
#include "../Scheme.h"
#include "../Variables/Variable.h"

Output::Output(const Options& iOptions,
      const Data& iData,
      int iDate,
      int iInit,
      const std::string& iVariable,
      const Configuration& iConfiguration) : Component(iOptions, iData),
   mDate(iDate), mInit(iInit), mVariable(iVariable), mConfiguration(iConfiguration) {
   iOptions.getRequiredValue("name", mTag);
   if(mTag == "parameters") {
      std::stringstream ss;
      ss << "Output: outputs are not allowed to have name=parmeters, because this word is reserved";
      Global::logger->write(ss.str(), Logger::error);
   }

   // Output locations
   std::vector<Location> locations;
   iData.getOutputLocations(locations);
   int L = 1;
   L = (int) locations.size();
   std::vector<int> locationIds;
   for(int i = 0; i < L; i++) {
      mLocations[locations[i].getId()] = locations[i];
      locationIds.push_back(locations[i].getId());
   }
   makeIdMap(locationIds, mLocationMap);

   const Variable* var = Variable::get(mVariable);
   var->getCdfX(mCdfX);
   var->getCdfInv(mCdfInv);
   var->getPdfX(mPdfX);

   // Create directories if necessary
   std::vector<std::string> directories;
   {
      std::stringstream ss;
      ss << getDirectory();
      directories.push_back(ss.str());
   }
   {
      std::stringstream ss;
      ss << getOutputDirectory();
      directories.push_back(ss.str());
   }
   {
      std::stringstream ss;
      ss << getDirectory() << "/parameters/";
      directories.push_back(ss.str());
   }
   for(int i = 0; i < directories.size(); i++) {
      if(!boost::filesystem::exists(directories[i])) {
         if(!boost::filesystem::create_directory(directories[i])) {
            std::stringstream ss;
            ss << "Output.cpp: Could not create directory: " << directories[i];
            Global::logger->write(ss.str(), Logger::error);
         }
      }
   }
}
std::string Output::getDirectory() const {
   std::stringstream ss;
   ss << "./results/" << mData.getRunName();
   return ss.str();
}
std::string Output::getOutputDirectory() const {
   std::stringstream ss;
   ss << getDirectory() << "/" << mTag << "/";
   return ss.str();
}

#include "Schemes.inc"

void Output::addSelectorData(float iOffset, const Location& iLocation, const std::vector<Field>& iFields) {
   ScalarKey key(iOffset, iLocation, mVariable);
   //omp_set_lock(&writelock);
   mSelectorKeys.push_back(key);
   mSelectorData.push_back(iFields);
   //omp_unset_lock(&writelock);
}
void Output::addDiscreteData(float iOffset, const Location& iLocation, float iP, Discrete::BoundaryType iType) {
   ScalarKey key(iOffset, iLocation, mVariable);
   //omp_set_lock(&writelock);
   if(iType == Discrete::TypeLower) {
      mDiscreteLowerKeys.push_back(key);
      mDiscreteLowerData.push_back(iP);
   }
   else if(iType == Discrete::TypeUpper){
      mDiscreteUpperKeys.push_back(key);
      mDiscreteUpperData.push_back(iP);
   }
   else {
      assert(0);
   }
   //omp_unset_lock(&writelock);
}
void Output::addEnsemble(Ensemble iEnsemble) {
   mEnsembles.push_back(iEnsemble);
}
void Output::addCdfData(float iOffset, const Location& iLocation, float iX, float iData) {
   // TODO: Currently only allow Pdf(x), since most of the time one would use x(cdf).
   assert(0); // Not implemented
   CdfKey key(iOffset, iLocation, mVariable, iX);
   //omp_set_lock(&writelock);
   mCdfKeys.push_back(key);
   mCdfData.push_back(iData);
   //omp_unset_lock(&writelock);
}
void Output::addPdfData(float iOffset, const Location& iLocation, float iX, float iData) {
   CdfKey key(iOffset, iLocation, mVariable, iX);
   //omp_set_lock(&writelock);
   mPdfKeys.push_back(key);
   mPdfData.push_back(iData);
   //omp_unset_lock(&writelock);
}
void Output::addCdfInvData(float iOffset, const Location& iLocation, float iCdf, float iData) {
   CdfKey key(iOffset, iLocation, mVariable, iCdf);
   //omp_set_lock(&writelock);
   mCdfInvKeys.push_back(key);
   mCdfInvData.push_back(iData);
   //omp_unset_lock(&writelock);
}
void Output::addDetData(float iOffset, const Location& iLocation, float iData) {
   ScalarKey key(iOffset, iLocation, mVariable);
   //omp_set_lock(&writelock);
   mDetKeys.push_back(key);
   mDetData.push_back(iData);
   //omp_unset_lock(&writelock);
}
void Output::addMetricData(float iOffset, const Location& iLocation, float iData, const Metric& iMetric) {
   MetricKey key(iOffset, iLocation, mVariable, iMetric);
   //omp_set_lock(&writelock);
   mMetricKeys.push_back(key);
   mMetricData.push_back(iData);
   //omp_unset_lock(&writelock);
}
Output::CdfKey::CdfKey(float iOffset, const Location& iLocation, std::string iVariable, float iX) :
   mOffset(iOffset), mLocation(iLocation), mVariable(iVariable), mX(iX) {
}
Output::ScalarKey::ScalarKey(float iOffset, const Location& iLocation, std::string iVariable) :
   mOffset(iOffset), mLocation(iLocation), mVariable(iVariable) {
}
Output::MetricKey::MetricKey(float iOffset, const Location& iLocation, std::string iVariable, const Metric& iMetric) :
   mOffset(iOffset), mLocation(iLocation), mVariable(iVariable), mMetric(&iMetric) {
}
void Output::addObs(const Obs& iObs) {
   //omp_set_lock(&writelock);
   mObs.push_back(iObs);
   //omp_unset_lock(&writelock);
}

