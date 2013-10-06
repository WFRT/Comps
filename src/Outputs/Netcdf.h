#ifndef OUTPUT_NETCDF_H
#define OUTPUT_NETCDF_H
#include "Output.h"
#include <netcdf.hh>

class OutputNetcdf : public Output {
   public:
      OutputNetcdf(const Options& iOptions, const Data& iData, int iDate, int iInit, const std::string& iVariable, const Configuration& iConfiguration);
      void writeForecasts() const;
      void writeVerifications() const;
   private:
      std::string getFilename() const;
      std::string getVerifFilename() const;
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
         delete values;
      };
};
#endif
