#ifndef POOLER_H
#define POOLER_H
#include "../Processor.h"
#include "../Cache.h"

class Data;
class Location;

//! Finds the index
class Pooler : public Component {
   public:
      Pooler(const Options& iOptions, const Data& iData);
      static Pooler* getScheme(const Options& iOptions, const Data& iData);
      static Pooler* getScheme(const std::string& iTag, const Data& iData);
      //! Find a suitable pooling index for location
      int find(const Location& iLocation) const;
      //! ... and offset in the form of a hash value
      float find(float iOffset) const;
   protected:
      //! Returns Global::MV if no suitable pooling index is found
      virtual int findCore(const Location& iLocation) const = 0;
      const Data& mData;
   private:
      bool mEvenBins;
      float mWindowLength;
      mutable Cache<Location,int> mLocationCache;
};
#endif
