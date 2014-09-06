import numpy as np
import Common
import sys
import inspect
def getAllMetrics():
   temp = inspect.getmembers(sys.modules[__name__], inspect.isclass)
   return temp

# Computes scores for each xaxis value
class Metric:
   def compute(self, data, threshold):
      size   = data.getAxisSize()
      scores = np.zeros(size, 'float')
      # Loop over x-axis
      for i in range(0,size):
         data.setIndex(i)
         scores[i] = self.computeCore(data, threshold)
      return scores
   @staticmethod
   def requiresThresholds():
      return False
   def getClassName(self):
      name = self.__class__.__name__
      return name
   def max(self):
      return None
   def min(self):
      return None
   def label(self, data):
      return self.name() + " (" + data.getUnits() + ")"
   def name(self):
      return self.getClassName()
   @staticmethod
   def description():
      return ""
   # Does it make sense to use '-x' with this metric?
   @staticmethod
   def supportsX():
      return False

class Mean(Metric):
   def __init__(self, name):
      self._name = name
   def computeCore(self, data, threshold):
      return np.mean(data.getScores(self._name))
   def name(self):
      return self._name

class Mae(Metric):
   def computeCore(self, data, threshold):
      [obs, fcst] = data.getScores(["obs", "fcst"])
      return np.mean(abs(obs - fcst))
   def min(self):
      return 0
   def name(self):
      return "MAE"
   @staticmethod
   def description():
      return "Mean absolute error"

class Bias(Metric):
   def computeCore(self, data, threshold):
      [obs, fcst] = data.getScores(["obs", "fcst"])
      return np.mean(obs - fcst)
   @staticmethod
   def description():
      return "Bias"

class StdError(Metric):
   def computeCore(self, data, threshold):
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
   def min(self):
      return 0
   def computeCore(self, data, threshold):
      return np.std(data.getScores(self._name))
   def name(self):
      return self._name
   def label(self, data):
      return "STD of forecasts (" + data.getUnits() + ")"
   @staticmethod
   def description():
      return "Standard deviation of forecast"

# Returns all PIT values
class Pit(Metric):
   def __init__(self, name="pit"):
      self._name = name
   def min(self):
      return 0
   def max(self):
      return 1
   def label(self, data):
      return "PIT"
   def compute(self, data, threshold):
      return data.getScores(self._name)
   def name(self):
      return "PIT"

class Rmse(Metric):
   def computeCore(self, data, threshold):
      [obs,fcst] = data.getScores(["obs", "fcst"])
      return np.mean((obs - fcst)**2)**0.5
   def min(self):
      return 0
   def name(self):
      return "RMSE"
   @staticmethod
   def description():
      return "Root mean squared error"

class Cmae(Metric):
   def computeCore(self, data, threshold):
      [obs,fcst] = data.getScores(["obs", "fcst"])
      return (np.mean(abs(obs**3 - fcst**3)))**(1.0/3)
   def min(self):
      return 0
   def name(self):
      return "CMAE"
   @staticmethod
   def description():
      return "Cube-root mean absolute cubic error"

class Dmb(Metric):
   def computeCore(self, data, threshold):
      [obs,fcst] = data.getScores(["obs", "fcst"])
      return np.mean(obs)/np.mean(fcst)
   def name(self):
      return "Degree of mass balance (obs/fcst)"
   @staticmethod
   def description():
      return "Degree of mass balance (obs/fcst)"

class Num(Metric):
   def computeCore(self, data, threshold):
      [fcst] = data.getScores(["fcst"])
      return len(fcst)
   def name(self):
      return "Number of valid forecasts"
   @staticmethod
   def description():
      return "Number of valid forecasts"

class Corr(Metric):
   def computeCore(self, data, threshold):
      [obs,fcst]  = data.getScores(["obs", "fcst"])
      return np.corrcoef(obs,fcst)[1,0]
   def max(self):
      return 1
   def min(self):
      return 0
   def name(self):
      return "Correlation"
   @staticmethod
   def description():
      return "Correlation between obesrvations and forecasts"

class Threshold(Metric):
   @staticmethod
   def supportsX():
      return True
   @staticmethod
   def requiresThresholds():
      return True

