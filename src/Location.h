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
      Location(const std::string& iDataset, int iId, float iLat, float iLon);
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
      float getLandFraction() const;
      std::string getCode() const;
      std::string getName() const;
      float getGradientX() const;
      float getGradientY() const;

      // Mutators
      void  setId(float iId);
      void  setDataset(std::string iDataset);
      void  setLat(float iLat);
      void  setLon(float iLon);
      void  setElev(float iElev);
      void  setLevel(float iLevel);
      void  setLandUse(int iLandUse);
      void  setLandFraction(float iLandFraction);
      void  setName(std::string iName);
      void  setCode(std::string iCode);
      void  setGradientX(float iGradientX);
      void  setGradientY(float iGradientY);

      //! Compute distance in metres between two points. Accurate to within 1 m.
      static float getDistance(float lat1, float lon1, float lat2, float lon2);
      //! Compute distance in metres between this point and the input point. Accurate to within 1 m.
      float getDistance(const Location& iLocation) const;

      //! Earth's radius in meters
      static double mRadiusEarth;
      bool operator<(const Location &right) const;
      static float deg2rad(float deg);
      static float rad2deg(float rad);
      //! Number of bytes this object uses
      int size() const;
   private:
      std::string mDataset;
      int mId;
      float mLat;
      float mLon;
      float mElev;
      int   mLevel;
      float mLandFraction;
      float mLandUse;
      std::string mName;
      std::string mCode;
      float mGradientX; // East-west gradient in topography (m/m)
      float mGradientY; // East-west gradient in topography (m/m)
};
#endif

