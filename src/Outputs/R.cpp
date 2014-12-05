#include "R.h"
#include "../Metrics/Metric.h"
#include "../Variables/Variable.h"

OutputR::OutputR(const Options& iOptions, const Data& iData) : Output(iOptions, iData),
      mValidOnly(false) {
   // Only write lines where obs and all members are valid
   iOptions.getValue("validOnly", mValidOnly);
   iOptions.check();
}

void OutputR::writeCore() const {
   // Find all configurations
   std::vector<VarConf> varConfs = getAllVarConfs();

   // Loop over configurations
   for(int c = 0; c < varConfs.size(); c++) {
      std::string variable      = varConfs[c].first;
      std::string configuration = varConfs[c].second;
      // Get distributions for this configuration
      std::map<VarConf,std::vector<Distribution::ptr> >::const_iterator it = mDistributions.find(varConfs[c]);
      std::vector<Distribution::ptr> distributions = it->second;
      if(distributions.size() == 0) {
         Global::logger->write("OutputR: Empty distributions", Logger::message);
         return;
      }
      // Handle case when the ensemble size is not uniform
      int ensSize = distributions[0]->getEnsemble().size();

      // Set up file
      std::string filename = getFilename(configuration);
      bool newFile = !boost::filesystem::exists(filename);
      std::ofstream ofs(filename.c_str(), std::ios_base::app);

      // Write header
      if(newFile) {
         ofs << "SITE LAT LON ELEV DATE INIT OFFSET OBS";
         for(int k = 0; k < ensSize; k++) {
            ofs << " ENS" << k;
         }
         ofs << std::endl;
      }

      // Write data
      for(int d = 0; d < distributions.size(); d++) {
         Distribution::ptr dist = distributions[d];
         int date = dist->getDate();
         int init = dist->getInit();
         float offset = dist->getOffset();
         Location location = dist->getLocation();

         // Get observation
         float obsValue = Global::MV;
         for(int i = 0; i < mObs.size(); i++) {
            Obs obs = mObs[i];
            int   realDate = Global::getDate(date, init, offset);
            float realOffset = Global::getOffset(date, offset-init);
            if(obs.getDate() == realDate && obs.getOffset() == realOffset
                  && obs.getLocation().getId() == location.getId() && obs.getVariable() == variable) {
               obsValue = obs.getValue();
               break;
            }
         }
         bool isObsValid = Global::isValid(obsValue);

         // Get ensemble
         Ensemble ens = dist->getEnsemble();
         std::vector<float> ensValues = ens.getValues();
         bool isEnsValid = Global::getNumValid(ensValues) > 0;

         if((isObsValid && isEnsValid) || !mValidOnly) {
            // Write metadata
            ofs << location.getId() << " " << location.getLat() << " " << location.getLon() << " " << location.getElev() <<" " ;
            ofs << date << " " << init << " " << offset << " ";

            if(isObsValid)
               ofs << obsValue << " ";
            else
               ofs << "NA ";

            // Write ensemble
            Ensemble ens = dist->getEnsemble();
            for(int k = 0; k < ens.size(); k++) {
               if(Global::isValid(ens[k]))
                  ofs << ens[k] << " ";
               else
                  ofs << "NA ";
            }
            ofs << std::endl;
         }
      }
   }
}

std::string OutputR::getFilename(std::string iConfiguration) const {
   std::stringstream ss;
   ss << getOutputDirectory() << iConfiguration << ".txt";
   return ss.str();
}
