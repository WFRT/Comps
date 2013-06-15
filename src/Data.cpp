#include "Data.h"
#include "Location.h"
#include "Member.h"
#include "Scheme.h"
#include "Variables/Variable.h"
#include "Outputs/Output.h"
#include "Configurations/Configuration.h"
#include "Configurations/Default.h"
#include "Metrics/Metric.h"
#include "Downscalers/Downscaler.h"
#include "Selectors/Clim.h"

Data::Data(const std::string& iRunTag) :
      mRunTag(iRunTag), mCurrDate(Global::MV), mCurrOffset(Global::MV) { 
   init();
}

std::string Data::mParameterIo = "";
void Data::init() {
   int dotPosition = -1;
   for(int i = 0; i < mRunTag.size(); i++) {
      if(mRunTag[i] == '.')
         dotPosition = i;
   }
   std::string filename;
   std::string tag;
   if(dotPosition == -1) {
      std::stringstream ss;
      ss << Namelist::getDefaultLocation() << "default/runs.nl";
      filename = ss.str();
      tag = mRunTag;
   }
   else {
      assert(dotPosition != 0);
      std::string folder = mRunTag.substr(0,dotPosition);
      tag = mRunTag.substr(dotPosition+1);//, iTag.size()-2);
      std::stringstream ss;
      ss << Namelist::getDefaultLocation() << folder << "/runs" << ".nl";
      filename = ss.str();
   }
   Namelist nlRuns(filename);
   std::string optString = nlRuns.findLine(tag);
   if(optString == "") {
      std::stringstream ss;
      ss << "Run " << mRunTag << " does not exist";
      Global::logger->write(ss.str(), Logger::error);
   }
   mRunOptions = Options(optString);

   // Debug
   int debug;
   if(!mRunOptions.getValue("debug", debug)) {
      debug = 0;
   }
   Global::logger->setMaxLevel((Logger::Level) debug);

   std::vector<std::string> inputsF;
   mRunOptions.getRequiredValues("inputsF", inputsF);
   std::vector<std::string> inputsO;
   mRunOptions.getRequiredValues("inputsO", inputsO);
   assert(inputsF.size() > 0);

   // Add forecast inputs backwards so that fVariables ...
   for(int i = (int) inputsF.size() - 1; i >= 0; i--) {
      std::string inputName = inputsF[i];
      Input* input = loadInput(inputName, Data::typeForecast);

      if(i == 0) {
         input->getDates(mDates);
         mMainInputF = input;
      }
   }
   // Add observation inputs
   for(int i = (int) inputsO.size() - 1; i >= 0; i--) {
      std::string inputName = inputsO[i];
      Input* input = loadInput(inputName, Data::typeObservation);

      if(i == 0) {
         mMainInputO = input;
      }
   }

   // Parameters
   mRunOptions.getValue("parameterIo", mParameterIo);

   // Set up output
   std::string outputTag;
   mRunOptions.getRequiredValue("output", outputTag);
   Options outputOptions;
   Scheme::getOptions(outputTag, outputOptions);

   std::string outputInputTag;
   outputOptions.getRequiredValue("name", mOutputName);

   std::string locationSetTag;
   std::string offsetSetTag;
   Input* offsetInput;
   Input* locationInput;
   if(outputOptions.getValue("locationTag", locationSetTag) &&
      outputOptions.getValue("offsetTag", offsetSetTag)) {
      // Location
      locationInput = loadInput(locationSetTag, Data::typeNone);
      /*
      Options locationSetOptions;
      Scheme::getOptions(locationSetTag, locationSetOptions);
      std::string locationSet;
      locationSetOptions.getRequiredValue("name", locationSet);
      Input* input = loadInput(locationSet, Data::typeNone);
      locationInput = input;
      */
      // Offsets
      offsetInput = loadInput(offsetSetTag, Data::typeNone);
      /*
      Options offsetSetOptions;
      Scheme::getOptions(offsetSetTag, offsetSetOptions);
      std::string offsetSet;
      offsetSetOptions.getRequiredValue("name", offsetSet);
      input = loadInput(offsetSet, Data::typeNone);
      offsetInput = input;
      */
   }
   else {
      // Output locations and offsets are taken from output, but we want locations from output
      // and offsets from input.
      // Why? output locations are obvious, but input offsets because that is all we will have available
      std::string outputInputName;
      outputOptions.getRequiredValue("input", outputInputName);
      Input* input = loadInput(outputInputName, Data::typeNone);
      offsetInput = mMainInputF;
      locationInput = input;
   }

   // If offsets are specified for the run, use them
   if(mRunOptions.getValues("offsets", mOutputOffsets)) {
      std::stringstream ss;
      ss << "Using offsets from run specifications";
      Global::logger->write(ss.str(), Logger::critical);
   }
   else  {
      offsetInput->getOffsets(mOutputOffsets);
      std::stringstream ss;
      ss << "Using offsets from " << offsetInput->getName();
      Global::logger->write(ss.str(), Logger::status);
   }
   /*
   std::vector<float> useOffsets;
   if(mRunOptions.getValues("offsets", useOffsets)) {
      std::vector<float> temp = mOutputOffsets;
      mOutputOffsets.clear();
      for(int k = 0; k < (int) temp.size(); k++) {
         for(int i = 0; i < (int) useOffsets.size(); i++) {
            if(temp[k] == useOffsets[i]) {
               mOutputOffsets.push_back(temp[k]);
            }
         }
      }
      if(mOutputOffsets.size() == 0) {
         std::stringstream ss;
         ss << "No valid offsets selected by 'offsets' run option";
         Global::logger->write(ss.str(), Logger::error);
      }
   }
  */

   locationInput->getLocations(mOutputLocations);
   std::vector<int> useLocations;
   if(mRunOptions.getValues("locations", useLocations)) {
      std::vector<Location> temp = mOutputLocations;
      mOutputLocations.clear();
      for(int k = 0; k < (int) temp.size(); k++) {
         for(int i = 0; i < (int) useLocations.size(); i++) {
            if(temp[k].getId() == useLocations[i]) {
               mOutputLocations.push_back(temp[k]);
            }
         }
      }
      if(mOutputLocations.size() == 0) {
         std::stringstream ss;
         ss << "No valid locations selected by 'locations' run option";
         Global::logger->write(ss.str(), Logger::error);
      }
   }

   // Variable-configurations
   std::vector<std::string> varConfs;
   mRunOptions.getValues("varconfs", varConfs);
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
      Options opt(line);
      // Variable
      std::string variable;
      opt.getRequiredValue("variable", variable);
      mOutputVariables.push_back(variable);

      // Configuration
      std::vector<std::string> confTags;
      opt.getRequiredValues("configurations", confTags);
      std::vector<Configuration*> configurations;
      for(int k = 0; k < (int) confTags.size(); k++) {
         Options opt;
         Configuration::getOptions(confTags[k], opt);

         // Add in numDaysParameterSearch, which is specified by the runs, not 
         // in the configuration
         // NOTE: This is how we could do cascading attributes, from runs down to 
         // configuration
         int num;
         if(mRunOptions.getValue("numDaysParameterSearch", num)) {
            std::stringstream ss;
            ss << "numDaysParameterSearch=" << num;
            opt.addOption(ss.str());
         }
         Configuration* configuration = Configuration::getScheme(opt, *this);
         configurations.push_back(configuration);
      }
      mOutputConfigurations[variable] = configurations;
   }

   /*
   mOutput = Output::getScheme(outputOptions, *this);
   */

   // Set up metrics
   std::vector<std::string> metricTags;
   mRunOptions.getValues("metrics", metricTags);
   for(int i = 0; i < (int) metricTags.size(); i++) {
      Options metricOptions;
      Scheme::getOptions(metricTags[i], metricOptions);
      Metric* metric = Metric::getScheme(metricOptions, *this);
      mOutputMetrics.push_back(metric);
   }

   // Set up downscaler
   if(0)
   {
      std::string schemeTag;
      mRunOptions.getRequiredValue("downscaler", schemeTag);
      Options opt;
      Scheme::getOptions(schemeTag, opt);
      mDownscaler      = Downscaler::getScheme(opt, *this);
   }

   // Set up climatology
   {
      Options opt("tag=t class=SelectorClim dayLength=15 hourLength=0 allowWrappedOffsets allowFutureValues futureBlackout=10");
      mClimSelector = new SelectorClim(opt, *this);
   }

}

