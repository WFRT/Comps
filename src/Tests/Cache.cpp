#define BOOST_TEST_MODULE Cache
#include "../Cache.h"
#include <gtest/gtest.h>
#include <vector>

namespace {
   class CacheTest : public ::testing::Test {
      protected:
         CacheTest() {
         }
         virtual ~CacheTest() {
         }
         virtual void SetUp() {
            // Code here will be called immediately after the constructor (right
            // before each test).
         }

         virtual void TearDown() {
            // Code here will be called immediately after each test (right
            // before the destructor).
         }
   };

   TEST_F(CacheTest, floatTest) {
      Cache<int,int> cache(Global::MV, "test");
      cache.add(3,2);
      EXPECT_EQ(true, cache.isCached(3));
      EXPECT_EQ(false, cache.isCached(1));
      EXPECT_EQ(2, cache.get(3));
      EXPECT_EQ(2*sizeof(int), cache.totalSize());
      // Check cache misses
      EXPECT_EQ(1, cache.getCacheMisses());
   }
   TEST_F(CacheTest, vectorTest) {
      Cache<int,std::vector<int> > cache(Global::MV, "test");
      cache.add(3,std::vector<int>(3,2));
      cache.add(2,std::vector<int>(5,2));
      EXPECT_EQ(true, cache.isCached(3));
      EXPECT_EQ(true, cache.isCached(2));
      EXPECT_EQ(false, cache.isCached(1));
      std::vector<int> values = cache.get(3);
      EXPECT_EQ(2, values[0]);
      EXPECT_EQ(3, values.size());
      EXPECT_EQ(10*sizeof(int), cache.totalSize());
   }
   // Check that cache gets trimmed
   TEST_F(CacheTest, trimTest) {
      Cache<int,int> cache(Global::MV, "test");
      int maxValues = 2;
      int maxCacheSize = (maxValues-1)*2*sizeof(int); // bytes
      int expCacheSize = (maxValues)*2*sizeof(int); // bytes
      cache.setMaxSize(maxCacheSize);
      cache.add(-1,-2);
      cache.add(0,0);
      for(int i = 1; i <= 10; i++) {
         cache.add(i,i*2);
         EXPECT_EQ(true, cache.isCached(i));
         EXPECT_EQ(true, cache.isCached(i-1));
         EXPECT_EQ(false, cache.isCached(i-2));
         EXPECT_EQ(expCacheSize, cache.totalSize());
         EXPECT_EQ(maxValues, cache.size());
      }
   }
}
int main(int argc, char **argv) {
     ::testing::InitGoogleTest(&argc, argv);
       return RUN_ALL_TESTS();
}
