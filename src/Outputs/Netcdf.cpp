#include "Netcdf.h"
#include "../Metrics/Metric.h"
#include "../Variables/Variable.h"

OutputNetcdf::OutputNetcdf(const Options& iOptions, const Data& iData, int iDate, int iInit, const std::string& iVariable, const Configuration& iConfiguration) : Output(iOptions, iData, iDate, iInit, iVariable, iConfiguration) {
}
void OutputNetcdf::arrangeData() const {
   // Locations
   /*
   std::vector<Location> locations;
   mInput->getLocations(locations);
   std::vector<float> locationIds;
   for(int i = 0; i < (int) locations.size(); i++) {
      locationIds.push_back(locations[i].getId());
   }
   makeIdMap(locationIds, mLocationMap);
   */
   makeIdMap(mOffsets, mOffsetMap);

   // Metrics
   mMetrics.clear();
   for(int i = 0; i < (int) mMetricKeys.size(); i++) {
      std::string name = mMetricKeys[i].mMetric->getTag();
      mMetrics.push_back(name);
   }
   makeIdMap(mMetrics, mMetricMap);
   mMetrics.clear();
   makeVector(mMetricMap, mMetrics);

   // X values
   std::vector<float> xs;
   for(int i = 0; i < (int) mPdfKeys.size(); i++) {
      xs.push_back(mPdfKeys[i].mX);
   }
   makeIdMap(xs, mXMap);

   // Cdf inv values
   std::vector<float> cdfs;
   for(int i = 0; i < (int) mCdfInvKeys.size(); i++) {
      cdfs.push_back(mCdfInvKeys[i].mX);
   }
   makeIdMap(cdfs, mCdfMap);
}

/*
void OutputNetcdf::makeIdMap(const std::vector<float>& iValues, std::map<float, int>& iMap) const {
   std::set<float> valuesSet;
   // Create set of values. Each value appears only one in the set
   for(int i = 0; i < (int) iValues.size(); i++) {
      valuesSet.insert(iValues[i]);
   }
   std::set<float>::iterator it;
   // Use the key's position in the set as a unique identifier
   int counter = 0;
   for(it = valuesSet.begin(); it != valuesSet.end(); it++) {
      float key = *it;
      iMap[key] = counter;
      counter++;
   }
}
void OutputNetcdf::makeIdMap(const std::vector<std::string>& iValues, std::map<std::string, int>& iMap) const {
   std::set<std::string> valuesSet;
   // Create set of values. Each value appears only one in the set
   for(int i = 0; i < (int) iValues.size(); i++) {
      valuesSet.insert(iValues[i]);
   }
   std::set<std::string>::iterator it;
   // Use the key's position in the set as a unique identifier
   int counter = 0;
   for(it = valuesSet.begin(); it != valuesSet.end(); it++) {
      std::string key = *it;
      iMap[key] = counter;
      counter++;
   }
}
*/
void OutputNetcdf::makeVector(const std::map<std::string, int>& iMap, std::vector<std::string>& iValues) const {
   std::map<std::string, int>::const_iterator it;
   for(it = iMap.begin(); it != iMap.end(); it++) {
      iValues.push_back(it->first);
   }
}

