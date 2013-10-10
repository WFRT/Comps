#include "Deterministic.h"

Deterministic::Deterministic() :
      mValue(Global::MV) {
}
Deterministic::Deterministic(float iValue, int iDate, int iInit, float iOffset, const Location& iLocation, const std::string& iVariable) :
      Entity(iDate, iInit, iOffset, iLocation, iVariable), 
      mValue(iValue) {

}
