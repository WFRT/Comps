#include "SchemesHeader.inc"
#include "Output.h"
#include "../Scheme.h"
#include "../Variables/Variable.h"

Output::Output(const Options& iOptions,
      const Data& iData,
      const Configuration& iConfiguration) : Component(iOptions, iData),
   mConfiguration(iConfiguration) {
   iOptions.getRequiredValue("name", mTag);
   if(mTag == "parameters") {
      std::stringstream ss;
      ss << "Output: outputs are not allowed to have name=parmeters, because this word is reserved";
      Global::logger->write(ss.str(), Logger::error);
   }

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

void Output::add(Ensemble iEnsemble) {
   mEnsembles.push_back(iEnsemble);
}
void Output::add(Distribution::ptr iDistribution) {
   mDistributions.push_back(iDistribution);
}
void Output::add(Deterministic iDeterministic) {
   mDeterministics.push_back(iDeterministic);
}
void Output::add(const Obs& iObs) {
   //omp_set_lock(&writelock);
   mObs.push_back(iObs);
   //omp_unset_lock(&writelock);
}
void Output::add(const Score& iScore) {
   //omp_set_lock(&writelock);
   mScores.push_back(iScore);
   //omp_unset_lock(&writelock);
}