void OutputNetcdf::writeForecasts() const {
   arrangeData();
   // Set up file
   std::string filename = getFilename();
   NcFile ncfile(filename.c_str(), NcFile::Replace);

   if(mEnsembles.size() == 0) {
      Global::logger->write("OutputNetcdf: Empty ensemble", Logger::message);
      return;
   }
   int numMembers = 0;
   for(int i = 0; i < mEnsembles.size(); i++) {
      if(mEnsembles[i].size() > numMembers)
         numMembers = mEnsembles[i].size();
   }
   assert(numMembers > 0);

   //ncfile.set_fill(NcFile::NoFill);
   // Dimensions
   NcDim* dimOffset   = ncfile.add_dim("Offset");
   assert(mPdfX.size() > 0);
   NcDim* dimX        = ncfile.add_dim("X", mPdfX.size());
   assert(mCdfInv.size() > 0);
   NcDim* dimCdf      = ncfile.add_dim("Cdf", mCdfInv.size());
   NcDim* dimVariable = ncfile.add_dim("Variable",1);
   assert(mLocations.size() > 0);
   NcDim* dimLocation = ncfile.add_dim("Location", mLocations.size());
   //NcDim* dimMember   = ncfile.add_dim("Member", mConfiguration.getSelector()->getMaxMembers());
   assert(numMembers > 0);
   NcDim* dimMember   = ncfile.add_dim("Member", numMembers);
   std::map<int, Location> map;

   // Variables
   //NcVar* varCdf      = ncfile.add_var("Cdf",          ncFloat, dimOffset, dimX, dimVariable, dimLocation);
   NcVar* varPdf      = ncfile.add_var("Pdf",          ncFloat, dimOffset, dimX, dimVariable, dimLocation);
   NcVar* varDet      = ncfile.add_var("Det",          ncFloat, dimOffset, dimVariable, dimLocation);
   NcVar* varDiscreteLower = NULL;
   if(mDiscreteLowerKeys.size() > 0)
      varDiscreteLower = ncfile.add_var("P0",      ncFloat, dimOffset, dimVariable, dimLocation);
   NcVar* varDiscreteUpper = NULL;
   if(mDiscreteUpperKeys.size() > 0)
      varDiscreteUpper = ncfile.add_var("P1",      ncFloat, dimOffset, dimVariable, dimLocation);
   NcVar* varEns      = ncfile.add_var("Ens",          ncFloat, dimOffset, dimMember, dimVariable, dimLocation);
   NcVar* varObs      = ncfile.add_var("Observations", ncFloat, dimOffset, dimVariable, dimLocation);
   NcVar* varLocation = ncfile.add_var("Location",     ncInt,   dimLocation);
   NcVar* varLat      = ncfile.add_var("Lat",          ncFloat, dimLocation);
   NcVar* varLon      = ncfile.add_var("Lon",          ncFloat, dimLocation);
   NcVar* varOffset   = ncfile.add_var("Offset",       ncFloat, dimOffset);
   //NcVar* varVariable = ncfile.add_var("Variable",     ncFloat, dimVariable);
   NcVar* varX        = ncfile.add_var("X",            ncFloat, dimX);
   NcVar* varCdfs     = ncfile.add_var("Cdfs",            ncFloat, dimCdf);
   NcVar* varCdfInv   = ncfile.add_var("CdfInv",       ncFloat,  dimOffset, dimCdf, dimVariable, dimLocation);
   NcVar* varSelectorDate  = ncfile.add_var("SelectorDate",  ncInt, dimOffset, dimMember, dimVariable, dimLocation);
   NcVar* varSelectorOffset= ncfile.add_var("SelectorOffset",ncInt, dimOffset, dimMember, dimVariable, dimLocation);
   NcVar* varSelectorSkill = ncfile.add_var("SelectorSkill", ncFloat, dimOffset, dimMember, dimVariable, dimLocation);
   NcVar* varNumEns   = ncfile.add_var("NumEns",       ncInt, dimOffset, dimVariable, dimLocation);

   // Attributes
   std::vector<const Component*> components;
   std::vector<Component::Type> types;
   mConfiguration.getAllComponents(components, types);
   for(int i = 0; i < components.size(); i++) {
      ncfile.add_att(Component::getComponentName(types[i]).c_str(), components[i]->getSchemeName().c_str());
   }
   ncfile.add_att("Configuration_name", mConfiguration.getName().c_str());
   const Variable* var = Variable::get(mVariable);
   ncfile.add_att("Variable", var->getName().c_str());
   ncfile.add_att("Units",    var->getUnits().c_str());
   ncfile.add_att("Date",     mDate);

   // Write data
   writeVariable(varOffset, mOffsets);
   //writeVariable(varVariable, DimVariable);
   
   // Write Location data
   std::vector<float> lats;
   std::vector<float> lons;
   std::vector<float> locationIds;
   std::map<int, Location>::const_iterator it;
   for(it = mLocations.begin(); it != mLocations.end(); it++) {
      locationIds.push_back(it->first);
      lats.push_back(it->second.getLat());
      lons.push_back(it->second.getLon());
   }
   writeVariable(varLocation, locationIds);
   writeVariable(varLat, lats);
   writeVariable(varLon, lons);

   // Write Field data
   for(int i = 0; i < (int) mSelectorKeys.size(); i++) {
      ScalarKey key = mSelectorKeys[i];
      int idLocation = mLocationMap[key.mLocation.getId()];
      int idOffset   = mOffsetMap[key.mOffset];
      std::vector<Field> slices = mSelectorData[i];
      for(int j = 0; j < (int) slices.size(); j++) {
         Field slice = slices[j];
         // TODO
         float values[5];
         values[1] = (float) slice.getInit();
         values[2] = (float) slice.getOffset();
         values[3] = (float) slice.getMember().getId();

         int date = slice.getDate();
         float offset = slice.getOffset();
         float skill = slice.getSkill();

         varSelectorDate->set_cur(idOffset, j, 0, idLocation);
         varSelectorDate->put(&(date), 1,1,1,1);
         varSelectorOffset->set_cur(idOffset, j, 0, idLocation);
         varSelectorOffset->put(&(offset), 1,1,1,1);
         varSelectorSkill->set_cur(idOffset, j, 0, idLocation);
         varSelectorSkill->put(&(skill), 1,1,1,1);
      }
   }

   // Write Ensemble data
   for(int i = 0; i < (int) mEnsembles.size(); i++) {
      Ensemble ens = mEnsembles[i];
      int idLocation = mLocationMap[ens.getLocation().getId()];
      int idOffset   = mOffsetMap[ens.getOffset()];
      for(int j = 0; j < (int) ens.size(); j++) {
         varEns->set_cur(idOffset, j, 0, idLocation);
         varEns->put(&ens[j], 1,1,1,1);
      }
      varNumEns->set_cur(idOffset, 0, idLocation);
      int numEns = ens.size();
      varNumEns->put(&numEns, 1,1,1);
   }

   // Write CDF data
   writeVariable(varX, mXMap);
   /*
   for(int i = 0; i < (int) mCdfKeys.size(); i++) {
      CdfKey key = mCdfKeys[i];
      int idX        = mXMap[mCdfKeys[i].mX];
      int idLocation = mLocationMap[mCdfKeys[i].mLocation.getId()];
      int idOffset   = mOffsetMap[mCdfKeys[i].mOffset];
      varCdf->set_cur(idOffset, idX, 0, idLocation);
      //std::cout << "Writing " << mCdfData[i] << " to [" << idX << " " << idLocation << " " << idOffset << "]" << std::endl;
      varCdf->put(&mCdfData[i], 1,1,1,1);
   }
   */
   for(int i = 0; i < (int) mPdfKeys.size(); i++) {
      CdfKey key = mPdfKeys[i];
      int idX        = mXMap[mPdfKeys[i].mX];
      int idLocation = mLocationMap[mPdfKeys[i].mLocation.getId()];
      int idOffset   = mOffsetMap[mPdfKeys[i].mOffset];
      varPdf->set_cur(idOffset, idX, 0, idLocation);
      //std::cout << "Writing " << mCdfData[i] << " to [" << idX << " " << idLocation << " " << idOffset << "]" << std::endl;
      varPdf->put(&mPdfData[i], 1,1,1,1);
   }

   // Write CDF inv data
   writeVariable(varCdfs, mCdfMap);
   for(int i = 0; i < (int) mCdfInvKeys.size(); i++) {
      CdfKey key = mCdfInvKeys[i];
      int idCdf      = mCdfMap[mCdfInvKeys[i].mX];
      int idLocation = mLocationMap[mCdfInvKeys[i].mLocation.getId()];
      int idOffset   = mOffsetMap[mCdfInvKeys[i].mOffset];
      varCdfInv->set_cur(idOffset, idCdf, 0, idLocation);
      //std::cout << "Writing " << mCdfInvData[i] << " to [" << idCdf << " " << idLocation << " " << idOffset << "]" << std::endl;
      varCdfInv->put(&mCdfInvData[i], 1,1,1,1);
   }

   // Write Scalar data
   for(int i = 0; i < (int) mDetKeys.size(); i++) {
      ScalarKey key = mDetKeys[i];
      int idLocation = mLocationMap[key.mLocation.getId()];
      int idOffset   = mOffsetMap[key.mOffset];
      varDet->set_cur(idOffset, 0, idLocation);
      //std::cout << "Writing " << mDetData[i] << " to [" << idLocation << " " << idOffset << "]" << std::endl;
      varDet->put(&mDetData[i], 1,1,1);
   }

   // Write Discrete probability data
   for(int i = 0; i < (int) mDiscreteLowerKeys.size(); i++) {
      ScalarKey key = mDiscreteLowerKeys[i];
      int idLocation = mLocationMap[key.mLocation.getId()];
      int idOffset   = mOffsetMap[key.mOffset];
      varDiscreteLower->set_cur(idOffset, 0, idLocation);
      varDiscreteLower->put(&mDiscreteLowerData[i], 1,1,1);
   }
   for(int i = 0; i < (int) mDiscreteUpperKeys.size(); i++) {
      ScalarKey key = mDiscreteUpperKeys[i];
      int idLocation = mLocationMap[key.mLocation.getId()];
      int idOffset   = mOffsetMap[key.mOffset];
      varDiscreteUpper->set_cur(idOffset, 0, idLocation);
      varDiscreteUpper->put(&mDiscreteUpperData[i], 1,1,1);
   }

   // Write Obs data
   for(int i = 0; i < (int) mObs.size(); i++) {
      int locationId = mObs[i].getLocation().getId();
      int idLocation = mLocationMap[locationId];

      // Compute offset
      float offset   = Global::getTimeDiff(mObs[i].getDate(), 0, mObs[i].getOffset(), mDate, 0, 0);
      int idOffset   = mOffsetMap[offset];

      varObs->set_cur(idOffset, 0, idLocation);
      float obs = mObs[i].getValue();
      varObs->put(&obs, 1,1,1);
      //std::cout << "Wrinting obs: " << offset << " " << idOffset << " " << mObs[i].getDate() << " " << mDate << " " << obs << std::endl;
   }
   ncfile.close();
}
void OutputNetcdf::writeVerifications() const {
   arrangeData();
   // Set up file
   assert(mCdfKeys.size() == mCdfData.size());
   assert(mPdfKeys.size() == mPdfData.size());
   std::string filename = getVerifFilename();
   NcFile ncfile0(filename.c_str());
   bool doExist  = ncfile0.is_valid();
   bool doAppend;

   // Determine if we should append to the verification file, or create a new file
   if(doExist) {
      doAppend = true;
      // Check if dimesions match
      if(ncfile0.get_dim("Offset")->size() != mOffsets.size() || 
         ncfile0.get_dim("X")->size() != mCdfX.size() ||
         ncfile0.get_dim("Location")->size() != mLocations.size()) {
         doAppend = false;
      }
      // Check that all variables are defined
      std::map<std::string, int>::const_iterator it;
      for(it = mMetricMap.begin(); it != mMetricMap.end(); it++) {
         NcError q(NcError::silent_nonfatal);
         std::string metricName = it->first;
         NcVar* varMetric = ncfile0.get_var(metricName.c_str());
         if(!varMetric)
            doAppend = false;
      }

      if(!doAppend) {
         Global::logger->write("Existing metrics file trashed, because dimensions/variables do not match", Logger::warning);
      }
   }
   else {
      doAppend = false;
   }

   ncfile0.close();
   NcFile::FileMode ncType;
   if(doAppend) {
      ncType = NcFile::Write;
   }
   else {
      ncType = NcFile::Replace;
   }
   NcFile ncfile = NcFile(filename.c_str(), ncType);;
   assert(ncfile.is_valid());

   //ncfile.set_fill(NcFile::NoFill);

   NcDim* dimDate;
   NcDim* dimOffset;
   NcDim* dimX;
   NcDim* dimVariable;
   NcDim* dimLocation;

   // Variables
   NcVar* varLocation;
   NcVar* varLat;
   NcVar* varLon;
   NcVar* varOffset;
   NcVar* varDate;
   NcVar* varVariable;
   std::vector<NcVar*> varMetrics;

   // Dimensions
   if(!doAppend) {
      dimDate     = ncfile.add_dim("Date");
      assert(mOffsets.size() > 0);
      dimOffset   = ncfile.add_dim("Offset", mOffsets.size());
      assert(mCdfX.size() > 0);
      dimX        = ncfile.add_dim("X", mCdfX.size());
      dimVariable = ncfile.add_dim("Variable",1);
      assert(mLocations.size() > 0);
      dimLocation = ncfile.add_dim("Location", mLocations.size());
      //std::cout << " dim locations = " << mLocations.size() << std::endl;

      // Variables
      varLocation = ncfile.add_var("Location",     ncInt,   dimLocation);
      varLat      = ncfile.add_var("Lat",          ncFloat, dimLocation);
      varLon      = ncfile.add_var("Lon",          ncFloat, dimLocation);
      varOffset   = ncfile.add_var("Offset",       ncFloat, dimOffset);
      varDate     = ncfile.add_var("Date",         ncInt, dimDate);
      varVariable = ncfile.add_var("Variable",     ncFloat, dimVariable);

      std::map<std::string, int>::const_iterator it;
      for(it = mMetricMap.begin(); it != mMetricMap.end(); it++) {
         std::string metricName = it->first;
         NcVar* varMetric = ncfile.add_var(metricName.c_str(), ncFloat, dimDate, dimOffset, dimVariable, dimLocation);
         varMetrics.push_back(varMetric);
      }

      // Attributes

      std::vector<const Component*> components;
      std::vector<Component::Type> types;
      mConfiguration.getAllComponents(components, types);
      for(int i = 0; i < components.size(); i++) {
         ncfile.add_att(Component::getComponentName(types[i]).c_str(), components[i]->getSchemeName().c_str());
      }
      ncfile.add_att("Configuration_name", mConfiguration.getName().c_str());
      const Variable* var = Variable::get(mVariable);
      ncfile.add_att("Variable", var->getName().c_str());
      ncfile.add_att("Units",    var->getUnits().c_str());
   }
   else {
      dimDate     = ncfile.get_dim("Date");
      dimOffset   = ncfile.get_dim("Offset");
      dimX        = ncfile.get_dim("X");
      dimVariable = ncfile.get_dim("Variable");
      dimLocation = ncfile.get_dim("Location");
      //std::cout << " dim locations = " << dimLocation->size() << std::endl;

      // Variables
      varLocation = ncfile.get_var("Location");
      varLat      = ncfile.get_var("Lat");
      varLon      = ncfile.get_var("Lon");
      varOffset   = ncfile.get_var("Offset");
      varDate     = ncfile.get_var("Date");
      varVariable = ncfile.get_var("Variable");
      std::map<std::string, int>::const_iterator it;
      for(it = mMetricMap.begin(); it != mMetricMap.end(); it++) {
         std::string metricName = it->first;
         NcVar* varMetric = ncfile.get_var(metricName.c_str());
         varMetrics.push_back(varMetric);
      }
   }
   
   // Dates
   int dateSize = dimDate->size();
   int* dates = new int[dateSize];
   varDate->get(dates, dateSize);
   bool foundCurrDate = false;
   for(int i = 0; i < dateSize; i++) {
      mDates.push_back(dates[i]);
      if(dates[i] == mDate) {
         foundCurrDate = true;
      }
   }
   if(!foundCurrDate) {
      mDates.push_back(mDate);
   }
   delete dates;
   makeIdMap(mDates, mDateMap);

   // Write data
   writeVariable(varOffset, mOffsets);
   writeVariable(varDate,   mDates);
   //writeVariable(varVariable, DimVariable);
   
   // Write Location data
   std::vector<float> lats;
   std::vector<float> lons;
   std::vector<float> locationIds;
   std::map<int, Location>::const_iterator it;
   for(it = mLocations.begin(); it != mLocations.end(); it++) {
      locationIds.push_back(it->first);
      lats.push_back(it->second.getLat());
      lons.push_back(it->second.getLon());
   }
   writeVariable(varLocation, locationIds);
   writeVariable(varLat, lats);
   writeVariable(varLon, lons);

   // Write Metric data
   for(int i = 0; i < (int) mMetricKeys.size(); i++) {
      MetricKey key = mMetricKeys[i];
      int idDate     = mDateMap[mDate];
      int idLocation = mLocationMap[mMetricKeys[i].mLocation.getId()];
      int idOffset   = mOffsetMap[mMetricKeys[i].mOffset];
      int idMetric   = mMetricMap[mMetricKeys[i].mMetric->getTag()];
      float score    = mMetricData[i];
      //std::cout << "DATE: " << mDate << " " << idDate  << std::endl;
      //std::cout << "Writing score: " << score << " " << idMetric << " " << idDate << " " << idOffset << " " << idLocation << std::endl;
      varMetrics[idMetric]->set_cur(idDate, idOffset, 0, idLocation);
      varMetrics[idMetric]->put(&score, 1,1,1,1);
   }

   ncfile.close();

}

