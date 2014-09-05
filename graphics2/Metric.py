import numpy as np
import Common
import sys
import inspect
def getAllMetrics():
   temp = inspect.getmembers(sys.modules[__name__], inspect.isclass)
   return temp
   #metrics = list()
   #for i in range(0, len(temp)):
   #   metrics.append(temp[i][0])
   #return metrics

class Metric:
   def compute(self, data):
      x = self.getX(data)
      maxInd = len(data.getAxisValues())
      scores = np.zeros(maxInd, 'float')
      for i in range(0,maxInd):
         data.setIndex(i)
         scores[i] = self.computeCore(data)
      return scores
   def getClassName(self):
      name = self.__class__.__name__
      return name
   def getX(self, data):
      return data.getAxisValues()
   def xlabel(self, data):
      return data.getAxis()
   def units(self, data):
      return data.getUnits()
   def ylabel(self, data):
      return self.description() + " (" + self.units(data) + ")"
   def name(self):
      Common.warning("Metric." + self.getClassName() + " has no name")
      return self.getClassName()
   @staticmethod
   def description():
      return ""

class Mean(Metric):
   def __init__(self, name):
      self._name = name
   def computeCore(self, data):
      return np.mean(data.getScores(self._name))
   def name(self):
      return self._name

class Mae(Metric):
   def computeCore(self, data):
      [obs, fcst] = data.getScores(["obs", "fcst"])
      return np.mean(abs(obs - fcst))
   @staticmethod
   def description():
      return "Mean absolute error"

class Bias(Metric):
   def computeCore(self, data):
      [obs, fcst] = data.getScores(["obs", "fcst"])
      return np.mean(obs - fcst)
   @staticmethod
   def description():
      return "Bias"

class Std(Metric):
   def __init__(self, name):
      self._name = name
   def computeCore(self, data):
      return np.std(data.getScores(self._name))
   def name(self):
      return self._name

class Pit(Metric):
   def __init__(self, name="pit"):
      self._name = name
   def getX(self, data):
      return np.linspace(0.1,0.9,9)
   def compute(self, data):
      return data.getScores(self._name)
   def xlabel(self, data):
      return "Cumulative probability"
   def ylabel(self, data):
      return "Observed frequency"

class Rmse(Metric):
   def computeCore(self, data):
      [obs,fcst] = data.getScores(["obs", "fcst"])
      return np.mean((obs - fcst)**2)**0.5
   def name(self):
      return "Root mean squared error"
   @staticmethod
   def description():
      return "Root mean squared error"

class Corr(Metric):
   def computeCore(self, data):
      [obs,fcst]  = data.getScores(["obs", "fcst"])
      return np.corrcoef(obs,fcst)[1,0]
   def name(self):
      return "Correlation"
   @staticmethod
   def description():
      return "Correlation between obesrvations and forecasts"

class Contingency(Metric):
   def __init__(self, thresholds):
      self._thresholds = thresholds
   def getX(self, data):
      return self._thresholds
   def compute(self, data):
      obs = data.getScores("obs")
      fcst = data.getScores("fcst")

      if(len(fcst) > 0):
         # Compute frequencies
         scores = np.nan*np.zeros([len(self._thresholds),1],'float')
         for i in range(0,len(self._thresholds)):
            threshold = self._thresholds[i]
            a    = np.ma.sum((fcst >= threshold) & (obs >= threshold))
            b    = np.ma.sum((fcst >= threshold) & (obs <  threshold))
            c    = np.ma.sum((fcst <  threshold) & (obs >= threshold))
            d    = np.ma.sum((fcst <  threshold) & (obs <  threshold))
            scores[i] = self.computeCore(a, b, c, d)
      else:
         self.warning(data.getFilename() + " does not have any valid forecasts")

      return scores
   def xlabel(self):
      return self._thresholds

class Ets(Contingency):
   def computeCore(self, a, b, c, d):
      return a + b + c + d
   @staticmethod
   def description():
      return "Equitable threat score"
