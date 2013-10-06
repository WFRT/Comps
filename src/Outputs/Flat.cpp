#include "Flat.h"
#include "../Configurations/Configuration.h"

OutputFlat::OutputFlat(const Options& iOptions, const Data& iData, int iDate, int iInit, const std::string& iVariable, const Configuration& iConfiguration) : Output(iOptions, iData, iDate, iInit, iVariable, iConfiguration) {
   // Clear file
   std::stringstream ss;
   ss << getOutputDirectory() << "ensemble.dat";
   std::string filename = ss.str();
   std::ofstream ofs(filename.c_str());
   ofs.close();
}
void OutputFlat::writeForecasts() const {
   writeEns();
}
void OutputFlat::writeVerifications() const {

}

void OutputFlat::writeCdf() const {
   // Set up file
   /*
   std::ofstream ofs(filename.c_str(), std::ios_base::app);
   assert(mCdfKeys.size() == mCdfData.size());
   for(int i = 0; i < (int) mCdfKeys.size(); i++) {
      CdfKey key = mCdfKeys[i];
      ofs << key.mOffset << " ";
      ofs << key.mLocation.getId() << " ";
      ofs << key.mX << " ";
      ofs << key.mVariable << " ";
      ofs << mCdfData[i] << std::endl;
   }
   ofs.close();
  */

}
void OutputFlat::writeEns() const {
   std::stringstream ss;
   ss << getOutputDirectory() << "ensemble.dat";
   std::string filename = ss.str();
   std::ofstream ofs(filename.c_str(), std::ios_base::app);
   std::vector<Location> locations;
   std::vector<float> offsets;
   getAllLocations(mEnsembles, locations);
   getAllOffsets(mEnsembles, offsets);

   // Loop over all locations
   for(int loc = 0; loc < locations.size(); loc++) {
      int currLocationId = locations[loc].getId();
      ofs << "# Location " << currLocationId << std::endl;
      // Initialize array
      std::vector<std::vector<float> > values; // offset, member
      values.resize(offsets.size());

      // Find all ensembles that matches current location
      for(int i = 0; i < mEnsembles.size(); i++) {
         Ensemble ens = mEnsembles[i];
         if(ens.getLocation().getId() == currLocationId) {
            float offset = ens.getOffset();
            int offsetIndex = Output::getPosition(offsets, offset);
            assert(Global::isValid(offsetIndex));
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

      /*
      // Populate array
      for(int i = 0; i < (int) mCdfKeys.size(); i++) {
         CdfKey key = mCdfKeys[i];
         // TODO
         if(key.mLocation.getId() == it->mLocation.getId()) {

         }
         ofs << key.mOffset << " ";
         ofs << key.mLocation.getId() << " ";
         ofs << key.mX << " ";
         ofs << key.mVariable << " ";
         ofs << mCdfData[i] << std::endl;
      }

      // Set up file
      std::string filename = getFilename("ens");
      std::ofstream ofs(filename.c_str());
      */
   }
   ofs.close();
}
std::string OutputFlat::getFilename(std::string iType) const {
   std::stringstream ss;
   ss << getOutputDirectory() << mDate << "_" << mVariable << "_" << mConfiguration.getName() << iType;
   return ss.str();
}
