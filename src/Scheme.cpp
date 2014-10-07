#include "Scheme.h"

Options Scheme::getOptions(const std::string iTag) {
   Options opt;

   // Scheme is embedded in tag, for example:
   // scheme=[class=... ]
   if(iTag.length() > 0 && iTag[0] == '[') {
      std::string temp = iTag;
      // Remove brackets
      temp.erase(0,1);
      temp.erase(temp.length()-1,1);
      opt = Options(temp);
   }
   // Parse options from namelist
   else {
      // Determine which folder the namelist is in
      int dotPosition = -1;
      for(int i = 0; i < iTag.size(); i++) {
         if(iTag[i] == '.')
            dotPosition = i;
      }
      std::string tag;
      std::string folder;
      if(dotPosition == -1) {
         folder = "default";
         tag = iTag;
      }
      else {
         // The scheme has a dot in it, therefore use the first part as the folder
         assert(dotPosition != 0);
         folder = iTag.substr(0,dotPosition);
         tag = iTag.substr(dotPosition+1);//, iTag.size()-2);
      }

      Namelist nl("schemes", folder);
      if(!nl.getOptions(tag, opt)) {
         std::stringstream ss;
         ss << "No scheme found with name '" << iTag << "'";
         Global::logger->write(ss.str(), Logger::error);
      }
   }
   return opt;
}
