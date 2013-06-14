#include "Flat.h"
#include "../Configurations/Configuration.h"

OutputFlat::OutputFlat(const Options& iOptions, const Data& iData, int iDate, int iInit, const std::string& iVariable, const Configuration& iConfiguration) : Output(iOptions, iData, iDate, iInit, iVariable, iConfiguration) {
   // Clear file
   std::stringstream ss;
   ss << getOutputDirectory() << mTag << "/output/" << "data.dat";
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

   /*
   // Find locations
   std::set<Location> locations;
   for(int i = 0; i < mEnsKeys.size(); i++) {
      locations.push_back(mEnsKeys[i].mLocation);
   }

   // Loop over all locations
   assert(mEnsKeys.size() == mEnsData.size());
   std::set<Location>::const_iterator it;
   for(it = locations.begin(); it != locations.end(); it++) {
      // Initialize array
      std::vector<std::vector<float> > values;
      values.resize(mOffsets.size());
      for(int i = 0; i < (int) mOffsets.size(); i++) {
         values[i].resize(mMembers.size(), Global::MV);
      }

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

      ofs.close();

   }
   */
}
std::string OutputFlat::getFilename(std::string iType) const {
   std::stringstream ss;
   ss << getOutputDirectory() << mTag << "/output/" << mDate << "_" << mVariable << "_" << mConfiguration.getName() << iType;
   return ss.str();
}
