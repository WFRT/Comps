# Analogs
analog      selector=an      downscaler=nearest averager=mean  continuous=mm1 

# Climatology
clim  selector=clim    downscaler=nearest averager=mean  continuous=mm1 
climP selector=climP   downscaler=nearest averager=mean  continuous=bpe  discreteLower=consensus

# GFS case study
gfs1  selector=def     downscaler=nearest  averager=mean continuous=mm2
gfs2  selector=def     downscaler=nearest  averager=mean continuous=mm2               calibrators=pitcal
gfs3  selector=def     downscaler=nearest  averager=mean continuous=mm2 correctors=kf
gfs4  selector=def     downscaler=nearest  averager=mean continuous=mm2 correctors=kf calibrators=pitcal
gfs5  selector=def     downscaler=distance averager=mean continuous=mm2
gfs6  selector=def     downscaler=distance averager=mean continuous=mm2               calibrators=pitcal
gfs7  selector=def     downscaler=distance averager=mean continuous=mm2 correctors=kf
gfs8  selector=def     downscaler=distance averager=mean continuous=mm2 correctors=kf calibrators=pitcal
gfsP1 selector=def     downscaler=distance averager=mean continuous=mm2 discreteLower=logit1
gfsP2 selector=def     downscaler=distance averager=mean continuous=mm2 discreteLower=logit1 correctors=dmb
gfsP3 selector=def     downscaler=distance averager=mean continuous=mm2 discreteLower=logit1
gfsP4 selector=def     downscaler=distance averager=mean continuous=mm2 discreteLower=logit1 correctors=dmb
gfsP5 selector=def     downscaler=distance averager=mean continuous=mm2 discreteLower=consensus
gfsP6 selector=def     downscaler=distance averager=mean continuous=mm2 discreteLower=consensus correctors=round0
gfsP7 selector=def     downscaler=distance averager=mean continuous=mm2 discreteLower=consensus correctors=dmb
gfsP8 selector=def     downscaler=distance averager=mean continuous=mm2 discreteLower=consensus correctors=dmb,round0

# Kalman Filter
kf          selector=def     downscaler=nearest correctors=kf     averager=mean  continuous=mm0 

update      selector=def     downscaler=nearest correctors=recentObs averager=mean continuous=mm0

# Persistence
persistence  selector=pers    downscaler=nearest averager=mean  continuous=mm0 
persistenceP selector=pers    downscaler=nearest averager=mean  continuous=bpe discreteLower=consensus 
persistence0 selector=pers0   downscaler=nearest averager=mean  continuous=mm0 

# Raw configurations
raw         selector=def     downscaler=nearest averager=mean  continuous=mm2
rawPrecip   selector=def     downscaler=nearest averager=mean  continuous=mm2 discreteLower=consensus
logit1      selector=def     downscaler=nearest averager=mean  continuous=mm2 discreteLower=logit1
logit2      selector=def     downscaler=nearest averager=mean  continuous=mm2 discreteLower=logit2
logit3      selector=def     downscaler=nearest averager=mean  continuous=mm2 discreteLower=logit3
bpe         selector=def     downscaler=nearest averager=mean  continuous=bpe
bpePrecip   selector=def     downscaler=nearest averager=mean  continuous=bpe discreteLower=consensus

# Regression
reg01    selector=def     downscaler=nearest correctors=reg01 averager=mean  continuous=mm0 
reg10    selector=def     downscaler=nearest correctors=reg10 averager=mean  continuous=mm0 
reg11    selector=def     downscaler=nearest correctors=reg11 averager=mean  continuous=mm0 

gammaPrecip    selector=def     downscaler=nearest averager=mean  continuous=gamma0 discreteLower=consensus
gaussianPrecip selector=def     downscaler=nearest correctors=round0 averager=mean  continuous=mm2 discreteLower=consensus
