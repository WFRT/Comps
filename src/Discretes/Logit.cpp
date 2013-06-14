#include "Logit.h"
#include "../Obs.h"
#include "../Scheme.h"
#include "../Parameters.h"
#include "../Variables/Variable.h"
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/io.hpp>

DiscreteLogit::DiscreteLogit(const Options& iOptions, const Data& iData) : Discrete(iOptions, iData),
      mUseConst(false),
      mUseMean(false),
      mUseFrac(false) {

   iOptions.getValue("useConst", mUseConst);
   iOptions.getValue("useMean",  mUseMean);
   iOptions.getValue("useFrac",  mUseFrac);

   mNumCoeff = mUseConst + mUseMean + mUseFrac;

   if(mNumCoeff == 0) {
      // TODO
      assert(0);
   }
}

DiscreteLogit::~DiscreteLogit() {

}

float DiscreteLogit::getPCore(const Ensemble& iEnsemble, const Parameters& iParameters) const {
   const Variable* var = Variable::get(iEnsemble.getVariable());

   std::vector<float> vars;
   getVariables(iEnsemble, vars);

   if(mEstimator) {
      assert(vars.size() == iParameters.size());
   }
   else {
      assert((int) vars.size() + vars.size()*vars.size() == iParameters.size());
   }

   // Add up regression terms
   float logit = 0;
   for(int i = 0; i < mNumCoeff; i++) {
      if(!Global::isValid(iParameters[i]) || !Global::isValid(vars[i])) {
         return Global::MV;
      }
      logit += iParameters[i] * vars[i];
   }

   assert(Global::isValid(logit));
   float P = exp(logit)/(1 + exp(logit));

   assert(P>0 && P < 1);

   return P;
}

void DiscreteLogit::getDefaultParametersCore(Parameters& iParameters) const {
   std::vector<float> param;
   if(mUseConst)
      param.push_back(0); // a
   if(mUseMean)
      param.push_back(0); // b
   if(mUseFrac)
      param.push_back(0); // c

   if(!mEstimator) {
      int N = param.size(); // number of coefficients
      // Create identity matrix of size NxN
      for(int i = 0; i < N; i++) {
         for(int j = 0; j < N; j++) {
            float value = (i==j) ? 1 : 0;
            param.push_back(value);
         }
      }
   }
   iParameters.setAllParameters(param);
}

void DiscreteLogit::getVariables(const Ensemble& iEnsemble, std::vector<float>& iVariables) const {
   const Variable* var = Variable::get(iEnsemble.getVariable());

   iVariables.clear();
   float logit = 0;
   int counter = 0;
   if(mUseConst) {
      iVariables.push_back(1);
   }
   if(mUseMean) {
      float ensMean = Global::mean(iEnsemble.getValues());
      float value = Global::MV;
      if(Global::isValid(ensMean))
         value = pow(ensMean, (float) 1/3);
      iVariables.push_back(value);
   }
   if(mUseFrac) {
      int counter = 0;
      int total   = 0;
      for(int i = 0; i < iEnsemble.size(); i++) {
         if(Global::isValid(iEnsemble[i])) {
            if(iEnsemble[i] == var->getMin()) {
               counter++;
            }
            total++;
         }
      }
      float frac = Global::MV;
      if(total > 0)
         frac = (float) counter / total;
      iVariables.push_back(frac);
   }
   assert((int) iVariables.size() == mNumCoeff);
}
void DiscreteLogit::getCoefficients(const Parameters& iParameters, Parameters& iCoefficients) const {
   std::vector<float> param;
   param.resize(mNumCoeff);

   assert(iParameters.size() == mNumCoeff + mNumCoeff*mNumCoeff);
   for(int i = 0; i < mNumCoeff; i++) {
      param[i] = iParameters[i];
   }
   iCoefficients.setAllParameters(param);
}
void DiscreteLogit::updateParametersCore(const Ensemble& iEnsemble, const std::vector<Obs>& iObs, Parameters& iParameters) const {
   // Set up coefficients


   // Set up D2S
   boost::numeric::ublas::matrix<float> D2S(mNumCoeff,mNumCoeff);
   boost::numeric::ublas::matrix<float> accumD2S(mNumCoeff,mNumCoeff);
   int Istart = mNumCoeff;
   for(int k = 0; k < mNumCoeff*mNumCoeff; k++) {
      int i = k % mNumCoeff;
      int j = floor(k/mNumCoeff);
      D2S(i,j) = iParameters[Istart + k];
      accumD2S(i,j) = 0;
   }
   boost::numeric::ublas::matrix<float> Rinv(mNumCoeff,mNumCoeff);
   bool status = getInverse(D2S, Rinv);
   assert(status);
   //std::cout << "D2S[0] = " << D2S(0,0) << " Inverse = " << Rinv(0,0) << std::endl;

   // Initialize new parameters
   std::vector<float> accumCoeff;
   accumCoeff.resize(mNumCoeff);

   for(int i = 0; i < mNumCoeff; i++) {
      accumCoeff[i] = 0;
   }
   int numUpdates = 0;

   for(int t = 0 ; t < (int) iObs.size(); t++) {
      if(Global::isValid(iObs[t].getValue())) {
         std::vector<float> vars;
         Parameters coeffs;
         getCoefficients(iParameters, coeffs);

         std::vector<float> H;
         bool status = getH(iObs[t], iEnsemble, iParameters, H);
         if(status) {
            //std::cout << "H = " << H[0] << std::endl;
            for(int i = 0; i < mNumCoeff; i++) {
               for(int j = 0; j < mNumCoeff; j++) {
                  accumD2S(i,j) += H[i]*H[j];
                  //if(H[j] > 200)
                  //std::cout << "H[j] = " << H[j] << " Rinv(i,j)= " << Rinv(i,j)  << std::endl;
                  accumCoeff[i] += H[j]*Rinv(i,j);
               }
            }
            numUpdates++;
         }
      }
   }

   if(numUpdates > 0) {
      // Update coefficients Pinson (eq 19)
      for(int i = 0; i < mNumCoeff; i++) {
         //std::cout << "Old parameter = " << iParameters[i];
         iParameters[i] = iParameters[i] + 1/mEfold * accumCoeff[i]/numUpdates;
         //std::cout << " new = " << iParameters[i] << std::endl;
      }

      // Update D2S Pinson (eq 20)
      for(int i = 0; i < mNumCoeff; i++) {
         for(int j = 0; j < mNumCoeff; j++) {
            int index = Istart + i + mNumCoeff*j;
            //std::cout << "Old est parameter = " << iParameters[index];
            iParameters[index] = (i==j) ? 1 : 0; //getLambda() * D2S(i,j) + 1/mEfold * accumD2S(i,j)/numUpdates;
            //iParameters[index] = getLambda() * D2S(i,j) + 1/mEfold * accumD2S(i,j)/numUpdates;
            //std::cout << " new = " << iParameters[index] << std::endl;
         }
      }
   }
   else {
      //std::cout << "Can't update" << std::endl;
   }
}

