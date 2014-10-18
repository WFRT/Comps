#include "Run.h"
#include "InputContainer.h"
#include "VarConf.h"

Run::Run(std::string iTag) : mRunName(iTag) {
   Options runOptions = loadRunOptions(iTag);
   init(runOptions);
}
Run::Run(const Options& iOptions) {
   init(iOptions);
}
Run::~Run() {
   delete mInputContainer;
   delete mDefaultData;
   std::map<std::string, Data*>::iterator it;
   for(it = mDefaultDataVariables.begin(); it != mDefaultDataVariables.end(); it++)
      delete it->second;
   for(int i = 0; i < mConfigDatas.size(); i++) {
      delete mConfigDatas[i];
   }
}

void Run::init(const Options& iOptions) {
   mRunOptions = iOptions;

   // Create input service
   mInputContainer = new InputContainer(Options(""));

   // Get variable/configurations
   std::map<std::string, std::vector<std::string> > varConfs;
   std::map<std::string, std::vector<std::string> > varMetrics;
   loadVarConfs(mRunOptions, varConfs, varMetrics, mVariables);

   // Create a default data object
   // Each configuration needs its own data object, since a configuration can choose its own
   // input sources, downscaler, etc. However we to reuse the downscaler, etc for those
   // configurations that do not need customization. Therefore create a default data object that can
   // reused if needed.
   Options dataOptions0;
   dataOptions0.addOption("runName", mRunName);
   Options::copyOption("inputs",    mRunOptions, dataOptions0);
   Options::copyOption("qcs",       mRunOptions, dataOptions0);
   Options::copyOption("variables", mRunOptions, dataOptions0);
   if(!dataOptions0.hasValues("inputs")) {
      std::stringstream ss;
      ss << "Cannot initialize data object. 'inputs' option not provided for run '"
         << mRunName << "'";
      Global::logger->write(ss.str(), Logger::error);
   }
   mDefaultData = new Data(dataOptions0, mInputContainer);

   // Loop over all variables
   for(int v = 0; v < mVariables.size(); v++) {
      std::string variable = mVariables[v];
      // Loop over all configurations
      std::vector<std::string> configurations = varConfs[variable];
      for(int c = 0; c < configurations.size(); c++) {
         // Set up options for configuration
         Options configOptions = Configuration::getOptions(configurations[c]);

         Options dataOptionsV = dataOptions0;

         dataOptionsV.addOption("configuration", configurations[c]);

         // Assign the downscaler from this configuration to the forecast variable
         std::vector<std::string> vars;
         mRunOptions.getValues("downscalerVariables", vars);
         vars.push_back(variable);
         std::vector<std::string> downscalers;
         mRunOptions.getValues("downscalers", downscalers);
         std::string currentDownscaler;
         configOptions.getValue("downscaler", currentDownscaler);
         downscalers.push_back(currentDownscaler);
         dataOptionsV.addOptions("downscalerVariables", vars);
         dataOptionsV.addOptions("downscalers", downscalers);

         ////////////////////////////////////////
         // Pass run options to configurations //
         ////////////////////////////////////////
         // Pass down which output offsets we are producing for (parameter estimation needs it)
         Options::copyOption("offsets", mRunOptions, configOptions);
         Options::copyOption("spreader", mRunOptions, configOptions);
         if(!configOptions.hasValue("parameterIo")) {
            Options::copyOption("parameterIo", mRunOptions, configOptions);
         }
         if(!configOptions.hasValue("numOffsetsSpreadObs")) {
            Options::copyOption("numOffsetsSpreadObs", mRunOptions, configOptions);
         }
         Options::copyOption("numDaysParameterSearch", mRunOptions, configOptions);
         if(!configOptions.hasValue("pooler")) {
            // Set up how to choose which obs to use in parameter estimation
            Options::copyOption("pooler", mRunOptions, configOptions);
         }

         /////////////////////////////////
         // Pass config options to data //
         /////////////////////////////////
         dataOptionsV.addOption("runName", mRunName);
         Options::copyOption("inputs",       configOptions, dataOptionsV);
         Options::copyOption("qcs",          configOptions, dataOptionsV);
         Options::copyOption("downscaler",   configOptions, dataOptionsV);
         // Append variables from run and configuration
         Options::appendOption("variables",  configOptions, dataOptionsV);
         if(!dataOptionsV.hasValues("inputs")) {
            std::stringstream ss;
            ss << "Cannot initialize data object. 'inputs' neither provided for run '"
               << mRunName << "' nor for configuration '"
               << configurations[c] << "'.";
            Global::logger->write(ss.str(), Logger::error);
         }
         Data* data = new Data(dataOptionsV, mInputContainer);
         mConfigDatas.push_back(data);
         Configuration* conf = Configuration::getScheme(configOptions, *data);
         mVarConfs[variable].push_back(conf);
      }

      // Metrics
      std::vector<std::string> metrics = varMetrics[variable];
      for(int m = 0; m < metrics.size(); m++) {
         Metric* metric = Metric::getScheme(metrics[m], *mDefaultDataVariables[variable]);
         mMetrics[variable].push_back(metric);
      }
   }

   // Outputs
   std::vector<std::string> outputsString;
   //! What output schemes should the forecasts we written to?
   mRunOptions.getValues("outputs", outputsString);
   for(int i = 0; i < outputsString.size(); i++) {
      Output* output = Output::getScheme(outputsString[i], *mDefaultData);
      mOutputs.push_back(output);
   }

   // Debug
   int debug = mDefaultDebugLevel;
   //! How much debug and status messages should be shown? 0 very little, 1000 all. Default to 50 (a
   //! reasonable amount).
   mRunOptions.getValue("debug", debug);
   Global::logger->setMaxLevel((Logger::Level) debug);

   ///////////////
   // Locations //
   ///////////////
   std::vector<std::string> locationTags;
   //! Which locations should forecasts be produced for? Give a list of datasets. If no list given
   //! the locations from the observation dataset will be used.
   if(mRunOptions.getValues("locations", locationTags)) {
      for(int i = 0; i < locationTags.size(); i++) {
         Input* input = mInputContainer->getInput(locationTags[i]);
         if(input != NULL) {
            std::vector<Location> locations = input->getLocations();
            for(int k = 0; k < locations.size(); k++) {
               mLocations.push_back(locations[k]);
            }
         }
         else {
            // This code should not execute, because if the input scheme doesn't exist, the program 
            // will already have been aborted. This is to protect against future changes.
            std::stringstream ss;
            ss << "Error when reading 'locations' option in " << getName() 
               << " run. Input '" << locationTags[i] << "' does not exist.";
            Global::logger->write(ss.str(), Logger::error);
         }
      }
   }
   else {
      // Assume that we want to forecast for locations where have obs
      Input* input = mDefaultData->getObsInput();
      // Otherwise assume that we want to forecast for locations where we have forecasts
      if(input == NULL) {
         input = mDefaultData->getInput();
         std::stringstream ss;
         ss << "No observation dataset specified. Producing forecasts for each location in the main forecast dataset.";
         Global::logger->write(ss.str(), Logger::warning);
      }
      if(input != NULL) {
         mLocations = input->getLocations();
      }
   }
   if(mLocations.size() == 0) {
      std::stringstream ss;
      ss << "Run: No locations to produce forecasts for are defined for run '"
         << mRunName << "'. Either add an observation dataset to the run's 'inputs' option, "
         << "or use the 'locations' option to specify which datasets to use locations from.";
      Global::logger->write(ss.str(), Logger::error);
   }

   std::vector<int> useLocations;
   //! Only produce forecasts for these location ids
   if(mRunOptions.getValues("locationIds", useLocations)) {
      std::vector<Location> temp = mLocations;
      mLocations.clear();
      for(int k = 0; k < (int) temp.size(); k++) {
         for(int i = 0; i < (int) useLocations.size(); i++) {
            if(temp[k].getId() == useLocations[i]) {
               mLocations.push_back(temp[k]);
            }
         }
      }
      if(mLocations.size() == 0) {
         std::stringstream ss;
         ss << "No valid locations selected by 'locationIds' run option";
         Global::logger->write(ss.str(), Logger::error);
      }
   }

   // Remove locations within missing lat/lon
   for(int i = mLocations.size()-1; i >= 0; i--) {
      float lat = mLocations[i].getLat();
      float lon = mLocations[i].getLon();
      if(!Global::isValid(lat) || !Global::isValid(lon))
         mLocations.erase(mLocations.begin() + i);
   }

   std::vector<float> latRange(2, Global::MV);
   std::vector<float> lonRange(2, Global::MV);
   //! Only produce forecasts for locations within this range of latitudes: southLat,northLat
   mRunOptions.getValues("latRange", latRange);
   if(latRange.size() != 2) {
      std::stringstream ss;
      ss << "latRange must have two values: <south>,<north>";
      Global::logger->write(ss.str(), Logger::error);
   }
   //! Only produce forecasts for locations within this range of longitudes: southLon,northLon
   mRunOptions.getValues("lonRange", lonRange);
   if(lonRange.size() != 2) {
      std::stringstream ss;
      ss << "lonRange must have two values: <west>,<east>";
      Global::logger->write(ss.str(), Logger::error);
   }

   // Remove locations that are outside the lat/lon range
   float minLat = latRange[0];
   float maxLat = latRange[1];
   float minLon = lonRange[0];
   float maxLon = lonRange[1];
   if(Global::isValid(minLat) ||Global::isValid(maxLat) || Global::isValid(minLon) || Global::isValid(maxLon)) {
      std::vector<Location> newLocations;
      for(int i = 0; i < mLocations.size(); i++) {
         float lat = mLocations[i].getLat();
         float lon = mLocations[i].getLon();
         if((!Global::isValid(minLat) || lat >= minLat) && (!Global::isValid(maxLat) || lat <= maxLat) &&
            (!Global::isValid(minLon) || lon >= minLon) && (!Global::isValid(maxLon) || lon <= maxLon)) {
            newLocations.push_back(mLocations[i]);
         }
      }
      mLocations = newLocations;
   }

   /////////////
   // Offsets //
   /////////////
   //! Produce forecasts for these offsets. If unspecified, use the offsets from the 'offsetTag'
   //! dataset or if that is unspecified, use the offsets from the forecast input dataset.
   if(mRunOptions.getValues("offsets", mOffsets)) {
      std::stringstream ss;
      ss << "Using offsets from run specifications";
      Global::logger->write(ss.str(), Logger::critical);
   }
   else {
      Input* input;
      std::string offsetSetTag;
      //! Produce forecasts for the same offsets that the 'offsetTag' input dataset has.
      if(mRunOptions.getValue("offsetTag", offsetSetTag)) {
         input = mInputContainer->getInput(offsetSetTag);
      }
      else {
         // Default to main forecast set, because ...
         input = mDefaultData->getInput();
      }
      std::stringstream ss;
      ss << "Using offsets from dataset " << input->getName();
      Global::logger->write(ss.str(), Logger::status);
      mOffsets = input->getOffsets();
   }
}

