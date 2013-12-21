#ifndef PARAMETERS_H
#define PARAMETERS_H
#include "Global.h"

//! Container class for a vector of parameters
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
   private:
      mutable std::vector<float> mValues;
      bool    mIsDefault;
};
#endif

