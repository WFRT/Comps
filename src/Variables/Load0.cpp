#include "Load0.h"
#include "../Data.h"
#include "../Ensemble.h"
#include "../Member.h"

VariableLoad0::VariableLoad0() : Variable("Load0") {
}

float VariableLoad0::computeCore(const Data& iData,
      int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      const Member& iMember,
      Input::Type iType) const {
   // Use observed load at the 0 forecast horizon
   if(iOffset == iInit) {
      return iData.getValue(iDate, iInit, iOffset, iLocation, iMember, "Load", Input::typeObservation);
   }

   // Find nearest temperature location
   std::vector<Location> nearest;
   iData.getInput("T", Input::typeForecast)->getSurroundingLocations(iLocation, nearest);

   std::vector<float> values;
   // Hour of day: 1.. 24
   int timeOfDay = (int) iOffset;
   timeOfDay += 1;
   timeOfDay = timeOfDay % 24 + 1;
   //if(timeOfDay == 0) timeOfDay = 24;
   
   //std::cout << "Time of day: " << timeOfDay << " " << iOffset << std::endl;

   // Previous load
   Ensemble ensLoad;
   float prevOffset = fmod(iOffset-1,24);
   float prevLoad = iData.getValue(iDate, iInit, prevOffset, iLocation, iMember, "Load0", Input::typeForecast);

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

   values.push_back(timeOfDay);
   values.push_back(prevLoad);
   values.push_back(month);
   values.push_back(T);
   values.push_back(Tprev);
   for(int i = 0; i < (int) values.size(); i++) {
      if(!Global::isValid(values[i])) {
         //std::cout << "Missing" << std::endl;
         std::stringstream ss;
         ss << "VariableLoad0: D" << iDate << " O" << iOffset << " L" << iLocation.getId() << " M" << iMember.getId() << " Missing ";
         ss << i << " " << std::endl;
         Global::logger->write(ss.str(), Logger::warning);
         return Global::MV;
      }
   }

   float load = gepModelL(values);
   return load;
}

float VariableLoad0::gepModelL(const std::vector<float>& iValues) {
   float G1C0 = 7.15625;
   float G1C1 = 2.938508;
   float G2C0 = 4.288299;
   float G2C1 = 6.900544;
   float G3C0 = -9.998749;
   float G3C1 = -4.421844;
   float G4C0 = 9.162597;
   float G4C1 = 9.376007;
   float G5C0 = 2.04776;
   float G5C1 = -2.143921;

   long unsigned int HH = 0;
   long unsigned int L1h = 1;
   long unsigned int MM = 2;
   long unsigned int T = 3;
   long unsigned int T1h = 4;

   double dblTemp = 0.0;

   dblTemp = (pow(log(pow(((G1C1+G1C0) > (G1C0 > iValues[HH] ? iValues[T] : iValues[HH]) ? iValues[HH] : G1C1),3)),3)-G1C0);
   dblTemp += pow((cos((sin((G2C1 < iValues[HH] ? iValues[HH] : G2C1))*(iValues[HH]+G2C0)))-iValues[HH]),2);
   dblTemp += ((((G3C0-iValues[HH])+iValues[L1h])-((iValues[HH]+iValues[HH])+iValues[HH]))-iValues[T]);
   dblTemp += ((G4C0*sqrt((iValues[HH]*G4C1)))*cos(pow((iValues[HH]/G4C1),3)));
   dblTemp += (iValues[T1h]-pow((((G5C0 > G5C0 ? iValues[T] : iValues[HH])+pow(G5C1,3))*(G5C0 > iValues[MM] ? G5C1 : G5C0)),2));

   return dblTemp;
}
