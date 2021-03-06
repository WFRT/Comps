#ifndef OUTPUT_H
#define OUTPUT_H
#include "../Global.h"
#include "../Component.h"
#include "../Options.h"
#include "../Discretes/Discrete.h"
#include "../Member.h"
#include "../Obs.h"
#include "../Score.h"
#include "../Distribution.h"
#include "../Field.h"
class Location;
class Input;
class Configuration;
class Metric;
class Variable;

class Output : public Component {
   public:
      typedef std::pair<std::string,std::string> VarConf;
      static Output* getScheme(const Options& iOptions, const Data& iData);
      static Output* getScheme(const std::string& iTag, const Data& iData);

      /*
      void addSelectorData();
      */
      void add(Distribution::ptr iDistribution, std::string iVariable, std::string iConfiguration);
      void addSelectorData(float iOffset, const Location& iLocation, const std::vector<Field>& iFields);
      void add(const Obs& iObs);
      void add(const Score& iScore, std::string iVariable, std::string iConfiguration);
      void write();
      virtual bool isMandatory() const {return false;};
      virtual bool needsTraining() const {return false;};
      //! What is the results directory for this run?
      std::string getDirectory() const;
      //! What directory does all output go to?
      std::string getOutputDirectory(int iDate=Global::MV, int iInit=Global::MV) const;
   protected:
      Output(const Options& iOptions, const Data& iData);
      std::vector<float> getCdfs() const;
      std::vector<float> getThresholds() const;
      virtual void writeCore() const = 0;
      bool mUseDateFolder;
      bool mUseInitFolder;
      std::string mFolder;
      //! Get a vector of all variable/configurations that have had some entity added. Preserves the order
      //! that entities were added.
      std::vector<VarConf> getAllVarConfs() const;

      std::string getVariableName(const std::string& iVariable) const;

      //! What are all unique offsets in iEntities?
      //! TODO: Should preserve the order
      std::vector<float> getAllOffsets(const std::vector<Distribution::ptr>& iDistributions) const;
      template<typename T>
      std::vector<float> getAllOffsets(const std::vector<T>& iEntities) const {
         // Store in a set, so that there are no duplicates
         std::set<float> offsets;
         for(int i = 0; i < iEntities.size(); i++) {
            offsets.insert(iEntities[i].getOffset());
         }

         return std::vector<float> (offsets.begin(), offsets.end());
      };
      //! What are all unique locatiosn in iEntities?
      //! TODO: Should preserve the order
      std::vector<Location> getAllLocations(const std::vector<Distribution::ptr>& iDistributions) const;
      template<typename T>
      std::vector<Location> getAllLocations(const std::vector<T>& iEntities) const {
         // Store in a set, so that there are no duplicates
         std::set<Location> locations;
         for(int i = 0; i < iEntities.size(); i++) {
            locations.insert(iEntities[i].getLocation());
         }

         return std::vector<Location> (locations.begin(), locations.end());
      };
      //! What are all unique dates in iEntities?
      std::vector<int> getAllDates(const std::vector<Distribution::ptr>& iDistributions) const;
      template<typename T>
      std::vector<int> getAllDates(const std::vector<T>& iEntities) const {
         std::set<int> dates;
         for(int i = 0; i < iEntities.size(); i++) {
            dates.insert(iEntities[i].getDate());
         }
         return std::vector<int> (dates.begin(), dates.end());
      };
      //! What are all unique inits in iEntities?
      std::vector<int> getAllInits(const std::vector<Distribution::ptr>& iDistributions) const;
      template<typename T>
      std::vector<int> getAllInits(const std::vector<T>& iEntities) const {
         std::set<int> inits;
         for(int i = 0; i < iEntities.size(); i++) {
            inits.insert(iEntities[i].getInit());
         }
         return std::vector<int> (inits.begin(), inits.end());
      };
      //! What are all unique variables in iEntities?
      std::vector<std::string> getAllVariables(const std::vector<Distribution::ptr>& iDistributions) const;
      template<typename T>
      std::vector<std::string> getAllVariables(const std::vector<T>& iEntities) const {
         std::set<std::string> variables;
         for(int i = 0; i < iEntities.size(); i++) {
            variables.insert(iEntities[i].getVariable());
         }
         return std::vector<std::string> (variables.begin(), variables.end());
      };

      std::string mTag;
      std::vector<float>      mDetData;

      std::vector<Obs> mObs;
      std::map<VarConf,std::vector<Distribution::ptr> > mDistributions; //var,conf  distributions
      std::map<VarConf,std::vector<Score> > mScores;

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
      const Data& mData;
   private:
      //! Store the order that configurations were added.
      std::vector<VarConf> mOrderedVarConfs; // var, conf
      // Use this to keep track of which configurations have been added
      void addVarConf(std::string iVariable, std::string iConfigurationName);
      std::set<VarConf>    mAllVarConfs;
      std::vector<float> mCdfs;
      std::vector<float> mThresholds;
      Input* mVariables;
};
#endif
