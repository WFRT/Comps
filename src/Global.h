#ifndef GLOBAL_H
#define GLOBAL_H
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <assert.h>
#include <limits>
#include <math.h>
#include <boost/shared_ptr.hpp>
#include "Loggers/Logger.h"
#include <set>
#include <omp.h>
#include <sys/time.h>
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/filesystem.hpp>
//static omp_lock_t writelock;
class Logger;
class Global {
   public:
   static float INF;
   static float MV; // Missing value
   static float NC; // Uncached value
   const static float TOL =0.001;
   static const float pi = 3.14159265;
   static Logger* logger;
   static void setLogger(Logger* iLogger);
   //! Checks if input is valid (i.e. not infinite, not not-a-number, and not missing)
   static bool  isValid(float iValue);
   static bool  isMissing(float iValue);
   static float getMoment(const std::vector<float>& iValues, int iMoment);
   static float getTime(int iDate, int iInit, float iOffset);
   static int   getDate(int iDate, int iInit, float iOffset);
   static int   getDate(int iDate, float iOffset);
   static float getOffset(int iDate, float iOffset);
   static int   getYear(int iDate);
   static int   getMonth(int iDate);
   static int   getDay(int iDate);
   // Returns day of week: Sunday = 0, Saturday = 6;
   static int   getDayOfWeek(int iDate);
   //! In hours
   static float getTimeDiff(int iDate1, int iInit1, float iOffset1, int iDate2, int iInit2, float iOffset2);
   static float getJulianDay(int iDate, int iInit=0, float iOffset=0);
   static double clock();
   static int   getCurrentDate();

   //! Create a new directory, if it does not already exist
   //! Returns false if it cannot create a directory
   //! Returns true if it creates the directory, or it is already created
   static bool  createDirectory(std::string iPath);

   static std::string getDirectory(std::string iFilename);

   // Array operations
   //! Note: iValues must be sorted
   //! Finds index into array pointing to the largest element smaller than or equal to iX.
   //! In case of a tie, return the index of the last tied elements. When the search value equals
   //! multiple values, the first element is returned. 
   static int getLowerIndex(float iX, const std::vector<float>&  iValues);
   //! Note: iValues must be sorted
   //! Finds index into array pointing to the smallest element greater than or equal to iX.
   //! In case of a tie, return the index of the first tied elements. When the search value equals
   //! multiple values, the last element is returned.
   static int getUpperIndex(float iX, const std::vector<float>& iValues);
   static int getNumValid(std::vector<float> iValues);
   static float interp(float x, float x0, float x1, float y0, float y1);
   //! Returns a random number between 0 and 1
   static float getRand();

   // Convert string to integer or float
   static int getInt(const std::string& iString);
   static float getFloat(const std::string& iString);

   // Computes correlation between two vectors
   template<class T> static float corr(const std::vector<T>& iData0, const std::vector<T>& iData1) {
      float corr;
      assert(iData0.size() == iData1.size());
      float var0 = variance(iData0);
      float var1 = variance(iData1);
      float mean0 = mean(iData0);
      float mean1 = mean(iData1);
      if(mean0 == Global::MV || mean1 == Global::MV ||
            var0 == Global::MV || var1 == Global::MV ||
            var0 <= Global::TOL || var1 <= Global::TOL) {
         return Global::MV;
      }
      float covar = 0;
      int counter = 0;
      for(int i = 0; i < (int) iData0.size(); i++) {
         if(iData0[i] != Global::MV && iData1[i] != Global::MV) {
            covar += (iData0[i] - mean0)*(iData1[i] - mean1);
            counter++;
         }
      }
      if(counter != 0) {
         covar /= counter;
      }
      else {
         covar = Global::MV;
      }
      corr = covar / sqrt(var0) / sqrt(var1);
      return corr;
   }
   template<class T> static float mean(const std::vector<T>& iData0) {
      float mean = 0;
      int counter = 0;
      for(int i = 0; i < (int) iData0.size(); i++) {
         if(iData0[i] != Global::MV) {
            mean += iData0[i];
            counter++;
         }
      }
      if(counter != 0)
         mean /= counter;
      else
         mean = Global::MV;
      return mean;
   }
   template<class T> static float variance(const std::vector<T>& iData0) {
      float mean = Global::mean(iData0);
      if(mean == Global::MV)
         return Global::MV;
      if(iData0.size() <= 1)
         return Global::MV;

      int counter = 0;
      float variance = 0;
      for(int i = 0; i < (int) iData0.size(); i++) {
         if(iData0[i] != Global::MV) {
            variance += (iData0[i] - mean)*(iData0[i] - mean);
            counter++;
         }
      }
      if(counter != 0)
         variance /= counter;
      else
         variance = Global::MV;
      return variance;
   }
   template<class T1, class T2> struct sort_pair_second {
      bool operator()(const std::pair<T1,T2>&left, const std::pair<T1,T2>&right) {
         return left.second < right.second;
      }
   };
   template<class T1, class T2> struct sort_pair_first {
      bool operator()(const std::pair<T1,T2>&left, const std::pair<T1,T2>&right) {
         return left.first < right.first;
      }
   };
   template<class T> static void arrayToMatrix(const std::vector<T>& iArray, std::vector<std::vector<T> >& iMatrix) {
      int N = (int) iArray.size();
      for(int i = 0; i < N; i++) {
         std::vector<T> curr;
         for(int j = 0; j < N; j++) {
            int index = i*N + j;
            curr.push_back(iArray[index]);
         }
         iMatrix.push_back(curr);
      }
   };
   private:
      static boost::variate_generator<boost::mt19937, boost::uniform_01<> > mRand;
};

#endif

