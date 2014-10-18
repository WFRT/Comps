#include "Component.h"
#include "Options.h"

Component::Component(const Options& iOptions) : mTag("unspecified") {
   iOptions.getValue("tag", mTag);
}
Component::~Component() {}

void Component::doesNotExist(const std::string& iClassName) {
   std::stringstream ss;
   ss << "Scheme " << iClassName << " does not exist" << std::endl;
   Global::logger->write(ss.str(), Logger::error);
}

void Component::underDevelopment() const {
   std::stringstream ss;
   ss << "The " << typeid(*this).name() << " scheme is under development and cannot be trusted" << std::endl;
   Global::logger->write(ss.str(), Logger::critical);
}

std::string Component::getSchemeName() const {
   std::string name = typeid(*this).name();
   std::stringstream ss;

   // Remove the length digits at the beginning of the component name
   int stringLength = name.length();
   int iStart = 1;
   if(stringLength >= 12)
      iStart = 2;
   else if(stringLength >= 102)
      iStart = 3;
   else
      iStart = 4;

   for(int i = iStart; i < stringLength; i++) {
      ss << name[i];
   }
   return ss.str();
}

std::string Component::getTag() const {
   return mTag;
}
