class Metric:
   def compute(self, data):
      x = self.getX(data)
      maxInd = data.getMaxIndex()
      scores = np.zeros(maxInd+1, 'float')
      for i in range(0,maxInd):
         data.setIndex(i)
         scores[i] = self.computeCore(data)
      return scores
   def xlabel(self, data):
      return data.xlabel()
   def units(self, data):
      return data.units()
   def ylabel(self, data):
      return self.name() + " (" + self.units() + ")"

class MetricPit(Metric):
   def getX(self, data):
      return np.linspace(0.1,0.9,9)
   def compute(self, data):
      # Compute PIT histogram
      pass
   def xlabel(self, data):
      return "PIT"
   def ylabel(self, data):
      return "PIT"

class MetricRmse(Metric):
   def getX(self, data):
      return data.getX()
   def computeCore(self, data):
      obs = data.getScores("obs")
      fcst = data.getScores("fcst")
      return np.mean((obs - fcst)**2)**0.5
   def name(self):
      return "Root mean squared error"

class MetricCorr(Metric):
   def compute(self, data):
      obs = data.getScores("obs")
      fcst = data.getScores("fcst")
      return np.mean((obs - fcst)**2)**0.5
   def name(self):
      return "Correlation"

class MetricThreshold(Metric):
   def __init__(self, thresholds):
      self._thresholds = self._thresholds
   def getX(self, data):
      return self._thresholds
   def compute(self, data):
      obs = data.getScores("obs")
      fcst = data.getScores("fcst")
      scores = np.zeros(len(obs), 'int')
      for i in range(0, len(self._thresholds)):
         threshold = self._thresholds[i]
         a = np.sum(obs > threshold & fcst > threshold)
         scores[i] = computeCore(a, b, c, d)
      return scores
   def xlabel(self):
      return self._thresholds

class MetricThresholdEts(MetricThreshold):
   def computeCore(a, b, c, d):
      return a + b + c + d
