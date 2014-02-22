#include "Run.h"
#include "InputContainer.h"

Run::Run(std::string iTag) : mRunName(iTag) {
   Options runOptions = loadRunOptions(iTag);
   init(runOptions);
}
Run::Run(const Options& iOptions) {
   init(iOptions);
}
void Run::init(const Options& iOptions) {
   mRunOptions = iOptions;

   // Create input service
   mInputContainer = new InputContainer(Options(""));

   // Create a default data object
   // Each configuration needs its own data object, since a configuration can choose its own
   // input sources, downscaler, etc. However we to reuse the downscaler, etc for those
   // configurations that do not need customization. Therefore create a default data object that can
   // reused if needed.
   Options dataOptions0;
   dataOptions0.addOption("runName", mRunName);
   Options::copyOption("inputs", mRunOptions, dataOptions0);
   Options::copyOption("qcs",    mRunOptions, dataOptions0);
   if(!dataOptions0.hasValue("inputs")) {
      std::stringstream ss;
      ss << "Cannot initialize data object. 'inputs' option not provided for run '"
         << mRunName << "'";
      Global::logger->write(ss.str(), Logger::error);
   }
   mDefaultData = new Data(dataOptions0, mInputContainer);

   // Get variable/configurations
   std::map<std::string, std::vector<std::string> > varConfs;
   std::map<std::string, std::vector<std::string> > varMetrics;
   loadVarConfs(mRunOptions, varConfs, varMetrics, mVariables);

   // Loop over all variables
   for(int v = 0; v < mVariables.size(); v++) {
      std::string variable = mVariables[v];

      // Loop over all configurations
      std::vector<std::string> configurations = varConfs[variable];
      for(int c = 0; c < configurations.size(); c++) {
         // Set up options for configuration
         Options configOptions;
         Configuration::getOptions(configurations[c], configOptions);

         ////////////////////////////////////////
         // Pass run options to configurations //
         ////////////////////////////////////////
         Options::copyOption("offsets", mRunOptions, configOptions); // This is needed by the parameter estimation
         if(!configOptions.hasValue("parameterIo")) {
            Options::copyOption("parameterIo", mRunOptions, configOptions);
         }
         if(!configOptions.hasValue("inputs")) {
            Options::copyOption("inputs", mRunOptions, configOptions);
         }
         if(!configOptions.hasValue("numOffsetsSpreadObs")) {
            Options::copyOption("numOffsetsSpreadObs", mRunOptions, configOptions);
         }
         Options::copyOption("numDaysParameterSearch", mRunOptions, configOptions);
         if(!configOptions.hasValue("pooler")) {
            // Set up how to choose which obs to use in parameter estimation
            Options::copyOption("pooler", mRunOptions, configOptions);
         }
         // Pass down which output offsets we are producing for
         Options::copyOption("offsets", mRunOptions, configOptions);

         ////////////////////////////////////////
         // Create data for this configuration //
         ////////////////////////////////////////
         Data* data;
         if(configOptions.hasValue("inputs") || configOptions.hasValue("qcs") || configOptions.hasValue("downscaler")) {
            // Create custom data object. NOTE: If any of options are cutomized by configuration,
            // then the downscaler and qcs cannot be reused, since their cached values
            // may be incorrect

            // Initialize with default, then overwrite
            Options dataOptions = dataOptions0;
            dataOptions.addOption("runName", mRunName);
            Options::copyOption("inputs", configOptions, dataOptions);
            // Only copy if it is there
            Options::copyOption("qcs",    configOptions, dataOptions);
            Options::copyOption("downscaler", configOptions, dataOptions);
            if(!dataOptions.hasValue("inputs")) {
               std::stringstream ss;
               ss << "Cannot initialize data object. 'inputs' neither provided for run '"
                  << mRunName << "' nor for configuration '"
                  << configurations[c] << "'.";
               Global::logger->write(ss.str(), Logger::error);
            }
            data = new Data(dataOptions, mInputContainer);
            mConfigDatas.push_back(data);
         }
         else {
            // No need to create a custom data object, use the default one
            data = mDefaultData;
         }
         Configuration* conf = Configuration::getScheme(configOptions, *data);
         mVarConfs[variable].push_back(conf);
      }

      // Metrics
      std::vector<std::string> metrics = varMetrics[variable];
      for(int m = 0; m < metrics.size(); m++) {
         Metric* metric = Metric::getScheme(metrics[m], *mDefaultData);
         mMetrics[variable].push_back(metric);
      }
   }

   // Outputs
   std::vector<std::string> outputsString;
   mRunOptions.getValues("outputs", outputsString);
   for(int i = 0; i < outputsString.size(); i++) {
      Output* output = Output::getScheme(outputsString[i], *mDefaultData);
      mOutputs.push_back(output);
   }

   // Debug
   int debug;
   if(!mRunOptions.getValue("debug", debug)) {
      debug = 0;
   }
   Global::logger->setMaxLevel((Logger::Level) debug);

   //////////////
   // Location //
   //////////////
   std::string locationSetTag;
   if(mRunOptions.getValue("locationTag", locationSetTag)) {
      Input* input = mInputContainer->getInput(locationSetTag);
      mLocations = input->getLocations();
   }
   else {
      // Assume that we want to forecast for locations where have obs
      Input* input = mDefaultData->getObsInput();
      if(input != NULL) {
         mLocations = input->getLocations();
      }
   }
   if(mLocations.size() == 0) {
      std::stringstream ss;
      ss << "Run: No locations to produce forecasts for are defined for run '"
         << mRunName << "'. Either add an observation dataset to the run's 'inputs' option, "
         << "or use the 'locationTag' option to specify which dataset to use locations from.";
      Global::logger->write(ss.str(), Logger::error);
   }
   std::vector<int> useLocations;
   if(mRunOptions.getValues("locations", useLocations)) {
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
         ss << "No valid locations selected by 'locations' run option";
         Global::logger->write(ss.str(), Logger::error);
      }
   }

   /////////////
   // Offsets //
   /////////////
   if(mRunOptions.getValues("offsets", mOffsets)) {
      std::stringstream ss;
      ss << "Using offsets from run specifications";
      Global::logger->write(ss.str(), Logger::critical);
   }
   else {
      Input* input;
      std::string offsetSetTag;
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

Run::~Run() {
   delete mInputContainer;
   delete mDefaultData;
   for(int i = 0; i < mConfigDatas.size(); i++) {
      delete mConfigDatas[i];
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
   std::string optString = nlRuns.findLine(tag);
   if(optString == "") {
      std::stringstream ss;
      ss << "Run " << iTag << " does not exist";
      Global::logger->write(ss.str(), Logger::error);
   }
   return Options(optString);
}

void Run::loadVarConfs(const Options& iRunOptions,
      std::map<std::string, std::vector<std::string> >& iVarConfs,
      std::map<std::string, std::vector<std::string> >& iMetrics,
      std::vector<std::string>& iVariables) const {
   // Variable-configurations
   std::vector<std::string> varConfs;
   std::set<std::string> variables;
   iRunOptions.getValues("varconfs", varConfs);
   if(varConfs.size() == 0) {
      Global::logger->write("No variable/configurations specified for this run", Logger::error);
   }
   for(int v = 0; v < (int) varConfs.size(); v++) {
      std::string currVarConf = varConfs[v];
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

      std::string line = nlVarConfs.findLine(tag);
      if(line == "") {
         std::stringstream ss;
         ss << "Variable/Configuration '" << tag << "' does not exist";
         Global::logger->write(ss.str(), Logger::error);
      }
      Options opt(line);
      // Variable
      std::string variable;
      opt.getRequiredValue("variable", variable);
      variables.insert(variable);

      // Configuration
      std::vector<std::string> configurations;
      opt.getRequiredValues("configurations", configurations);
      for(int i = 0; i < configurations.size(); i++)
         iVarConfs[variable].push_back(configurations[i]);

      // Set up metrics
      std::vector<std::string> metricTags;
      opt.getValues("metrics", metricTags);
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
