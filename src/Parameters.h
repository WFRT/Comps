#ifndef PARAMETERS_H
#define PARAMETERS_H
#include "Global.h"

class Parameters {
   public:
      Parameters(std::vector<float> mValues, bool iIsDefault=false);
      Parameters();
      std::vector<float> getAllParameters() const;
      void getSubset(int iStart, int iEnd, Parameters& iParameters) const;
      void getSubset(int iRecord, Parameters& iParameters) const;

      void setAllParameters(std::vector<float> iValues);

      float& operator[](unsigned int i);
      float const& operator[](unsigned int i) const;

      int size() const;
      const static int maxSize = 1000;


      bool getIsDefault() const;
      void setIsDefault(bool iIsDefault);
      void setRecords(const std::vector<int>& iRecords);
      void setSubset(int iRecord, const Parameters& iParameters);
      void addRecord(int iRecord);
   private:
      mutable std::vector<float> mValues;
      bool    mIsDefault;
      std::vector<int> mRecords;
};
#endif

