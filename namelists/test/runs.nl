run      inputsF=test.input,gfs   inputsO=test.input varconfs=test.T output=test.output debug=10 writeVerifications metric=mae
emwxnet  inputsF=gfs,wfrt.emwxnetOp      inputsO=wfrt.ubc-mv-obs varconfs=T output=test.output debug=10 writeVerifications metric=mae
info     inputsF=test.input   inputsO=test.input varconfs=test.T output=test.output debug=10 writeVerifications metric=mae
run2     inputsF=test.input   inputsO=test.input varconfs=test.T output=test.output debug=10 writeForecasts writeVerifications metric=mae locations=415
