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

# Computes scores for each xaxis value
class Metric:
   def compute(self, data):
      x = self.getX(data)
      size   = data.getAxisSize()
      scores = np.zeros(size, 'float')
      # Loop over x-axis
      for i in range(0,size):
         data.setIndex(i)
         scores[i] = self.computeCore(data)
      return scores
   @staticmethod
   def supportsX():
      return True
   def getAxisFormatter(self, data):
      return data.getAxisFormatter()
   def getClassName(self):
      name = self.__class__.__name__
      return name
   def ylim(self):
      return [None, None]
   def xlim(self):
      return [None, None]
   def getX(self, data):
      return data.getAxisValues()
   def xlabel(self, data):
      return data.getAxisLabel()
   def units(self, data):
      return data.getUnits()
   def ylabel(self, data):
      return self.name() + " (" + self.units(data) + ")"
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
   def ylim(self):
      return [0, None]
   def name(self):
      return "MAE"
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

class StdError(Metric):
   def computeCore(self, data):
      [obs, fcst] = data.getScores(["obs", "fcst"])
      bias = np.mean(obs - fcst)
      return np.mean((obs - fcst - bias)**2)**0.5
   def name(self):
      return "Standard error"
   @staticmethod
   def description():
      return "Standard error (i.e. RMSE if forecast had no bias)"

class Std(Metric):
   def __init__(self, name):
      self._name = name
   def ylim(self):
      return [0, None]
   def computeCore(self, data):
      return np.std(data.getScores(self._name))
   def name(self):
      return self._name

class Pit(Metric):
   def __init__(self, name="pit"):
      self._name = name
   def getX(self, data):
      return np.linspace(0.1,0.9,9)
   def ylim(self):
      return [0, 1]
   def xlim(self):
      return [0, 1]
   def compute(self, data):
      return data.getScores(self._name)
   def xlabel(self, data):
      return "Cumulative probability"
   def ylabel(self, data):
      return "Observed frequency"
   def name(self):
      return "PIT"

class Rmse(Metric):
   def computeCore(self, data):
      [obs,fcst] = data.getScores(["obs", "fcst"])
      return np.mean((obs - fcst)**2)**0.5
   def ylim(self):
      return [0, None]
   def name(self):
      return "RMSE"
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

class Within(Metric):
   def __init__(self, threshold):
      self._threshold = threshold
      if(threshold == None):
         Common.error("Metric within requires one threshold")
   def computeCore(self, data):
      [obs,fcst]  = data.getScores(["obs", "fcst"])
      diff = abs(obs - fcst)
      return np.mean(diff <= self._threshold)
   def ylim(self):
      return [0,1]
   def name(self):
      return "Within"
   @staticmethod
   def description():
      return "The percentage of forecasts within some error bound (use -r)"

class Contingency(Metric):
   def __init__(self, thresholds):
      if(thresholds == None):
         Common.error("Contingency plots need at least one threshold (-r)")
      self._thresholds = thresholds
   @staticmethod
   def supportsX():
      return False
   def getAxisFormatter(self, data):
      from matplotlib.ticker import ScalarFormatter
      return ScalarFormatter()
   def xlabel(self, data):
      return data.getVariableAndUnits()
   def ylabel(self, data):
      return self.name()
   def getX(self, data):
      return self._thresholds
   def compute(self, data):
      thresholds = self._thresholds
      [obs,fcst] = data.getScores(["obs", "fcst"])
      data.setAxis("none")
      data.setIndex(0)
      if(len(fcst) > 0):
         # Compute frequencies
         scores = np.nan*np.zeros([len(thresholds),1],'float')
         for i in range(0,len(thresholds)):
            threshold = thresholds[i]
            a    = np.ma.sum((fcst >= threshold) & (obs >= threshold))
            b    = np.ma.sum((fcst >= threshold) & (obs <  threshold))
            c    = np.ma.sum((fcst <  threshold) & (obs >= threshold))
            d    = np.ma.sum((fcst <  threshold) & (obs <  threshold))
            scores[i] = self.computeCore(a, b, c, d)
      else:
         self.warning(data.getFilename() + " does not have any valid forecasts")

      return scores

class Ets(Contingency):
   def computeCore(self, a, b, c, d):
      N = a + b + c + d
      ar   = (a + b) / 1.0 / N * (a + c)
      if(a+b+c-ar > 0):
         return (a - ar) / 1.0 / (a + b + c - ar)
      else:
         return np.nan
   def name(self):
      return "ETS"
   @staticmethod
   def description():
      return "Equitable threat score"

# Dunny class to provide description
class Reliability(Metric):
   def compute(self):
      return None
   def name(self):
      return "Reliability"
   @staticmethod
   def description():
      return "Reliability diagram for a certain threshold (-r)"

