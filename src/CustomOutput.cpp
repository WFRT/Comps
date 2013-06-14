#include "CustomOutput.h"
#include "Global.h"
#include "Ensemble.h"
#include "Obs.h"
#include <iostream>
#include <iomanip>

CustomOutput::CustomOutput() : mStartYear(Global::MV), mVariable(""), mBasePath("") {
}
CustomOutput::CustomOutput(int iStartYear, Location iLocation, std::string iVariable)
      : mStartYear(iStartYear), mLocation(iLocation), mVariable(iVariable),
      mBasePath("results/naefs/output/") {
      
   if(iVariable == "PCPT24") {
      mNumOffsets = mNumOffsetsPCPT24;
   }
   else {
      mNumOffsets = mNumOffsetsT;
   }
}

void CustomOutput::add(Ensemble iEnsemble) {
   int actualDate = Global::getDate(iEnsemble.getDate(), iEnsemble.getOffset());
   mEnsembles[actualDate][getOffsetIndex(iEnsemble.getOffset())] = iEnsemble;
   assert(iEnsemble.size() == mNumMembers);
   //std::cout << "Adding ensemble: " << actualDate << " " << getOffsetIndex(iEnsemble.getOffset()) << " " << iEnsemble[0] << " " << iEnsemble.size()<< std::endl;
}

void CustomOutput::add(Obs iObs) {
   mObs[iObs.getDate()] = iObs;
}
void CustomOutput::write() const {
   std::vector<int> dates;
   getDates(dates);
   std::string filename = getFilename();

   std::ofstream ofs(filename.c_str());
   //std::cout << "Writing to " << filename << std::endl;

   ofs << std::fixed << std::setprecision(1);

   // Loop over offsets
   int maxOffset = mNumOffsets;
   for(int o = 1; o <= maxOffset; o++) {
      // Write offset line
      for(int e = 0; e < mNumMembers+2; e++) {
         ofs << o;
         if(e < mNumMembers+1) {
            ofs << " ";
         }
         else {
            ofs << std::endl;
         }
      }
      for(int i = 0; i < (int) dates.size(); i++) {
         int currDate = dates[i];
         std::map<int, std::map<int, Ensemble> >::const_iterator itDate = mEnsembles.find(currDate);
         std::vector<float> ens;
         if(itDate == mEnsembles.end()) {
            // No ensemble found for today
            ens.resize(mNumMembers, Global::MV);
         }
         else {
            std::map<int, Ensemble>::const_iterator itOffset = itDate->second.find(o);
            if(itOffset == itDate->second.end()) {
               // No ensemble found for this offset
               ens.resize(mNumMembers, Global::MV);
            }
            else {
               ens = itOffset->second.getValues();
               assert(ens.size() == mNumMembers);
            }
         }
         // Write date
         ofs << currDate << " ";
         // Write ensemble
         for(int e = 0; e < mNumMembers; e++) {
            ofs << ens[e] << " ";
         }
         // Write obs
         std::map<int,Obs>::const_iterator itObs = mObs.find(currDate);
         if(itObs == mObs.end()) {
            // No obs
            ofs << Global::MV;
         }
         else {
            ofs << itObs->second.getValue();
         }
         ofs << std::endl;
      }
   }
}

void CustomOutput::getDates(std::vector<int>& iDates) const {
   int currDate = mStartYear * 10000 + 1001;
   int endDate  = (mStartYear + 1)*10000 + 930;

   while(currDate <= endDate) {
      iDates.push_back(currDate);
      currDate = Global::getDate(currDate, 0, 24);
   }
}

std::string CustomOutput::getFilename() const {
   std::stringstream ss;
   ss << mBasePath << mLocation.getCode() << "_" << mStartYear * 10000 + 1001 << "-"
      << (mStartYear + 1)*10000 + 930 << "_ENS_" << mVariable << ".txt";
   return ss.str();
}

int CustomOutput::getOffsetIndex(float iOffset) {
   return iOffset/24 + 1;
}
