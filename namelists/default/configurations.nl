# Analogs
analog      selector=an      downscaler=nearest averager=mean  continuous=mm1 

# Climatology
clim  selector=clim    downscaler=nearest averager=mean  continuous=mm1 
clim2 selector=clim2   downscaler=nearest averager=mean  continuous=mm1 

# Kalman Filter
kf          selector=def     downscaler=nearest correctors=kf     averager=mean  continuous=mm0 

# Persistence
persistence selector=pers    downscaler=nearest averager=mean  continuous=mm0 
persistence0 selector=pers0   downscaler=nearest averager=mean  continuous=mm0 

# Raw configurations
raw         selector=def     downscaler=nearest averager=mean  continuous=mm2
rawPrecip   selector=def     downscaler=nearest averager=mean  continuous=mm2 discreteLower=consensus
bpe         selector=def     downscaler=nearest averager=mean  continuous=bpe

# Regression
reg01    selector=def     downscaler=nearest correctors=reg01 averager=mean  continuous=mm0 
reg10    selector=def     downscaler=nearest correctors=reg10 averager=mean  continuous=mm0 
reg11    selector=def     downscaler=nearest correctors=reg11 averager=mean  continuous=mm0 

gammaPrecip    selector=def     downscaler=nearest averager=mean  continuous=gamma0 discreteLower=consensus
gaussianPrecip selector=def     downscaler=nearest correctors=round0 averager=mean  continuous=mm2 discreteLower=consensus
