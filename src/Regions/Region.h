#ifndef REGION_H
#define REGION_H
#include "../Processor.h"
#include "../Cache.h"

class Data;
class Location;

//! Finds the index
class Region : public Component {
   public:
      Region(const Options& iOptions, const Data& iData);
      static Region* getScheme(const Options& iOptions, const Data& iData);
      static Region* getScheme(const std::string& iTag, const Data& iData);
      //! Find a suitable region for location
      int find(const Location& iLocation) const;
      //! ... and offset in the form of a hash value
      float find(float iOffset) const;
   protected:
      //! Returns Global::MV if no suitable region is found
      virtual int findCore(const Location& iLocation) const = 0;
      const Data& mData;
   private:
      bool mEvenBins;
      float mWindowLength;
      mutable Cache<Location,int> mLocationCache;
};
#endif
