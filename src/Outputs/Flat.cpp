#include "Flat.h"
#include "../Configurations/Configuration.h"

OutputFlat::OutputFlat(const Options& iOptions, const Data& iData) : Output(iOptions, iData) {
   iOptions.check();
}
void OutputFlat::writeCore() const {
   writeEns();
}

void OutputFlat::writeCdf() const {

}
void OutputFlat::writeEns() const {
   // Find all configurations
   std::vector<VarConf> varConfs = getAllVarConfs();

   // Loop over configurations
   for(int c = 0; c < varConfs.size(); c++) {
      VarConf varConf = varConfs[c];
      std::string var = varConf.first;
      std::string conf = varConf.second;
      // Get entities for this configuration
      std::map<VarConf,std::vector<Distribution::ptr> >::const_iterator it = mDistributions.find(varConf);
      std::vector<Distribution::ptr> distributions = it->second;

      std::stringstream ss;
      ss << getOutputDirectory() << var << "_" << conf << "_ensemble.dat";
      std::string filename = ss.str();
      std::ofstream ofs(filename.c_str(), std::ios_base::out);
      std::vector<Location> locations = getAllLocations(distributions);
      std::vector<float> offsets      = getAllOffsets(distributions);

      // Loop over all locations
      for(int loc = 0; loc < locations.size(); loc++) {
         int currLocationId = locations[loc].getId();
         ofs << "# Location " << currLocationId << std::endl;
         // Initialize array
         std::vector<std::vector<float> > values; // offset, member
         values.resize(offsets.size());

         // Find all ensembles that matches current location
         for(int i = 0; i < distributions.size(); i++) {
            Distribution::ptr dist = distributions[i];
            if(dist->getLocation().getId() == currLocationId) {
               float offset = dist->getOffset();
               int offsetIndex = Output::getPosition(offsets, offset);
               assert(Global::isValid(offsetIndex));
               Ensemble ens = dist->getEnsemble();
               values[offsetIndex] = ens.getValues();
            }
         }

         // Write output for this location
         for(int o = 0; o < offsets.size(); o++) {
            float offset = offsets[o];
            ofs << offset << " ";
            for(int i = 0; i < values[o].size(); i++) {
               ofs << values[o][i] << " ";
            }
            ofs << std::endl;
         }
      }
      ofs.close();
   }
}
std::string OutputFlat::getFilename(int iDate, int iInit, std::string iVariable, std::string iConfiguration) const {
   std::stringstream ss;
   ss << getOutputDirectory(iDate, iInit) << iDate << "_" << iInit << "_" << iVariable << "_" << iConfiguration << ".txt";
   return ss.str();
}
