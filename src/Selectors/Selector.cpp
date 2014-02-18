#include "SchemesHeader.inc"
#include "Selector.h"
#include "../Inputs/Input.h"
#include "../Data.h"
#include "../Global.h"
#include "../Options.h"
#include "../Parameters.h"
#include "../Location.h"
#include "../Field.h"
#include "../Ensemble.h"
#include "../Data.h"

Selector::Selector(const Options& iOptions, const Data& iData) :
      Processor(iOptions, iData),
      mRemoveMissing(false),
      mNumNeighbours(0) {
   //! Removes ensemble members that value missing forecasts
   iOptions.getValue("removeMissing", mRemoveMissing);
   //! Instead of downscaling, should neighbouring grid points be used? If so, how many?
   iOptions.getValue("numNeighbours", mNumNeighbours);
}
#include "Schemes.inc"

Ensemble Selector::select(int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      const std::string& iVariable,
      const Parameters& iParameters) const {
   std::vector<Field> fields;
   selectCore(iDate, iInit, iOffset, iLocation, iVariable, iParameters, fields);
   if(mRemoveMissing) {
      std::vector<Field>::iterator it = fields.begin();
      while(it !=  fields.end()) {
         std::cout << "Checking: " << it->getMember().getId() << std::endl;
         float value = mData.getValue(iDate, iInit, iOffset, iLocation, it->getMember(), iVariable);
         if(!Global::isValid(value)) {
            it = fields.erase(it);
         }
         else {
            it++;
         }
      }
   }
   // Create the ensemble
   Ensemble ens;
   std::vector<float> values;
   std::vector<float> skills;
   if(fields.size() == 0) {
      values.push_back(Global::MV);
      skills.push_back(Global::MV);
   }
   else {
      for(int i = 0; i < (int) fields.size(); i++) {
         Field field = fields[i];
         // There are two ways to get values for a field
         // 1) Let Data (downscaler) create one value for each field at the location
         // 2) Find neighbouring points to the location for the field
         if(mNumNeighbours == 0) {
            // 1) Get downscaled values
            float value = mData.getValue(field.getDate(), field.getInit(), field.getOffset(), iLocation, field.getMember(), iVariable);
            values.push_back(value);
            skills.push_back(field.getSkill());
         }
         else {
            // 2) Get neighbours on the grid
            std::string dataset = field.getMember().getDataset();
            Input* input = mData.getInput(dataset);
            std::vector<Location> locations;
            input->getSurroundingLocations(iLocation, locations, mNumNeighbours);

            for(int i = 0; i < locations.size(); i++) {
               float value = input->getValue(field.getDate(), field.getInit(), field.getOffset(), locations[i].getId(), field.getMember().getId(), iVariable);
               values.push_back(value);
               skills.push_back(Global::MV);
            }
         }
      }
   }
   ens.setValues(values);
   ens.setSkills(skills);
   ens.setInfo(iDate, iInit, iOffset, iLocation, iVariable);


   /*
   // If selector gives the same results regardless of location or offset
   // only compute these once
   int locationId = mSelector->isLocationDependent() ? iLocation.getId() : 0;
   float offset   = mSelector->isOffsetDependent()   ? iOffset : 0;

   Key::Ensemble key(iDate, iInit, offset, locationId, iVariable);

   // Get selector indices
   if(mSelectorCache.isCached(key)) {
      const std::vector<Field>& slices = mSelectorCache.get(key);
      iFields = slices;
   }
   else {
      Parameters parSelector;
      int selectorIndex = 0; // Only one selector
      getParameters(Component::TypeSelector, iDate, iInit, offsetCode, locationCode, iVariable, selectorIndex, parSelector);
      mSelector->select(iDate, iInit, offset, iLocation, iVariable, parSelector, iFields);
      mSelectorCache.add(key, iFields);
   }

   // TODO: Is this the best way to interpret isOffsetDependent? Because it could also
   // mean that selector returns the same offset in the slice regardless of the search offset
   if(!mSelector->isOffsetDependent()) {
      // Adjust offset back
      for(int i = 0; i < (int) iFields.size(); i++) {
         iFields[i].setOffset(iOffset);
      }
   }

   // Check that selector doesn't violate by selecting future indices
   // Perhaps this slows it down?
   if(0 && !mSelector->allowedToCheat()) {
      for(int i = 0; i < (int) iFields.size(); i++) {
         // TODO:
         // Its ok to pick future dates if forecasts
         //if(iFields[i].getMember().getDataset()) {
         //}

         float timeDiff = Global::getTimeDiff(iDate, iInit, iInit, iFields[i].getDate(), iFields[i].getInit(), iFields[i].getOffset());
         if(timeDiff < 0) {
            std::stringstream ss;
            ss << "ConfigurationDefault: Selector picked a date/offset ("
               << iFields[i].getDate() << " " << iFields[i].getOffset() << ") "
               << "which would not be available operationally for date "
               << iDate;
            Global::logger->write(ss.str(), Logger::error);
         }
      }
   }
   */

   return ens;
}

int Selector::getMaxMembers() const {
   return mDefaultMaxMembers;
}

bool Selector::isConstSize() const {
   return isConstSizeCore() && !mRemoveMissing;
}
