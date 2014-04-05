#include "Kmeans.h"
#include "../Obs.h"
#include "../Parameters.h"

float CorrectorKmeans::mTol = 1e-3;
CorrectorKmeans::CorrectorKmeans(const Options& iOptions, const Data& iData) :
      Corrector(iOptions, iData) {
   //! Number of clusters to identify
   iOptions.getRequiredValue("numMeans", mNumMeans);
}
void CorrectorKmeans::correctCore(const Parameters& iParameters, Ensemble& iEnsemble) const {
   // find mNumMeans centres
   float mean = iEnsemble.getMoment(1);
   float var  = iEnsemble.getMoment(2);

   // Check if empty ensemble
   if(!Global::isValid(mean) || !Global::isValid(var)) {
      std::vector<float> values;
      values.resize(mNumMeans, Global::MV);
      iEnsemble.setValues(values);
      return;
   }

   // Initialize means to be evenly spaced on the interval: [mean-std mean+std]
   std::vector<float> means;
   means.resize(mNumMeans);
   for(int i = 0; i < mNumMeans; i++) {
      means[i] = mean + (2.0*i/(mNumMeans-1.0) - 1)*pow(var,0.5);
   }

   // Iterate
   int counter = 1;
   while(true) {
      std::vector<int> counts;
      counts.resize(mNumMeans,0);

      std::vector<float> accum;
      accum.resize(mNumMeans, 0);

      int N = iEnsemble.size();
      // Which kmean is each member closest to?
      for(int i = 0; i < N; i++) {
         int nearest = Global::MV;
         float distNearest = Global::INF;
         if(Global::isValid(iEnsemble[i])) {
            for(int k = 0; k < mNumMeans; k++) {
               if(Global::isValid(means[k])) {
                  float dist = std::fabs(means[k] - iEnsemble[i]);
                  if(dist < distNearest) {
                     nearest = k;
                     distNearest = dist;
                  }
               }
            }
         }
         if(Global::isValid(nearest)) {
            // iEnsemble[i] is closest to 'nearest' mean
            accum[nearest] += iEnsemble[i];
            counts[nearest]++;
         }
      }

      // Compute new means
      float totalDifference = 0;
      for(int i = 0; i < mNumMeans; i++) {
         // Find 
         float lastMean = means[i];
         if(counts[i] > 0) {
            means[i] = accum[i] / counts[i];
         }
         else {
            means[i] = Global::MV;
         }
         totalDifference += std::fabs(means[i] - lastMean);
      }

      // Check if we need to stop;
      if(totalDifference < mTol) {
         break;
      }
      if(counter > 1000) {
         std::stringstream ss;
         ss << "CorrectorKmeans: Could not converge on " << mNumMeans
            << " means after " << counter << " iterations. Means = [";
         for(int i = 0; i < mNumMeans; i++) {
            ss << " " << means[i];
         }
         ss << " ]";
         Global::logger->write(ss.str(), Logger::warning);
         break;
      }
      counter++;
   }
   iEnsemble.setValues(means);
}
