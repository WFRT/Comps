from VerifPlot import *
class DefaultPlot(Plot):
   def __init__(self, metric):
      Plot.__init__(self)
      self.metric = metric

   def getName(self):
      return self.metric
      
   def computeCore(self, files):
      NF = len(files)
      y = np.zeros([len(files[0].getX()), NF], 'float')
      for nf in range(0,NF):
         file = files[nf]
         y[:,nf] = file.getY(self.metric)
      return y

   def getMetric(self):
      return self.metric

class StdErrorPlot(Plot):
   @staticmethod
   def description():
      return "Plots the standard error of the forecasts"
   def computeCore(self, ax):
      NF = len(self.files)
      N  = self.files[0].getLength()
      y = np.zeros([N, NF], 'float')
      for nf in range(0,NF):
         file = self.files[nf]
         offsets = file.getOffsets()
         lineColor = self.getColor(nf, NF)
         lineStyle = self.getStyle(nf, NF)
         bias = file.getScores("bias")
         values  = np.zeros([len(bias[0,:]),1], 'float')
         mbias = np.ma.masked_array(bias,np.isnan(bias))

         dim = file.getByAxis()
         if(dim == 0):
            for i in range(0, N):
               y[i,nf] = np.std(mbias[i,:,:].flatten())
         elif(dim == 1):
            for i in range(0, N):
               y[i,nf] = np.std(mbias[:,i,:].flatten())
         elif(dim == 2):
            for i in range(0, N):
               y[i,nf] = np.std(mbias[:,:,i].flatten())
      return y
   def getMetric(self):
      return "Standard error"

class NumPlot(Plot):
   @staticmethod
   def description():
      return "Plots the number of valid observations and forecasts"
   def getYLabel(self):
      return "Number of valid data"
   def computeCore(self, files):
      NF = len(files)
      N  = self.files[0].getLength()
      y = np.zeros([N, NF], 'float')
      for nf in range(0,NF):
         file = self.files[nf]
         lineColor = self.getColor(nf, NF)
         lineStyle = self.getStyle(nf, NF)
         offsets = file.getOffsets()
         obs  = file.getScores('obs')
         fcst = file.getScores('fcst')

         mobs = np.ma.masked_array(obs,np.isnan(obs))
         mfcst = np.ma.masked_array(fcst,np.isnan(fcst))

         dim = file.getByAxis()
         if(dim == 0):
            for i in range(0, N):
               y[i,nf] = mfcst[i,:,:].count()
         elif(dim == 1):
            for i in range(0, N):
               y[i,nf] = mfcst[:,i,:].count()
         elif(dim == 2):
            for i in range(0, N):
               y[i,nf] = mfcst[:,:,i].count()
      return y

class RmsePlot(Plot):
   @staticmethod
   def description():
      return "Plots the root mean squared error of the forecasts"
   def __init__(self, metric=None):
      Plot.__init__(self)
      self.metric = metric

   def getMetric(self):
      return "RMSE"

   def computeCore(self, ax):
      NF = len(self.files)
      N  = self.files[0].getLength()
      y = np.zeros([N, NF], 'float')
      for nf in range(0,NF):
         file = self.files[nf]
         if(self.metric != None):
            metric = self.metric
         elif(file.hasScore("mae")):
            metric = "mae"
         elif(file.hasScore("bias")):
            metric = "bias"

         bias = file.getScores("bias")
         values  = np.zeros([len(bias[0,:]),1], 'float')
         mbias = np.ma.masked_array(bias,np.isnan(bias))

         dim = file.getByAxis()
         if(dim == 0):
            for i in range(0, N):
               y[i,nf] = np.sqrt(np.mean((mbias[i,:,:]**2).flatten()))
         elif(dim == 1):
            for i in range(0, N):
               y[i,nf] = np.sqrt(np.mean((mbias[:,i,:]**2).flatten()))
         elif(dim == 2):
            for i in range(0, N):
               y[i,nf] = np.sqrt(np.mean((mbias[:,:,i]**2).flatten()))
      return y

