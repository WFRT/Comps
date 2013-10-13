#include "Offset.h"

ObsSelectorOffset::ObsSelectorOffset(const Options& iOptions, const Data& iData) :
      ObsSelector(iOptions, iData),
      mWindowLength(0), 
      mMovingWindow(0) {
   //! Use observations within this many number of hours
   iOptions.getValue("windowLength", mWindowLength);
   //! Center the window on the offset? Alternatively bins will created by the following bin edges
   //! [0 windowLength 2*windowLength ...]
   iOptions.getValue("movingWindow", mMovingWindow);
   if(mWindowLength < 0) {
      std::stringstream ss;
      ss << "ObsSelectorOffset: 'windowLength' must be >= 0";
      Global::logger->write(ss.str(), Logger::error);
   }
}

void ObsSelectorOffset::select(std::vector<Obs>& iObs, const Location& iLocation, float iOffset) const {
   float offset = fmod(iOffset,24);
   std::vector<Obs> keepObs;
   for(int i = 0; i < iObs.size(); i++) {
      float obsOffset = iObs[i].getOffset();
      if(mMovingWindow) {
         if(abs(obsOffset - iOffset) <= mWindowLength) {
            keepObs.push_back(iObs[i]);
         }
      }
      else {
         if(mWindowLength == 0) {
            if(obsOffset == offset)
               keepObs.push_back(iObs[i]);
         }
         else {
            int lowerOffset = floor(iOffset/mWindowLength)*mWindowLength;
            int upperOffset = lowerOffset + mWindowLength;
            if(obsOffset >= lowerOffset && obsOffset <= upperOffset) {
               keepObs.push_back(iObs[i]);
            }
         }
      }
   }
   iObs = keepObs;
}
