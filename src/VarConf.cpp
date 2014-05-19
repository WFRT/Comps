#include "VarConf.h"
#include "Options.h"

VarConf::VarConf(const Options& iOptions) {

   //! Which variable from variables.nl?
   iOptions.getRequiredValue("variable", mVariable);

   //! Which configurations from configurations.nl?
   iOptions.getRequiredValues("configurations", mConfigurations);

   //! Which metrics from schemes.nl?
   iOptions.getValues("metrics", mMetrics);

}

std::string VarConf::getVariable() const {return mVariable;};
std::vector<std::string> VarConf::getConfigurations() const {return mConfigurations;};
std::vector<std::string> VarConf::getMetrics() const {return mMetrics;};
