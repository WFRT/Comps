#ifndef CACHE_H
#define CACHE_H
#include "Global.h"
#include "Key.h"

//! Generic container for storing and retrieving cached data. The container is registered in Logger,
//! so that the cache size and number of cache misses can be monitored.
//! @tparam K key
//! @tparam V values
template <class K, class V> class Cache {
   public:
      Cache(float iMaxSize=Global::MV, std::string iName="Unspecified") {
         mMaxSize = new float;
         *mMaxSize = iMaxSize;
         mTotalSize = new float;
         *mTotalSize = 0;
         mName = new std::string;
         *mName = iName;
         mCacheMisses = new int;
         *mCacheMisses = 0;
         //mFilename = "";

         Global::logger->registerCache(mName, mTotalSize, mMaxSize, mCacheMisses);
      }
      ~Cache() {
         Global::logger->unRegisterCache(mName, mTotalSize, mMaxSize, mCacheMisses);
         delete mMaxSize;
         delete mTotalSize;
         delete mName;
      };
      //! Adds key/value pair into cache
      void add(const K& iKey, V iValues) {
         if(*mTotalSize > *mMaxSize) {
            trim();
         }
         mValues[iKey] = iValues;
         // TODO:
         *mTotalSize += getNumBytes(iValues) + getNumBytes(iKey);
      }
      //! Returns true if iKey has an entry in the cache
      bool isCached(K iKey) const {
         typename std::map<K, V>::const_iterator it = mValues.find(iKey);
         if(it == mValues.end())
            (*mCacheMisses)++;
         return !(it == mValues.end());
      };
      void showContent() const {
         typename std::map<K, V>::const_iterator it;
         std::cout << "Cache contents:" << std::endl;
         for(it = mValues.begin(); it != mValues.end(); it++) {
            std::cout << it->first.toString() << std::endl;
         }
      };
      //! Returns the values corresponding to the key
      const V& get(const K& iKey) const {
         typename std::map<K, V>::const_iterator it = mValues.find(iKey);
         assert(it != mValues.end());
         return it->second;
      };
      typename std::map<K, V>::iterator get(const K& iKey, bool iTest) {
         typename std::map<K, V>::iterator it = mValues.find(iKey);
         assert(it != mValues.end());
         return it;
      };
      //! Returns the number of entries in the cache
      int size() const {
         return mValues.size();
      };
      void setName(std::string iName) {
         *mName = iName;
      };
      void setMaxSize(float iMaxSize) {
         *mMaxSize = iMaxSize;
      }
      //! Returns the maximum number of bytes the cache is allowed to use
      float maxSize() const {
         if(Global::isMissing(*mMaxSize))
            return Global::MV;
         else
            return *mMaxSize;
      };
      //! Returns the number of bytes used by this cache
      float totalSize() const {
         return *mTotalSize;
      };
      //! Trims the cache so that it is smaller than the maximum allowed cache size
      //! Returns the number of bytes trimmed from the cache
      float trim() {
         float trimSize = 0;
         if(Global::isValid(*mMaxSize)) {
            std::stringstream ss;
            ss << "Trimming cache: " << mValues.size();
            // Remove entries until the right size
            while(*mTotalSize > *mMaxSize) {
               trimSize += getNumBytes(mValues.begin()->second) + getNumBytes(mValues.begin()->first);
               *mTotalSize -= trimSize;
               mValues.erase(mValues.begin());
            }
            ss << " -- > " << mValues.size();
            Global::logger->write(ss.str(), Logger::warning);
         }
         return trimSize;
      };
      int getCacheMisses() const {
         return *mCacheMisses;
      };
         
   private:
      std::map<K, V> mValues;
      float* mMaxSize;
      std::string* mName;
      float* mTotalSize;
      int* mCacheMisses;
      //std::string mFilename;
};
template<class V> static int getNumBytes(V iValues) {
   return iValues.size() * sizeof(float);
};
//! Overload getNumBytes for floats/ints, which
//! do not have the size() method
int getNumBytes(float iValues);
int getNumBytes(int iValues);
#endif