Data::~Data() {
   // Delete inputs
   std::map<std::string,Input*>::iterator it;
   for(it = mInputs.begin(); it != mInputs.end(); it++) {
      delete it->second;
   }
   // Delete output
   //delete mOutput;
   //delete mDownscaler;
   delete mClimSelector;
}

float Data::getValue(int iDate,
                     int iInit,
                     float iOffset, 
                     const Location& iLocation,
                     const Member& iMember,
                     std::string iVariable,
                     Input::Type iType) const {
   //std::cout << "Data::getValue(" << iDate << " I" << iInit << " O" << iOffset << " L" << iLocation.getId() << " M" << iMember.getId() << " " << iMember.getDataset() << " " << iVariable << " " << iType << ")" << std::endl;
   if(iType == Input::typeUnspecified) {
      // Find input based on member

      std::string dataset = iMember.getDataset();
      //assert(iMember.getDataset() != "");
      //assert(hasInput(dataset));

      if(!hasVariable(iVariable, dataset)) {
         // Use derived variable
         return Variable::get(iVariable)->compute(*this, iDate, iInit, iOffset, iLocation, iMember, iType);
      }
      else {
         Input* input = mInputs[dataset];
         //return mDownscaler->downscale(input, iDate, iInit, iOffset, iLocation, iMember, iVariable);
         assert(input->getName() == iLocation.getDataset());
         return input->getValue(iDate, iInit, iOffset, iLocation.getId(), iMember.getId(), iVariable);
      }
   }

   if(hasVariable(iVariable, iType)) {
      // Here we don't know which dataset this comes from. We just know if they want an obs or fcst

      Input* input = NULL;
      // Use raw data
      if(iType == Input::typeForecast) {
         input = mInputMapF[iVariable];
      }
      else if(iType == Input::typeObservation) {
         /*
         if(iDate > mCurrDate || (iDate == mCurrDate && iOffset == mCurrOffset)) {
            std::stringstream ss;
            ss << "WARNING: Attempted to retrive obs data for " << iDate << ":" << iOffset << "which is in the future (currDate = " << mCurrDate << ":" << mCurrOffset << ")";
            Global::logger->write(ss.str(), Logger::critical);
            assert(0);
            return Global::MV;
         }
         */
         input = mInputMapO[iVariable];
      }
      else if(iType == Input::typeBest) {
         // TODO
         input = mInputMapF[iVariable];
      }
      else {
         assert(0);
      }
      //return mDownscaler->downscale(input, iDate, iInit, iOffset, iLocation, iMember, iVariable);

      // This part works as long as the output locations come from the observation dataset
      // I.e. it doesn't work for load when it asks for T from wfrt.mv but for the load location
      assert(input->getName() == iLocation.getDataset());
      return input->getValue(iDate, iInit, iOffset, iLocation.getId(), iMember.getId(), iVariable);
   }
   else {
      // Use derived variable
      return Variable::get(iVariable)->compute(*this, iDate, iInit, iOffset, iLocation, iMember, iType);
   }
}

