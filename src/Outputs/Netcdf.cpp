#include "Netcdf.h"
#include "../Metrics/Metric.h"
#include "../Variables/Variable.h"

OutputNetcdf::OutputNetcdf(const Options& iOptions, const Data& iData) : Output(iOptions, iData) {}

void OutputNetcdf::writeCore() const {

   // Find all configurations
   std::map<std::string, std::vector<Ensemble> >::const_iterator it;
   std::vector<std::string> configurations;
   for(it = mEnsembles.begin(); it != mEnsembles.end(); it++) {
      configurations.push_back(it->first);
   }

   // Loop over configurations
   for(int c = 0; c < configurations.size(); c++) {
      std::string configuration = configurations[c];
      // Get entities for this configuration
      std::map<std::string,std::vector<Ensemble> >::const_iterator it = mEnsembles.find(configuration);
      std::vector<Ensemble> ensembles = it->second;
      std::map<std::string,std::vector<Distribution::ptr> >::const_iterator it2 = mDistributions.find(configuration);
      std::vector<Distribution::ptr> distributions = it2->second;
      std::map<std::string,std::vector<Deterministic> >::const_iterator it3 = mDeterministics.find(configuration);
      std::vector<Deterministic> deterministics = it3->second;
      if(ensembles.size() == 0) {
         Global::logger->write("OutputNetcdf: Empty ensemble", Logger::message);
         return;
      }
      int numMembers = 0;
      for(int i = 0; i < ensembles.size(); i++) {
         if(ensembles[i].size() > numMembers)
            numMembers = ensembles[i].size();
      }

      // TODO: Can't assume that ensembles have the same number as other entities
      std::vector<Location> locations;
      std::vector<float> offsets;
      std::vector<int> dates;
      std::vector<std::string> variables;
      getAllLocations(ensembles, locations);
      getAllOffsets(ensembles, offsets);
      getAllDates(ensembles, dates);
      // TODO: All variables are the same for this configuration...
      getAllVariables(ensembles, variables);

      for(int v = 0; v < variables.size(); v++) {
         std::string variable = variables[v];
         const Variable* var = Variable::get(variable);
         for(int d = 0; d < dates.size(); d++) {
            int date = dates[d];

            // Dimensions
            assert(var->getPdfX().size() > 0);
            assert(var->getCdfInv().size() > 0);
            assert(locations.size() > 0);
            assert(numMembers > 0);
            // Set up file
            std::string filename = getFilename(date, variable, configuration);
            NcFile ncfile(filename.c_str(), NcFile::Replace);

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
            /*
            std::vector<const Component*> components;
            std::vector<Component::Type> types;
            mConfiguration.getAllComponents(components, types);
            for(int i = 0; i < components.size(); i++) {
               ncfile.add_att(Component::getComponentName(types[i]).c_str(), components[i]->getSchemeName().c_str());
            }
            */
            ncfile.add_att("Configuration_name", configuration.c_str());
            ncfile.add_att("Variable", var->getName().c_str());
            ncfile.add_att("Units",    var->getUnits().c_str());
            ncfile.add_att("Date",     date);

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

            // Write Ensemble data (OK)
            for(int i = 0; i < ensembles.size(); i++) {
               Ensemble ens = ensembles[i];
               if(ens.getDate() == date && ens.getVariable() == variable) {
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
            }

            // Write deterministic data
            for(int i = 0; i < ensembles.size(); i++) {
               Deterministic det = deterministics[i];
               if(det.getDate() == date && det.getVariable() == variable) {
                  int locationId    = det.getLocation().getId();
                  int locationIndex = Output::getPosition(locationIds, locationId);
                  int offsetIndex   = Output::getPosition(offsets, det.getOffset());
                  assert(Global::isValid(locationIndex) && Global::isValid(offsetIndex));
                  varDet->set_cur(offsetIndex, locationIndex);
                  float value = det.getValue();
                  varDet->put(&value,1,1);
               }
            }

            // Write CDF inv data
            std::vector<float> cdfs = var->getCdfInv();
            writeVariable(varCdfs, cdfs);
            for(int d = 0; d < distributions.size(); d++) {
               Distribution::ptr dist = distributions[d];
               if(dist->getDate() == date && dist->getVariable() == variable) {
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
            }

            // Write Obs data
            for(int i = 0; i < mObs.size(); i++) {
               if(mObs[i].getVariable() == variable) {
                  int locationId    = mObs[i].getLocation().getId();
                  int locationIndex = getPosition(locationIds, locationId);
                  assert(Global::isValid(locationIndex));

                  // Compute offset
                  float offset      = Global::getTimeDiff(mObs[i].getDate(), 0, mObs[i].getOffset(), date, 0, 0);
                  int offsetIndex   = getPosition(offsets, offset);
                  if(Global::isValid(offsetIndex)) {
                     assert(Global::isValid(offsetIndex));

                     varObs->set_cur(offsetIndex, locationIndex);
                     float obs = mObs[i].getValue();
                     varObs->put(&obs, 1,1);
                  }
               }
            }
            ncfile.close();
         }
      }
   }
}

std::string OutputNetcdf::getFilename(int iDate, std::string iVariable, std::string iConfiguration) const {
   // Clear file
   std::stringstream ss;
   ss << getOutputDirectory() << iDate << "_" << iConfiguration << ".nc";
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
