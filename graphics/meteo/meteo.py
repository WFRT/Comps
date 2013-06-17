from File import *
from Plot import *
import sys
import matplotlib.pyplot as mpl
ifile = sys.argv[1]
ofile = ""
if(len(sys.argv) == 3):
   ofile = sys.argv[2]

file = NetCdfFile(ifile,location=0)
pl = CdfPlot(file)

pl.plot(mpl.gca())

if(ofile != ""):
   mpl.savefig(ofile)
else:
   mpl.show()
