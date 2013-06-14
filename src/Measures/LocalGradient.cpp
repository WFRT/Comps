#include "LocalGradient.h"
#include "../Options.h"
#include "../Data.h"
#include "../Ensemble.h"
#include "../Location.h"

MeasureLocalGradient::MeasureLocalGradient(const Options& iOptions, const Data& iData) :
      Measure(iOptions, iData),
      mVariable("") {
   Component::underDevelopment();
   iOptions.getValue("variable", mVariable);
}

float MeasureLocalGradient::measureCore(const Ensemble& iEnsemble, const Parameters& iParameters) const {
   int   date   = iEnsemble.getDate();
   int init     = iEnsemble.getInit();
   float offset = iEnsemble.getOffset();
   Location loc = iEnsemble.getLocation();

   // Determine which variable to use
   std::string var = mVariable;
   if(mVariable == "") {
      var = iEnsemble.getVariable();
   }

   std::string dataset = loc.getDataset();
   float centerValue = iEnsemble.getMoment(1);

   // Get nearby locations
   Input* input = mData.getInput(dataset);
   std::vector<Location> nearbyLocations;
   input->getSurroundingLocations(loc, nearbyLocations, 4);

   // Compute gradient
   float grad;
   int counter = 0;
   for(int i = 0; i < (int) nearbyLocations.size(); i++) {
      Ensemble ens;
      mData.getEnsemble(date, init, offset, nearbyLocations[i], dataset, var, ens);
      float mean = ens.getMoment(1);
      if(Global::isValid(mean)) {
         // TODO
         float dist = 0;
         float dvdx = 0;
         float dydy = 0;
         counter++;
      }
   }

   return iEnsemble.getMoment(2);
}
