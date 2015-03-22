# tag    variable          configurations                       metrics
T         variable=T        configurations=raw,kf   metrics=mae,bias,fcst,obs,hit5,pit,p11,ensSpread
Precip    variable=Precip   configurations=logit1,logit2,logit3 metrics=p0,p5
X         variable=LorenzX  configurations=bpe
MSLP      variable=MSLP     configurations=raw metrics=mae
tutorial  variable=T        configurations=raw,kf,persistence,clim metrics=mae,bias,fcst,obs,pit,p11,sharp,ign,crps,p0,q10,q90
