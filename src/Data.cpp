#include "Data.h"
#include "Location.h"
#include "Member.h"
#include "Scheme.h"
#include "Variables/Variable.h"
#include "Configurations/Configuration.h"
#include "Configurations/Default.h"
#include "Metrics/Metric.h"
#include "Downscalers/Downscaler.h"
#include "Downscalers/NearestNeighbour.h"
#include "Selectors/Clim.h"
#include "Selectors/Selector.h"
#include "Field.h"
#include "Qcs/Qc.h"
#include "Value.h"
#include "InputContainer.h"

Data::Data(Options iOptions, InputContainer* iInputContainer) :
      mInputContainer(iInputContainer), mCurrDate(Global::MV), mCurrOffset(Global::MV),
      mMainInputF(NULL), mMainInputO(NULL) { 

   iOptions.getValue("runName", mRunName);
   // Load inputs
   std::vector<std::string> datasets;
   iOptions.getValues("inputs", datasets);
   if(datasets.size() == 0) {
      std::stringstream ss;
      ss << "No default inputs are defined for this run";
      Global::logger->write(ss.str(), Logger::warning);
   }
   for(int i = 0; i < datasets.size(); i++) {
      loadInput(datasets[i]);
   }
   
   // Qcs
   std::vector<std::string> qcsString;
   iOptions.getValues("qcs", qcsString);
   for(int i = 0; i < qcsString.size(); i++) {
      Qc* qc = Qc::getScheme(qcsString[i], *this);
      mQcs.push_back(qc);
   }

   // Downscaler
   std::string downscalerTag;
   if(!iOptions.getValue("downscaler", downscalerTag)) {
      mDownscaler = Downscaler::getScheme(Options("tag=test class=DownscalerNearestNeighbour"));
   }
   else {
      mDownscaler = Downscaler::getScheme(downscalerTag);
   }

   // Set up climatology
   std::string climSelector;
   if(iOptions.getValue("climSelector", climSelector)){
      mClimSelector = Selector::getScheme(climSelector, *this);
   }
   else {
      Options opt("tag=t class=SelectorClim dayLength=15 hourLength=0 allowWrappedOffsets allowFutureValues futureBlackout=10");
      mClimSelector = new SelectorClim(opt, *this);
   }
   if(mClimSelector->needsTraining()) {
      // This should never happen, but lets safeguard against future changes to SelectorClim
      std::stringstream ss;
      ss << "The climatology selector defined for " << getRunName() << " requires training. This is not supported.";
      Global::logger->write(ss.str(), Logger::error);
   }
}

Data::~Data() {
   delete mClimSelector;
   delete mDownscaler;
}

Ensemble Data::getEnsemble(int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      const std::string& iVariable,
      const std::string& iDataset) const {
   Input* input = getInput(iDataset);
   Ensemble ens;

   if(input->hasVariable(iVariable)) {
      // Use data straight from input
      if(iLocation.getDataset() == input->getName()) {
         input->getValues(iDate, iInit, iOffset, iLocation.getId(), iVariable, ens);
      }
      // Downscale the input
      else {
         const std::vector<Member> members = input->getMembers();
         assert(mDownscaler != NULL);
         std::vector<float> values;
         for(int i = 0; i < members.size(); i++) {
            assert(members[i].getDataset() == input->getName());
            float value = mDownscaler->downscale(input, iDate, iInit, iOffset, iLocation, members[i].getId(), iVariable);
            values.push_back(value);
         }
         ens.setVariable(iVariable);
         ens.setValues(values);
      }
   }
   else {
      // Derived variable
      const std::vector<Member> members = input->getMembers();
      std::vector<float> values(members.size(), Global::MV);
      for(int i = 0; i < members.size(); i++) {
         float value = Variable::get(iVariable)->compute(*this, iDate, iInit, iOffset, iLocation, members[i], input->getType());
         values.push_back(value);
      }
      ens.setVariable(iVariable);
      ens.setValues(values);
   }
   qc(ens);
   return ens;
}

