# Analogs
analog selector=sAnalog  downscaler=dNearest   continuous=mm1 

# Climatology
clim  selector=sClim    downscaler=dNearest   continuous=mm1
climP selector=sClim    downscaler=dNearest   continuous=bpe  discreteLower=diConsensus # For precip

# GFS case study
gfs1  selector=sDef     downscaler=dNearest   continuous=mm2
gfs2  selector=sDef     downscaler=dNearest   continuous=mm2               calibrators=pitcal
gfs3  selector=sDef     downscaler=dNearest   continuous=mm2 correctors=cKf
gfs4  selector=sDef     downscaler=dNearest   continuous=mm2 correctors=cKf calibrators=pitcal
gfs5  selector=sDef     downscaler=dDist16    continuous=mm2
gfs6  selector=sDef     downscaler=dDist16    continuous=mm2               calibrators=pitcal
gfs7  selector=sDef     downscaler=dDist16    continuous=mm2 correctors=cKf
gfs8  selector=sDef     downscaler=dDist16    continuous=mm2 correctors=cKf calibrators=pitcal
gfsP1 selector=sDef     downscaler=dDist16    continuous=mm2 discreteLower=diLogit1
gfsP2 selector=sDef     downscaler=dDist16    continuous=mm2 discreteLower=diLogit1 correctors=cDmb
gfsP3 selector=sDef     downscaler=dDist16    continuous=mm2 discreteLower=diLogit1
gfsP4 selector=sDef     downscaler=dDist16    continuous=mm2 discreteLower=diLogit1 correctors=cDmb
gfsP5 selector=sDef     downscaler=dDist16    continuous=mm2 discreteLower=diConsensus
gfsP6 selector=sDef     downscaler=dDist16    continuous=mm2 discreteLower=diConsensus correctors=round0
gfsP7 selector=sDef     downscaler=dDist16    continuous=mm2 discreteLower=diConsensus correctors=cDmb
gfsP8 selector=sDef     downscaler=dDist16    continuous=mm2 discreteLower=diConsensus correctors=cDmb,round0

# Kalman Filter
kf          selector=sDef     downscaler=dNearest correctors=cKf       continuous=mm0 
update      selector=sDef     downscaler=dNearest correctors=recentObs  continuous=mm0

# Persistence
persistence  selector=sPers    downscaler=dNearest   continuous=mm0 
persistenceP selector=sPers    downscaler=dNearest   continuous=bpe discreteLower=diConsensus # For precip
persistence0 selector=sPers0   downscaler=dNearest   continuous=mm0 

# Raw configurations
raw         selector=sDef     downscaler=dNearest   continuous=mm0
fake        selector=sDef     downscaler=dNearest   correctors=cKf,fixed1 continuous=mm0
rawPrecip   selector=sDef     downscaler=dNearest   continuous=mm2 discreteLower=diConsensus
logit1      selector=sDef     downscaler=dNearest   continuous=mm2 discreteLower=diLogit1
logit2      selector=sDef     downscaler=dNearest   continuous=mm2 discreteLower=diLogit2
logit3      selector=sDef     downscaler=dNearest   continuous=mm2 discreteLower=diLogit3
bpe         selector=sDef     downscaler=dNearest   continuous=bpe
bpePrecip   selector=sDef     downscaler=dNearest   continuous=bpe discreteLower=diConsensus

# Regression
reg01    selector=sDef     downscaler=dNearest correctors=reg01   continuous=mm0
reg10    selector=sDef     downscaler=dNearest correctors=reg10   continuous=mm0 
reg11    selector=sDef     downscaler=dNearest correctors=reg11   continuous=mm0 

gammaPrecip    selector=sDef     downscaler=dNearest   continuous=gamma0 discreteLower=diConsensus
gaussianPrecip selector=sDef     downscaler=dNearest correctors=round0   continuous=mm2 discreteLower=diConsensus
