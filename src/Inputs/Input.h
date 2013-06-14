#ifndef INPUT_H
#define INPUT_H
#include "../Global.h"
#include "../Component.h"
#include "../Cache.h"
#include "../Key.h"

#include <algorithm>
#include <boost/filesystem.hpp>
#include <boost/unordered_map.hpp>

class Variable;
class Global;
class Options;
class Location;
class Member;
class Logger;
class Obs;
class Ensemble;

//! Interface to retrieving forecast data */
class Input : public Component {
   public:
      enum Type {typeObservation = 0, typeForecast = 10, typeBest = 20, typeUnspecified = 30};
      Input(const Options& iOptions, const Data& iData);
      virtual       ~Input();
      int           getNumMembers() const;
      int           getNumOffsets() const;
      float         getValue(int rDate, int rInit, float iOffset, int rLocationNum, int rMemberId, std::string rVariable) const;
      void          getValues(int rDate, int rInit, float iOffset, int rLocationId, std::string rVariable, std::vector<float>& iValues) const;
      void          getValues(int rDate, int rInit, float iOffset, int rLocationId, std::string rVariable, Ensemble& iEnsemble) const;
      void          getAllValues(int iDate, int iInit, const std::string& iVariable, std::vector<Obs>& iObs) const;
      void          getAvailableVariables(std::vector<std::string>& rVariables) const;
      static        Input* getScheme(const Options& iOptions, const Data& iData);
      static        Input* getScheme(const std::string& iTag, const Data& iData);
      void          getDates(std::vector<int>& rDates) const;
      void          getSurroundingLocations(const Location& iTarget, std::vector<Location>& iLocations, int iNumPoints=1) const;
      //! In metres
      void          getSurroundingLocationsByRadius(const Location& rTarget, std::vector<Location>& rLocations, float iRadius) const;

      void          getMembers(std::vector<Member>& rMembers) const;
      void          getLocations(std::vector<Location>& rLocations) const;
      void          getOffsets(std::vector<float>& iOffsets) const;

      std::string   getName() const;
      std::string   getFolder() const;
      void          setCacheOtherOffsets(bool iStatus);
      void          setCacheOtherLocations(bool iStatus);
      void          setCacheOtherMembers(bool iStatus);
      float         getCacheSize() const; // In Bytes
      float         getMaxCacheSize() const; // In Bytes
      void          setMaxCacheSize(float iMaxCacheSize); // In Bytes
      //! Trim the cache and return number of bytes trimmed
      Type          getType() const;
      //! Get description of iType
      static std::string   getTypeDescription(Input::Type iType);
      static        std::string getInputDirectory();
      void          getVariables(std::vector<std::string>& iVariables) const;
      //! Checks if variable is available
      bool          hasVariable(std::string iVariable) const;
      bool          hasOffset(float iOffset) const;
      int           getOffsetIndex(float iOffset) const;
      int           getNearestOffsetIndex(float iOffset) const;
      std::string   getLocalVariableName(std::string rVariable) const;

      //! Writes data of this type, using data from iData and dimensions from iDimensions
      void          write(const Input& iData, const Input& iDimensions, int iDate) const;
      //! Writes data of this type, using data and dimensinos from iInput
      void          write(const Input& iInput, int iDate) const;
   protected:
      //! Loads offsets/members/etc. Must be called by inheriting classes.
      void          init();
      virtual float getValueCore(int rDate, int rInit, float iOffset, int rLocationId, int rMemberId, std::string rVariable) const;
      virtual float getValueCore(const Key::Input& key) const;
      // Retrives all available dates. No need to sort or cache.
      virtual bool  getDatesCore(std::vector<int>& iDates) const;
      std::string   getFileExtension() const;
      std::string   mDirectory;
      std::string   mDataDirectory;
      std::string   mName;
      std::string   mFolder;
      std::string   getConfigFilename(std::string type) const;
      Type          mType;

      // Caching
      void          addToCache(const Key::Input& iKey, float iValue) const;
      bool          mCacheOtherOffsets;
      bool          mCacheOtherLocations;
      bool          mCacheOtherMembers;
      bool          mCacheOtherVariables;
      mutable std::vector<float>* mLastCachedVector;
      mutable Key::Input          mLastCachedKey;
      //! Set the caching booleans to optimial values for this dataset
      virtual void optimizeCacheOptions();

      float         mMaxCacheSize;
      std::string mFileExtension;

      virtual void  writeCore(const Input& iData, const Input& iDimensions, int iDate) const;

      // Variables
      virtual void  loadVariables() const;
      mutable std::vector<std::string>   mVariables;

      // Phase this one out
      mutable std::map<std::string, int> mVariableMap;

      //! Converts standard variable name to local id
      mutable std::map<std::string, int> mVariable2Id;
      mutable std::map<int, std::string> mId2Variable;

      mutable std::map<std::string, int> mLocalVariable2Id;
      mutable std::map<int, std::string> mId2LocalVariable;

      // Locations
      //! Reads locations from namelist by default
      virtual void  loadLocations() const;
      void          selectLocations();
      mutable Cache<Location, std::vector<int> > mCacheSurroundingLocations; // location Id, closest Ids
      mutable Cache<Location, std::vector<int> > mCacheNearestLocation; // location Id, closest Id

      // Members
      virtual void  loadMembers() const;

      // Offsets
      virtual void  loadOffsets() const;

      //! Tells the user that the caching options are invalid for this dataset
      void invalidCacheOptions() const;

      mutable std::map<float, int> mOffsetMap; // Offset, index
      mutable std::map<int, int> mLocationMap;
      int          mFilenameDateStartIndex;

      //! Enable quality control of values?
      bool   mDoQc;

      mutable std::vector<Location> mLocations;
      mutable std::vector<Member> mMembers;
      mutable std::vector<float> mOffsets;

      mutable Cache<Key::Input, std::vector<float> > mCache;
      bool mUseMainCache;

      virtual bool needsTraining() const {return false;};
      mutable std::vector<int> mDates;
      mutable bool mIsDatesCached;
      bool         mOptimizeCache;
   private:
      mutable std::map<std::string,std::string> mVariableRename;
      mutable std::map<std::string,float> mVariableScale;
      mutable std::map<std::string,float> mVariableOffset;
      mutable std::map<std::string,float> mVariableMin;
      mutable std::map<std::string,float> mVariableMax;
      void    loadVariableMap();
      std::vector<int> mAllowLocations;
      bool mAllowTimeInterpolation;

      int getCacheIndex(const Key::Input& iKey) const;
      int getCacheVectorSize() const;

      void         makeLocationMap() const;
      void         makeOffsetMap() const;
      bool         mHasInit;
      mutable bool mHasWarnedCacheMiss; //> Only warn about cache misses once

};
#endif
