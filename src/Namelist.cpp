#include "Namelist.h"

Namelist::Namelist(const std::string& iFilename) : mProject("") {
   std::stringstream ss;
   ss << iFilename;
   std::string filename = ss.str();
   parse(filename);
}
Namelist::Namelist(const std::string& iType, const std::string& iProject) : mProject(iProject) {
   std::stringstream ss;
   ss << getDefaultLocation() << iProject << "/" << iType << ".nl";
   std::string filename = ss.str();
   parse(filename);
}

void Namelist::parse(const std::string& iFilename) {
   std::ifstream ifs(iFilename.c_str(), std::ifstream::in);
   // Loop over each line
   while(ifs.good()) {
      char line[10000];
      ifs.getline(line, 10000, '\n');
      if(line[0] != '\0' && line[0] != '#' && line[0] != ' ') {
         // Get the tag for this line
         std::stringstream ss(line);
         std::string tag;
         ss >> tag;
         assert(tag != "");

         // Alter line such that it has tag=<someTag>
         std::stringstream ss2;
         // Add tag
         if(mProject != "" && mProject != "default") {
            ss2 << "tag=" << mProject << "." << tag;
         }
         else {
            ss2 << "tag=" << tag;
         }
         // Append all options after the tag
         while(ss.good()) {
            std::string temp;
            ss >> temp;
            ss2 << " " << temp;
         }

         std::string newLine = ss2.str();
         mMap[tag] = Options(newLine);
      }
   }
}
std::string Namelist::getDefaultLocation() {
   return "./namelists/";
}

bool Namelist::getOptions(const std::string& iTag, Options& iOptions) const {
   std::map<std::string, Options>::iterator it = mMap.find(iTag);
   if(it == mMap.end())
      return false;

   iOptions = it->second;
   return true;
}

std::vector<std::string> Namelist::getTags() const {
   std::vector<std::string> tags;

   for(std::map<std::string, Options>::const_iterator it = mMap.begin(); it != mMap.end(); it++) {
      std::string tag = it->first;
      tags.push_back(tag);
   }
   return tags;
}
