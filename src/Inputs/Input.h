#ifndef INPUT_H
#define INPUT_H
#include "../Global.h"
#include "../Component.h"
#include "../Cache.h"
#include "../Key.h"
#include "../Member.h"

#include <algorithm>
#include <boost/filesystem.hpp>
#include <boost/unordered_map.hpp>

class Variable;
class Global;
class Options;
class Location;
class Logger;
class Obs;
class Ensemble;
class Data;

//! Interface to retrieving forecast/obs data
class Input : public Component {
   public:
      static        Input* getScheme(const Options& iOptions);
      static        Input* getScheme(const std::string& iTag);
      virtual       ~Input();

      enum Type {typeObservation = 0, typeForecast = 10, typeBest = 20, typeUnspecified = 30};
      static std::string getTypeDescription(Input::Type iType);
      ////////////////////////
      // Dataset dimensions //
      ////////////////////////
      std::vector<Member>      getMembers() const;
      const std::vector<Location>& getLocations() const;
      std::vector<float>       getOffsets() const;
      std::vector<int>         getInits() const;
      std::vector<std::string> getVariables() const;
      void                     getDates(std::vector<int>& iDates) const;
      int                      getNumLocations() const;
      int                      getNumOffsets() const;
      int                      getNumInits() const;
      int                      getNumVariables() const;
      int                      getNumMembers() const;

      ////////////
      // Values //
      ////////////
      //! iCalibrate: Should the value be calibrated before being returned?
      float         getValue(int iDate, int iInit, float iOffset, int iLocationNum, int iMemberId, std::string iVariable, bool iCalibrate=true) const;
      void          getValues(int iDate, int iInit, float iOffset, int iLocationId, std::string iVariable, std::vector<float>& iValues) const;
      void          getValues(int iDate, int iInit, float iOffset, int iLocationId, std::string iVariable, Ensemble& iEnsemble) const;
      //! Get the 'iNumPoints' nearest neighbours
      void          getSurroundingLocations(const Location& iTarget, std::vector<Location>& iLocations, int iNumPoints=1) const;
      //! Get all surrounding locations within 'iRadius' metres
      void          getSurroundingLocationsByRadius(const Location& iTarget, std::vector<Location>& iLocations, float iRadius) const;

      std::string   getName() const;
      Type          getType() const;
      //! Get description of iType
      static        std::string getInputDirectory();
      //! Checks if variable is available
      bool          hasVariable(std::string iVariable) const;
      bool          hasOffset(float iOffset) const;
      int           getOffsetIndex(float iOffset) const;
      int           getLocationIndex(float iLocationId) const;
      int           getNearestOffsetIndex(float iOffset) const;
      // Find the most recent data/init that is available at iDate/iInit. Returns true if any
      // suitable is found. When iHandleDelay is true, the function simulates delay in when data is
      // available.
      bool          getNearestDateInit(int iDate, int iInit, int& iNewDate, int& iNewInit, bool iHandleDelay) const;
      std::string   getSampleFilename() const;
      std::string   getLocationFilename() const;
      bool          needsTraining() const {return false;};

      ///////////////////////
      // Write this format //
      ///////////////////////
      //! Writes data of this type, using data from iData and dimensions from iDimensions
      void          write(const Data& iData, const std::vector<int>& iDates, int iInit, const std::vector<float>& iOffsets, const std::vector<Location>& iLocations, const std::vector<Member>& iMembers, const std::vector<std::string>& iVariables);

      //! Convert between the variable name used by the dataset and the variable name used in COMPS
      //! Returns true if found
      bool getLocalVariableName(std::string iVariable, std::string& iLocalVariable) const;
      bool getVariableName(std::string iLocalVariable, std::string& iVariable) const;

   protected:
      Input(const Options& iOptions);
      ////////////////////////////
      // Subclass can implement //
      ////////////////////////////
      virtual float getValueCore(const Key::Input& key) const = 0;
      virtual void  getMembersCore(std::vector<Member>& iMembers) const {};
      virtual void  getLocationsCore(std::vector<Location>& iLocations) const {};
      virtual void  getInitsCore(std::vector<int>& iInits) const {};
      virtual void  getOffsetsCore(std::vector<float>& iOffsets) const {};
      //! Retrives all available dates. No need to sort or cache.
      virtual bool  getDatesCore(std::vector<int>& iDates) const;
      //! Set the caching booleans to optimal values for this dataset
      virtual void  optimizeCacheOptions();
      virtual void  writeCore(const Data& iData, int iDate, int iInit, const std::vector<float>& iOffsets, const std::vector<Location>& iLocations, const std::vector<Member>& iMembers, const std::vector<std::string>& iVariables) const;
      virtual std::string getDefaultFileExtension() const {return "";};
      virtual std::string getSampleFilenameCore() const;
              void  writeVariablesNamelist(const std::vector<std::string>& iVariables) const;
      virtual void  writeLocationsNamelist(const std::vector<Location>& iLocations) const;
      virtual void  writeMembersNamelist(const std::vector<Member>& iMembers) const;

