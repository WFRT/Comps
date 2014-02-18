#ifndef DOWNSCALER_H
#define DOWNSCALER_H
#include "../Processor.h"
#include "../Inputs/Input.h"

class Location;
class Data;
class Obs;
class Field;
class Member;

class Downscaler : public Component {
   public:
      Downscaler(const Options& iOptions);
      //! Generate downscaled value for a particular location
      virtual float downscale(const Input* iInput, 
            int iDate, int iInit, float iOffset,
            const Location& iLocation,
            int iMemberId,
            const std::string& iVariable) const = 0;
      static Downscaler* getScheme(const Options& iOptions);
      static Downscaler* getScheme(const std::string& iTag);
};
#endif
