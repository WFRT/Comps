gfs      inputsF=gfs inputsO=rda336 varconfs=T outputs=netcdf debug=10 getDist  qcs=qc locations=0:5 offsets=0:1:24 region=regionDefault
tutorial inputsF=tutFcst inputsO=tutObs varconfs=T outputs=netcdf,verif debug=50 getDist  parameterIo=parNetcdf region=regionDefault

# Idealized runs
lorenz   inputsF=lorenz63 inputsO=lorenz63obs varconfs=X outputs=netcdf debug=50 getDist  region=regionDefault
sine     inputsF=sine inputsO=sineObs varconfs=T outputs=netcdf debug=50 getDist  parameterIo=parNetcdf qcs=qc region=regionDefault

convert  inputsF=tutFcst   inputsO=tutObs varconfs=T outputs=netcdf debug=10 metric=mae region=regionDefault