Ensemble Data::getEnsemble(int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      const std::string& iVariable,
      Input::Type iType) const {
   Ensemble ens;
   Input* input;
   if(hasVariable(iVariable, iType)) {
      input = getInput(iVariable, iType);
   }
   else {
      input = getInput();
   }

   if(input->hasVariable(iVariable)) {
      // Use data straight from input
      if(iLocation.getDataset() == input->getName()) {
         input->getValues(iDate, iInit, iOffset, iLocation.getId(), iVariable, ens);
      }
      // Downscale the input
      else {
         std::vector<Member> members;
         getMembers(iVariable, iType, members);
         assert(mDownscaler != NULL);
         std::vector<float> values;
         for(int i = 0; i < members.size(); i++) {
            assert(members[i].getDataset() == input->getName());
            float value = mDownscaler->downscale(input, iDate, iInit, iOffset, iLocation, members[i].getId(), iVariable);
            values.push_back(value);
         }
         ens.setVariable(iVariable);
         ens.setValues(values);
      }
   }
   else {
      // Derived variable
      std::vector<float> values;
      const std::vector<Member> members = input->getMembers();
      for(int i = 0; i < members.size(); i++) {
         float value = Variable::get(iVariable)->compute(*this, iDate, iInit, iOffset, iLocation, members[i], iType);
         values.push_back(value);
      }
      ens.setVariable(iVariable);
      ens.setValues(values);
   }
   qc(ens);
   return ens;
}

float Data::getValue(int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      const Member& iMember,
      const std::string& iVariable) const {
   std::string dataset = iMember.getDataset();
   Input* input = getInput(dataset);
   float value = Global::MV;
   if(input->hasVariable(iVariable)) {
      if(iLocation.getDataset() == input->getName()) {
         value = input->getValue(iDate, iInit, iOffset, iLocation.getId(), iMember.getId(), iVariable);
      }
      else {
         // Downscale
         value = mDownscaler->downscale(input, iDate, iInit, iOffset, iLocation, iMember.getId(), iVariable);
      }
   }
   else {
      // Downscale
      value = Variable::get(iVariable)->compute(*this, iDate, iInit, iOffset, iLocation, iMember, input->getType());
   }

   value = qc(value, iDate, iOffset, iLocation, iVariable, input->getType());
   return value;
}

void Data::loadInput(const std::string& iDataset) const {
   Input* input = mInputContainer->getInput(iDataset);
   Input::Type type = input->getType();
   mInputs[iDataset] = input;

   // We might still need to assign available variables, even if the input is already loaded,
   // because we might be loading for a different iType than before
   std::vector<std::string> variables = input->getVariables();
   for(int i = 0; i < (int) variables.size(); i++) {
      mHasVariables[iDataset][variables[i]] = true;

      if(type == Input::typeForecast) {
         mInputMapF[variables[i]] = input;
      }
      else if(type == Input::typeObservation) {
         mInputMapO[variables[i]] = input;
      }
   }
   mInputNames[iDataset];

   if(type == Input::typeForecast) {
      mInputsF[iDataset] = input;
      if(mMainInputF == NULL)
         mMainInputF = input;
   }
   else if(type == Input::typeObservation) {
      mInputsO[iDataset] = input;
      if(mMainInputO == NULL)
         mMainInputO = input;
   }
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
      std::map<std::string, Input*>::const_iterator it;
      it = mInputMapO.find(iVariable);
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

Input* Data::getInput() const {
   assert(mMainInputF != NULL);
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
         Global::logger->write(ss.str(), Logger::warning);
         return NULL;
      }
      counter++;
   }

   assert(hasVariable(var, iType));
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
      std::stringstream ss;
      ss << "Loading auxillary input " << iDataset;
      Global::logger->write(ss.str(), Logger::warning);
      return mInputContainer->getInput(iDataset);
   }
   else {
      Input* input = mInputs[iDataset];
      assert(input != NULL);
      return input;
   }
}

/*
void Data::setCurrTime(int iDate, float iOffset) {
   mCurrDate = iDate;
   mCurrOffset = iOffset;
}
*/

