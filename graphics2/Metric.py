import numpy as np
import Common
import sys
import inspect
def getAllMetrics():
   temp = inspect.getmembers(sys.modules[__name__], inspect.isclass)
   return temp

# Computes scores for each xaxis value
class Metric:
   # Compute the score
   # data: use getScores([metric1, metric2...]) to get data
   #       data has already been configured to only retrieve data along a certain dimension
   # tRange: [lowerThreshold, upperThreshold]
   def compute(self, data, tRange):
      #assert(isinstance(tRange, list))
      #assert(len(tRange) == 2)
      size   = data.getAxisSize()
      scores = np.zeros(size, 'float')
      # Loop over x-axis
      for i in range(0,size):
         data.setIndex(i)
         scores[i] = self.computeCore(data, tRange)
      return scores

   # Implement these
   def computeCore(self, data, tRange):
      Common.error("Metric '" + self.getClassName() + "' has not been implemented yet")

   @staticmethod
   def requiresThresholds():
      return False
   def getClassName(self):
      name = self.__class__.__name__
      return name
   # Minimum value the metric can take on
   def min(self):
      return None
   # Maximum value the metric can take on
   def max(self):
      return None
   def label(self, data):
      return self.name() + " (" + data.getUnits() + ")"
   def name(self):
      return self.getClassName()
   @staticmethod
   def description():
      return ""
   # Does it make sense to use '-x threshold' with this metric?
   @staticmethod
   def supportsThreshold():
      return False
   # Does it make sense to use '-x' with this metric?
   @staticmethod
   def supportsX():
      return False

class Mean(Metric):
   def __init__(self, name):
      self._name = name
   def computeCore(self, data, tRange):
      return np.mean(data.getScores(self._name))
   def name(self):
      return self._name

class Mae(Metric):
   def computeCore(self, data, tRange):
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
   def computeCore(self, data, tRange):
      [obs, fcst] = data.getScores(["obs", "fcst"])
      return np.mean(obs - fcst)
   @staticmethod
   def description():
      return "Bias"

class Extreme(Metric):
   def calc(self, data, func, variable):
      [value] = data.getScores([variable])
      if(len(value) == 0):
         return np.nan
      return func(value)

class MaxObs(Extreme):
   def computeCore(self, data, tRange):
      return self.calc(data, np.max, "obs")
   @staticmethod
   def description():
      return "Maximum observed value"

class MinObs(Extreme):
   def computeCore(self, data, tRange):
      return self.calc(data, np.min, "obs")
   @staticmethod
   def description():
      return "Minimum observed value"

class MaxFcst(Extreme):
   def computeCore(self, data, tRange):
      return self.calc(data, np.max, "fcst")
   @staticmethod
   def description():
      return "Maximum forecasted value"

class MinFcst(Extreme):
   def computeCore(self, data, tRange):
      return self.calc(data, np.min, "fcst")
   @staticmethod
   def description():
      return "Minimum forecasted value"

class StdError(Metric):
   def computeCore(self, data, tRange):
      [obs, fcst] = data.getScores(["obs", "fcst"])
      bias = np.mean(obs - fcst)
      return np.mean((obs - fcst - bias)**2)**0.5
   def name(self):
      return "Standard error"
   def min(self):
      return 0
   @staticmethod
   def description():
      return "Standard error (i.e. RMSE if forecast had no bias)"

class Std(Metric):
   def min(self):
      return 0
   def computeCore(self, data, tRange):
      return np.std(data.getScores("fcst"))
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
   def compute(self, data, tRange):
      return data.getScores(self._name)
   def name(self):
      return "PIT"

class Rmse(Metric):
   def computeCore(self, data, tRange):
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
   def computeCore(self, data, tRange):
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
   def computeCore(self, data, tRange):
      [obs,fcst] = data.getScores(["obs", "fcst"])
      return np.mean(obs)/np.mean(fcst)
   def name(self):
      return "Degree of mass balance (obs/fcst)"
   @staticmethod
   def description():
      return "Degree of mass balance (obs/fcst)"

class Num(Metric):
   def computeCore(self, data, tRange):
      [fcst] = data.getScores(["fcst"])
      return len(fcst)
   def name(self):
      return "Number of valid forecasts"
   @staticmethod
   def description():
      return "Number of valid forecasts"

