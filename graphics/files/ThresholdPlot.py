from VerifPlot import *
# Abstract class for plotting scores based on the 2x2 contingency table
# Derived classes must implement getY and getYLabel
class ContingencyPlot(Plot):
   def __init__(self, thresholds=None):
      Plot.__init__(self)
      self._thresholds = thresholds

   # Determine smart thresholds based on observed data
   def getThresholds(self, files):
      if(self._thresholds == None):
         N = 20
         obs  = files[0].getScores('obs')
         mobs = np.ma.masked_array(obs, np.isnan(obs))
         thresholds = np.linspace(np.ma.min(mobs.flatten()), np.ma.max(mobs.flatten()),N+1)
      else:
         thresholds = self._thresholds
      return thresholds

   @staticmethod
   def supportsCompute():
      return True
   @staticmethod
   def supportsThreshold():
      return True

   def computeCore(self, files):
      NF = len(files)
      y = np.zeros([len(files[0].getX()), NF], 'float')
      thresholds = self.getThresholds(self.files)
      for nf in range(0,NF):
         file = files[nf]
         temp = np.zeros([len(files[0].getX()), len(thresholds)], 'float')
         for t in range(0, len(thresholds)):
            threshold = thresholds[t]
            temp[:,t] = self.computeForThreshold(file, threshold)
         y[:,nf] = np.mean(temp, axis=1)
      return y

   def computeFlat(self, file, threshold):
      obs  = file.getScores('obs').flatten()
      fcst  = file.getScores('fcst').flatten()
      dim = file.getByAxis()
   
      # Compute frequencies
      a    = np.ma.sum((fcst >= threshold) & (obs >= threshold))
      b    = np.ma.sum((fcst >= threshold) & (obs <  threshold))
      c    = np.ma.sum((fcst <  threshold) & (obs >= threshold))
      d    = np.ma.sum((fcst <  threshold) & (obs <  threshold))
   
      return self.getY(a, b, c, d)

   # Compute the score for a given threshold
   def computeForThreshold(self, file, threshold):
      N  = file.getLength()
      y = np.zeros([N], 'float')
      obs  = file.getScores('obs')
      fcst  = file.getScores('fcst')
      dim = file.getByAxis()
      for i in range(0, N):
         if(dim == 0):
            fcst0 = fcst[i,:,:].flatten()
            obs0  = obs[i,:,:].flatten()
         elif(dim == 1):
            fcst0 = fcst[:,i,:].flatten()
            obs0  = obs[:,i,:].flatten()
         elif(dim == 2):
            fcst0 = fcst[:,:,i].flatten()
            obs0  = obs[:,:,i].flatten()
         else:
            fcst0 = fcst.flatten()
            obs0  = obs.flatten()

         # Compute frequencies
         a    = np.ma.sum((fcst0 >= threshold) & (obs0 >= threshold))
         b    = np.ma.sum((fcst0 >= threshold) & (obs0 <  threshold))
         c    = np.ma.sum((fcst0 <  threshold) & (obs0 >= threshold))
         d    = np.ma.sum((fcst0 <  threshold) & (obs0 <  threshold))

         y[i] = self.getY(a, b, c, d)
      return y

   def plotCore(self, ax):
      NF = len(self.files)
      units = ""
      thresholds = self.getThresholds(self.files)
      dim = self.files[0].getByAxis()

      for nf in range(0,NF):
         file = self.files[nf]
         style = self.getStyle(nf, NF)
         color = self.getColor(nf, NF)

         # Compute frequencies
         if(dim >= 0 and dim <= 2):
            y = np.zeros([file.getLength(), len(thresholds)], 'float')
            for i in range(0,len(thresholds)):
               threshold = thresholds[i]
               y[:,i] = self.computeForThreshold(file, threshold)
            y = np.mean(y, axis=1)
            x = file.getX()
            ax.plot(x, y, style, color=color)
         else:
            y = np.nan*np.zeros([len(thresholds),1],'float')
            for i in range(0,len(thresholds)):
               threshold = thresholds[i]
               y[i] = self.computeFlat(file, threshold)
            ax.plot(thresholds, y, style, color=color)

         units = " (" + file.getUnits() + ")"

      ylim = self.getYlim()
      if(ylim != None):
         ax.set_ylim(ylim)
      if(dim >= 0 and dim <= 2):
         ax.set_xlabel(file.getXLabel())
      else:
         ax.set_xlabel("Threshold" + units)

      ax.set_ylabel(self.getYLabel(file))

   # Default for most contingency plots is [0,1]
   def getYlim(self):
      return [0,1]