class Within(Threshold):
   def computeCore(self, data, threshold):
      [obs,fcst]  = data.getScores(["obs", "fcst"])
      diff = abs(obs - fcst)
      return np.mean(diff <= threshold)
   def min(self):
      return 0
   def max(self):
      return 1
   def name(self):
      return "Within"
   @staticmethod
   def description():
      return "The percentage of forecasts within some error bound (use -r)"

class Brier(Threshold):
   def computeCore(self, data, threshold):
      var = data.getPvar(threshold)
      [obs,p]  = data.getScores(["obs",var])
      I = np.where(obs < threshold)
      p[I] = 1 - p[I]
      return np.mean(p)
   def min(self):
      return 0
   def max(self):
      return 1
   def name(self):
      return "Brier score"
   @staticmethod
   def description():
      return "Brier score"

class Contingency(Threshold):
   @staticmethod
   def getAxisFormatter(self, data):
      from matplotlib.ticker import ScalarFormatter
      return ScalarFormatter()
   @staticmethod
   def requiresThresholds():
      return True
   def label(self, data):
      return self.name()
   def min(self):
      return 0
   def max(self):
      return 1
   def computeCore(self, data, threshold):
      if(threshold == None):
         Common.error("Metric " + self.getClassName() + " requires '-r <threshold>'")
      [obs,fcst] = data.getScores(["obs", "fcst"])
      #data.setAxis("none")
      #data.setIndex(0)
      value = np.nan
      if(len(fcst) > 0):
         # Compute frequencies
         a    = np.ma.sum((fcst >= threshold) & (obs >= threshold))
         b    = np.ma.sum((fcst >= threshold) & (obs <  threshold))
         c    = np.ma.sum((fcst <  threshold) & (obs >= threshold))
         d    = np.ma.sum((fcst <  threshold) & (obs <  threshold))
         value = self.calc(a, b, c, d)
         if(np.isinf(value)):
            value = np.nan
      else:
         self.warning(data.getFilename() + " does not have any valid forecasts")

      return value
   def name(self):
      return self.description()

class Ets(Contingency):
   def calc(self, a, b, c, d):
      N = a + b + c + d
      ar   = (a + b) / 1.0 / N * (a + c)
      return (a - ar) / 1.0 / (a + b + c - ar)
   def name(self):
      return "ETS"
   @staticmethod
   def description():
      return "Equitable threat score"

class Threat(Contingency):
   def calc(self, a, b, c, d):
      return a / 1.0 / (a + b + c)
   @staticmethod
   def description():
      return "Threat score"

class BiasFreq(Contingency):
   def calc(self, a, b, c, d):
      return 1.0 * (a + b) / (a + c)
   def max(self):
      return None
   def name(self):
      return "Bias frequency"
   @staticmethod
   def description():
      return "Bias frequency (fcst >= threshold) / (obs >= threshold)"

class BaseRate(Contingency):
   def calc(self, a, b, c, d):
      return (a + c) / 1.0 / (a + b + c + d)
   @staticmethod
   def description():
      return "Base rate"

class OddsRatioSS(Contingency):
   def calc(self, a, b, c, d):
      return (a * d - b * c) / 1.0 / (a * d + b * c)
   @staticmethod
   def description():
      return "Odds ratio skill score"

class HanssenKuiper(Contingency):
   def calc(self, a, b, c, d):
      return (a*d-b*c)* 1.0 / ((a + c)*(b + d))
   @staticmethod
   def description():
      return "Hanssen Kuiper score"

class HitRate(Contingency):
   def calc(self, a, b, c, d):
      return a / 1.0 / (a + c)
   @staticmethod
   def description():
      return "Hit rate"

class FalseAlarm(Contingency):
   def calc(self, a, b, c, d):
      return b / 1.0 / (b + d)
   @staticmethod
   def description():
      return "False alarm rate"

# Dunny class to provide description
class Reliability(Metric):
   def computeCore(self, data):
      return None
   def name(self):
      return "Reliability"
   @staticmethod
   def description():
      return "Reliability diagram for a certain threshold (-r)"

