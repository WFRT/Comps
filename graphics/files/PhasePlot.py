from VerifPlot import *
class PhasePlot(Plot):
   @staticmethod
   def description():
      return "Verifies precip phase"
   def __init__(self):
      Plot.__init__(self)
   def plotCore(self, ax):
      NF = len(self.files)
      for nf in range(0,NF):
         mpl.subplot(1,NF,nf+1)
         file = self.files[nf]

         [fcst,obs] = file.getFlatScores(["fcst", "obs"])
         fcst = np.round(fcst)
         obs  = np.round(obs)

         data = np.zeros([4,4], 'float')

         mpl.plot(fcst,obs)

         print file.getFilename()
         for j in range(0,4):
            for i in range(0,4):
               data[i,j] = np.ma.sum((fcst == i) & (obs == j))
               mpl.bar(range(0,4), data[i,:])
            print "Obs = " + str(j) + str(data[:,j]) + " %2.1f%%" % (data[j,j]/sum(data[range(1,4),j])*100)
         for i in range(0,4):
            print "FA = " + "%2.1f%%" % ((sum(data[i,range(1,4)]) - data[i,i])/sum(data[i,range(1,4)])*100)


         total = 0
         hit   = 0
         # Loop over observations
         for j in range(1,4):
            # Loop over forecasts
            for i in range(1,4):
               n = np.ma.sum((fcst == i) & (obs == j))
               total = total + n
               if(i == j):
                  hit = hit + n

         print hit, total, "%2.1f%%" % (hit * 1.0 / total * 100)
         color = "gray" #self.getColor(nf, NF)

         # X-axis
         #mpl.gca().set_xlim([0,1])
         mpl.gca().set_xlabel("Cumulative probability")

         # Y-axis
         if(nf == 0):
            mpl.gca().set_ylabel("Observed frequency")
         else:
            mpl.gca().set_yticks([])

   def legend(self, ax, names=None):
      if(names == None):
         names = list()
         for i in range(0, len(self.files)):
            names.append(self.files[i].getFilename())
      NF = len(self.files)
      for nf in range(0,NF):
         mpl.subplot(1,NF,nf+1)
         mpl.title(names[nf])
