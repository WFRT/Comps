#ifndef OUTPUT_H
#define OUTPUT_H
#include "../Global.h"
#include "../Component.h"
#include "../Options.h"
#include "../Discretes/Discrete.h"
#include "../Member.h"
#include "../Obs.h"
#include "../Slice.h"
class Configuration;
class Location;
class Input;
class Configuration;
class Metric;
class Variable;

class Output : public Component {
   public:
      enum Dim {DimVariable, DimOffset, DimLocation, DimX};
      Output(const Options& iOptions,
            const Data& iData,
            int iDate,
            int iInit,
            const std::string& iVariable,
            const Configuration& iConfiguration);
      Input* getInput();
      Input* getObsInput();
      void getCdfX(std::vector<float>& iCdfX) const;
      void getPdfX(std::vector<float>& iPdfX) const;
      void getCdfInv(std::vector<float>& iCdfInv) const;
      virtual std::string getOutputFileName() const {return "";};

      /*
      void addDistribution(Distribution::ptr iDistribution);
      void addEnsemble(Ensemble iEnsemble);
      void addSelectorData();
      void addMetricData();
      */

      void addSelectorData(float iOffset,
                              const Location& iLocation,
                              const std::vector<Slice>& iSlices);
      void addEnsembleData(float iOffset,
                           const Location& iLocation,
                           const std::vector<float>& iValues);
      void addCdfData(float iOffset,
                      const Location& iLocation,
                      float iX,
                      float iValue);
      void addPdfData(float iOffset,
                      const Location& iLocation,
                      float iX,
                      float iValue);
      void addDiscreteData(float iOffset,
                           const Location& iLocation,
                           float iP,
                           Discrete::BoundaryType iType);
      void addCdfInvData(float iOffset,
                      const Location& iLocation,
                      float iCdf,
                      float iValue);
      //void addPdfData(const std::vector<float>& iId, float iData);
      void addDetData(float iOffset,
                      const Location& iLocation,
                      float iValue);
      void addMetricData(float iOffset,
                         const Location& iLocation,
                         float iData,
                         const Metric& iMetric);
      //void addInvData(const std::vector<float>& iId, float iData);
      void addObs(const Obs& iObs);
      virtual void writeForecasts() const = 0;
      virtual void writeVerifications() const = 0;
      static Output* getScheme(const Options& rOptions, const Data& iData, int iDate, int iInit, std::string iVariable, const Configuration& iConfiguration);
      static Output* getScheme(const std::string& iTag, const Data& iData, int iDate, int iInit, std::string iVariable, const Configuration& iConfiguration);
      virtual bool isMandatory() const {return false;};
      virtual bool needsTraining() const {return false;};
   protected:
      void init();
      static std::string getOutputDirectory();
      std::string mTag;
      class CdfKey {
         public:
            CdfKey(float iOffset, const Location& iLocation, std::string iVariable, float iX);
            float mOffset;
            Location mLocation;
            float mX;
            std::string mVariable;
      };
      class ScalarKey {
         public:
            ScalarKey(float iOffset, const Location& iLocation, std::string iVariable);
            float mOffset;
            Location mLocation;
            std::string mVariable;
      };
      class MetricKey {
         public:
            MetricKey(float iOffset, const Location& iLocation, std::string iVariable, const Metric& iMetric);
            float mOffset;
            Location mLocation;
            std::string mVariable;
            const Metric* mMetric;
      };
      int mDate;
      int mInit;
      std::string mVariable;
      const Configuration& mConfiguration;
      Input* mInput;
      std::vector<CdfKey>     mCdfKeys;
      std::vector<float>      mCdfData;
      std::vector<CdfKey>     mPdfKeys;
      std::vector<float>      mPdfData;
      std::vector<CdfKey>     mCdfInvKeys;
      std::vector<float>      mCdfInvData;
      std::vector<ScalarKey>  mEnsKeys;
      std::vector<std::vector<float> > mEnsData;
      std::vector<ScalarKey>  mDetKeys;
      std::vector<float>      mDetData;

      std::vector<ScalarKey>  mDiscreteLowerKeys;
      std::vector<float>      mDiscreteLowerData;
      std::vector<ScalarKey>  mDiscreteUpperKeys;
      std::vector<float>      mDiscreteUpperData;

      std::vector<MetricKey>  mMetricKeys;
      std::vector<float>      mMetricData;
      std::vector<float>      mData;
      std::vector<float>      mCdfX;
      std::vector<float>      mPdfX;
      std::vector<float>      mCdfInv;
      std::map<int, Location> mLocations; // Id, Location
      std::vector<float>      mOffsets;
      std::vector<Member>     mMembers;
      mutable std::vector<int> mDates;
      std::vector<Obs> mObs;
      std::vector<ScalarKey>  mSelectorKeys;
      std::vector<std::vector<Slice> > mSelectorData;
      template <class T> void makeIdMap(const std::vector<T>& iValues, std::map<T, int>& iMap) const {
         std::set<T> valuesSet;
         // Create set of values. Each value appears only one in the set
         for(int i = 0; i < (int) iValues.size(); i++) {
            valuesSet.insert(iValues[i]);
         }
         typename std::set<T>::iterator it;
         // Use the key's position in the set as a unique identifier
         int counter = 0;
         for(it = valuesSet.begin(); it != valuesSet.end(); it++) {
            T key = *it;
            iMap[key] = counter;
            counter++;
         }
      };

      mutable std::map<int, int> mLocationMap;
};
#endif
