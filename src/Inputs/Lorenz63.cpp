#include "Lorenz63.h"
#include "../Member.h"
#include "../Options.h"
#include "../Location.h"
#include "../Member.h"

InputLorenz63::InputLorenz63(const Options& iOptions) : Input(iOptions),
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

   iOptions.check();
   init();
}
InputLorenz63::~InputLorenz63() {}

float InputLorenz63::getValueCore(const Key::Input& iKey) const {
   // Implement this recursively. Cache the values at the offsets specified in the namelists.
   std::string localVariable;
   bool found = getLocalVariableName(iKey.variable, localVariable);
   assert(found);

   std::vector<float> values;
   values.resize(3);
   // Initial condition
   if(iKey.offset <= 0) {
      // Add noice to initial conditions
      if(localVariable == "LorenzX")
         values[0] = mX0 + mRand() * sqrt(mXVar);
      if(localVariable == "LorenzY")
         values[1] = mY0 + mRand() * sqrt(mYVar);
      if(localVariable == "LorenzZ")
         values[2] = mZ0 + mRand() * sqrt(mZVar);
   }
   else {
      // Get values at last offset
      int currIndex = getOffsetIndex(iKey.offset);
      assert(Global::isValid(currIndex));
      int lastIndex = currIndex - 1;
      assert(lastIndex >= 0);
      float lastOffset = getOffsets()[lastIndex];

      Key::Input key = iKey;
      key.offset = lastOffset;
      key.variable = 0;
      float x0 = getValue(key.date, key.init, key.offset, key.location, key.member, "LorenzX", false);
      key.variable = 1;
      float y0 = getValue(key.date, key.init, key.offset, key.location, key.member, "LorenzY", false);
      key.variable = 2;
      float z0 = getValue(key.date, key.init, key.offset, key.location, key.member, "LorenzZ", false);

      float offset = lastOffset;
      // Iterate forward in time to the offset
      while(offset < iKey.offset) {
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
         offset += mDt;

         assert(Global::isValid(x));
         assert(Global::isValid(y));
         assert(Global::isValid(z));

         x0 = x;
         y0 = y;
         z0 = z;
      }

   }

   float returnValue = Global::MV;
   Key::Input key = iKey;
   // Cache the value
   for(key.variable = 0; key.variable < 3; key.variable++) {
      Input::addToCache(key, values[key.variable]);
   }
   if(localVariable == "LorenzX")
      returnValue = values[0];
   if(localVariable == "LorenzY")
      returnValue = values[1];
   if(localVariable == "LorenzZ")
      returnValue = values[2];
   return returnValue;
}

void InputLorenz63::getMembersCore(std::vector<Member>& iMembers) const {
   for(int i = 0; i < mEnsSize; i++) {
      Member member(getName(), i);
      iMembers.push_back(member);
   }
}
