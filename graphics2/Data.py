class Data:
   def __init__(self, file):
      self._axes = 0
      self._index = 0
      self._file = file
      self._findex = 0
   def getScores(self, metric):
      data = self._file.variables[metric]
      data = Common.clean(data)
      if(self._axis == 0):
         return data[self._index,:,:].flatten()
      elif(self._axis == 1):
         return data[:,self._index,:].flatten()
      elif(self._axis == 2):
         return data[:,:,self._index].flatten()
      else:
         return data.flatten()
   def setAxis(self, axes):
      if(axis == "date"):
         self._axis = + 0
      elif(axis == "offset"):
         self._axis = + 1
      elif(axis == "location"):
         self._axis = + 2
   def setIndex(self, index):
      self._index = index
   def setFileIndex(self, index):
      self._findex = index
   def getNumFiles(self):
      return len(self.files)
   # What values represent this axis?
   def getAxisValues(self):
      if(self._axis == 0):
         return
      elif(self._axis == 1):
         return
      elif(self._axis == 2):
         return
      else:
         return 0
