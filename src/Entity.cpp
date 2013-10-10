#include "Entity.h"

Entity::Entity() :
      mDate(Global::MV),
      mInit(Global::MV),
      mOffset(Global::MV),
      mVariable("") {
}

Entity::Entity(int iDate, int iInit, float iOffset, const Location& iLocation, const std::string& iVariable) :
      mDate(iDate),
      mInit(iInit),
      mOffset(iOffset),
      mLocation(iLocation),
      mVariable(iVariable) {

}
