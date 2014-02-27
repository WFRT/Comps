#ifndef RUN_H
#define RUN_H
#include "Global.h"
#include "Location.h"
#include "Metrics/Metric.h"
#include "Outputs/Output.h"
#include "Configurations/Configuration.h"
#include "Data.h"

class Run {
   public:
      Run(std::string iTag);
      Run(const Options& iOptions);
      ~Run();
      //! Which offsets are forecasts being produced for?
      std::vector<float> getOffsets() const;
      //! Which locations are forecasts being produced for?
      std::vector<Location> getLocations() const;
      //! Which metrics are forecasts being produced for?
      std::vector<Metric*> getMetrics(const std::string& iVariable) const;
      //! Which variables are forecasts being produced for?
      std::vector<std::string> getVariables() const;
      //! Which outputs are forecasts being produced for?
      std::vector<Output*> getOutputs() const;
      //! Which configurations are forecasts being produced for?
      std::vector<Configuration*> getConfigurations(const std::string& iVariable) const;
      std::map<std::string, std::vector<Configuration*> > getVarConfs() const;
      //! Get all configurations for all variables
      std::vector<Configuration*> getConfigurations() const;
      void getRunOptions(Options& iOptions) const;
      Data* getData() const;
      //! Get the name of this run
      std::string getName() const;
      /*
      void    getDates(std::vector<int>& iDates) const;
      void    getForecastOffsets(std::vector<float>& iOffsets) const;
      */
   private:
      void init(const Options& iOptions);
      Options loadRunOptions(std::string iTag) const;
      void loadVarConfs(const Options& iRunOptions,
            std::map<std::string, std::vector<std::string> >& iVarConfs,
            std::map<std::string, std::vector<std::string> >& iVarMetrics,
            std::vector<std::string>& iVariables) const;
      std::map<std::string, std::vector<Configuration*> > mVarConfs;
      std::map<std::string, std::vector<Metric*> > mMetrics;
      Data* mDefaultData;
      std::vector<Data*> mConfigDatas;
      Options mRunOptions;
      std::vector<std::string> mVariables;
      std::vector<float> mOffsets;
      std::vector<Location> mLocations;
      std::vector<Output*> mOutputs;
      InputContainer* mInputContainer;
      std::string mRunName;

};
#endif