class CorrPlot(Plot):
   @staticmethod
   def description():
      return "Plots the correlation between observations and forecasts. Accept -c."
   def getYLabel(self):
      return "Correlation"
   def computeCore(self, ax):
      NF = len(self.files)
      N  = self.files[0].getLength()
      y = np.zeros([N, NF], 'float')
      for nf in range(0,NF):
         file = self.files[nf]
         obs   = file.getScores("obs")
         fcst  = file.getScores("fcst")
         mobs  = np.ma.masked_array(obs,np.isnan(obs))
         mfcst = np.ma.masked_array(fcst,np.isnan(fcst))

         dim = file.getByAxis()
         if(dim == 0):
            for i in range(0, N):
               y[i,nf] = np.ma.corrcoef(mobs[i,:,:].flatten(), mfcst[i,:,:].flatten())[1,0]
         elif(dim == 1):
            for i in range(0, N):
               y[i,nf] = np.ma.corrcoef(mobs[:,i,:].flatten(), mfcst[:,i,:].flatten())[1,0]
         elif(dim == 2):
            for i in range(0, N):
               y[i,nf] = np.ma.corrcoef(mobs[:,:,i].flatten(), mfcst[:,:,i].flatten())[1,0]
      return y

   def getYLim(self):
      return [0,1]

class WithinPlot(Plot):
   @staticmethod
   def description():
      return "Plots the percentage of forecasts within some error bound (use -r)"
   def __init__(self, threshold=None):
      Plot.__init__(self)
      if(threshold != None):
         if(len(threshold) > 1):
            self.error("Within plot cannot take multiple thresholds")
         self.threshold = threshold[0]
      else:
         self.threshold = threshold
   def computeCore(self, ax):
      NF = len(self.files)
      N  = self.files[0].getLength()
      y = np.zeros([N, NF], 'float')
      n = np.zeros([N, NF], 'float')
      for nf in range(0,NF):
         file = self.files[nf]
         offsets = file.getOffsets()
         lineColor = self.getColor(nf, NF)
         lineStyle = self.getStyle(nf, NF)

         obs = file.getScores("obs")
         fcst = file.getScores("fcst")
         values  = np.zeros([len(obs[0,:]),1], 'float')
         mobs = np.ma.masked_array(obs,np.isnan(obs))
         mfcst = np.ma.masked_array(fcst,np.isnan(fcst))
         mdiff = abs(mobs - mfcst)

         dim = file.getByAxis()
         if(dim == 0):
            for i in range(0, N):
               y[i,nf] = np.mean(mdiff[i,:,:].flatten() < self.threshold)
               n[i,nf] = np.ma.count(mdiff[i,:,:].flatten())
         elif(dim == 1):
            for i in range(0, N):
               y[i,nf] = np.mean(mdiff[:,i,:].flatten() < self.threshold)
               n[i,nf] = np.ma.count(mdiff[:,i,:].flatten())
         elif(dim == 2):
            for i in range(0, N):
               y[i,nf] = np.mean(mdiff[:,:,i].flatten() < self.threshold)
               n[i,nf] = np.ma.count(mdiff[:,:,i].flatten())
      return y

class VariabilityPlot(Plot):
   @staticmethod
   def description():
      return "Plots the standard deviation of the forecasts"

   def computeCore(self, ax):
      NF = len(self.files)
      N  = self.files[0].getLength()
      y = np.zeros([N, NF], 'float')
      for nf in range(0,NF):
         file = self.files[nf]

         fcst = file.getScores("fcst")
         mar = np.ma.masked_array(fcst,np.isnan(fcst))
         dim = file.getByAxis()
         if(dim == 0):
            for i in range(0, N):
               y[i,nf] = np.ma.std(mar[i,:,:]).flatten()
         elif(dim == 1):
            for i in range(0, N):
               y[i,nf] = np.ma.std(mar[:,i,:]).flatten()
         elif(dim == 2):
            for i in range(0, N):
               y[i,nf] = np.ma.std(mar[:,:,i]).flatten()
      return y
