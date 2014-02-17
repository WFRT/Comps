#ifndef DOWNSCALER_H
#define DOWNSCALER_H
#include "../Processor.h"

class Location;
class Parameters;
class Data;
class Obs;
class Field;
class Input;
class Member;

class Downscaler : public Processor {
   public:
      Downscaler(const Options& iOptions, const Data& iData);
      //! Generate downscaled value for a particular location
      virtual float downscale(const Field& iField,
            const std::string& iVariable,
            const Location& iLocation,
            const Parameters& iParameters) const = 0;
      //float downscale(const Key::Input& iKey) const;
      float downscale(const Input* iInput, 
            int iDate, int iInit, float iOffset,
            const Location& iLocation,
            const Member& iMember,
            std::string iVariable) const;
      virtual void updateParameters(const Field& iField,
            const std::string& iVariable,
            const Location &iLocation,
            const Obs& iObs,
            Parameters& iParameters) const {};
      virtual void updateParameters(const std::vector<Field>& iFields,
            const std::vector<Obs>& iObs,
            Parameters& iParameters) const {};
      static Downscaler* getScheme(const Options& iOptions, const Data& iData);
      static Downscaler* getScheme(const std::string& iTag, const Data& iData);
};
#endif
