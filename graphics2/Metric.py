import numpy as np
import Common
import sys
import inspect
def getAllMetrics():
   temp = inspect.getmembers(sys.modules[__name__], inspect.isclass)
   return temp

# Computes scores for each xaxis value
class Metric:
   # Overload these variables
   _description  = ""    # Overwrite this to let the metric show up in the documentation of ./verif
   _min          = None  # Minimum value this metric can produce
   _max          = None  # Maximum value this mertic can produce
   _defaultAxis  = "offset" # If no axis is specified, use this axis as default
   _defaultBinType = None
   _reqThreshold = False # Does this metric require thresholds?
   _supThreshold = False # Does this metric support thresholds?
   _experimental = False # Is this metric not fully tested yet?

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

   # Implement this
   def computeCore(self, data, tRange):
      Common.error("Metric '" + self.getClassName() + "' has not been implemented yet")

   @classmethod
   def description(cls):
      extra = ""
      if(cls._experimental):
         extra = " " + Common.experimental()
      return cls._description + extra

   # Does this metric require thresholds in order to be computable?
   @classmethod
   def requiresThresholds(cls):
      return cls._reqThreshold

   # If this metric is to be plotted, along which axis should it be plotted by default?
   @classmethod
   def defaultAxis(cls):
      return cls._defaultAxis

   @classmethod
   def defaultBinType(cls):
      return cls._defaultBinType

   # Does it make sense to use '-x threshold' with this metric?
   @classmethod
   def supportsThreshold(cls):
      return cls._supThreshold

   # Minimum value the metric can take on
   @classmethod
   def min(cls):
      return cls._min

   # Maximum value the metric can take on
   @classmethod
   def max(cls):
      return cls._max

   def getClassName(self):
      name = self.__class__.__name__
      return name
   def label(self, data):
      return self.name() + " (" + data.getUnits() + ")"
   def name(self):
      return self.getClassName()

class Mean(Metric):
   def __init__(self, name):
      self._name = name
   def computeCore(self, data, tRange):
      return np.mean(data.getScores(self._name))
   def name(self):
      return "Mean of " + self._name

class Mae(Metric):
   _min = 0
   _description = "Mean absolute error"
   def computeCore(self, data, tRange):
      [obs, fcst] = data.getScores(["obs", "fcst"])
      return np.mean(abs(obs - fcst))
   def name(self):
      return "MAE"

class Bias(Metric):
   _description = "Bias"
   def computeCore(self, data, tRange):
      [obs, fcst] = data.getScores(["obs", "fcst"])
      return np.mean(obs - fcst)

class Extreme(Metric):
   def calc(self, data, func, variable):
      [value] = data.getScores([variable])
      if(len(value) == 0):
         return np.nan
      return func(value)

class MaxObs(Extreme):
   _description = "Maximum observed value"
   def computeCore(self, data, tRange):
      return self.calc(data, np.max, "obs")

class MinObs(Extreme):
   _description = "Minimum observed value"
   def computeCore(self, data, tRange):
      return self.calc(data, np.min, "obs")

class MaxFcst(Extreme):
   _description = "Maximum forecasted value"
   def computeCore(self, data, tRange):
      return self.calc(data, np.max, "fcst")

class MinFcst(Extreme):
   _description = "Minimum forecasted value"
   def computeCore(self, data, tRange):
      return self.calc(data, np.min, "fcst")

class StdError(Metric):
   _min = 0
   _description = "Standard error (i.e. RMSE if forecast had no bias)"
   def computeCore(self, data, tRange):
      [obs, fcst] = data.getScores(["obs", "fcst"])
      bias = np.mean(obs - fcst)
      return np.mean((obs - fcst - bias)**2)**0.5
   def name(self):
      return "Standard error"

class Std(Metric):
   _min = 0
   _description = "Standard deviation of forecast"
   def computeCore(self, data, tRange):
      return np.std(data.getScores("fcst"))
   def label(self, data):
      return "STD of forecasts (" + data.getUnits() + ")"

# Returns all PIT values
class Pit(Metric):
   _min = 0
   _max = 1
   def __init__(self, name="pit"):
      self._name = name
   def label(self, data):
      return "PIT"
   def compute(self, data, tRange):
      return data.getScores(self._name)
   def name(self):
      return "PIT"

class Rmse(Metric):
   _min = 0
   _description = "Root mean squared error"
   def computeCore(self, data, tRange):
      [obs,fcst] = data.getScores(["obs", "fcst"])
      return np.mean((obs - fcst)**2)**0.5
   def name(self):
      return "RMSE"

class Cmae(Metric):
   _min = 0
   _description = "Cube-root mean absolute cubic error"
   def computeCore(self, data, tRange):
      [obs,fcst] = data.getScores(["obs", "fcst"])
      return (np.mean(abs(obs**3 - fcst**3)))**(1.0/3)
   def name(self):
      return "CMAE"

class Dmb(Metric):
   _description = "Degree of mass balance (obs/fcst)"
   def computeCore(self, data, tRange):
      [obs,fcst] = data.getScores(["obs", "fcst"])
      return np.mean(obs)/np.mean(fcst)
   def name(self):
      return "Degree of mass balance (obs/fcst)"

