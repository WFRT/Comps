#ifndef VAR_SELECTOR_H
#define VAR_SELECTOR_H
#include "../Global.h"
#include "../Processor.h"

class Options;
class Data;
class Location;
class DetMetric;
class SelectorAnalog;

class VarSelector : public Component {
   public:
      VarSelector(const Options& iOptions, const Data& iData);
      virtual ~VarSelector();
      virtual void getVariables(const Data& iData,
            int iInit,
            float iOffset,
            const Location& iLocation,
            std::vector<std::string>& iVariables) const = 0;
      static VarSelector* getScheme(const Options& iOptions, const Data& iData);
      static VarSelector* getScheme(const std::string& iTag, const Data& iData);
      void   setStartDate(int iStartDate);
      void   setEndDate(int iEndDate);
      int    getStartDate() const;
      int    getEndDate() const;
      float  run(const Data& iData, int iInit, float iOffset, const Location& iLocation, const std::vector<std::string>& iVariables, const DetMetric& iDetMetric) const;
   protected:
      int    mStartDate;
      int    mEndDate;
      //SelectorAnalog* mAnalog;
      std::string mVariable;
      Options makeOptions(std::string iVariable) const;
      Options makeOptions(std::vector<std::string> iVariables) const;
      void writeScore(std::string iVariable, float score) const;
      int mMinValidDates;
      DetMetric* mDetMetric;
   private:
      const Data& mData;
};
#endif
