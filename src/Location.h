#ifndef LOCATION_H
#define LOCATION_H
#include "Global.h"
#include "Namelist.h"
#include "Options.h"

//! Represents a spatial coordinate specific to a dataset
class Location {
   public:
      //! @param iDataset Dataset that this location belongs to
      //! @param iId      Id within this dataset
      //! @param iLat     Latitude in degrees
      //! @param iLon     Longitude in degrees
      //! @param iElev    Elevation in meters
      //! @param iLevel   Model level (if forecast dataset)
      Location(const std::string& iDataset, int iId=Global::MV,
            float iLat=Global::MV, float iLon=Global::MV,
            float iElev=Global::MV, std::string iName="", int iLevel=0);
      //! Empty location
      Location();
      // Accessors
      int getId() const;
      std::string getDataset() const;
      float getLat() const;
      float getLon() const;
      float getElev() const;
      int   getLevel() const;
      int   getLandUse() const;
      std::string getCode() const;
      std::string getName() const;

      // Mutators
      void  setValues(float iLat, float iLon, float iElev, int iLevel=0);
      void  setCode(std::string iCode);

      //! Compute distance in metres between two points. Accurate to within 1 m.
      static float getDistance(float lat1, float lon1, float lat2, float lon2);
      //! Compute distance in metres between this point and the input point. Accurate to within 1 m.
      float getDistance(const Location& iLocation) const;

      //! Earth's radius in meters
      static const double mRadiusEarth = 6.378137e6;
      bool operator<(const Location &right) const;
      static float deg2rad(float deg);
      static float rad2deg(float rad);
      //! Number of bytes this object uses
      int size() const;
   private:
      std::string mDataset;
      int mId;
      std::string mCode;
      float mLat;
      float mLon;
      float mElev;
      int   mLevel;
      std::string mName;
};
#endif