class Corr(Metric):
   def computeCore(self, data, tRange):
      [obs,fcst]  = data.getScores(["obs", "fcst"])
      if(len(obs) == 0):
         return np.nan
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

# Metrics based on 2x2 contingency table for a given threshold
class Threshold(Metric):
   @staticmethod
   def supportsThreshold():
      return True
   @staticmethod
   def supportsX():
      return True
   @staticmethod
   def requiresThresholds():
      return True
   @staticmethod
   def within(x, range):
      return (x >= range[0]) & (x <= range[1])

class Within(Threshold):
   def computeCore(self, data, tRange):
      [obs,fcst]  = data.getScores(["obs", "fcst"])
      diff = abs(obs - fcst)
      return np.mean(self.within(diff, tRange))*100
   def min(self):
      return 0
   def max(self):
      return 100
   def name(self):
      return "Within"
   def label(self, data):
      return "% of forecasts"
   @staticmethod
   def description():
      return "The percentage of forecasts within some error bound (use -r)"

class Conditional(Threshold):
   def __init__(self, x="obs", y="fcst"):
      self._x = x
      self._y = y
   def computeCore(self, data, tRange):
      [obs,fcst]  = data.getScores([self._x, self._y])
      I = np.where(self.within(obs, tRange))[0]
      return np.mean(fcst[I])

class Count(Threshold):
   def __init__(self, x):
      self._x = x
   def computeCore(self, data, tRange):
      values  = data.getScores(self._x)
      I = np.where(self.within(values, tRange))[0]
      return len(I)

class Brier(Threshold):
   def computeCore(self, data, tRange):
      p0 = 0
      p1 = 1
      if(tRange[0] != -np.inf and tRange[1] != np.inf):
         var0 = data.getPvar(tRange[0])
         var1 = data.getPvar(tRange[1])
         [obs, p0, p1] = data.getScores(["obs", var0, var1])
      elif(tRange[0] != -np.inf):
         var0 = data.getPvar(tRange[0])
         [obs, p0] = data.getScores(["obs", var0])
      elif(tRange[1] != np.inf):
         var1 = data.getPvar(tRange[1])
         [obs, p1] = data.getScores(["obs", var1])
      
      obsP = self.within(obs, tRange)
      p    = p1 - p0 # Prob of obs within range
      bs   = (obsP - p)**2
      return np.mean(bs)
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
   def computeCore(self, data, tRange):
      if(tRange == None):
         Common.error("Metric " + self.getClassName() + " requires '-r <threshold>'")
      [obs,fcst] = data.getScores(["obs", "fcst"])
      value = np.nan
      if(len(fcst) > 0):
         # Compute frequencies
         a    = np.ma.sum((self.within(fcst,tRange)) & (self.within(obs, tRange)))
         b    = np.ma.sum((self.within(fcst,tRange)) & (self.within(obs, tRange)==0))
         c    = np.ma.sum((self.within(fcst,tRange)==0) & (self.within(obs, tRange)))
         d    = np.ma.sum((self.within(fcst,tRange)==0) & (self.within(obs, tRange)==0))
         value = self.calc(a, b, c, d)
         if(np.isinf(value)):
            value = np.nan

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
   @staticmethod
   def description():
      return "Bias frequency (number of fcsts / number of obs)"

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

class Hit(Contingency):
   def calc(self, a, b, c, d):
      return a / 1.0 / (a + c)
   @staticmethod
   def description():
      return "Hit rate"

class Miss(Contingency):
   def calc(self, a, b, c, d):
      return c / 1.0 / (a + c)
   @staticmethod
   def description():
      return "Miss rate"

# Fraction of non-events that are forecasted as events
class FalseAlarm(Contingency):
   def calc(self, a, b, c, d):
      return b / 1.0 / (b + d)
   @staticmethod
   def description():
      return "False alarm rate"

# Fraction of forecasted events that are false alarms
class FalseAlarmRatio(Contingency):
   def calc(self, a, b, c, d):
      return b / 1.0 / (a + b)
   @staticmethod
   def description():
      return "False alarm ratio"
