#ifndef OUTPUT_NETCDF_H
#define OUTPUT_NETCDF_H
#include "Output.h"
#include <netcdf.hh>

class OutputNetcdf : public Output {
   public:
      OutputNetcdf(const Options& iOptions, const Data& iData, int iDate, int iInit, const std::string& iVariable, const Configuration& iConfiguration);
      void writeForecasts() const;
      void writeVerifications() const;
      void setLocations(const std::vector<Location>& iLocations);
      std::string getOutputFileName() const;
   private:
      std::string getFilename() const;
      std::string getVerifFilename() const;
      void writeVariable(NcVar* iVariable, const std::map<float,int>& iValues) const;
      //void writeVariable(NcVar* iVariable, const std::vector<float>& iValues) const;
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
      int  getDimSize(Output::Dim iDim) const;
      mutable std::map<float, int> mOffsetMap;
      mutable std::map<std::string, int> mVariableMap;
      mutable std::map<std::string, int> mMetricMap;
      mutable std::map<int, int> mDateMap;
      mutable std::vector<float> mLats;
      mutable std::vector<float> mLons;
      mutable std::map<float, int> mXMap;
      mutable std::map<float, int> mCdfMap;
      mutable std::map<float, int> mXPdfMap;
      mutable std::vector<std::string> mMetrics;

      void arrangeData() const;
      //void makeIdMap(const std::vector<float>& iValues, std::map<float, int>& iMap) const;
      //void makeIdMap(const std::vector<std::string>& iValues, std::map<std::string, int>& iMap) const;
      void makeVector(const std::map<std::string, int>& iMap, std::vector<std::string>& iValues) const;

      void writeForecast(const Configuration& iConfiguration);
};
#endif
