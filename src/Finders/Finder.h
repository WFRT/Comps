#ifndef FINDER_H
#define FINDER_H
#include "../Component.h"

class Data;
class Location;

//! Finds the index
class Finder : public Component {
   public:
      Finder(const Options& iOptions, const Data& iData);
      static Finder* getScheme(const Options& iOptions, const Data& iData);
      static Finder* getScheme(const std::string& iTag, const Data& iData);
      int find(const Location& iLocation) const;
      // Do we really need this?
      int size() const;
   protected:
      virtual int findCore(const Location& iLocation) const = 0;
      //! Must be set at initialization and never changed afterwards
      int mSize;
};
#endif
