# tag    variable          configurations                       metrics
T         variable=T        configurations=raw,persistence,kf   metrics=mae,bias,fcst,obs,hit5,pit
Precip    variable=Precip   configurations=logit1,logit2,logit3 metrics=p0,p5
X         variable=LorenzX  configurations=bpe
MSLP      variable=MSLP     configurations=raw metrics=mae

gfsT      variable=T        configurations=clim,persistence,gfs1,gfs2,gfs3,gfs4,gfs5,gfs6,gfs7,gfs8 metrics=mae,bias,fcst,obs
gfsMSLP   variable=MSLP     configurations=clim,persistence,gfs1,gfs2,gfs3,gfs4,gfs5,gfs6,gfs7,gfs8 metrics=mae,bias,fcst,obs
gfsPrecip variable=Precip24 configurations=climP,persP,gfsP1,gfsP2,gfsP3,gfsP4,gfsP5,gfsP6,gfsP7,gfsP8 metrics=p0,p5,fcst,obs
