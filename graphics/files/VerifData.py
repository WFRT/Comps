import numpy as np
# Wrapper on file to only return a subset of the data
class Data:
   def __init__(self, file, offset=None, location=None, dates=None):
      self.file = file
      self.offset = offset
      self.location = location
      allDates = self.file.getDates()
      if(dates == None):
         self.dateIndices = range(0,len(allDates))
      else:
         self.dateIndices = np.in1d(allDates, dates)

   def getDates(self):
      dates = self.file.getDates()
      return dates[self.dateIndices]

   def hasScore(self, metric):
      return self.file.hasScore()


   def getScores(self, metrics):
      data = self.file.getScores(metrics)
      data = data[self.dateIndices,:,:]
      if(self.location is not None and self.offset is not None):
         data = data[:,self.offset,self.location, None]
      elif(self.location is not None):
         data = data[:,:,self.location, None]
      elif(self.offset is not None):
         data = data[:,self.offset,:, None]
      return data

   def getFlatScores(self, metrics):
      if(not isinstance(metrics, list)):
         data = self.getScores(metrics)
         data = data.flatten()
         mask = np.where(np.isnan(data) == 0)
         data = data[mask]
         return data
      else:
         data = list()
         curr = self.getScores(metrics[0]).flatten()
         data.append(curr)
         I = np.where(np.isnan(data[0]) == 0)[0]
         for i in range(1, len(metrics)):
            curr = self.getScores(metrics[i]).flatten()
            data.append(curr)
            Icurr = np.where(np.isnan(curr) == 0)[0]
            I    = np.intersect1d(I, Icurr)
         for i in range(0, len(metrics)):
            data[i] = data[i][I]
         return data

   def getOffsets(self):
      #if(self.offset is not None):
      return self.file.getOffsets()

   def getFilename(self):
      return self.file.getFilename()

   def getUnits(self):
      return self.file.getUnits()
