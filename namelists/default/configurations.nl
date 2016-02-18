# Analogs
analog selector=sAnalog  downscaler=dNearest   continuous=mm1 

# Climatology
clim  selector=sClim    downscaler=dNearest   continuous=bpe
climP selector=sClim    downscaler=dNearest   continuous=bpe  discreteLower=diConsensus # For precip
climP2 selector=sClim    downscaler=dNearest   continuous=bpe  discreteLower=diConsensus averager=mist.mean

# Kalman Filter
kf          selector=sDef     downscaler=dNearest correctors=cKf        continuous=mm0 
update      selector=sDef     downscaler=dNearest correctors=cRecentObs continuous=mm0

# Persistence
persistence  selector=sPers    downscaler=dNearest   continuous=mm0 
persistenceP selector=sPers    downscaler=dNearest   continuous=bpe discreteLower=diConsensus # For precip
persistence0 selector=sPers0   downscaler=dNearest   continuous=mm0 

# Raw configurations
raw         selector=sDef     downscaler=dNearest   continuous=bpe
rawPrecip   selector=sDef     downscaler=dNearest   continuous=bpe discreteLower=diConsensus
logit1      selector=sDef     downscaler=dNearest   continuous=mm2 discreteLower=diLogit1
logit2      selector=sDef     downscaler=dNearest   continuous=mm2 discreteLower=diLogit2
logit3      selector=sDef     downscaler=dNearest   continuous=mm2 discreteLower=diLogit3
bpe         selector=sDef     downscaler=dNearest   continuous=bpe
bpePrecip   selector=sDef     downscaler=dNearest   continuous=bpe discreteLower=diConsensus

# Regression
reg          selector=sDef     downscaler=dNearest correctors=cReg      continuous=mm0
meanBias     selector=sDef     downscaler=dNearest correctors=cMeanBias continuous=mm0

gammaPrecip    selector=sDef     downscaler=dNearest   continuous=gamma0 discreteLower=diConsensus
gaussianPrecip selector=sDef     downscaler=dNearest correctors=cRound0   continuous=mm2 discreteLower=diConsensus
