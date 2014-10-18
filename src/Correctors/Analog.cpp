#include "Analog.h"
#include "../Obs.h"
#include "../Parameters.h"
#include "../Data.h"
#include "../Inputs/Input.h"
#include "../Selectors/Selector.h"
#include "../Neighbourhoods/Neighbourhood.h"
#include "../Field.h"

CorrectorAnalog::CorrectorAnalog(const Options& iOptions, const Data& iData) : Corrector(iOptions, iData),
      mMultiplicative(false) {

   Component::underDevelopment();
   //! Use the same selector as in the configuration. For an analog date, how should the forecast be
   //! selected on that day?
   std::string selectorTag;
   iOptions.getRequiredValue("selector", selectorTag);
   mSelector = Selector::getScheme(selectorTag, mData);

   //! Which observation locations should be used to do the correction? Defaults to nearest 
   //! observation location.
   std::string neighbourhoodTag;
   if(iOptions.getValue("neighbourhood", neighbourhoodTag)) {
      mNeighbourhood = Neighbourhood::getScheme(neighbourhoodTag);
   }
   else {
      mNeighbourhood = Neighbourhood::getScheme(Options("class=NeighbourhoodNearest num=1"));
   }

   //! Which selector should be used to find analogs?
   std::string analogSelectorTag;
   iOptions.getRequiredValue("analogSelector", analogSelectorTag);
   mAnalogSelector = Selector::getScheme(analogSelectorTag, mData);

   //! Should the adjustment be multiplicative (instead of additive)?
   iOptions.getValue("multiplicative", mMultiplicative);
   iOptions.check();
}
void CorrectorAnalog::correctCore(const Parameters& iParameters, Ensemble& iUnCorrected) const {

   int date = iUnCorrected.getDate();
   int init = iUnCorrected.getInit();
   float offset = iUnCorrected.getOffset();
   std::string variable = iUnCorrected.getVariable();
   Location location = iUnCorrected.getLocation();

   // Find similar days
   std::vector<Field> analogs;
   mAnalogSelector->selectCore(date, init, offset, location, variable, Parameters(), analogs);

   // Find all obs and forecasts on those days
   std::vector<Location> obsLocations = mNeighbourhood->select(mData.getObsInput(), location);
   std::vector<float> fcstTotal(analogs.size(), 0);
   std::vector<float> obsTotal(analogs.size(), 0);
   std::vector<int> counter(analogs.size(), 0);
   std::vector<float> newValues;
   float fcstGrandTotal = 0;
   float obsGrandTotal = 0;
   int counterGrandTotal = 0;
   for(int d = 0; d < analogs.size(); d++) {
      for(int i = 0; i < obsLocations.size(); i++) {
         int date = analogs[d].getDate();
         int init = analogs[d].getInit();
         float offset = analogs[d].getOffset();

         // Obs and forecasts are for the same point
         Obs obs;
         mData.getObs(date, init, offset, obsLocations[i], variable, obs);
         Ensemble pastEns = mSelector->select(date, init, offset, location, variable, Parameters());
         float fcst = pastEns.getMoment(1);
         if(Global::isValid(obs.getValue()) && Global::isValid(fcst)) {
            fcstTotal[d] += fcst;
            obsTotal[d]  += obs.getValue();
            counter[d]++;
            fcstGrandTotal += fcst;
            obsGrandTotal  += obs.getValue();
            counterGrandTotal++;
         }
      }
   }

   // Adjust forecast by adding new member
   if(fcstGrandTotal > 0 && obsGrandTotal > 0 && counterGrandTotal > 0) {
      float dmb = obsGrandTotal / fcstGrandTotal;
      float bias = (obsGrandTotal - fcstGrandTotal)/counterGrandTotal;
      if(dmb > 2) dmb = 2;
      if(dmb < 0.5) dmb = 0.5;
      for(int i = 0; i < iUnCorrected.size(); i++) {
         if(Global::isValid(iUnCorrected[i])) {
            float value;
            if(mMultiplicative) {
               value = iUnCorrected[i] * dmb;
            }
            else {
               value = iUnCorrected[i] + bias;
            }
            iUnCorrected[i] = value;
         }
      }
   }

   if(0) {
      // Adjust forecast by adding new member
      for(int a = 0; a < analogs.size(); a++) {
         if(fcstTotal[a] > 0 && obsTotal[a] > 0 && counter[a] > 0) {
            float dmb = obsTotal[a] / fcstTotal[a];
            float bias = (obsTotal[a] - fcstTotal[a])/counter[a];
            if(dmb > 2) dmb = 2;
            if(dmb < 0.5) dmb = 0.5;
            for(int i = 0; i < iUnCorrected.size(); i++) {
               if(Global::isValid(iUnCorrected[i])) {
                  float value;
                  if(mMultiplicative) {
                     value = iUnCorrected[i] * dmb;
                  }
                  else {
                     value = iUnCorrected[i] + bias;
                  }
                  newValues.push_back(value);
               }
            }
         }
      }
      if(newValues.size() > 0)
         iUnCorrected.setValues(newValues);
   }
}