class HitRatePlot(ContingencyPlot):
   @staticmethod
   def description():
      return "Plots the hit rate for one or more thresholds. Accepts -c."
   def getY(self, a, b, c, d):
      return a / 1.0 / (a + c)
   def getYLabel(self, file):
      return "Hit rate"

class FalseAlarmPlot(ContingencyPlot):
   @staticmethod
   def description():
      return "Plots the false alarm rate for one or more thresholds. Accepts -c."
   def getY(self, a, b, c, d):
      return b / 1.0 / (b + d)
   def getYLabel(self, file):
      return "False alarm rate"

class EtsPlot(ContingencyPlot):
   @staticmethod
   def description():
      return "Plots the Equitable Threat Score for one or more thresholds. Accepts -c."
   def getY(self, a, b, c, d):
      # Divide by length(I) early on so we don't get integer overflow:
      N = a + b + c + d
      ar   = (a + b) / 1.0 / N * (a + c)
      if(a+b+c-ar > 0):
         return (a - ar) / 1.0 / (a + b + c - ar)
      else:
         return 0
   def getYLabel(self, file):
      return "Equitable Threat Score"

class ThreatPlot(ContingencyPlot):
   @staticmethod
   def description():
      return "Plots the Threat Score for one or more thresholds. Accepts -c."
   def getY(self, a, b, c, d):
      if(a+b+c > 0):
         return a / 1.0 / (a + b + c)
      else:
         return 0
   def getYLabel(self, file):
      return "Threat Score"

class BiasFreqPlot(ContingencyPlot):
   @staticmethod
   def description():
      return "Plots the bias frequency (fcst >= threshold) / (obs >= threshold) for one or more " + \
            "thresholds. Accepts -c."
   def getY(self, a, b, c, d):
      if(a+c > 0):
         return 1.0 * (a + b) / (a + c)
      else:
         return 0
   def getYLabel(self, file):
      return "Bias frequency (fcst / obs)"
   def getYlim(self):
      return None

class HanssenKuiperPlot(ContingencyPlot):
   @staticmethod
   def description():
      return "Plots the Hanssen Kuiper skill score for one or more " + \
            "thresholds. Accepts -c."
   def getY(self, a, b, c, d):
      if((a+c)*(b+d)> 0):
         return (a*d-b*c)* 1.0 / ((a + c)*(b + d))
      else:
         return 0
   def getYLabel(self, file):
      return "Hanssen Kuiper Skill Score"
   def getYlim(self):
      return None

class RocPlot(Plot):
   @staticmethod
   def description():
      return "Plots the receiver operating characteristics curve for an ensemble " \
         + "forecast for a single threshold (-r)"
   def __init__(self, threshold):
      self.error("RocPlot currently not implemented")
      Plot.__init__(self)
      self.thresholds = threshold

   @staticmethod
   def supportsThreshold():
      return True

   def plotCore(self, ax):
      NF = len(self.files)
      for nf in range(0,NF):
         file = self.files[nf]
         style = self.getStyle(nf, NF)
         color = self.getColor(nf, NF)