std::string OutputNetcdf::getFilename() const {
   // Clear file
   std::stringstream ss;
   ss << getOutputDirectory() << mDate << "_" << mVariable << "_" << mConfiguration.getName() << ".nc";
   return ss.str();
}
std::string OutputNetcdf::getVerifFilename() const {
   // Clear file
   std::stringstream ss;
   ss << getOutputDirectory() << mVariable << "_" << mConfiguration.getName() << ".nc";
   return ss.str();
}
void OutputNetcdf::writeVariable(NcVar* iVariable, const std::map<float,int>& iValues) const {
   int N = (int) iValues.size();
   float* values = new float[N];
   std::map<float, int>::const_iterator it;
   int counter = 0;
   for(it = iValues.begin(); it != iValues.end(); it++) {
      values[counter] = it->first;
      counter++;
   }
   long int pos = 0;
   iVariable->set_cur(&pos);
   iVariable->put(values, N);
   delete values;
}
/*
void OutputNetcdf::writeVariable(NcVar* iVariable, const std::vector<float>& iValues) const {
   int N = (int) iValues.size();
   float* values = new float[N];
   for(int i = 0; i < (int) iValues.size(); i++) {
      values[i] = iValues[i];
   }
   long int pos = 0;
   iVariable->set_cur(&pos);
   iVariable->put(values, N);
   delete values;
}
*/
void OutputNetcdf::writeVariable(NcVar* iVariable, const std::vector<std::string>& iValues) const {
   int N = (int) iValues.size();
   char values[N*40];
   for(int i = 0; i < (int) iValues.size(); i++) {
      for(int j = 0; j < (int) iValues[i].length(); j++) {
         assert(j < 40);
         values[i*40+j] = iValues[i][j];
      }
      values[i*40+iValues[i].length()] = '\0';
   }
   iVariable->set_cur(0, 0);
   iVariable->put(values, N, 40);
}
int OutputNetcdf::getDimSize(Output::Dim iDim) const {
   for(int i = 0; i < (int) mCdfKeys.size(); i++) {
      
   }
   return Global::MV;
}

std::string OutputNetcdf::getOutputFileName() const {
   return getFilename();
}
