#include "SchemesHeader.inc"
#include "Output.h"
#include "../Scheme.h"
#include "../Variables/Variable.h"
#include <iomanip>

Output::Output(const Options& iOptions, const Data& iData) : Component(iOptions, iData),
      mFolder(""),
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

void Output::add(Ensemble iEnsemble, std::string iConfiguration) {
   mEnsembles[iConfiguration].push_back(iEnsemble);
}
void Output::add(Distribution::ptr iDistribution, std::string iConfiguration) {
   mDistributions[iConfiguration].push_back(iDistribution);
}
void Output::add(Deterministic iDeterministic, std::string iConfiguration) {
   mDeterministics[iConfiguration].push_back(iDeterministic);
}
void Output::add(const Obs& iObs) {
   //omp_set_lock(&writelock);
   mObs.push_back(iObs);
   //omp_unset_lock(&writelock);
}
void Output::add(const Score& iScore, std::string iConfiguration) {
   //omp_set_lock(&writelock);
   mScores[iConfiguration].push_back(iScore);
   //omp_unset_lock(&writelock);
}
void Output::write() {
   writeCore();
   mEnsembles.clear();
   mDistributions.clear();
   mDeterministics.clear();
   mScores.clear();
}