/*
void Data::getObs(int iDate,
      int iInit,
      float iOffset, 
      const Location& iLocation,
      std::string iVariable,
      Obs& iObs) const {
   float value = getValue(iDate, iInit, iOffset, iLocation, Member(0), iVariable, Input::typeObservation);
   iObs = Obs(value, iDate, iOffset, iVariable, iLocation);
}
  */

void Data::getRecentObs(const Location& iLocation,
      std::string iVariable,
      Obs& iObs) const {
   Input* input = getInput(iVariable, Input::typeObservation);
   assert(input);
   std::vector<float> offsets;
   input->getOffsets(offsets);

   bool found = false;
   int currDate     = getCurrentDate();
   float currOffset = getCurrentOffset();

   // Initialize
   int offsetIndex = offsets.size()-1;
   int date = currDate;
   int counter = 0;

   while(!found && counter < 1000) {
      float offset = offsets[offsetIndex];
      if(date < currDate || offset < currOffset) {
         float value = getValue(date, 0, offset, iLocation, Member("",0), iVariable, Input::typeObservation);
         if(Global::isValid(value)) {
            // We found a recent observation
            iObs = Obs(value, date, offset, iVariable, iLocation);
            found = true;
         }
      }

      // Decrement date/offset
      if(offsetIndex == 0) {
         // Look yesterday
         offsetIndex = offsets.size()-1;
         date = Global::getDate(date, 0, -24);
      }
      else {
         // Look earlier offset
         offsetIndex--;
      }
   }
   if(counter == 1000) {
      std::stringstream ss;
      ss << "Data.cpp:getRecentObs cannot find a recent obs. Tried 1000 recent times";
      Global::logger->write(ss.str(), Logger::critical);
   }
}

