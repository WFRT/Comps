#include "Lorenz63.h"
#include "../DataKey.h"
#include "../Member.h"
#include "../Options.h"
#include "../Location.h"
#include "../Member.h"

InputLorenz63::InputLorenz63(const Options& iOptions, const Data& iData) : Input(iOptions, iData),
      mEnsSize(1), mXVar(0), mYVar(0), mZVar(0), mR(28), mS(10), mB(8.0/3),
      mRand(boost::mt19937(0), boost::normal_distribution<>()) {
   //! Initial x condition
   iOptions.getRequiredValue("x0", mX0);
   //! Initial y condition
   iOptions.getRequiredValue("y0", mY0);
   //! Initial z condition
   iOptions.getRequiredValue("z0", mZ0);

   //! Model parameter 'r'
   iOptions.getValue("r", mR);
   //! Model parameter 's'
   iOptions.getValue("s", mS);
   //! Model parameter 'b'
   iOptions.getValue("b", mB);

   //! Integration timestep in seconds
   iOptions.getRequiredValue("dt", mDt);

   //! Ensemble size (for perturbations)
   iOptions.getValue("ensSize", mEnsSize);
   //! Variance of x perturbation
   iOptions.getValue("xVar", mXVar);
   //! Variance of y perturbation
   iOptions.getValue("yVar", mYVar);
   //! Variance of z perturbation
   iOptions.getValue("zVar", mZVar);

   // ADd noice to initial conditions
   mX0 += mRand() * sqrt(mXVar);
   mY0 += mRand() * sqrt(mYVar);
   mZ0 += mRand() * sqrt(mZVar);

   mLocalCache.setName("Lorenz63");

   init();
}
InputLorenz63::~InputLorenz63() {}

float InputLorenz63::getValueCore(const Key::Input& iKey) const {
   std::string localVariable = mId2LocalVariable[iKey.variable];
   if(iKey.offset <= 0) {
      if(localVariable == "LorenzX")
         return mX0;
      if(localVariable == "LorenzY")
         return mY0;
      if(localVariable == "LorenzZ")
         return mZ0;
      return Global::MV;
   }

   // Get previous values
   float x0 = mX0 + mRand() * sqrt(mXVar);
   float y0 = mY0;
   float z0 = mZ0;
   int it = round(iKey.offset / mDt);
   int currIt = 0;

   int lastMajorIt = it - round(1/mDt);

   //std::cout << "Last major iteration: " << lastMajorIt << std::endl;
   if(mLocalCache.isCached(lastMajorIt)) {
      const std::vector<float> values = mLocalCache.get(lastMajorIt);
      x0 = values[0];
      y0 = values[1];
      z0 = values[2];
      currIt = lastMajorIt;
      //std::cout << "Cache hit" << std::endl;
   }

   std::vector<float> values;
   values.resize(3);
   while(currIt <= it-1) {
      // Update values
      float x1 = x0 + mDt * mS * (y0 - x0);
      float y1 = y0 + mDt * (mR * x0 - y0 - (x0*z0));
      float z1 = z0 + mDt * (x0*y0 - mB*z0);

      float x2 = x1 + mDt * mS * (y1-x1);
      float y2 = y1 + mDt * (mR * x1 - y1 - x1*z1);
      float z2 = z1 + mDt * (x1*y1 - mB*z1);

      float x = 0.5 * (x2 + x0);
      float y = 0.5 * (y2 + y0);
      float z = 0.5 * (z2 + z0);

      values[0] = x;
      values[1] = y;
      values[2] = z;
      currIt++;
      mLocalCache.add(currIt, values);

      x0 = x;
      y0 = y;
      z0 = z;
   }

   float returnValue = Global::MV;
   if(localVariable == "LorenzX")
      returnValue = values[0];
   if(localVariable == "LorenzY")
      returnValue = values[1];
   if(localVariable == "LorenzZ")
      returnValue = values[2];
   Input::addToCache(iKey, returnValue);
   return Global::MV;
}

void InputLorenz63::loadMembers() const {
   for(int i = 0; i < mEnsSize; i++) {
      Member member(mName, i);
      mMembers.push_back(member);
   }
}
