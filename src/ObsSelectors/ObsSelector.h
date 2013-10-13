#ifndef OBS_SELECTOR_H
#define OBS_SELECTOR_H
#include "../Component.h"
#include "../Obs.h"
#include "../Data.h"
#include "../Options.h"

class ObsSelector : public Component {
   public:
      ObsSelector(const Options& iOptions, const Data& iData);
      static ObsSelector* getScheme(const Options& rOptions, const Data& iData);
      static ObsSelector* getScheme(const std::string& iTag, const Data& iData);
      virtual void select(std::vector<Obs>& iObs, const Location& iLocation, float iOffset) const = 0;
      //void select(std::vector<Obs>& iObs, const Location& iLocation, float iOffset) const = 0;
};
#endif