int Data::getCurrentDate() const {
   if(Global::isValid(mCurrDate)) {
      return mCurrDate;
   }
   else {
      // TODO: Set the current date
      assert(0);
      return 0;
   }
}
float Data::getCurrentOffset() const {
   if(Global::isValid(mCurrOffset)) {
      return mCurrOffset;
   }
   else {
      // TODO: Set the current date
      assert(0);
      return 0;
   }
}
void Data::setCurrentTime(int iDate, float iOffset) {
   mCurrDate = iDate;
   mCurrOffset = iOffset;
}

void Data::getEnsemble(int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      std::string iVariable,
      Input::Type iType,
      Ensemble& iEnsemble) const {
   assert(iType != Input::typeUnspecified);

   // TODO downscale

   if(hasVariable(iVariable, iType)) {
      Input* input;
      if(iType == Input::typeForecast) {
         input = mInputMapF[iVariable];
      }
      else if(iType == Input::typeObservation) {
         input = mInputMapO[iVariable];
      }
      else {
         // TODO
         assert(0);
      }
      // Use raw data
      //return mDownscaler->downscale(input, iDate, iInit, iOffset, iLocation, iMember, iVariable);
      assert(input->getName() == iLocation.getDataset());
      input->getValues(iDate, iInit, iOffset, iLocation.getId(), iVariable, iEnsemble);
   }
   else {
      // Derived variable
      std::vector<float> values;
      // TODO
      std::vector<Member> members;
      getMembers(iVariable, iType, members);
      for(int i = 0; i < members.size(); i++) {
         float value = Variable::get(iVariable)->compute(*this, iDate, iInit, iOffset, iLocation, members[i], iType);
         values.push_back(value);
      }
      iEnsemble.setVariable(iVariable);
      iEnsemble.setValues(values);
   }
}

void Data::getEnsemble(int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      const std::string& iDataset,
      const std::string& iVariable,
      Ensemble& iEnsemble) const {

   Input* input = getInput(iDataset);
   if(hasVariable(iVariable, iDataset)) {
      input->getValues(iDate, iInit, iOffset, iLocation.getId(), iVariable, iEnsemble);
   }
   else {
      // Derived variable
      std::vector<float> values;
      // TODO
      std::vector<Member> members;
      input->getMembers(members);
      values.resize(members.size());
      for(int i = 0; i < members.size(); i++) {
         float value = Variable::get(iVariable)->compute(*this, iDate, iInit, iOffset, iLocation, members[i]);
         values.push_back(value);
      }
      iEnsemble.setVariable(iVariable);
      iEnsemble.setValues(values);
   }
}

