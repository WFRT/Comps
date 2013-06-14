from File import *
from Plot import *
import sys
import matplotlib.pyplot as mpl
ifile = sys.argv[1]
ofile = sys.argv[2]

file = NetCdfFile(ifile,location=0)
pl = CdfPlot(file)

pl.plot(mpl.gca())
mpl.savefig(ofile)
