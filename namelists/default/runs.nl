gfs      inputs=gfs,rda336             varconfs=T outputs=netcdf qcs=qc locationIds=0:5 offsets=0:1:24
tutorial inputs=sineFcst,sineObs       varconfs=tutorial outputs=netcdf,verif parameterIo=parNetcdf

# Idealized runs
lorenz   inputs=lorenz63,lorenz63obs   varconfs=X outputs=netcdf qcs=qc
