from VerifPlot import *
class PitPlot(Plot):
   @staticmethod
   def description():
      return "Plots a PIT histogram of the forecasts (analogous to a rank histogram, "\
             + "but for probabilities)"
   def __init__(self, threshold=None, numBins=10):
      Plot.__init__(self)
      self.threshold = threshold
      self.numBins = numBins
   def plotCore(self, ax):
      NF = len(self.files)
      width = 1.0 / self.numBins / NF
      width = 1.0 / self.numBins
      for nf in range(0,NF):
         mpl.subplot(1,NF,nf+1)
         file = self.files[nf]
         if(self.threshold == None):
            pits = file.getFlatScores("pit")
         else:
            [p,obs] = file.getFlatScores(["p11", "obs"])
            pits = np.array(p)
            I0 = np.where(obs <= 11)[0]
            I1 = np.where(obs > 11)[0]
            pits[I1] = 1-pits[I1]
            pits[I0] = pits[I0] * np.random.random(len(I0))
            pits[I1] = 1 - pits[I1] * np.random.random(len(I1))

         x = np.linspace(0,1,self.numBins+1)
         n = np.histogram(pits.flatten(), x)[0]
         n = n * 1.0 / sum(n)
         color = "gray" #self.getColor(nf, NF)
         xx = x[range(0,len(x)-1)]
         mpl.bar(xx, n, width=width, color=color)
         mpl.plot([0,1],[1.0/self.numBins, 1.0/self.numBins], 'k--')

         # X-axis
         mpl.gca().set_xlim([0,1])
         mpl.gca().set_xlabel("Cumulative probability")

         # Y-axis
         ytop = 2.0/self.numBins
         mpl.gca().set_ylim([0,ytop])
         if(nf == 0):
            mpl.gca().set_ylabel("Observed frequency")
         else:
            mpl.gca().set_yticks([])

         # Compute calibration deviation
         D  = np.sqrt(1.0 / self.numBins * np.sum((n - 1.0 / self.numBins)**2))
         D0 = np.sqrt((1.0 - 1.0 / self.numBins) / (len(pits) * self.numBins))
         mpl.text(0, ytop, "Dev: %2.4f\nExp: %2.4f" % (D,D0), verticalalignment="top")

   def legend(self, ax, names=None):
      if(names == None):
         names = list()
         for i in range(0, len(self.files)):
            names.append(self.files[i].getFilename())
      NF = len(self.files)
      for nf in range(0,NF):
         mpl.subplot(1,NF,nf+1)
         mpl.title(names[nf])

