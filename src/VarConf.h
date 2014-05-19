#ifndef VARCONF_H
#define VARCONF_H
#include "Global.h"
class Options;

class VarConf {
   public:
      VarConf(const Options& iOptions);
      std::string getVariable() const;
      std::vector<std::string> getConfigurations() const;
      std::vector<std::string> getMetrics() const;
   private:
      std::string mVariable;
      std::vector<std::string> mConfigurations;
      std::vector<std::string> mMetrics;
};
#endif
