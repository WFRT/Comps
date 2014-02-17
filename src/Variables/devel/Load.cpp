#include "Load.h"
#include "../Data.h"
#include "../Ensemble.h"
#include "../Member.h"

VariableLoad::VariableLoad() : Variable("Load") {
}

float VariableLoad::computeCore(const Data& iData,
      int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      const Member& iMember,
      Input::Type iType) const {
   // Use observed load at the 0 forecast horizon
   //std::cout << "VariableLoad(" << iDate << " I" << iInit << " O" << iOffset << " L" << iLocation.getId() << " M" << iMember.getId() << " " << iMember.getDataset() << ")" << std::endl;
   if(iOffset == iInit) {
      // TODO
      // What if we take ensemble not value?
      // iData.getEnsemble(iDate, iInit, iOffset, iLocation, "Load", Input::typeObservation);
      return iData.getValue(iDate, iInit, iOffset, iLocation, iMember, "Load", Input::typeObservation);
   }
   /*
   else {
      Ensemble ensLoad;
      std::cout << "--- " << iOffset << std::endl;
      iData.getEnsemble(iDate, iInit, iOffset-1, iLocation, "Load", Input::typeForecast, ensLoad);
      return Global::mean(ensLoad.getValues());
   }
  */

   // Find nearest temperature location
   std::vector<Location> nearest;
   iData.getInput("T", Input::typeForecast)->getSurroundingLocations(iLocation, nearest);

   // Hour of day: 1.. 24
   int timeOfDay = (int) iOffset;
   timeOfDay += 1;
   timeOfDay = timeOfDay % 24 + 1;

   // Previous load
   Ensemble ensLoad;
   float prevOffset = fmod(iOffset-1,24);
   float prevLoad = iData.getValue(iDate, iInit, prevOffset, iLocation, iMember, "Load", Input::typeForecast);

   // Month
   int month = Global::getMonth(iDate);

   // Current temperature
   Ensemble ensT;
   iData.getEnsemble(iDate, iInit, iOffset, nearest[0], "T", Input::typeForecast, ensT);
   float T = Global::mean(ensT.getValues()) ;

   // Previous temperature
   Ensemble ensT0;

   iData.getEnsemble(iDate, iInit, iOffset-1, nearest[0], "T", Input::typeForecast, ensT0);
   float Tprev = Global::mean(ensT0.getValues());

   int dayType = Global::getDayOfWeek(iDate) + 1;

   // Compute yesterday's bias
   int prevDate = Global::getDate(iDate, 0, -24);
   Ensemble ensLoadObsYesterday;
   float loadYesterday = iData.getValue(prevDate, iInit, iOffset, iLocation, iMember, "Load0", Input::typeForecast);
   float obsYesterday  = iData.getValue(prevDate, iInit, iOffset, iLocation, iMember, "Load", Input::typeObservation);
   float biasYesterday = Global::MV;
   if(Global::isValid(loadYesterday) && Global::isValid(obsYesterday)) {
      biasYesterday = loadYesterday - obsYesterday;
   }

   std::vector<float> values;
   values.push_back(biasYesterday);
   values.push_back(dayType);
   values.push_back(timeOfDay);
   values.push_back(prevLoad);
   values.push_back(month);
   values.push_back(T);
   values.push_back(Tprev);
   for(int i = 0; i < (int) values.size(); i++) {
      if(!Global::isValid(values[i])) {
         std::stringstream ss;
         ss << "VariableLoad: D" << iDate << " O" << iOffset << " L" << iLocation.getId() << " M" << iMember.getId() << " Missing ";
         ss << i << " " << std::endl;
         Global::logger->write(ss.str(), Logger::warning);
         return Global::MV;
      }
   }
   float bias = gepModelBias(values);
   //std::cout << iOffset << " BiasYesterday = " << loadYesterday << std::endl;
   if(!Global::isValid(bias)) {
      bias = 0;
   }
   bias = 0;
  
   Ensemble ensLoad0;
   iData.getEnsemble(iDate, iInit, iOffset, iLocation, "Load0", Input::typeForecast, ensLoad0);
   float load0 = Global::mean(ensLoad0.getValues());

   if(!Global::isValid(load0))
      return Global::MV;

   return load0 - bias;
}

float VariableLoad::gepModelBias(const std::vector<float>& iValues) {
   float G1C0 = -4.217773;
   float G1C1 = -8.039184;
   float G2C0 = 4.973267;
   float G2C1 = -9.10083;
   float G3C0 = -5.269378;
   float G3C1 = 7.125732;
   float G4C0 = 8.452576;
   float G4C1 = -5.98816;

   int Bias24h = 0;
   int CD = 1;
   int HH = 2;

   double dblTemp = 0.0;

   dblTemp = (sin((cos(G1C0)*(iValues[CD]*G1C1)))*((G1C1+G1C1)+iValues[HH]));
   dblTemp += (iValues[Bias24h]/(cos(iValues[HH])+((G2C1+iValues[CD])+(G2C0+G2C1))));
   dblTemp += log(pow(pow(atan(pow((sin(G3C0)*(G3C1-iValues[HH])),2)),3),3));
   dblTemp += (iValues[Bias24h] <= (((G4C0*iValues[HH])-pow(G4C1,3))+((G4C1+G4C1)*G4C0)) ? iValues[Bias24h] : (((G4C0*iValues[HH])-pow(G4C1,3))+((G4C1+G4C1)*G4C0)));

   return dblTemp;
}
