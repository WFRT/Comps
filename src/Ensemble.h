#ifndef ENSEMBLE_H
#define ENSEMBLE_H
#include "Global.h"
#include "Location.h"


//! Encapsulates an ensemble of values, valid for one particular time and location
//! Automatically resizes and fills missing values
class Ensemble {
   public:
      //! Initialize empty ensemble
      Ensemble();
      Ensemble(std::vector<float> iValues, std::string iVariable);
      Ensemble(std::vector<float> iValues, std::vector<float> iSkills, std::string iVariable);

      // Accessors
      float const& operator[](unsigned int i) const; //! Access the i'th element
      std::vector<float> getValues() const;
      std::vector<float> getSkills() const;
      int size() const; ///! Ensemble size
      float getMin() const;
      float getMax() const;
      int  getDate() const;
      int  getInit() const;
      float getOffset() const;
      Location getLocation() const;
      std::string getVariable() const;
      float getMoment(int iMoment) const;

      // Mutators
      float& operator[](unsigned int i); //! Mutate the i'th element
      void setSkills(std::vector<float> iSkills);
      void setValues(std::vector<float> iValues);
      void setVariable(std::string iVariable);
      void setInfo(int iDate, int iInit, float iOffset, Location iLocation, std::string iVariable);
      void setSize(int iSize); //! Truncates if size is smaller than current
                               //! size. Fills missing value if larger
   private:
      std::vector<float> mValues;
      mutable std::vector<float> mSkills;
      //! Prevent accidentally creating large ensembles when accessing with a large index
      const static int maxSize = 100000;

      std::string mVariable;
      int mDate;
      float mOffset;
      int mInit;
      Location mLocation;
      mutable std::vector<float> mMoments;
      const static int mNumCachedMoments;
      void resetCachedMoments() const;
};
#endif
