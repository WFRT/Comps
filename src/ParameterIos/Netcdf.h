#ifndef PARAMETER_IO_NETCDF_H
#define PARAMETER_IO_NETCDF_H
#include "ParameterIo.h"
#include <netcdf.hh>

class ParameterIoNetcdf : public ParameterIo {
   public:
      ParameterIoNetcdf(const Options& iOptions, const Data& iData);
   protected:
      bool readCore(const Key::Par& iKey, Parameters& iParameters) const;
      void writeCore();
   private:
      std::string getFilename(const Key::Par& iKey) const;
      std::string getFilename(const Key::DateVarConfig& iKey) const;
      //! Get the variable name for the variable representing parameter size
      static std::string getSizeName(const std::string& iName);
      static std::string getIndexName(const std::string& iName);
      mutable std::map<int,bool> mAvailableDates;
      // Call this if parameter file is corrupt. Throws error.
      static void isCorrupt(std::string iFilename, std::string iMessage);
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