float DiscreteLogit::getLambda() const {
   assert(Global::isValid(mEfold));
   return 1 - 1/mEfold;
}

bool DiscreteLogit::getH(const Obs& iObs, const Ensemble& iEnsemble, const Parameters& iParameters, std::vector<float>& iH) const {

   iH.clear();
   iH.resize(mNumCoeff);

   std::vector<float> gradL;
   bool status = getGradL(iObs, iEnsemble, iParameters, gradL);
   if(!status) {
      return false;
   }
   //std::cout << "Grad = " << gradL[0] << std::endl;
   assert((int) gradL.size() == mNumCoeff);

   float L = getLikelihood(iObs.getValue(), iEnsemble, iParameters);
   assert(L > 0);
   if(!Global::isValid(L)) {
      return false;
   }
   for(int i = 0; i < mNumCoeff; i++) {
      float value = gradL[i]/L;
      iH[i] = value;
   }
   return true;
}

bool DiscreteLogit::getGradL(const Obs& iObs, const Ensemble& iEnsemble, const Parameters& iParameters, std::vector<float>& iGradL) const {

   iGradL.clear();
   iGradL.resize(mNumCoeff);

   Parameters coeffs;
   getCoefficients(iParameters, coeffs);

   float dx = 0.001;
   float L0 = getLikelihood(iObs.getValue(), iEnsemble, iParameters);
   if(!Global::isValid(L0))
      return false;
   for(int i = 0; i < mNumCoeff; i++) {
      // Perturb current parameter
      Parameters par = iParameters;
      par[i] += dx;

      // Compute likelihood derivative
      float currL = getLikelihood(iObs.getValue(), iEnsemble, par);
      if(!Global::isValid(currL))
         return false;
      float dL = currL - L0;
      iGradL[i] = dL/dx;
   }
   return true;
}

bool DiscreteLogit::getInverse(boost::numeric::ublas::matrix<float> iMatrix, boost::numeric::ublas::matrix<float>& iInverse) {
   // Taken from https://gist.github.com/2464434
   using namespace boost::numeric::ublas;
   typedef permutation_matrix<std::size_t> pmatrix;
   // create a working copy of the input
   matrix<float> A(iMatrix);
   // create a permutation matrix for the LU-factorization
   pmatrix pm(A.size1());

   // perform LU-factorization
   int res = lu_factorize(A,pm);
   if( res != 0 ) return false;

   // create identity matrix of "inverse"
   iInverse.assign(identity_matrix<float>(A.size1()));

   // backsubstitute to get the inverse
   lu_substitute(A, pm, iInverse);

   return true;
}
