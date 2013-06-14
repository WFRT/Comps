#include "Lorenz63.h"
#include "../DataKey.h"
#include "../Member.h"
#include "../Options.h"
#include "../Location.h"
#include "../Member.h"

InputLorenz63::InputLorenz63(const Options& iOptions, const Data& iData) : Input(iOptions, iData),
      mEnsSize(1), mXVar(0), mYVar(0), mZVar(0), mR(28), mS(10), mB(8.0/3),
      mRand(boost::mt19937(0), boost::normal_distribution<>()) {
   // Initial conditions
   iOptions.getRequiredValue("x0", mX0);
   iOptions.getRequiredValue("y0", mY0);
   iOptions.getRequiredValue("z0", mZ0);

   // Model parameters
   iOptions.getValue("r", mR);
   iOptions.getValue("s", mS);
   iOptions.getValue("b", mB);

   // Timestep
   if(mType == Input::typeObservation) {
      mDt = mObsDt;
      iOptions.getValue("dt", mDt);
   }
   else {
      iOptions.getRequiredValue("dt", mDt);
   }

   // Ensemble perturbation
   iOptions.getValue("ensSize", mEnsSize);
   iOptions.getValue("xVar", mXVar);
   iOptions.getValue("yVar", mYVar);
   iOptions.getValue("zVar", mZVar);
   mX0 += mRand() * sqrt(mXVar);

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
      //std::cout << "[" << x << " " << y << " " << z << "]" << std::endl;
      currIt++;
      //std::cout << "Caching " << currIt << std::endl;
      mLocalCache.add(currIt, values);

      x0 = x;
      y0 = y;
      z0 = z;
   }

   if(localVariable == "LorenzX")
      return values[0];
   if(localVariable == "LorenzY")
      return values[1];
   if(localVariable == "LorenzZ")
      return values[2];
   return Global::MV;
}

void InputLorenz63::loadLocations() const {
   mLocations.push_back(Location("0", 0));
}
void InputLorenz63::loadMembers() const {
   for(int i = 0; i < mEnsSize; i++) {
      Member member(mName, i);
      mMembers.push_back(member);
   }
}
