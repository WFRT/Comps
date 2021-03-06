#include "SchemesHeader.inc"
#include "Output.h"
#include "../Scheme.h"
#include "../Variables/Variable.h"
#include <iomanip>

Output::Output(const Options& iOptions, const Data& iData) : Component(iOptions),
      mData(iData),
      mFolder(""),
      mVariables(NULL),
      mUseDateFolder(false),
      mUseInitFolder(false) {
   iOptions.getRequiredValue("name", mTag);
   if(mTag == "parameters") {
      std::stringstream ss;
      ss << "Output: outputs are not allowed to have name=parmeters, because this word is reserved";
      Global::logger->write(ss.str(), Logger::error);
   }

   //! Which folder should the output be placed in?
   iOptions.getValue("folder", mFolder);

   //! Are files placed in folders according to date (YYYYMMDD)?
   iOptions.getValue("useDateFolder", mUseDateFolder);

   //! What cumulative probabilities (quantiles) should be computed?
   if(!iOptions.getValues("cdfs", mCdfs)) {
      // Default Cdfs
      mCdfs.push_back(0.01);
      for(int i = 1; i <= 9; i++)
         mCdfs.push_back(i/10.0);
      mCdfs.push_back(0.99);
   }
   //! For which thresholds should cumulative probabilities be computed for
   iOptions.getValues("thresholds", mThresholds);

   std::string variableDataset;
   if(iOptions.getValue("variables", variableDataset)) {
      mVariables = Input::getScheme(variableDataset);
   }

   // Create directories if necessary
   std::vector<std::string> directories;
   if(mFolder == "") {
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
      bool status = Global::createDirectory(directories[i]);
      if(status == false) {
         std::stringstream ss;
         ss << "Output.cpp: Could not create directory: " << directories[i];
         Global::logger->write(ss.str(), Logger::error);
      }
   }
}
std::string Output::getDirectory() const {
   std::stringstream ss;
   ss << "./results/" << mData.getRunName();
   return ss.str();
}
std::string Output::getOutputDirectory(int iDate, int iInit) const {
   std::stringstream ss;
   if(mFolder != "")    
      ss << mFolder << "/";
   else
      ss << getDirectory() << "/" << mTag << "/";

   if(mUseDateFolder && Global::isValid(iDate))
      ss << iDate << "/";
   if(mUseInitFolder && Global::isValid(iInit))
      ss << std::setfill('0') << std::setw(2) << iInit;
   return ss.str();
}

#include "Schemes.inc"

void Output::add(Distribution::ptr iDistribution, std::string iVariable, std::string iConfiguration) {
   addVarConf(iVariable, iConfiguration);
   std::pair<std::string, std::string> varConf(iVariable, iConfiguration);
   mDistributions[varConf].push_back(iDistribution);
}
void Output::add(const Obs& iObs) {
   //omp_set_lock(&writelock);
   mObs.push_back(iObs);
   //omp_unset_lock(&writelock);
}
void Output::add(const Score& iScore, std::string iVariable, std::string iConfiguration) {
   addVarConf(iVariable, iConfiguration);
   //omp_set_lock(&writelock);
   VarConf varConf(iVariable, iConfiguration);
   mScores[varConf].push_back(iScore);
   //omp_unset_lock(&writelock);
}
void Output::write() {
   writeCore();
   mDistributions.clear();
   mScores.clear();
   mObs.clear();
}


std::vector<std::pair<std::string, std::string> > Output::getAllVarConfs() const {
   return mOrderedVarConfs;
}

void Output::addVarConf(std::string iVariable, std::string iConfigurationName) {
   VarConf varConf(iVariable, iConfigurationName);
   std::set<VarConf>::const_iterator it = mAllVarConfs.find(varConf);
   if(it == mAllVarConfs.end()) {
      mAllVarConfs.insert(varConf);
      mOrderedVarConfs.push_back(varConf);
   }
}
std::vector<float> Output::getCdfs() const {
   return mCdfs;
}
std::vector<float> Output::getThresholds() const {
   return mThresholds;
}

std::vector<Location> Output::getAllLocations(const std::vector<Distribution::ptr>& iDistributions) const {
   // Store in a set, so that there are no duplicates
   std::set<Location> locations;
   for(int i = 0; i < iDistributions.size(); i++) {
      locations.insert(iDistributions[i]->getLocation());
   }

   return std::vector<Location> (locations.begin(), locations.end());
}

std::vector<float> Output::getAllOffsets(const std::vector<Distribution::ptr>& iDistributions) const {
   std::set<float> offsets;
   for(int i = 0; i < iDistributions.size(); i++) {
      offsets.insert(iDistributions[i]->getOffset());
   }

   return std::vector<float> (offsets.begin(), offsets.end());
}

std::vector<int> Output::getAllDates(const std::vector<Distribution::ptr>& iDistributions) const {
   std::set<int> dates;
   for(int i = 0; i < iDistributions.size(); i++) {
      dates.insert(iDistributions[i]->getDate());
   }

   return std::vector<int> (dates.begin(), dates.end());
}

std::vector<int> Output::getAllInits(const std::vector<Distribution::ptr>& iDistributions) const {
   std::set<int> inits;
   for(int i = 0; i < iDistributions.size(); i++) {
      inits.insert(iDistributions[i]->getInit());
   }

   return std::vector<int> (inits.begin(), inits.end());
}

std::vector<std::string> Output::getAllVariables(const std::vector<Distribution::ptr>& iDistributions) const {
   std::set<std::string> variables;
   for(int i = 0; i < iDistributions.size(); i++) {
      variables.insert(iDistributions[i]->getVariable());
   }

   return std::vector<std::string> (variables.begin(), variables.end());
}

std::string Output::getVariableName(const std::string& iVariable) const {
   if(mVariables == NULL)
      return iVariable;
   else {
      std::string localVariableName = iVariable;
      mVariables->getLocalVariableName(iVariable, localVariableName);
      return localVariableName;
   }
}
