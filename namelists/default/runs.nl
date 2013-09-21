gfs      inputsF=gfs inputsO=rda336 varconfs=T outputs=netcdf debug=10 writeForecasts  writeVerifications  getCdf  metrics=mae,bias,fcst,obs qcs=qc locations=0:5 offsets=0:3:24 
lorenz   inputsF=lorenz63 inputsO=lorenz63obs varconfs=X outputs=netcdf debug=50 writeForecasts  writeVerifications  getCdf  metrics=mae
tutorial inputsF=tutFcst inputsO=tutObs varconfs=T outputs=netcdf debug=50 writeForecasts  writeVerifications  getCdf metrics=mae,bias
sine     inputsF=sine inputsO=sineObs varconfs=T outputs=netcdf debug=50 writeForecasts  writeVerifications  getCdf metrics=mae,bias parameterIo=parNetcdf qcs=qc

convert  inputsF=gfs   inputsO=rda336 varconfs=T outputs=netcdf debug=10 writeVerifications  metric=mae