class DRocPlot(Plot):
   @staticmethod
   def description():
      return "Plots the receiver operating characteristics curve for the deterministic " \
         + "forecast for a single threshold. Uses different forecast thresholds to create points. Accepts -c."
   def __init__(self, thresholds=None):
      Plot.__init__(self)
      if(thresholds == None):
         self.error("DRoc plot needs one or more thresholds (use -r)")
      self.thresholds = thresholds

   @staticmethod
   def supportsThreshold():
      return True

   def plotCore(self, ax):
      N = 31
      fthresholds = np.linspace(min(0,min(self.thresholds)-10), max(self.thresholds)+10, N)

      NF = len(self.files)
      for nf in range(0,NF):
         file = self.files[nf]
         style = self.getStyle(nf, NF)
         color = self.getColor(nf, NF)

         obs  = file.getScores('obs')
         fcst = file.getScores('fcst')

         # Remove points with missing forecasts and/or observations
         I    = np.where(np.logical_not(np.isnan(fcst)) & np.logical_not(np.isnan(obs)))
         fcst = np.array(fcst[I])
         obs  = np.array(obs[I])

         # Compute frequencies
         y = np.nan*np.zeros([len(fthresholds),1],'float')
         x = np.nan*np.zeros([len(fthresholds),1],'float')
         for t in range(0, len(self.thresholds)):
            threshold = self.thresholds[t]
            for i in range(0,len(fthresholds)):
               fthreshold = fthresholds[i]
               a    = np.ma.sum((fcst >= fthreshold) & (obs >= threshold)) # Hit
               b    = np.ma.sum((fcst >= fthreshold) & (obs <  threshold)) # FA
               c    = np.ma.sum((fcst <  fthreshold) & (obs >= threshold)) # Miss
               d    = np.ma.sum((fcst <  fthreshold) & (obs <  threshold)) # Correct rejection
               if(a + c > 0 and b + d > 0):
                  y[i] = a / 1.0 / (a + c) 
                  x[i] = b / 1.0 / (b + d) 
            ax.plot(x, y, style, color=color)
         ax.set_xlim([0,1])
         ax.set_ylim([0,1])
         ax.set_xlabel("False alarm rate")
         ax.set_ylabel("Hit rate")
         units = " " + file.getUnits()
         ax.set_title("Threshold: " + str(self.thresholds) + units)

class DRoc0Plot(Plot):
   @staticmethod
   def description():
      return "Plots the receiver operating characteristics curve for the deterministic " \
         + "forecast for a single threshold. Accepts -c."
   def __init__(self, thresholds=None):
      Plot.__init__(self)
      if(thresholds == None):
         self.error("DRoc plot needs one or more thresholds (use -r)")
      self.thresholds = thresholds

   @staticmethod
   def supportsThreshold():
      return True

   def plotCore(self, ax):
      N = 31
      fthreshold = self.thresholds

      NF = len(self.files)
      for nf in range(0,NF):
         file = self.files[nf]
         style = self.getStyle(nf, NF)
         color = self.getColor(nf, NF)

         obs  = file.getScores('obs')
         fcst = file.getScores('fcst')

         # Remove points with missing forecasts and/or observations
         I    = np.where(np.logical_not(np.isnan(fcst)) & np.logical_not(np.isnan(obs)))
         fcst = np.array(fcst[I])
         obs  = np.array(obs[I])

         # Compute frequencies
         for t in range(0, len(self.thresholds)):
            threshold = self.thresholds[t]
       	    a    = np.ma.sum((fcst >= fthreshold) & (obs >= threshold)) # Hit
       	    b    = np.ma.sum((fcst >= fthreshold) & (obs <  threshold)) # FA
            c    = np.ma.sum((fcst <  fthreshold) & (obs >= threshold)) # Miss
            d    = np.ma.sum((fcst <  fthreshold) & (obs <  threshold)) # Correct rejection
            if(a + c > 0 and b + d > 0):
	       y = a / 1.0 / (a + c) 
	       x = b / 1.0 / (b + d) 
            ax.plot([0,x,1], [0,y,1], style, color=color)
         ax.set_xlim([0,1])
         ax.set_ylim([0,1])
         ax.set_xlabel("False alarm rate")
         ax.set_ylabel("Hit rate")
         units = " " + file.getUnits()
         ax.set_title("Threshold: " + str(self.thresholds) + units)
