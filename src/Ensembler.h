#ifndef ENSEMBLER_H
#define ENSEMBLER_H
#include "Global.h"
#include "Distribution.h"

class Ensemble;

class Ensembler {
   public:
      Ensembler();
      static void getEnsemble(Distribution::ptr iDistribution, int iNumMembers, Ensemble& iEnsemble);
};
#endif