class ReliabilityPlot(Plot):
   @staticmethod
   def description():
      return "Plots a reliability diagram for a certain threshold (-r)"
   def __init__(self, threshold=None):
      Plot.__init__(self)
      if(threshold == None):
         self.error("Reliability plot needs a threshold (use -r)")
      if(len(threshold) > 1):
         self.error("Reliability plot cannot take multiple thresholds")
      self.threshold = threshold[0]
   def plotCore(self, ax):
      NF = len(self.files)
      for nf in range(0,NF):
         file = self.files[nf]

         N = 6
         edges = np.linspace(0,1,N+1)
         bins  = np.linspace(0.5/N,1-0.5/N,N)
         var = file.getPvar(self.threshold)
            
         p   = 1-file.getScores(var).flatten()
         obs = file.getScores('obs').flatten() > self.threshold
         # Remove points with missing forecasts and/or observations
         I   = np.where(np.logical_not(np.isnan(p)) & np.logical_not(np.isnan(obs)))
         p   = p[I]
         obs = obs[I]

         clim = np.mean(obs)
         # Compute frequencies
         y = np.nan*np.zeros([len(edges)-1,1],'float')
         n = np.zeros([len(edges)-1,1],'float')
         for i in range(0,len(edges)-1):
            q = (p >= edges[i])& (p < edges[i+1])
            I = np.where(q)
            n[i] = len(obs[I])
            # Need at least 10 data points to be valid
            if(n[i] >= 10):
               y[i] = np.mean(obs[I])
            bins[i] = np.mean(p[I])

         lineColor = self.getColor(nf, NF)
         lineStyle = self.getStyle(nf, NF)
         ax.plot(bins, y, lineStyle, color=lineColor)
         self.plotConfidence(ax, bins, y, n, color=lineColor)

         ax.plot([0,1], [0,1], color="k")
         #mpl.gca().yaxis.grid(False)
         
         ax.plot([0,1], [clim,clim], ":", color=lineColor)
         ax.plot([clim,clim], [0,1], ":", color=lineColor)
         ax.plot([0,1], [clim/2,1-(1-clim)/2], "--", color=lineColor)
         ax.axis([0,1,0,1])
         ax.set_xlabel("Cumulative probability")
         ax.set_ylabel("Observed frequency")
         units = " " + file.getUnits()
         ax.set_title("Threshold: " + str(self.threshold) + units)

         #ax2 = mpl.gcf().add_axes([0.2,0.7,0.15,0.15])
         #ax2.get_xaxis().set_visible(False)
         #ax2.set_xlim(1,N)
         #mpl.bar(range(1,N+1), n, width=1, log=True)

   def plotConfidence(self, ax, bins, y, n, color):
      z = 1.96 # 95% confidence interval
      type = "normal"
      style = "--"
      if type == "normal":
         mean = y
         lower = mean - z*np.sqrt(y*(1-y)/n)
         upper = mean + z*np.sqrt(y*(1-y)/n)
      elif type == "wilson":
         mean =  1/(1+1.0/n*z**2) * ( y + 0.5*z**2/n)
         upper = mean + 1/(1+1.0/n*z**2)*z*np.sqrt(y*(1-y)/n + 0.25*z**2/n**2)
         lower = mean - 1/(1+1.0/n*z**2)*z*np.sqrt(y*(1-y)/n + 0.25*z**2/n**2)
      ax.plot(bins, upper, style, color=color)
      ax.plot(bins, lower, style, color=color)
      self._fill(ax, bins, lower, upper, color, alpha=0.3)

class SpreadSkillPlot(Plot):
   @staticmethod
   def description():
      return "Plots forecast error (RMSE) as a function of forecast spread (STD)"
   def plotCore(self, ax):
      NF = len(self.files)
      for nf in range(0,NF):
         file = self.files[nf]
         metric = ""
         if(file.hasScore("ensSpread")):
            metric = "ensSpread"
         elif(file.hasScore("sharp")):
            metric = "sharp"
         else:
            self.error("No suitable ensemble spread metric found")
         [spread, skill] = file.getFlatScores([metric, 'mae'])

         N = 10
         edges = np.linspace(0,max(spread),N+1)
         bins  = np.zeros(N, 'float')

         style  = self.getStyle(nf, NF)
         color  = self.getColor(nf, NF)

         # Compute frequencies
         y = np.nan*np.zeros([len(edges)-1,1],'float')
         n = np.zeros([len(edges)-1,1],'float')
         for i in range(0,len(edges)-1):
            q = (spread >= edges[i])& (spread < edges[i+1])
            I = np.where(q)
            n[i] = len(skill[I])
            # Need at least 10 data points to be valid
            if(n[i] >= 5):
               y[i] = np.mean(skill[I])
            bins[i] = np.mean(spread[I])
         ax.plot(spread, skill, ".", color=color, alpha=0.3, zorder=-1)
         ax.plot(bins, y, style, color="black", lw=4, zorder=1)
         ax.plot(bins, y, style, color=color, ms=8, lw=3, zorder=1)
         ax.set_xlabel("Ensemble spread")
         ax.set_ylabel("Ensemble mean skill (MAE)")