class Num(Metric):
   _description = "Number of valid forecasts"
   def computeCore(self, data, tRange):
      [fcst] = data.getScores(["fcst"])
      return len(fcst)
   def name(self):
      return "Number of valid forecasts"

class Corr(Metric):
   _min = 0 # Technically -1, but values below 0 are not as interesting
   _max = 1
   _description = "Correlation between obesrvations and forecasts"
   def computeCore(self, data, tRange):
      [obs,fcst]  = data.getScores(["obs", "fcst"])
      if(len(obs) == 0):
         return np.nan
      return np.corrcoef(obs,fcst)[1,0]
   def name(self):
      return "Correlation"

# Metrics based on 2x2 contingency table for a given threshold
class Threshold(Metric):
   _reqThreshold = True
   _supThreshold = True
   @staticmethod
   def within(x, range):
      return (x >= range[0]) & (x <= range[1])

class Within(Threshold):
   _min = 0
   _max = 100
   _description = "The percentage of forecasts within some error bound (use -r)"
   _defaultBinType = "below"
   def computeCore(self, data, tRange):
      [obs,fcst]  = data.getScores(["obs", "fcst"])
      diff = abs(obs - fcst)
      return np.mean(self.within(diff, tRange))*100
   def name(self):
      return "Within"
   def label(self, data):
      return "% of forecasts"

# Mean y conditioned on x
# For a given range of x-values, what is the average y-value?
class Conditional(Threshold):
   def __init__(self, x="obs", y="fcst"):
      self._x = x
      self._y = y
   def computeCore(self, data, tRange):
      [obs,fcst]  = data.getScores([self._x, self._y])
      I = np.where(self.within(obs, tRange))[0]
      return np.mean(fcst[I])

# Mean x when conditioned on x
# Average x-value that is within a given range. The reason the y-variable is added
# is to ensure that the same data is used for this metric as for the Conditional metric.
class XConditional(Threshold):
   def __init__(self, x="obs", y="fcst"):
      self._x = x
      self._y = y
   def computeCore(self, data, tRange):
      [obs,fcst]  = data.getScores([self._x, self._y])
      I = np.where(self.within(obs, tRange))[0]
      return np.mean(obs[I])

class Count(Threshold):
   def __init__(self, x):
      self._x = x
   def computeCore(self, data, tRange):
      values  = data.getScores(self._x)
      I = np.where(self.within(values, tRange))[0]
      return len(I)

class Brier(Threshold):
   _min = 0
   _max = 1
   _description = "Brier score"
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
   def name(self):
      return "Brier score"

class Contingency(Threshold):
   _min = 0
   _max = 1
   _defaultAxis = "threshold"
   _reqThreshold = True
   @staticmethod
   def getAxisFormatter(self, data):
      from matplotlib.ticker import ScalarFormatter
      return ScalarFormatter()
   def label(self, data):
      return self.name()
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
   _description = "Equitable threat score"
   def calc(self, a, b, c, d):
      N = a + b + c + d
      ar   = (a + b) / 1.0 / N * (a + c)
      if(a + b + c - ar == 0):
         return np.nan
      return (a - ar) / 1.0 / (a + b + c - ar)
   def name(self):
      return "ETS"

class Threat(Contingency):
   _description = "Threat score"
   def calc(self, a, b, c, d):
      if(a + b + c == 0):
         return np.nan
      return a / 1.0 / (a + b + c)

class BiasFreq(Contingency):
   _max = None
   _description = "Bias frequency (number of fcsts / number of obs)"
   def calc(self, a, b, c, d):
      if(a + c == 0):
         return np.nan
      return 1.0 * (a + b) / (a + c)

class BaseRate(Contingency):
   _description = "Base rate"
   def calc(self, a, b, c, d):
      if(a + b + c + d == 0):
         return np.nan
      return (a + c) / 1.0 / (a + b + c + d)

class OddsRatioSS(Contingency):
   _description = "Odds ratio skill score"
   def calc(self, a, b, c, d):
      if(a * d + b * c == 0):
         return np.nan
      return (a * d - b * c) / 1.0 / (a * d + b * c)

class HanssenKuiper(Contingency):
   _description = "Hanssen Kuiper score"
   def calc(self, a, b, c, d):
      if((a + c)*(b + d) == 0):
         return np.nan
      return (a*d-b*c)* 1.0 / ((a + c)*(b + d))

class Hit(Contingency):
   _description = "Hit rate"
   def calc(self, a, b, c, d):
      if(a + c == 0):
         return np.nan
      return a / 1.0 / (a + c)

class Miss(Contingency):
   _description = "Miss rate"
   def calc(self, a, b, c, d):
      if(a + c == 0):
         return np.nan
      return c / 1.0 / (a + c)

# Fraction of non-events that are forecasted as events
class FalseAlarm(Contingency):
   _description = "False alarm rate"
   def calc(self, a, b, c, d):
      if(b+d == 0):
         return np.nan
      return b / 1.0 / (b + d)

# Fraction of forecasted events that are false alarms
class FalseAlarmRatio(Contingency):
   _description = "False alarm ratio"
   def calc(self, a, b, c, d):
      if(a + b == 0):
         return np.nan
      return b / 1.0 / (a + b)
