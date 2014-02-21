#from mpl_toolkits.basemap import Basemap
from mpl_toolkits.mplot3d import Axes3D
import numpy as np
import sys
import matplotlib.pyplot as mpl
from scipy.io import netcdf
from matplotlib.mlab import griddata
from copy import deepcopy

def clean(data):
   data = data[:].astype(float)
   q = deepcopy(data)
   mask = np.where(q == -999);
   q[mask] = None
   mask = np.where(q < -100000);
   q[mask] = None
   mask = np.where(q > 1e30);
   q[mask] = None
   return q


filename = sys.argv[1]
offsets = -1
if(len(sys.argv) > 2):
   offsets = [int(sys.argv[2])]

f = netcdf.netcdf_file(filename, 'r')
lats = clean(f.variables['Lat'][:])
lons = clean(f.variables['Lon'][:])

dets = clean(f.variables['Det'][:,:])
N = len(dets)
if(offsets == -1):
   offsets = range(0,N)
N = len(offsets)

if(N > 6):
   X = np.ceil(np.sqrt(N)*1.3/2)*2
   Y = np.ceil(N / X)
else:
   X = N
   Y = 1
for offset in offsets:
   mpl.subplot(Y,X,offset+1)
   #fig = mpl.figure()
   #ax = fig.gca(projection='3d')

   xi = np.linspace(min(lons), max(lons))
   yi = np.linspace(min(lats), max(lats))
   zi = griddata(lons, lats, dets[offset,], xi, yi)
   mpl.contourf(xi, yi, zi, 50)
   mpl.colorbar()
   mpl.plot(lons, lats, '.')
   #ax.plot(lons, lats,dets[offsets,], '.')
#mpl.subplots_adjust(left=0.02, right=1, bottom=0.02, top=1)
mpl.show()
