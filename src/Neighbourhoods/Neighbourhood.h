#ifndef NEIGHBOURHOOD_H
#define NEIGHBOURHOOD_H
#include "../Global.h"
#include "../Component.h"
#include "../Location.h"
#include "../Inputs/Input.h"
#include "../Options.h"

// Defines the neighbourhood around a point
class Neighbourhood : public Component {
   public:
      Neighbourhood(const Options& iOptions);
      static        Neighbourhood* getScheme(const Options& iOptions);
      static        Neighbourhood* getScheme(const std::string& iTag);
      //! Creates a vector of locations representing the neighbourhood of 'iLocation'
      std::vector<Location> select(const Input* iInput, const Location& iLocation) const;
   protected:
      virtual std::vector<Location> selectCore(const Input* iInput, const Location& iLocation) const = 0;
};
#endif
