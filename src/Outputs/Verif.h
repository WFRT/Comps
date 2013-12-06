#ifndef OUTPUT_VERIF_H
#define OUTPUT_VERIF_H
#include "Output.h"
#include <netcdf.hh>

class OutputVerif : public Output {
   public:
      OutputVerif(const Options& iOptions, const Data& iData);
   private:
      void writeCore() const;
      std::string getFilename(int iInit, std::string iVariable, std::string iConfiguration) const;
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
};
#endif
