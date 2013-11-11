#ifndef OUTPUT_H
#define OUTPUT_H
#include "../Global.h"
#include "../Component.h"
#include "../Options.h"
#include "../Discretes/Discrete.h"
#include "../Member.h"
#include "../Obs.h"
#include "../Score.h"
#include "../Deterministic.h"
#include "../Distribution.h"
#include "../Field.h"
class Configuration;
class Location;
class Input;
class Configuration;
class Metric;
class Variable;

class Output : public Component {
   public:
      static Output* getScheme(const Options& iOptions, const Data& iData, const Configuration& iConfiguration);
      static Output* getScheme(const std::string& iTag, const Data& iData, const Configuration& iConfiguration);

      /*
      void addSelectorData();
      */
      void add(Ensemble iEnsemble);
      void add(Distribution::ptr iDistribution);
      void add(Deterministic iDeterministic);
      void addSelectorData(float iOffset,
                              const Location& iLocation,
                              const std::vector<Field>& iFields);
      void add(const Obs& iObs);
      void add(const Score& iScore);
      virtual void write() const = 0;
      virtual bool isMandatory() const {return false;};
      virtual bool needsTraining() const {return false;};
      //! What is the results directory for this run?
      std::string getDirectory() const;
      //! What directory does all output go to?
      std::string getOutputDirectory() const;
   protected:
      Output(const Options& iOptions,
            const Data& iData,
            const Configuration& iConfiguration);
      //! What are all unique offsets in iEntities?
      //! TODO: Should preserve the order
      template<typename T>
      void getAllOffsets(const std::vector<T>& iEntities, std::vector<float>& iOffsets) const {
         // Store in a set, so that there are no duplicates
         std::set<float> offsets;
         for(int i = 0; i < iEntities.size(); i++) {
            offsets.insert(iEntities[i].getOffset());
         }

         iOffsets = std::vector<float> (offsets.begin(), offsets.end());
      };
      //! What are all unique locatiosn in iEntities?
      //! TODO: Should preserve the order
      template<typename T>
      void getAllLocations(const std::vector<T>& iEntities, std::vector<Location>& iLocations) const {
         // Store in a set, so that there are no duplicates
         std::set<Location> locations;
         for(int i = 0; i < iEntities.size(); i++) {
            locations.insert(iEntities[i].getLocation());
         }

         iLocations = std::vector<Location> (locations.begin(), locations.end());
      };
      //! What are all unique dates in iEntities?
      template<typename T>
      void getAllDates(const std::vector<T>& iEntities, std::vector<int>& iDates) const {
         std::set<int> dates;
         for(int i = 0; i < iEntities.size(); i++) {
            dates.insert(iEntities[i].getDate());
         }
         iDates = std::vector<int> (dates.begin(), dates.end());
      };
      //! What are all unique variables in iEntities?
      template<typename T>
      void getAllVariables(const std::vector<T>& iEntities, std::vector<std::string>& iVariables) const {
         std::set<std::string> variables;
         for(int i = 0; i < iEntities.size(); i++) {
            variables.insert(iEntities[i].getVariable());
         }
         iVariables = std::vector<std::string> (variables.begin(), variables.end());
      };

      std::string mTag;
      const Configuration& mConfiguration;
      std::vector<float>      mDetData;

      std::vector<Ensemble> mEnsembles;
      std::vector<Deterministic>    mDeterministics;
      std::vector<Distribution::ptr> mDistributions;
      std::vector<Obs> mObs;
      std::vector<Score> mScores;

      //! What position is iValue within iVector?
      template<class T>
      int getPosition(const std::vector<T>& iVector, T iValue) const {
         typename std::vector<T>::const_iterator pos;
         pos = std::find(iVector.begin(), iVector.end(), iValue);
         int value = Global::MV;
         if(pos != iVector.end())
            value = pos - iVector.begin();
         return value;
      };

      std::vector<std::vector<Field> > mSelectorData;
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
};
#endif
