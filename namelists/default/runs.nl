gfs      inputsF=gfs inputsO=rda336 varconfs=T outputs=netcdf debug=10 writeForecasts  writeVerifications  getDist  qcs=qc locations=0:5 offsets=0:1:24 region=regionDefault
tutorial inputsF=tutFcst inputsO=tutObs varconfs=T outputs=netcdf debug=50 writeForecasts  writeVerifications  getDist  parameterIo=parNetcdf region=regionDefault offsets=0:24

# Idealized runs
lorenz   inputsF=lorenz63 inputsO=lorenz63obs varconfs=X outputs=netcdf debug=50 writeForecasts  writeVerifications  getDist  region=regionDefault
sine     inputsF=sine inputsO=sineObs varconfs=T outputs=netcdf debug=50 writeForecasts  writeVerifications  getDist  parameterIo=parNetcdf qcs=qc region=regionDefault

convert  inputsF=gfs   inputsO=rda336 varconfs=T outputs=netcdf debug=10 writeVerifications  metric=mae region=regionDefault
