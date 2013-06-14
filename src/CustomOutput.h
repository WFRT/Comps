#ifndef CUSTOM_OUTPUT_H
#define CUSTOM_OUTPUT_H
#include "Global.h"
#include "Location.h"

class Ensemble;
class Obs;

class CustomOutput {
   public:
      CustomOutput();
      CustomOutput(int iStartYear, Location iLocation, std::string iVariable);
      void add(Ensemble iEnsemble);
      void add(Obs iObs);
      void write() const;
   private:
      std::string getFilename() const;
      // Represents ensembles valid for date, not issued at date
      std::map<int, std::map<int, Ensemble> > mEnsembles; // date, offset, ensemble
      std::map<int, Obs> mObs;                            // date, obs
      int mStartYear;
      Location mLocation;
      void getDates(std::vector<int>& iDates) const;
      static const int mNumOffsetsT      = 16; // Number of offstes for temperature
      static const int mNumOffsetsPCPT24 = 15; // Number of offsets for precip
      static const int mNumMembers = 42;       // Ensemble members
      std::string mVariable;
      std::string mBasePath;
      static int getOffsetIndex(float iOffset);
      int mNumOffsets;
};
#endif