Options Run::loadRunOptions(std::string iTag) const {
   int dotPosition = -1;
   for(int i = 0; i < iTag.size(); i++) {
      if(iTag[i] == '.')
         dotPosition = i;
   }
   std::string filename;
   std::string tag;
   if(dotPosition == -1) {
      std::stringstream ss;
      ss << Namelist::getDefaultLocation() << "default/runs.nl";
      filename = ss.str();
      tag = iTag;
   }
   else {
      assert(dotPosition != 0);
      std::string folder = iTag.substr(0,dotPosition);
      tag = iTag.substr(dotPosition+1);//, iTag.size()-2);
      std::stringstream ss;
      ss << Namelist::getDefaultLocation() << folder << "/runs" << ".nl";
      filename = ss.str();
   }
   Namelist nlRuns(filename);
   Options options;
   if(!nlRuns.getOptions(tag, options)) {
      std::stringstream ss;
      ss << "Run " << iTag << " does not exist";
      Global::logger->write(ss.str(), Logger::error);
   }
   return options;
}

void Run::loadVarConfs(const Options& iRunOptions,
      std::map<std::string, std::vector<std::string> >& iVarConfs,
      std::map<std::string, std::vector<std::string> >& iMetrics,
      std::vector<std::string>& iVariables) const {
   // Variable-configurations
   std::vector<std::string> varConfs0;
   std::set<std::string> variables;
   //! Which variable-configurations should be run?
   iRunOptions.getRequiredValues("varconfs", varConfs0);

   for(int v = 0; v < (int) varConfs0.size(); v++) {
      std::string currVarConf = varConfs0[v];
      // Read from file
      int dotPosition = -1;
      for(int i = 0; i < currVarConf.size(); i++) {
         if(currVarConf[i] == '.')
            dotPosition = i;
      }
      std::string folder;
      std::string tag;
      if(dotPosition == -1) {
         folder = "default";
         tag = currVarConf;
      }
      else {
         assert(dotPosition != 0);
         folder = currVarConf.substr(0,dotPosition);
         tag = currVarConf.substr(dotPosition+1);//, iTag.size()-2);
      }
      Namelist nlVarConfs("varconfs", folder);

      Options options;
      if(!nlVarConfs.getOptions(tag, options)) {
         std::stringstream ss;
         ss << "Variable/Configuration '" << tag << "' does not exist";
         Global::logger->write(ss.str(), Logger::error);
      }
      VarConf varConf(options);
      // Variable
      std::string variable = varConf.getVariable();
      variables.insert(variable);

      // Configuration
      std::vector<std::string> configurations = varConf.getConfigurations();
      for(int i = 0; i < configurations.size(); i++)
         iVarConfs[variable].push_back(configurations[i]);

      // Set up metrics
      std::vector<std::string> metricTags = varConf.getMetrics();
      for(int i = 0; i < metricTags.size(); i++)
         iMetrics[variable].push_back(metricTags[i]);
   }
   iVariables = std::vector<std::string>(variables.begin(), variables.end());
}
void Run::getRunOptions(Options& iOptions) const {
   iOptions = mRunOptions;
}

