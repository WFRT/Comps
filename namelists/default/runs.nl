gfs      inputs=gfs,rda336 varconfs=T outputs=netcdf debug=10 qcs=qc locationIds=0:5 offsets=0:1:24 pooler=poolerDefault
tutorial inputs=tutFcst,tutObs varconfs=T outputs=netcdf,verif debug=50 parameterIo=parNetcdf pooler=poolerDefault offsets=0:12:24

# Idealized runs
lorenz   inputs=lorenz63,lorenz63obs varconfs=X outputs=netcdf debug=50 pooler=poolerDefault
sine     inputs=sine,sineObs varconfs=T outputs=netcdf debug=50 parameterIo=parNetcdf qcs=qc pooler=poolerDefault

convert  inputs=tutFcst,tutObs varconfs=T outputs=netcdf debug=10 metric=mae pooler=poolerDefault
