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
   // Find locations
   std::set<Location> locations;
   for(int i = 0; i < mEnsembles.size(); i++) {
      locations.insert(mEnsembles[i].getLocation());
   }

   // Loop over all locations
   std::set<Location>::const_iterator it;
   for(it = locations.begin(); it != locations.end(); it++) {
      int currLocationId = it->getId();
      ofs << "# Location " << currLocationId << std::endl;
      // Initialize array
      std::vector<std::vector<float> > values; // offset, member
      values.resize(mOffsets.size());

      for(int i = 0; i < mEnsembles.size(); i++) {
         Ensemble ens = mEnsembles[i];
         if(ens.getLocation().getId() == currLocationId) {
            float offset = ens.getOffset();
            const std::vector<float>::const_iterator pos = std::find(mOffsets.begin(), mOffsets.end(), offset);
            assert(pos != mOffsets.end());
            int offsetIndex = pos - mOffsets.begin();
            values[offsetIndex] = ens.getValues();
         }
      }

      for(int o = 0; o < mOffsets.size(); o++) {
         float offset = mOffsets[o];
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