std::vector<Output*> Run::getOutputs() const {
   return mOutputs;
}

Data* Run::getData() const {
   return mDefaultData;
}
std::vector<Location> Run::getLocations() const {
   return mLocations;
}
std::vector<float> Run::getOffsets() const {
   return mOffsets;
}
std::vector<std::string> Run::getVariables() const {
   return mVariables;
}
std::vector<Configuration*> Run::getConfigurations(const std::string& iVariable) const {
   std::vector<Configuration*> configurations;
   std::map<std::string, std::vector<Configuration*> >::const_iterator it = mVarConfs.find(iVariable);
   if(it == mVarConfs.end()) {
      std::stringstream ss;
      ss << "Run: No configuration specified for variable " << iVariable;
      Global::logger->write(ss.str(), Logger::debug);
   }
   else {
      configurations =  it->second;
   }
   return configurations;
}

std::vector<Configuration*> Run::getConfigurations() const {
   std::vector<Configuration*> configurations;
   std::map<std::string, std::vector<Configuration*> >::const_iterator it;
   for(it = mVarConfs.begin(); it != mVarConfs.end(); it++) {
      for(int i = 0; i < it->second.size(); i++) {
         configurations.push_back(it->second[i]);
      }
   }
   return configurations;
}

std::map<std::string, std::vector<Configuration*> > Run::getVarConfs() const {
   return mVarConfs;
}

std::vector<Metric*> Run::getMetrics(const std::string& iVariable) const {
   std::vector<Metric*> metrics;
   std::map<std::string, std::vector<Metric*> >::const_iterator it = mMetrics.find(iVariable);
   if(it == mMetrics.end()) {
      std::stringstream ss;
      ss << "Run: No metrics specified for variable " << iVariable;
      Global::logger->write(ss.str(), Logger::debug);
   }
   else {
      metrics = it->second;
   }
   return metrics;
}

std::string Run::getName() const {
   return mRunName;
}