      // Read dimensions from namelists
      void readLocationsNamelist(std::vector<Location>& iLocations) const;
      void readInitsNamelist(std::vector<int>& iInits) const;
      void readMembersNamelist(std::vector<Member>& iMembers) const;

      // Returns true if found
      bool getLocalVariableName(int iVariableId, std::string& iLocalVariable) const;
      bool getVariableName(int iVariableId, std::string& iVariable) const;
      bool getVariableIdFromVariable(std::string iVariable, int& iVariableId) const;
      bool getVariableIdFromLocalVariable(std::string iLocalVariable, int& iVariableId) const;


      //! Loads offsets/members/etc. Must be called by inheriting classes.
      void          init();

      std::string   mLocationFilename; // Filename where location data exists
      int           mStartDate;
      int           mEndDate;
      //! Does this dataset have data in files?
      virtual bool  hasDataFiles() const {return true;};

      std::string   getFileExtension() const;
      //! Base directory where namelists are located
      std::string   getDirectory() const;
      //! Directory where data is located
      std::string   getDataDirectory() const;
      std::string   getFilename(const Key::Input& iKey) const;
      std::string   mFileFormat;
      std::string   getNamelistFilename(std::string type) const;

      // Checks if data at a given date and init is missing. Determines this by checking if a subset
      // of values are missing. Can be overloaded to provide more efficient implementation.
      virtual bool isMissing(int iDate, int iInit) const;

      // Variables
      //std::string   getLocalVariableName(std::string iVariable) const;
      //std::string   getLocalVariableName(int iVariableId) const;
      //std::string   getVariableName(std::string iLocalVariable) const;
      //std::string   getVariableName(int iVariableId) const;
      //int           getVariableIdFromVariable(std::string iVariable) const;
      //int           getVariableIdFromLocalVariable(std::string iLocalVariable) const;

     // Caching
      void addToCache(const Key::Input& iKey, float iValue) const;
      bool mCacheOtherOffsets;
      bool mCacheOtherLocations;
      bool mCacheOtherMembers;
      bool mCacheOtherVariables;

      //! Tells the user that the caching options are invalid for this dataset
      void notifyInvalidCacheOptions() const;

   private:
      std::vector<int> mAllowLocations;
      std::vector<int> mRemoveLocations;
      float mMinLat;
      float mMaxLat;
      float mMinLon;
      float mMaxLon;
      bool mAllowTimeInterpolation;

      float getVariableOffset(const std::string& iVariable) const;
      float getVariableScale(const std::string& iVariable) const;

      // Helper function to load the nearest 'iNum' locations into cache
      void loadSurroundingLocations(const Location& iTarget, int iNum=Global::MV) const;

      float mInitDelay;

      bool         mHasInit;
      mutable bool mHasWarnedCacheMiss; //> Only warn about cache misses once

      mutable Cache<Key::Two<float,float>, std::vector<std::pair<int,float> > > mCacheSurroundingLocations; // lat, lon, locationIndex, dist
      mutable Cache<Key::Three<int,int,bool>, Key::DateInit> mCacheNearestTimeStamp;
      mutable Cache<Key::Three<int,int,bool>, int> mCacheNearestTimeStampMissing;
      mutable Cache<Key::DateInit, int> mCacheDateInitMissing;
      mutable std::vector<float>* mLastCachedVector;
      mutable Key::Input          mLastCachedKey;
      mutable Cache<Key::Input, std::vector<float> > mCache;
      int getCacheIndex(const Key::Input& iKey) const;
      int getCacheVectorSize() const;
      bool mOptimizeCache;
      mutable float mLastOffset;
      mutable int   mLastOffsetIndex;

      // TODO: Not implemented. Allow filenames to have junk at the beginning before the date.
      int mFilenameDateStartIndex;
      std::string mFileExtension;
      Type mType;
      std::string mName;
      mutable bool mIsDatesCached;
      mutable std::vector<int> mDates;

      std::string   mDataDirectory;
      std::string   mDirectory;
      std::string   mFolder;

      // Dataset dimensions
      // Never access these, use the accessor functions
      mutable std::vector<Location> mLocations;
      mutable std::map<int, int> mLocationMap; // Id, index
      mutable std::vector<Member> mMembers;
      mutable std::vector<float> mOffsets;
      mutable std::vector<int> mInits;
      mutable std::map<float, int> mOffsetMap; // Offset, index
      mutable std::vector<std::string>   mVariables;

      // Variable conversions
      mutable std::map<std::string,std::string> mVariable2LocalVariable; // Comps name, local name
      mutable std::map<std::string,std::string> mLocalVariable2Variable; // Local name, Comps name
      mutable std::map<std::string, int> mVariable2Id;
      mutable std::vector<std::string> mId2Variable;
      mutable std::map<std::string, int> mLocalVariable2Id;
      mutable std::vector<std::string> mId2LocalVariable;

      mutable std::map<std::string,float> mVariableScale;
      mutable std::map<std::string,float> mVariableOffset;

      mutable int mNumLocations;
      mutable int mNumOffsets;
      mutable int mNumInits;
      mutable int mNumMembers;
      mutable int mNumVariables;
      bool mForceLimits;
      bool mReplaceMissing;
      void setLocations(std::vector<Location> iLocations);
};
#endif