void Data::getObs(int iDate, int iInit, float iOffset, const Location& iLocation, std::string iVariable, Obs& iObs) const {
   float obs = Global::MV;
   std::string dataset = iLocation.getDataset();
   if(hasVariable(iVariable, Input::typeObservation)) {
      Input* input = getInput(iVariable, Input::typeObservation);
      assert(input != NULL);
      // This part works as long as the output locations come from the observation dataset
      // I.e. it doesn't work for load when it asks for T from wfrt.mv but for the load location
      if(input->getName() == dataset) {
         obs = input->getValue(iDate, iInit, iOffset, iLocation.getId(), 0, iVariable);
      }
   }
   else {
      // Use derived variable
      Input* input = getObsInput();
      if(input != NULL && input->getName() == dataset) {
         Member member(iLocation.getDataset(), 0);
         obs = Variable::get(iVariable)->compute(*this, iDate, iInit, iOffset, iLocation, member, Input::typeObservation);
      }
   }
   obs = qc(obs, iDate, iOffset, iLocation, iVariable, Input::typeObservation);
   iObs = Obs(obs, iDate, iInit, iOffset, iVariable, iLocation);
}

float Data::getClim(int iDate,
      int iInit,
      float iOffset, 
      const Location& iLocation,
      std::string iVariable) const {
   Ensemble ens = mClimSelector->select(iDate, iInit, iOffset, iLocation, iVariable, Parameters());
   float value = Global::MV;
   if(ens.size() == 1) {
      value = ens[0];
      value = qc(value, iDate, iOffset, iLocation, iVariable);
   }
   return value;
}

void Data::getMostRecentObs(int iDate,
      int iInit,
      float iOffset, 
      const Location& iLocation,
      std::string iVariable,
      Obs& iObs) const {

   Input* input = getInput(iVariable, Input::typeObservation);
   assert(input);
   std::vector<float> offsets = input->getOffsets();

   bool found = false;

   // Initialize
   int offsetIndex = offsets.size()-1;
   int date = iDate;
   int counter = 0;

   while(!found && counter < mMaxSearchRecentObs) {
      float offset = offsets[offsetIndex];
      // TODO:
      int init = 0;
      if(date < iDate || offset < iOffset) {
         float value = getValue(date, 0, offset, iLocation, Member("",0), iVariable);
         value = qc(value, date, offset, iLocation, iVariable, Input::typeObservation);
         if(Global::isValid(value)) {
            // We found a recent observation
            iObs = Obs(value, date, init, offset, iVariable, iLocation);
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
      counter++;
   }
   if(counter == mMaxSearchRecentObs) {
      std::stringstream ss;
      ss << "Data.cpp:getRecentObs cannot find a recent obs. Tried " << mMaxSearchRecentObs <<" recent times";
      Global::logger->write(ss.str(), Logger::critical);
   }
}

float Data::qc(float iValue, int iDate, float iOffset, const Location& iLocation, const std::string& iVariable, Input::Type iType) const {
   if(mQcs.size() == 0) {
      //std::cout << "Value = " << iValue << std::endl;
      return iValue;
   }
   else {
      for(int i = 0; i < mQcs.size(); i++) {
         // If any test fails, return missing
         // TODO:
         int init = 0;
         if(!mQcs[i]->check(Value(iValue, iDate, init, iOffset, iLocation, iVariable, iType))) {
            return Global::MV;
         }
      }
      return iValue;
   }
}

void Data::qc(Ensemble& iEnsemble) const {
   if(mQcs.size() > 0) {
      for(int i = 0; i < mQcs.size(); i++) {
         mQcs[i]->qc(iEnsemble);
      }
   }
}

void Data::getObsLocations(std::vector<Location>& iLocations) const {
   if(getObsInput() != NULL) {
      iLocations = getObsInput()->getLocations();
   }
}

void Data::getMembers(const std::string& iVariable, Input::Type iType, std::vector<Member>& iMembers) const {
   iMembers.clear();
   if(hasVariable(iVariable, iType)) {
      iMembers = getInput(iVariable, iType)->getMembers();
   }
   else {
      // Custom variable
      // TODO
      if(iType == Input::typeObservation) {
         if(getObsInput() != NULL)
            iMembers = getObsInput()->getMembers();
      }
      else
         iMembers = getInput()->getMembers();
      //iMembers.push_back(Member("custom", 0)); // Default member
   }
}

std::string Data::getRunName() const {
   return mRunName;
}

Downscaler* Data::getDownscaler() const {
   return mDownscaler;
}
