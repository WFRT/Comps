#include "Selector.h"
#include "Clim.h"
#include "../Data.h"
#include "../Member.h"
#include "../Field.h"

SelectorClim::SelectorClim(const Options& iOptions, const Data& iData) :
      Selector(iOptions, iData),
      mAllowFutureValues(false),
      mAllowWrappedOffsets(false),
      mFutureBlackout(0) {
   //! Window length for seasonal cycle (days)
   iOptions.getRequiredValue("dayLength", mDayWindow);
   //! Window length for diurnal cycle (hours)
   iOptions.getRequiredValue("hourLength", mHourWindow);
   //! Allow dates in the future
   iOptions.getValue("allowFutureValues", mAllowFutureValues);
   //! Future values must be this number of days into the future
   iOptions.getValue("futureBlackout", mFutureBlackout);
   //! ?
   iOptions.getValue("allowWrappedOffsets", mAllowWrappedOffsets);
}
void SelectorClim::selectCore(int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      const std::string& iVariable,
      const Parameters& iParameters,
      std::vector<Field>& iFields) const {

   std::vector<int> dates;
   Input* input = mData.getInput(iVariable,Input::typeObservation);
   input->getDates(dates);

   std::vector<float> offsets;
   input->getOffsets(offsets);
   for(int d = 0; d < (int) dates.size(); d++) {
      // TODO:
      // For efficiency reasons the date is checked independently of the offset
      // This means that some offsets near the edge of the day difference alloweed
      // will or will not be included properly
      int date = dates[d];
      int dayDiff   = fabs(Global::getJulianDay(Global::getDate(iDate,iInit,iOffset))
                      - Global::getJulianDay(date));
      if(dayDiff > 365/2)
         dayDiff = 365 - dayDiff;
      assert(dayDiff >= 0);
      //bool validDate  = (dayDiff <= mDayWindow || (365 - dayDiff) <= mDayWindow);
      bool validDate  = (dayDiff <= mDayWindow);
      if(!mAllowFutureValues) {
         // Must have occurred in the past
         validDate = (validDate && iDate > date);
      }
      else {
         // TODO:
         if(mFutureBlackout) {
            // Don't allow dates that are close to the forecast date, because this would make the
            // statistics in sample
            bool withinBlackout = (Global::getTimeDiff(date, 0, 0, iDate, 0, 0) <= mFutureBlackout) &&
                                  (date >= iDate);
            validDate = (validDate && !withinBlackout);
         }
      }
      if(validDate) {
         for(int h = 0; h < offsets.size(); h++) {
            float offset = offsets[h];
            int init = 0;
            float time = fmod(iOffset, 24);
            bool validOffset;
            float hourDiff = fmod(fabs(offset - time), 24);
            if(hourDiff > 12) {
               hourDiff = 24 - hourDiff;
            }
            if(mAllowWrappedOffsets) {
               // TODO:
               // Currently assume that this mode is used for observations
               //validOffset = (offset < 24 &&
               //      ((offset <= time + mHourWindow && offset >= time - mHourWindow) ||
               //       (24 - offset <= time + mHourWindow && 24 - offset >= time - mHourWindow)));
               validOffset = (offset < 24 && hourDiff <= mHourWindow);
            }
            else {
               validOffset = (fabs(offset - iOffset) <= mHourWindow);
            }
            if(validOffset) {
               std::string dataset = input->getName();
               Member member(dataset);
               int useDate = Global::getDate(date, offset);
               float useOffset = fmod(offset, 24);
               float skill = (float) dayDiff/365+ hourDiff/24;
               Field slice(useDate, init, useOffset, member, skill);
               iFields.push_back(slice);
            }
         }
      }
   }
}