Input* Data::loadInput(std::string iTag, Data::Type iType) const {
   Options opt;
   Scheme::getOptions(iTag, opt);
   if(!hasInput(iTag)) {
      Input* input =  Input::getScheme(opt, *this);
      mInputs[iTag] = input;
   }
   Input* input = mInputs[iTag];

   // Unspecified datatype should not be used for forecasting or observations
   if(iType == Data::typeNone)
      return input;

   // We might still need to assign available variables, even if the input is already loaded,
   // because we might be loading for a different iType than before

   std::vector<std::string> variables;
   input->getVariables(variables);
   for(int i = 0; i < (int) variables.size(); i++) {
      mHasVariables[iTag][variables[i]] = true;

      if(iType == Data::typeForecast) {
         mInputMapF[variables[i]] = input;
      }
      else if(iType == Data::typeObservation) {
         mInputMapO[variables[i]] = input;
      }
   }
   mInputNames[iTag];

   if(iType == Data::typeForecast) {
      mInputsF[iTag] = input;
   }
   else if(iType == Data::typeObservation) {
      mInputsO[iTag] = input;
   }
   return input;
}

bool Data::hasVariable(const std::string& iVariable, const std::string& iDataset) const {
   if(!hasVariable(iVariable) || !hasInput(iDataset)) {
      return false;
   }
   assert(hasInput(iDataset));
   std::map<std::string, bool>::const_iterator it = mHasVariables[iDataset].find(iVariable);
   return it != mHasVariables[iDataset].end();
}
bool Data::hasVariable(const std::string& iVariable, Input::Type iType) const {
   if(iType == Input::typeForecast) {
      std::map<std::string, Input*>::const_iterator it = mInputMapF.find(iVariable);
      return it != mInputMapF.end();
   }
   else if(iType == Input::typeObservation) {
      std::map<std::string, Input*>::const_iterator it = mInputMapO.find(iVariable);
      return it != mInputMapO.end();
   }
   else {
      std::map<std::string, Input*>::const_iterator itF = mInputMapF.find(iVariable);
      std::map<std::string, Input*>::const_iterator itO = mInputMapO.find(iVariable);
      return itF != mInputMapF.end() || itO != mInputMapO.end();
   }
}

bool Data::hasInput(const std::string& iInputName) const {
   std::map<std::string, bool>::const_iterator it = mInputNames.find(iInputName);
   return it != mInputNames.end();
}

Output* Data::getOutput(int iDate, int iInit, const std::string& iVariable, const Configuration& iConfiguration) const {
   // output
   std::string outputTag;
   mRunOptions.getRequiredValue("output", outputTag);
   Options outputOptions;
   Scheme::getOptions(outputTag, outputOptions);
   return Output::getScheme(outputOptions, *this, iDate, iInit, iVariable, iConfiguration);
}
void Data::getOutputLocations(std::vector<Location>& iLocations) const {
   iLocations = mOutputLocations;
}
void Data::getOutputOffsets(std::vector<float>& iOffsets) const {
   iOffsets = mOutputOffsets;
}

Options Data::getRunOptions() const {
   return mRunOptions;

}

void Data::getOutputMetrics(std::vector<Metric*>& iMetrics) const {
   iMetrics = mOutputMetrics;
}

void Data::getOutputConfigurations(const std::string& iVariable, std::vector<Configuration*>& iConfigurations) const {
   std::map<std::string, std::vector<Configuration*> >::const_iterator it = mOutputConfigurations.find(iVariable);
   if(it == mOutputConfigurations.end()) {
      std::stringstream ss;
      ss << "Run: No configuration specified for variable " << iVariable;
      Global::logger->write(ss.str(), Logger::debug);
   }
   else {
      for(int i = 0; i < (int) it->second.size(); i++) {
         iConfigurations.push_back(it->second[i]);
      }
   }
}
void Data::getOutputVariables(std::vector<std::string>& iVariables) const {
   iVariables = mOutputVariables;
}

