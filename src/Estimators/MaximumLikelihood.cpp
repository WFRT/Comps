#include "MaximumLikelihood.h"
#include "../Ensemble.h"
#include "../Parameters.h"
#include "../Probabilistic.h"
#include "../Obs.h"
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/io.hpp>

EstimatorMaximumLikelihood::EstimatorMaximumLikelihood(const Options& iOptions, const Data& iData, const Probabilistic& iScheme) :
   EstimatorProbabilistic(iOptions, iData, iScheme) {
}
void EstimatorMaximumLikelihood::update(const std::vector<Ensemble>& iEnsemble,
      const std::vector<Obs>& iObs, 
      Parameters& iParameters) const {

   // Set up coefficients
   // Set up D2S
   int N = getNumCoefficients(iParameters);
   boost::numeric::ublas::matrix<float> D2S(N,N);
   boost::numeric::ublas::matrix<float> accumD2S(N,N);
   int Istart = N;
   for(int k = 0; k < N*N; k++) {
      int i = k % N;
      int j = floor(k/N);
      D2S(i,j) = iParameters[Istart + k];
      accumD2S(i,j) = 0;
   }
   boost::numeric::ublas::matrix<float> Rinv(N,N);
   bool status = getInverse(D2S, Rinv);
   assert(status);
   //std::cout << "D2S[0] = " << D2S(0,0) << " Inverse = " << Rinv(0,0) << std::endl;

   // Initialize new parameters
   std::vector<float> accumCoeff;
   accumCoeff.resize(N);

   for(int i = 0; i < N; i++) {
      accumCoeff[i] = 0;
   }
   int numUpdates = 0;

   for(int t = 0 ; t < (int) iObs.size(); t++) {
      float obs = iObs[t].getValue();
      Ensemble ens = iEnsemble[t];
      if(Global::isValid(obs)) {
         Parameters coeffs;
         getCoefficients(iParameters, coeffs);

         std::vector<float> H;
         bool status = getH(obs, ens, coeffs, H);
         if(status) {
            //std::cout << "H = " << H[0] << std::endl;
            for(int i = 0; i < N; i++) {
               for(int j = 0; j < N; j++) {
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
      for(int i = 0; i < N; i++) {
         //std::cout << "Old parameter = " << iParameters[i];
         iParameters[i] = iParameters[i] + 1/mEfold * accumCoeff[i]/numUpdates;
         //std::cout << " new = " << iParameters[i] << std::endl;
      }

      // Update D2S Pinson (eq 20)
      for(int i = 0; i < N; i++) {
         for(int j = 0; j < N; j++) {
            int index = Istart + i + N*j;
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

void EstimatorMaximumLikelihood::getDefaultParameters(const Parameters& iSchemeParameters,
      Parameters& iParameters) const {
   int N = iSchemeParameters.size();
   std::vector<float> param;
   for(int i = 0; i < N; i++) {
      for(int j = 0; j < N; j++) {
         param.push_back(i == j);
      }
   }
   iParameters.setAllParameters(param);
}
int EstimatorMaximumLikelihood::getIndex(int i, int j, int iSize) {
   return i*iSize + j;
}
float EstimatorMaximumLikelihood::getLambda() const {
   return 1 - 1/mEfold;
}

// TODO: Pass only coefficients in here
bool EstimatorMaximumLikelihood::getH(float iObs, const Ensemble& iEnsemble, const Parameters& iCoeffs, std::vector<float>& iH) const {
   int N = iCoeffs.size();

   iH.clear();
   iH.resize(N);

   std::vector<float> gradL;
   bool status = getGradL(iObs, iEnsemble, iCoeffs, gradL);
   if(!status) {
      return false;
   }
   //std::cout << "Grad = " << gradL[0] << std::endl;
   assert((int) gradL.size() == N);

   float L = mScheme.getLikelihood(iObs, iEnsemble, iCoeffs);
   if(L == 0) {
      std::cout << "MaximumLikelihood: Obs = " << iObs << std::endl;
   }
   assert(L > 0);
   if(!Global::isValid(L)) {
      return false;
   }
   for(int i = 0; i < N; i++) {
      float value = gradL[i]/L;
      iH[i] = value;
   }
   return true;
}

bool EstimatorMaximumLikelihood::getGradL(float iObs, const Ensemble& iEnsemble, const Parameters& iCoeffs, std::vector<float>& iGradL) const {
   int N = iCoeffs.size();

   iGradL.clear();
   iGradL.resize(N);

   float dx = 0.001;
   float L0 = mScheme.getLikelihood(iObs, iEnsemble, iCoeffs);
   if(!Global::isValid(L0))
      return false;
   for(int i = 0; i < N; i++) {
      // Perturb current parameter
      Parameters par = iCoeffs;
      par[i] += dx;

      // Compute likelihood derivative
      float currL = mScheme.getLikelihood(iObs, iEnsemble, par);
      if(!Global::isValid(currL))
         return false;
      float dL = currL - L0;
      iGradL[i] = dL/dx;
   }
   return true;
}

bool EstimatorMaximumLikelihood::getInverse(boost::numeric::ublas::matrix<float> iMatrix, boost::numeric::ublas::matrix<float>& iInverse) {
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

int EstimatorMaximumLikelihood::getNumCoefficients(const Parameters& iParameters) const {
   int N = (-1 + pow(1 + 4*iParameters.size(),0.5))/2;
   assert(N + N*N == iParameters.size());
   return N;
}
