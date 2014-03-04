#ifndef OUTPUT_NETCDF_CF_H
#define OUTPUT_NETCDF_CF_H
#include "Output.h"
#include <netcdf.hh>

class OutputNetcdfCf : public Output {
   public:
      OutputNetcdfCf(const Options& iOptions, const Data& iData);
   private:
      void writeCore() const;
      std::string getFilename(int iDate, int iInit, std::string iVariable, std::string iConfiguration) const;
      void writeVariable(NcVar* iVariable, const std::map<float,int>& iValues) const;
      void writeVariable(NcVar* iVariable, const std::vector<std::string>& iValues) const;
      template <class T> void writeVariable(NcVar* iVariable, const std::vector<T>& iValues) const {
         int N = (int) iValues.size();
         T* values = new T[N];
         for(int i = 0; i < (int) iValues.size(); i++) {
            values[i] = iValues[i];
         }
         long int pos = 0;
         iVariable->set_cur(&pos);
         iVariable->put(values, N);
         delete[] values;
      };

      // Dimension names
      std::string mDimOffset;
      std::string mDimThreshold;

      // Variable names
      std::string mVarOffset;
      std::string mVarLat;
      std::string mVarLon;
      std::string mVarElev;
      std::string mVarThreshold;

      // To grid
      int mNumX;

};
#endif