std::string Data::getOutputName() const {
   return mOutputName;
}

void Data::getMembers(const std::string& iVariable, Input::Type iType, std::vector<Member>& iMembers) const {
   iMembers.clear();
   if(hasVariable(iVariable, iType)) {
      getInput(iVariable, iType)->getMembers(iMembers);
   }
   else {
      // Custom variable
      // TODO
      if(iType == Input::typeObservation)
         mMainInputO->getMembers(iMembers);
      else
         mMainInputF->getMembers(iMembers);
      //iMembers.push_back(Member("custom", 0)); // Default member
   }
}

void Data::getDates(std::vector<int>& iDates) const {
   iDates = mDates;
}

void Data::getDates(std::string iDataset, std::vector<int>& iDates) const {
   assert(hasInput(iDataset));
   mInputs[iDataset]->getDates(iDates);
}
Input* Data::getInput() const {
   return mMainInputF;
}

Input* Data::getInput(const std::string& iVariable, Input::Type iType) const {
   std::string var = iVariable;
   int counter = 0;
   while(!hasVariable(var, iType)) {
      var = Variable::get(var)->getBaseVariable();
      if(counter > 10) {
         std::stringstream ss;
         ss << "Data::getInput: Cannot find basevariable of " << iVariable
            << ". Recursion limit reached";
         Global::logger->write(ss.str(), Logger::error);
      }
      counter++;
   }

   assert(hasVariable(iVariable, iType));
   if(iType == Input::typeForecast) {
      return mInputMapF[var];
   }
   else if(iType == Input::typeObservation) {
      return mInputMapO[var];
   }
   else {
      // TODO:
      assert(0);
   }

}
Input* Data::getObsInput() const {
   return mMainInputO;
}

Input* Data::getInput(const std::string& iDataset) const {
   if(!hasInput(iDataset)) {
      loadInput(iDataset, Data::typeNone);
   }
   return mInputs[iDataset];
}

/*
void Data::setCurrTime(int iDate, float iOffset) {
   mCurrDate = iDate;
   mCurrOffset = iOffset;
}
*/

void Data::getObs(int iDate, int iInit, float iOffset, const Location& iLocation, std::string iVariable, Obs& iObs) const {
   if(hasVariable(iVariable, Input::typeObservation)) {
      Input* input = getInput(iVariable, Input::typeObservation);

      // This part works as long as the output locations come from the observation dataset
      // I.e. it doesn't work for load when it asks for T from wfrt.mv but for the load location
      assert(input->getName() == iLocation.getDataset());
      float obs = input->getValue(iDate, iInit, iOffset, iLocation.getId(), 0, iVariable);
      iObs = Obs(obs, iDate, iOffset, iVariable, iLocation);
   }
   else {
      // Use derived variable
      Member member(iLocation.getDataset(), 0);
      float obs = Variable::get(iVariable)->compute(*this, iDate, iInit, iOffset, iLocation, member, Input::typeObservation);
      iObs = Obs(obs, iDate, iOffset, iVariable, iLocation);
   }
}

float Data::getClim(int iDate,
      int iInit,
      float iOffset, 
      const Location& iLocation,
      std::string iVariable) const {
   std::vector<Slice> slices;
   mClimSelector->select(iDate, iInit, iOffset, iLocation, iVariable, Parameters(), slices);
   float total = 0;
   int counter = 0;
   for(int i = 0; i < (int) slices.size(); i++) {
      Obs obs;
      getObs(slices[i].getDate(), slices[i].getInit(), slices[i].getOffset(), iLocation, iVariable, obs);
      if(Global::isValid(obs.getValue())) {
         total += obs.getValue();
         counter ++;
      }
   }
   if(counter > 0)
      return total / counter;
   else
      return Global::MV;
}

std::string Data::getParameterIo() {
   return mParameterIo;
}