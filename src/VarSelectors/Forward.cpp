#include "Forward.h"
#include "../Options.h"
#include "../Data.h"
#include "../Inputs/Input.h"
#include "../Location.h"
#include "../Member.h"
#include "../Scheme.h"
#include "../DetMetrics/DetMetric.h"

VarSelectorForward::VarSelectorForward(const Options& iOptions, const Data& iData) : VarSelector(iOptions, iData) {
   iOptions.getRequiredValue("maxVars", mMaxVars);
}

void VarSelectorForward::getVariables(const Data& iData,
      int iInit,
      float iOffset,
      const Location& iLocation,
      std::vector<std::string>& iVariables) const {
   Input* inputF = iData.getInput();
   Input* inputO = iData.getObsInput();

   std::vector<std::string> variables;
   inputF->getVariables(variables);

   int locationId = iLocation.getId();

   // Members
   std::vector<Member> members;
   inputF->getMembers(members);

   std::vector<std::string> varList;
   std::vector<std::string> varRemaining = variables;
   std::vector<std::string> bestVarList;
   float bestScore = Global::INF;

   std::vector<float> obs;

   while((int) varList.size() < mMaxVars) {

      std::vector<std::pair<std::string, float> > scores;
      // Try adding in a new variable
      for(int v = 0; v < (int) varRemaining.size(); v++) {
         // DEBUG: Print variables in map
         /*
         std::vector<std::string> tempVars;
         inputF->getVariables(tempVars);
         for(int i = 0; i < (int) tempVars.size(); i++) {
            std::cout << "VARS: " << tempVars[i] << std::endl;
         }
         */

         double startTime = Global::clock();

         std::vector<std::string> currVarList = varList;
         currVarList.push_back(varRemaining[v]);

         float score = VarSelector::run(iData, iInit, iOffset, iLocation, currVarList, *mDetMetric);

         if(Global::isValid(score)) {
            std::pair<std::string, float> p(varRemaining[v], score);
            scores.push_back(p);
         }

         if(Global::isValid(score) && score < bestScore) {
            bestScore = score;
            bestVarList = currVarList;
         }

         //std::cout << "Time: " << Global::clock() - startTime << std::endl;
         std::cout << "SCORE: ";
         for(int i = 0; i < (int) currVarList.size(); i++) {
            std::cout << currVarList[i] << " ";
         }
         std::cout << " " << score << std::endl;
      }

      // Sort and find the best addition
      std::sort(scores.begin(), scores.end(), Global::sort_pair_second<std::string, float>());

      // Add to list
      assert(scores.size() > 0);
      std::string bestVar = scores[0].first;
      varList.push_back(bestVar);
      varRemaining.erase(std::find(varRemaining.begin(), varRemaining.end(), bestVar));
      std::cout << "# " << varList.size() << " " << scores[0].second << std::endl;
   }
   iVariables = bestVarList; //varList;
}
