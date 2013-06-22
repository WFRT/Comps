gfs      inputsF=gfs inputsO=rda336 varconfs=T output=oGfs debug=10 writeForecasts  writeVerifications  getCdf  metrics=mae,bias,fcst,obs qcs=qc
lorenz   inputsF=lorenz63 inputsO=lorenz63obs varconfs=X output=oLorenz debug=50 writeForecasts  writeVerifications  getCdf  metrics=mae qcs=qc
tutorial inputsF=tutFcst inputsO=tutObs varconfs=T output=oTutorial debug=50 writeForecasts  writeVerifications  getCdf metrics=mae,bias qcs=qc
sine     inputsF=sine inputsO=sineObs varconfs=Precip output=oSine    debug=50 writeForecasts writeVerifications  getCdf metrics=mae,bias parameterIo=parNetcdf qcs=qc
