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

   const Variable* var = Variable::get(mVariable);

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
void Output::addEnsemble(Ensemble iEnsemble) {
   mEnsembles.push_back(iEnsemble);
}
void Output::addDistribution(Distribution::ptr iDistribution) {
   mDistributions.push_back(iDistribution);
}
void Output::addDeterministic(Value iDeterministic) {
   mDeterministics.push_back(iDeterministic);
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

