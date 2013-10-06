gfs      inputsF=gfs inputsO=rda336 varconfs=T outputs=netcdf debug=10 writeForecasts  writeVerifications  getDist  qcs=qc locations=0:5 offsets=0:1:24 
tutorial inputsF=tutFcst inputsO=tutObs varconfs=T outputs=netcdf debug=50 writeForecasts  writeVerifications  getDist  parameterIo=parNetcdf

# Idealized runs
lorenz   inputsF=lorenz63 inputsO=lorenz63obs varconfs=X outputs=netcdf debug=50 writeForecasts  writeVerifications  getDist
sine     inputsF=sine inputsO=sineObs varconfs=T outputs=netcdf debug=50 writeForecasts  writeVerifications  getDist  parameterIo=parNetcdf qcs=qc

convert  inputsF=gfs   inputsO=rda336 varconfs=T outputs=netcdf debug=10 writeVerifications  metric=mae
