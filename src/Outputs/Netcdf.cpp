#include "Netcdf.h"
#include "../Metrics/Metric.h"
#include "../Variables/Variable.h"

OutputNetcdf::OutputNetcdf(const Options& iOptions, const Data& iData, int iDate, int iInit, const std::string& iVariable, const Configuration& iConfiguration) : Output(iOptions, iData, iDate, iInit, iVariable, iConfiguration) {}

void OutputNetcdf::writeForecasts() const {
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

   // TODO: Can't assume that ensembles have the same number as other entities
   std::vector<Location> locations;
   std::vector<float> offsets;
   getAllLocations(mEnsembles, locations);
   getAllOffsets(mEnsembles, offsets);

   // Dimensions
   const Variable* var = Variable::get(mVariable);
   assert(var->getPdfX().size() > 0);
   assert(var->getCdfInv().size() > 0);
   assert(locations.size() > 0);
   assert(numMembers > 0);
   NcDim* dimOffset   = ncfile.add_dim("Offset");
   NcDim* dimX        = ncfile.add_dim("X", var->getPdfX().size());
   NcDim* dimCdf      = ncfile.add_dim("Cdf", var->getCdfInv().size());
   NcDim* dimLocation = ncfile.add_dim("Location", locations.size());
   NcDim* dimMember   = ncfile.add_dim("Member", numMembers);

   // Variables
   //NcVar* varCdf    = ncfile.add_var("Cdf",          ncFloat, dimOffset, dimX, dimLocation);
   NcVar* varPdf      = ncfile.add_var("Pdf",          ncFloat, dimOffset, dimX, dimLocation);
   NcVar* varDet      = ncfile.add_var("Det",          ncFloat, dimOffset, dimLocation);
   NcVar* varDiscreteLower = NULL;
   if(var->isLowerDiscrete())
      varDiscreteLower = ncfile.add_var("P0",          ncFloat, dimOffset, dimLocation);
   NcVar* varDiscreteUpper = NULL;
   if(var->isUpperDiscrete())
      varDiscreteUpper = ncfile.add_var("P1",          ncFloat, dimOffset, dimLocation);
   NcVar* varEns      = ncfile.add_var("Ens",          ncFloat, dimOffset, dimMember, dimLocation);
   NcVar* varObs      = ncfile.add_var("Observations", ncFloat, dimOffset, dimLocation);
   NcVar* varLocation = ncfile.add_var("Location",     ncInt,   dimLocation);
   NcVar* varLat      = ncfile.add_var("Lat",          ncFloat, dimLocation);
   NcVar* varLon      = ncfile.add_var("Lon",          ncFloat, dimLocation);
   NcVar* varOffset   = ncfile.add_var("Offset",       ncFloat, dimOffset);
   NcVar* varX        = ncfile.add_var("X",            ncFloat, dimX);
   NcVar* varCdfs     = ncfile.add_var("Cdfs",         ncFloat, dimCdf);
   NcVar* varCdfInv   = ncfile.add_var("CdfInv",       ncFloat,  dimOffset, dimCdf, dimLocation);
   NcVar* varSelectorDate  = ncfile.add_var("SelectorDate",  ncInt, dimOffset, dimMember, dimLocation);
   NcVar* varSelectorOffset= ncfile.add_var("SelectorOffset",ncInt, dimOffset, dimMember, dimLocation);
   NcVar* varSelectorSkill = ncfile.add_var("SelectorSkill", ncFloat, dimOffset, dimMember, dimLocation);
   NcVar* varNumEns   = ncfile.add_var("NumEns",       ncInt, dimOffset, dimLocation);

   // Attributes
   std::vector<const Component*> components;
   std::vector<Component::Type> types;
   mConfiguration.getAllComponents(components, types);
   for(int i = 0; i < components.size(); i++) {
      ncfile.add_att(Component::getComponentName(types[i]).c_str(), components[i]->getSchemeName().c_str());
   }
   ncfile.add_att("Configuration_name", mConfiguration.getName().c_str());
   ncfile.add_att("Variable", var->getName().c_str());
   ncfile.add_att("Units",    var->getUnits().c_str());
   ncfile.add_att("Date",     mDate);

   // Write data
   writeVariable(varOffset, offsets);
   
   // Write Location data
   std::vector<float> lats;
   std::vector<float> lons;
   std::vector<int> locationIds;
   for(int i = 0; i < locations.size(); i++) {
      locationIds.push_back(locations[i].getId());
      lats.push_back(locations[i].getLat());
      lons.push_back(locations[i].getLon());
   }
   writeVariable(varLocation, locationIds);
   writeVariable(varLat, lats);
   writeVariable(varLon, lons);

   // Write Field data
   for(int i = 0; i < (int) mSelectorKeys.size(); i++) {
      ScalarKey key = mSelectorKeys[i];
      int locationIndex = getPosition(locationIds, key.mLocation.getId());
      int offsetIndex   = getPosition(offsets, key.mOffset);
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

         varSelectorDate->set_cur(offsetIndex, j, locationIndex);
         varSelectorDate->put(&(date), 1,1,1);
         varSelectorOffset->set_cur(offsetIndex, j, locationIndex);
         varSelectorOffset->put(&(offset), 1,1,1);
         varSelectorSkill->set_cur(offsetIndex, j, locationIndex);
         varSelectorSkill->put(&(skill), 1,1,1);
      }
   }

   // Write Ensemble data (OK)
   for(int i = 0; i < mEnsembles.size(); i++) {
      Ensemble ens = mEnsembles[i];
      int locationId    = ens.getLocation().getId();
      int locationIndex = Output::getPosition(locationIds, locationId);
      int offsetIndex   = Output::getPosition(offsets, ens.getOffset());
      assert(Global::isValid(locationIndex) && Global::isValid(offsetIndex));
      varEns->set_cur(offsetIndex, 0, locationIndex);
      varEns->put((float*) &(ens.getValues()[0]),1,ens.size(),1);

      varNumEns->set_cur(offsetIndex, locationIndex);
      int numEns = ens.size();
      varNumEns->put(&numEns, 1,1);
   }

   // Write CDF inv data
   std::vector<float> cdfs = var->getCdfInv();
   writeVariable(varCdfs, cdfs);
   for(int d = 0; d < mDistributions.size(); d++) {
      Distribution::ptr dist = mDistributions[d];
      for(int i = 0; i < cdfs.size(); i++) {
         float inv = dist->getInv(cdfs[i]);
         int locationIndex = Output::getPosition(locationIds, dist->getLocation().getId());
         int offsetIndex   = Output::getPosition(offsets,     dist->getOffset());
         varCdfInv->set_cur(offsetIndex, i, locationIndex);
         varCdfInv->put(&inv, 1,1,1);

         if(var->isLowerDiscrete()) {
            float p0  = dist->getP0();
            varDiscreteLower->set_cur(offsetIndex, locationIndex);
            varDiscreteLower->put(&p0, 1,1);
         }
         if(var->isUpperDiscrete()) {
            float p1  = dist->getP1();
            varDiscreteUpper->set_cur(offsetIndex, locationIndex);
            varDiscreteUpper->put(&p1, 1,1);
         }
      }
   }

   // Write Scalar data
   for(int i = 0; i < (int) mDetKeys.size(); i++) {
      ScalarKey key     = mDetKeys[i];
      int locationId    = key.mLocation.getId();
      int locationIndex = getPosition(locationIds, locationId);
      assert(Global::isValid(locationIndex));

      float offset      = key.mOffset;
      int offsetIndex   = getPosition(offsets, offset);
      assert(Global::isValid(offsetIndex));

      varDet->set_cur(offsetIndex, locationIndex);
      varDet->put(&mDetData[i], 1,1);
   }

   // Write Obs data
   for(int i = 0; i < mObs.size(); i++) {
      int locationId    = mObs[i].getLocation().getId();
      int locationIndex = getPosition(locationIds, locationId);
      assert(Global::isValid(locationIndex));

      // Compute offset
      float offset      = Global::getTimeDiff(mObs[i].getDate(), 0, mObs[i].getOffset(), mDate, 0, 0);
      int offsetIndex   = getPosition(offsets, offset);
      assert(Global::isValid(offsetIndex));

      varObs->set_cur(offsetIndex, locationIndex);
      float obs = mObs[i].getValue();
      varObs->put(&obs, 1,1);
   }
   ncfile.close();
}
void OutputNetcdf::writeVerifications() const {
   // TODO: Can't assume that ensembles have the same number as other entities
   std::vector<Location> locations;
   std::vector<float> offsets;
   getAllLocations(mEnsembles, locations);
   getAllOffsets(mEnsembles, offsets);
   const Variable* var = Variable::get(mVariable);

   // Set up file
   std::string filename = getVerifFilename();
   NcFile ncfile0(filename.c_str());
   bool doExist  = ncfile0.is_valid();
   bool doAppend;

   // Metrics
   std::set<std::string> metricsSet;
   for(int i = 0; i < (int) mScores.size(); i++) {
      std::string name = mScores[i].getMetric();
      metricsSet.insert(name);
   }
   std::vector<std::string> metrics(metricsSet.begin(), metricsSet.end());

   // Determine if we should append to the verification file, or create a new file
   if(doExist) {
      doAppend = true;
      // Check if dimesions match
      if(ncfile0.get_dim("Offset")->size() != offsets.size() || 
         ncfile0.get_dim("X")->size() != var->getCdfX().size() ||
         ncfile0.get_dim("Location")->size() != locations.size()) {
         doAppend = false;
      }
      // Check that all variables are defined
      std::map<std::string, int>::const_iterator it;
      for(int i = 0; i < metrics.size(); i++) {
         NcError q(NcError::silent_nonfatal);
         std::string metricName = metrics[i];
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
   NcDim* dimLocation;

   // Variables
   NcVar* varLocation;
   NcVar* varLat;
   NcVar* varLon;
   NcVar* varOffset;
   NcVar* varDate;
   std::vector<NcVar*> varMetrics;

   // Dimensions
   if(!doAppend) {
      dimDate     = ncfile.add_dim("Date");
      assert(offsets.size() > 0);
      dimOffset   = ncfile.add_dim("Offset", offsets.size());
      assert(var->getCdfX().size() > 0);
      dimX        = ncfile.add_dim("X", var->getCdfX().size());
      assert(locations.size() > 0);
      dimLocation = ncfile.add_dim("Location", locations.size());
      //std::cout << " dim locations = " << mLocations.size() << std::endl;

      // Variables
      varLocation = ncfile.add_var("Location",     ncInt,   dimLocation);
      varLat      = ncfile.add_var("Lat",          ncFloat, dimLocation);
      varLon      = ncfile.add_var("Lon",          ncFloat, dimLocation);
      varOffset   = ncfile.add_var("Offset",       ncFloat, dimOffset);
      varDate     = ncfile.add_var("Date",         ncInt, dimDate);

      std::map<std::string, int>::const_iterator it;
      for(int i = 0; i < metrics.size();  i++) {
         std::string metricName = metrics[i];
         NcVar* varMetric = ncfile.add_var(metricName.c_str(), ncFloat, dimDate, dimOffset, dimLocation);
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
      dimLocation = ncfile.get_dim("Location");
      //std::cout << " dim locations = " << dimLocation->size() << std::endl;

      // Variables
      varLocation = ncfile.get_var("Location");
      varLat      = ncfile.get_var("Lat");
      varLon      = ncfile.get_var("Lon");
      varOffset   = ncfile.get_var("Offset");
      varDate     = ncfile.get_var("Date");
      std::map<std::string, int>::const_iterator it;
      for(int i = 0; i < metrics.size(); i++) {
         std::string metricName = metrics[i];
         NcVar* varMetric = ncfile.get_var(metricName.c_str());
         varMetrics.push_back(varMetric);
      }
   }
   
   // Dates
   int dateSize = dimDate->size();
   int* cdates = new int[dateSize];
   varDate->get(cdates, dateSize);
   std::vector<int> dates(cdates, cdates + dateSize);
   delete cdates;
   // Add current date
   bool foundCurrDate = false;
   for(int i = 0; i < dates.size(); i++) {
      if(dates[i] == mDate) {
         foundCurrDate = true;
      }
   }
   if(!foundCurrDate) {
      dates.push_back(mDate);
   }

   // Write dimension values
   writeVariable(varOffset, offsets);
   writeVariable(varDate,   dates);
   std::vector<float> lats;
   std::vector<float> lons;
   std::vector<int> locationIds;
   for(int i = 0; i < locations.size(); i++) {
      locationIds.push_back(locations[i].getId());
      lats.push_back(locations[i].getLat());
      lons.push_back(locations[i].getLon());
   }
   writeVariable(varLocation, locationIds);
   writeVariable(varLat, lats);
   writeVariable(varLon, lons);

   // Write Metric data
   for(int i = 0; i < (int) mScores.size(); i++) {
      Score score = mScores[i];
      int dateIndex     = getPosition(dates, score.getDate());
      int metricIndex   = getPosition(metrics, score.getMetric());
      int locationIndex = getPosition(locationIds, score.getLocation().getId());
      int offsetIndex   = getPosition(offsets, score.getOffset());
      float value       = score.getValue();
      varMetrics[metricIndex]->set_cur(dateIndex, offsetIndex, locationIndex);
      varMetrics[metricIndex]->put(&value, 1,1,1);
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
