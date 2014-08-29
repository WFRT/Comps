#include "Netcdf.h"
#include "../Metrics/Metric.h"
#include "../Variables/Variable.h"

OutputNetcdf::OutputNetcdf(const Options& iOptions, const Data& iData) : Output(iOptions, iData),
      mDontWriteProb(false) {
   iOptions.getValue("dontWriteProb", mDontWriteProb);
   iOptions.check();
}

void OutputNetcdf::writeCore() const {

   // Find all configurations
   std::map<std::string, std::vector<Ensemble> >::const_iterator it;
   std::vector<VarConf> varConfs = getAllVarConfs();

   // Loop over configurations
   for(int c = 0; c < varConfs.size(); c++) {
      std::string variable      = varConfs[c].first;
      std::string configuration = varConfs[c].second;
      // Get entities for this configuration
      std::map<VarConf,std::vector<Distribution::ptr> >::const_iterator it = mDistributions.find(varConfs[c]);
      std::vector<Distribution::ptr> distributions = it->second;
      if(distributions.size() == 0) {
         Global::logger->write("OutputNetcdf: Empty distributions", Logger::message);
         return;
      }
      int numMembers = 0;
      for(int i = 0; i < distributions.size(); i++) {
         Ensemble ens = distributions[i]->getEnsemble();
         if(ens.size() > numMembers)
            numMembers = ens.size();
      }

      std::vector<Location> locations = getAllLocations(distributions);
      std::vector<float> offsets      = getAllOffsets(distributions);
      std::vector<int> dates          = getAllDates(distributions);
      std::vector<int> inits          = getAllInits(distributions);

      const Variable* var = Variable::get(variable);
      for(int initI = 0; initI < inits.size(); initI++) {
         int init = inits[initI];
         for(int d = 0; d < dates.size(); d++) {
            int date = dates[d];

            // Dimensions
            assert(locations.size() > 0);
            assert(numMembers > 0);
            // Set up file
            std::string filename = getFilename(date, init, variable, configuration);
            NcFile ncfile(filename.c_str(), NcFile::Replace);

            NcDim* dimOffset    = ncfile.add_dim("Offset");
            NcDim* dimLocation  = ncfile.add_dim("Location", locations.size());
            NcDim* dimMember    = ncfile.add_dim("Member", numMembers);
            NcDim* dimThreshold = NULL;
            NcDim* dimProb      = NULL;
            if(getThresholds().size() > 0)
               dimThreshold   = ncfile.add_dim("Threshold", getThresholds().size());
            if(getCdfs().size() > 0)
               dimProb = ncfile.add_dim("Prob", getCdfs().size());

            // Variables
            // NcVar* varPdf = NULL;
            NcVar* varCdf = NULL;

            if(dimThreshold != NULL) {
               // varPdf = ncfile.add_var("Pdf", ncFloat, dimOffset, dimThreshold, dimLocation);
               varCdf = ncfile.add_var("Cdf", ncFloat, dimOffset, dimThreshold, dimLocation);
            }
            NcVar* varDet      = ncfile.add_var("Det",          ncFloat, dimOffset, dimLocation);
            NcVar* varDiscreteLower = NULL;
            if(var->isLowerDiscrete())
               varDiscreteLower = ncfile.add_var("P0",          ncFloat, dimOffset, dimLocation);
            NcVar* varDiscreteUpper = NULL;
            if(var->isUpperDiscrete())
               varDiscreteUpper = ncfile.add_var("P1",          ncFloat, dimOffset, dimLocation);
            NcVar* varEns      = ncfile.add_var("Ens",          ncFloat, dimOffset, dimMember, dimLocation);
            NcVar* varObs      = ncfile.add_var("Observations", ncFloat, dimOffset, dimLocation);
            NcVar* varLat      = ncfile.add_var("Lat",          ncFloat, dimLocation);
            NcVar* varLon      = ncfile.add_var("Lon",          ncFloat, dimLocation);
            NcVar* varElev     = ncfile.add_var("Elev",          ncFloat, dimLocation);
            NcVar* varCdfInv   = ncfile.add_var("CdfInv",       ncFloat,  dimOffset, dimProb, dimLocation);
            // NcVar* varSelectorDate  = ncfile.add_var("SelectorDate",  ncInt, dimOffset, dimMember, dimLocation);
            // NcVar* varSelectorOffset= ncfile.add_var("SelectorOffset",ncInt, dimOffset, dimMember, dimLocation);
            // NcVar* varSelectorSkill = ncfile.add_var("SelectorSkill", ncFloat, dimOffset, dimMember, dimLocation);
            NcVar* varNumEns   = ncfile.add_var("NumEns",       ncInt, dimOffset, dimLocation);

            // Variables defining the dimensions
            NcVar* varDimLocation  = ncfile.add_var("Location",  ncInt,   dimLocation);
            NcVar* varDimOffset    = ncfile.add_var("Offset",    ncFloat, dimOffset);
            NcVar* varDimThreshold = NULL;
            if(dimThreshold != NULL)
               varDimThreshold = ncfile.add_var("Threshold", ncFloat, dimThreshold);
            NcVar* varDimProb       = ncfile.add_var("Prob",       ncFloat, dimProb);

            // Attributes
            /*
            std::vector<const Component*> components;
            std::vector<Processor::Type> types;
            mConfiguration.getAllComponents(components, types);
            for(int i = 0; i < components.size(); i++) {
               ncfile.add_att(Processor::getProcessorName(types[i]).c_str(), components[i]->getSchemeName().c_str());
            }
            */
            ncfile.add_att("Configuration_name", configuration.c_str());
            ncfile.add_att("Variable", var->getName().c_str());
            ncfile.add_att("Units",    var->getUnits().c_str());
            ncfile.add_att("Date",     date);
            ncfile.add_att("Init",     init);

            // Write data
            writeVariable(varDimOffset, offsets);
            
            // Write Location data
            std::vector<float> lats;
            std::vector<float> lons;
            std::vector<float> elevs;
            std::vector<int> locationIds;
            for(int i = 0; i < locations.size(); i++) {
               locationIds.push_back(locations[i].getId());
               lats.push_back(locations[i].getLat());
               lons.push_back(locations[i].getLon());
               elevs.push_back(locations[i].getElev());
            }
            writeVariable(varDimLocation, locationIds);
            writeVariable(varLat, lats);
            writeVariable(varLon, lons);
            writeVariable(varElev, elevs);

            if(varDimThreshold != NULL) {
               writeVariable(varDimThreshold, getThresholds());
            }

            // Write CDF inv data
            std::vector<float> cdfs = getCdfs();
            writeVariable(varDimProb, cdfs);
            for(int d = 0; d < distributions.size(); d++) {
               Distribution::ptr dist = distributions[d];
               if(dist->getDate() == date && dist->getVariable() == variable) {
                  int locationIndex = Output::getPosition(locationIds, dist->getLocation().getId());
                  int offsetIndex   = Output::getPosition(offsets,     dist->getOffset());
                  assert(Global::isValid(locationIndex));
                  assert(Global::isValid(offsetIndex));
                  if(!mDontWriteProb) {
                     for(int i = 0; i < cdfs.size(); i++) {
                        float inv = dist->getInv(cdfs[i]);
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
                     if(varDimThreshold != NULL) {
                        std::vector<float> thresholds = getThresholds();
                        for(int i = 0; i < thresholds.size(); i++) {
                           float cdf = dist->getCdf(thresholds[i]);
                           varCdf->set_cur(offsetIndex, i, locationIndex);
                           varCdf->put(&cdf, 1, 1, 1);
                        }
                     }
                  }

                  // Write ensemble data
                  Ensemble ens = dist->getEnsemble();
                  std::vector<float> values = ens.getValues();
                  int numEns = values.size();
                  varEns->set_cur(offsetIndex, 0, locationIndex);
                  varEns->put((float*) &(values[0]),1,numEns,1);

                  varNumEns->set_cur(offsetIndex, locationIndex);
                  varNumEns->put(&numEns, 1,1);

                  // Write deterministic data
                  varDet->set_cur(offsetIndex, locationIndex);
                  float value = dist->getDeterministic();
                  varDet->put(&value,1,1);
               }
            }

            // Write Obs data
            for(int i = 0; i < mObs.size(); i++) {
               if(mObs[i].getVariable() == variable) {
                  int locationId    = mObs[i].getLocation().getId();
                  int locationIndex = getPosition(locationIds, locationId);
                  assert(Global::isValid(locationIndex));

                  // Compute offset
                  float offset      = Global::getTimeDiff(mObs[i].getDate(), mObs[i].getInit(), mObs[i].getOffset(), date, init, 0);
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

std::string OutputNetcdf::getFilename(int iDate, int iInit, std::string iVariable, std::string iConfiguration) const {
   // Clear file
   std::stringstream ss;
   ss << getOutputDirectory(iDate, iInit) << iDate << "_" << iInit << "_" << iVariable << "_" << iConfiguration << ".nc";
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
   delete[] values;
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
   delete[] values;
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
