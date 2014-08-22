#include "Verif.h"
#include "../Metrics/Metric.h"
#include "../Variables/Variable.h"

OutputVerif::OutputVerif(const Options& iOptions, const Data& iData) : Output(iOptions, iData) {
   iOptions.check();
}

void OutputVerif::writeCore() const {
   // Find all configurations
   std::map<std::string, std::vector<Score> >::const_iterator it;
   std::vector<VarConf> varConfs = getAllVarConfs();

   // Loop over configurations
   for(int c = 0; c < varConfs.size(); c++) {
      std::string variable      = varConfs[c].first;
      std::string configuration = varConfs[c].second;
      // Get scores for this configuration
      std::map<VarConf,std::vector<Score> >::const_iterator it = mScores.find(varConfs[c]);
      std::vector<Score> scores = it->second;

      std::vector<Location> locations = getAllLocations(scores);
      std::vector<float> offsets      = getAllOffsets(scores);
      std::vector<int> dates          = getAllDates(scores);
      std::vector<int> inits          = getAllInits(scores);

      const Variable* var = Variable::get(variable);
      for(int initI = 0; initI < inits.size(); initI++) {
         int init = inits[initI];
         for(int d = 0; d < dates.size(); d++) {
            int date = dates[d];

            // Set up file
            std::string filename = getFilename(init, variable, configuration);
            NcFile ncfile0(filename.c_str());
            bool doExist  = ncfile0.is_valid();
            bool doAppend;

            // Metrics
            std::set<std::string> metricsSet;
            for(int i = 0; i < (int) scores.size(); i++) {
               if(scores[i].getVariable() == variable) {
                  std::string name = scores[i].getMetric();
                  metricsSet.insert(name);
               }
            }
            std::vector<std::string> metrics(metricsSet.begin(), metricsSet.end());

            // Determine if we should append to the verification file, or create a new file
            if(doExist) {
               doAppend = true;
               // Check if dimesions match
               if(ncfile0.get_dim("Offset")->size() != offsets.size() || 
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
            NcDim* dimLocation;

            // Variables
            NcVar* varLocation;
            NcVar* varLat;
            NcVar* varLon;
            NcVar* varElev;
            NcVar* varOffset;
            NcVar* varDate;
            std::vector<NcVar*> varMetrics;

            ///////////////////////
            // Create a new file //
            ///////////////////////
            if(!doAppend) {
               dimDate     = ncfile.add_dim("Date");
               assert(offsets.size() > 0);
               dimOffset   = ncfile.add_dim("Offset", offsets.size());
               assert(locations.size() > 0);
               dimLocation = ncfile.add_dim("Location", locations.size());

               // Variables
               varLocation = ncfile.add_var("Location",     ncInt,   dimLocation);
               varLat      = ncfile.add_var("Lat",          ncFloat, dimLocation);
               varLon      = ncfile.add_var("Lon",          ncFloat, dimLocation);
               varElev     = ncfile.add_var("Elev",         ncFloat, dimLocation);
               varOffset   = ncfile.add_var("Offset",       ncFloat, dimOffset);
               varDate     = ncfile.add_var("Date",         ncInt, dimDate);

               std::map<std::string, int>::const_iterator it;
               for(int i = 0; i < metrics.size();  i++) {
                  std::string metricName = metrics[i];
                  NcVar* varMetric = ncfile.add_var(metricName.c_str(), ncFloat, dimDate, dimOffset, dimLocation);
                  varMetrics.push_back(varMetric);
               }

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
               ncfile.add_att("Init",     init);
            }
            ///////////////////
            // Existing file //
            ///////////////////
            else {
               dimDate     = ncfile.get_dim("Date");
               dimOffset   = ncfile.get_dim("Offset");
               dimLocation = ncfile.get_dim("Location");
               //std::cout << " dim locations = " << dimLocation->size() << std::endl;

               // Variables
               varLocation = ncfile.get_var("Location");
               varLat      = ncfile.get_var("Lat");
               varLon      = ncfile.get_var("Lon");
               varElev     = ncfile.get_var("Elev");
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
            delete[] cdates;
            // Add current date
            bool foundCurrDate = false;
            for(int i = 0; i < dates.size(); i++) {
               if(dates[i] == date) {
                  foundCurrDate = true;
               }
            }
            if(!foundCurrDate) {
               dates.push_back(date);
            }

            // Write dimension values
            writeVariable(varOffset, offsets);
            writeVariable(varDate,   dates);
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
            writeVariable(varLocation, locationIds);
            writeVariable(varLat, lats);
            writeVariable(varLon, lons);
            writeVariable(varElev, elevs);

            // Write Metric data
            for(int i = 0; i < (int) scores.size(); i++) {
               Score score = scores[i];
               if(score.getInit() == init) {
                  int dateIndex     = getPosition(dates, score.getDate());
                  int metricIndex   = getPosition(metrics, score.getMetric());
                  int locationIndex = getPosition(locationIds, score.getLocation().getId());
                  int offsetIndex   = getPosition(offsets, score.getOffset());
                  float value       = score.getValue();
                  assert(Global::isValid(metricIndex));
                  assert(Global::isValid(dateIndex));
                  assert(Global::isValid(locationIndex));
                  assert(Global::isValid(offsetIndex));
                  varMetrics[metricIndex]->set_cur(dateIndex, offsetIndex, locationIndex);
                  varMetrics[metricIndex]->put(&value, 1,1,1);
               }
            }
            ncfile.close();
         }
      }
   }
}

std::string OutputVerif::getFilename(int iInit, std::string iVariable, std::string iConfiguration) const {
   // Clear file
   std::stringstream ss;
   ss << getOutputDirectory() << iVariable << "_" << iConfiguration << "_" << iInit << ".nc";
   return ss.str();
}
void OutputVerif::writeVariable(NcVar* iVariable, const std::map<float,int>& iValues) const {
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
void OutputVerif::writeVariable(NcVar* iVariable, const std::vector<float>& iValues) const {
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
void OutputVerif::writeVariable(NcVar* iVariable, const std::vector<std::string>& iValues) const {
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
