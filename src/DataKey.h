#ifndef DATAKEY_H
#define DATAKEY_H
#include "Global.h"
#include <boost/unordered_map.hpp>
class DataKey {
   public:
      DataKey(int rDate, int rInit, float iOffset, int rLocationId, std::string rVariableName, int rMemberId=0);
      //DataKey(const Point& rPoint, int rEnsI);
      int getDate() const;
      int getInit() const;
      int getOffset() const;
      int getLocationId() const;
      std::string getVariableName() const;
      int getMemberId() const;

      void setDate(int rDate);
      void setInit(int rInit);
      void setOffset(float iOffset);
      void setLocationId(int rLocationId);
      void setVariableName(std::string rVariableName);
      void setMemberId(int rMemberId);

      bool operator<(const DataKey &right) const;
      std::size_t hash_value(const DataKey& iKey);
      class Hash : std::unary_function<DataKey, std::size_t> {
         public:
            std::size_t operator()(DataKey const& iKey) const {
               std::size_t seed = 0;
               boost::hash_combine(seed, iKey.getDate());
               boost::hash_combine(seed, iKey.getInit());
               boost::hash_combine(seed, iKey.getOffset());
               boost::hash_combine(seed, iKey.getLocationId());
               boost::hash_combine(seed, iKey.getMemberId());
               boost::hash_combine(seed, iKey.getVariableName());
               return seed;
            };
      };
      class Equal : std::binary_function<DataKey, DataKey, bool> {
         public:
            bool operator()(DataKey const& x, DataKey const& y) const {
               return x.getDate()       == y.getDate() &&
                      x.getInit()       == y.getInit() &&
                      x.getOffset()     == y.getOffset() &&
                      x.getLocationId() == y.getLocationId() &&
                      x.getMemberId()   == y.getMemberId() &&
                      x.getVariableName() == y.getVariableName();
            };
      };

   private:
      int mDate;
      int mInit;
      int mOffset;
      int mLocationId;
      std::string mVariableName;
      int mMemberId;
};
#endif

