#include "VarSelector.h"
#include "SchemesHeader.inc"
#include "../Options.h"
#include "../Selectors/Analog.h"
#include "../Scheme.h"
#include "../Inputs/Input.h"
#include "../Data.h"
#include "../Parameters.h"
#include "../Field.h"
#include "../Location.h"
#include "../DetMetrics/DetMetric.h"

VarSelector::VarSelector(const Options& iOptions, const Data& iData) :
      Component(iOptions),
      mData(iData),
      mMinValidDates(100) {
   iOptions.getRequiredValue("startDate", mStartDate);
   iOptions.getRequiredValue("endDate", mEndDate);
   iOptions.getRequiredValue("variable", mVariable);

   std::string metricTag;
   iOptions.getRequiredValue("detMetric", metricTag);
   mDetMetric = DetMetric::getScheme(metricTag);

   // Get the analog scheme
   /*
   std::string analogTag;
   iOptions.getRequiredValue("analog", analogTag);
   Options analogOptions = Scheme::getOptions(analogTag);
   mAnalog = new SelectorAnalog(analogOptions, iData);
   */
}
VarSelector::~VarSelector() {
   delete mDetMetric;
   //delete mAnalog;
}
#include "Schemes.inc"

void VarSelector::setStartDate(int iStartDate) {
   mStartDate = iStartDate;
}
void VarSelector::setEndDate(int iEndDate) {
   mEndDate = iEndDate;
}
int VarSelector::getStartDate() const {
   return mStartDate;
}
int VarSelector::getEndDate() const {
   return mEndDate;
}

float VarSelector::run(const Data& iData, int iInit, float iOffset, const Location& iLocation, const std::vector<std::string>& iVariables, const DetMetric& iDetMetric) const {

   Input* inputF = iData.getInput();
   Input* inputO = iData.getObsInput();

   SelectorAnalog selector(makeOptions(iVariables), iData);

   int currDate = mStartDate;
   int idDate = 0;

   int numDays = (int) Global::getTimeDiff(mEndDate, 0, 0, mStartDate, 0, 0)/24;
   std::vector<float> forecast;
   std::vector<float> obs;
   obs.resize(numDays);
   forecast.resize(numDays);

   int counter = 0;

   while(currDate < mEndDate) {
      std::vector<Field> slices;
      Parameters par;
      selector.getDefaultParameters(par);
      selector.select(currDate, iInit, iOffset, iLocation, mVariable, par, slices);

      // Compute forecast at slice locations
      std::vector<float> temp;
      for(int i = 0; i < (int) slices.size(); i++) {
         temp.push_back(inputO->getValue(slices[i].getDate(), slices[i].getInit(),
                  slices[i].getOffset(), iLocation.getId(),
                  slices[i].getMember().getId(), mVariable));
      }
      forecast[idDate] = Global::mean(temp);

      // Get observation
      obs[idDate] = inputO->getValue(currDate, iInit, iOffset, iLocation.getId(), 0, mVariable);

      if(Global::isValid(forecast[idDate]) && Global::isValid(obs[idDate]))
         counter++;

      currDate = Global::getDate(currDate, 24);
      idDate++;
   }

   // Check that we have enough valid forecasts
   //std::cout << "  Valid: " << counter << std::endl;
   float score = Global::MV;
   if(counter >= mMinValidDates)
      score = iDetMetric.compute(forecast, obs, Parameters(), iData, mVariable);
   return score;
}

Options VarSelector::makeOptions(std::string iVariable) const {
   std::vector<std::string> variables;
   variables.push_back(iVariable);
   return makeOptions(variables);
}
Options VarSelector::makeOptions(std::vector<std::string> iVariables) const {
   std::stringstream ss;
   ss << "tag=bogus ";
   ss << "class=SelectorAnalog analogMetric=normMetric ";
   ss << "numAnalogs=15 averager=mean normalize=1 dayWidth=365 ";
   ss << "variables=";
   for(int v = 0; v < (int) iVariables.size(); v++) {
      ss << iVariables[v];
      if(v != iVariables.size()-1) {
         ss << ",";
      }
   }
   return Options(ss.str());
}

void VarSelector::writeScore(std::string iVariable, float score) const {
   std::cout << "SCORE: " << iVariable << " " << score << std::endl;
}