class IgnDecompPlot(Plot):
   def __init__(self, threshold=None, numBins=10):
      Plot.__init__(self)
      self.numBins = numBins
      self.edges = np.linspace(0,1,self.numBins+1)

      if(threshold != None):
         if(len(threshold) > 1):
            self.error("IgnDecomp plot cannot take multiple thresholds")
         self.threshold = threshold[0]
      else:
         self.threshold = threshold
   @staticmethod
   def description():
      return "Plots the decomposition of the ignorance score. Accepts -r."

   # Compute the calibration deviation from a sequence of PIT values
   def getDeviation(self, ar):
      N = np.histogram(ar, self.edges)[0]
      f = N * 1.0 / sum(N)
      fmean = 1.0 / (len(self.edges)-1)
      return np.sum(f*np.log2(f / fmean))

   # What deviation is expected, due to sampling error, for a perfectly calibrated system?
   def getExpectedDeviation(self, N):
      if(self.numBins == 10):
         # Emperical fit
         return 7.0 / N
      else:
         return None

   def plotCore(self, ax):
      NF = len(self.files)

      for nf in range(0,NF):
         file = self.files[nf]
         style  = self.getStyle(nf, NF)
         color  = self.getColor(nf, NF)

         # Retrieve the ignorance score
         if(self.threshold == None):
            [pits, ign] = file.getFlatScores(['pit', 'ign'])
         else:
            # Compute ignorance for a threshold
            pvar = file.getPvar(self.threshold)
            [p,obs] = file.getFlatScores([pvar, "obs"])
            pits = np.array(p)
            I0 = np.where(obs <= 11)[0]
            I1 = np.where(obs > 11)[0]
            pits[I1] = 1-pits[I1]
            pits[I0] = pits[I0] * np.random.random(len(I0))
            pits[I1] = 1 - pits[I1] * np.random.random(len(I1))
            ign  = np.array(pits)
            ign = np.log2(ign)
         ign = np.mean(ign)
         dev = self.getDeviation(pits)
         ax.plot(dev, ign, "o", color=color, ms=8)

         # Draw a diagnoal line from the ignorance down to its potential ignorance
         exp   = self.getExpectedDeviation(len(pits))
         if(dev > exp):
            x = [exp,dev]
            y = [ign-dev+exp, ign]
            ax.plot(x, y, '-', color=color)
            ax.plot(x[0], y[0], "o", mfc="white", mec=color, color=color, ms=8)

         ax.set_xlabel("Calibration deviation")
         ax.set_ylabel("Ignorance")
         ax.set_aspect('equal')
      xlim = ax.get_xlim()
      ylim = ax.get_ylim()
      if(exp != None):
         ax.plot([exp,exp], ylim, "--")
         ax.text(exp, ylim[1], "Expected ", rotation=90, verticalalignment="top", horizontalalignment="left")

      # Draw diagonal lines
      for i in np.arange(int(ylim[0])-2,1 + int(ylim[1]), 0.1):
         ax.plot([0,1],[i,i+1], 'k:')
      ax.set_ylim(ylim)
      ax.set_xlim(xlim)

class BrierPlot(Plot):
   @staticmethod
   def description():
      return "Plots the brier score for a threshold"
   def __init__(self, threshold=None):
      Plot.__init__(self)
      if(threshold != None):
         if(len(threshold) > 1):
            self.error("Brier plot cannot take multiple thresholds")
         self.threshold = threshold[0]
      else:
         self.threshold = threshold

   def getY(self, file, p, obs):
      mp   = np.ma.masked_array(p,np.isnan(p*obs))
      mobs = np.ma.masked_array(obs,np.isnan(p*obs))
      dim = file.getByAxis()
      I = np.where(mobs < self.threshold)
      mp[I] = 1 - mp[I]
      if(dim == 0):
         N = len(p[:,0,0]) 
         y = np.zeros(N, 'float')
         for i in range(0, N):
            y[i] = np.mean(mp[i,:,:]).flatten()
      elif(dim == 1):
         N = len(p[0,:,0]) 
         y = np.zeros(N, 'float')
         for i in range(0, N):
            y[i] = np.mean(mp[:,i,:]).flatten()
      elif(dim == 2):
         N = len(p[0,0,:]) 
         y = np.zeros(N, 'float')
         for i in range(0, N):
            y[i] = np.mean(mp[:,:,i]).flatten()
      return y

   def plotCore(self, ax):
      NF = len(self.files)
      for nf in range(0,NF):
         file = self.files[nf]
         lineColor = self.getColor(nf, NF)
         lineStyle = self.getStyle(nf, NF)
         # Plot the variability of the forecast
         pvar = file.getPvar(self.threshold)
         p    = file.getScores(pvar)
         obs  = file.getScores("obs")
         x    = file.getX()
         y    = self.getY(file, p, obs)

         ax.plot(x, y, lineStyle, color=lineColor)
         ax.set_xlabel(file.getXLabel())
         ax.set_ylabel("Brier score")
         mpl.gca().xaxis.set_major_formatter(file.getXFormatter())
         ax.set_ylim([0,1])

