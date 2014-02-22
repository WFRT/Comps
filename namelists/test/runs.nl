run      inputsF=test.input,gfs   inputsO=test.input varconfs=test.T output=test.output debug=10 metric=mae pooler=poolerDefault
emwxnet  inputsF=gfs,wfrt.emwxnetOp      inputsO=wfrt.ubc-mv-obs varconfs=T output=test.output debug=10 metric=mae pooler=poolerDefault
info     inputsF=test.input   inputsO=test.input varconfs=test.T output=test.output debug=10 metric=mae pooler=poolerDefault
run2     inputsF=test.input   inputsO=test.input varconfs=test.T output=test.output debug=10 metric=mae locations=415 pooler=poolerDefault
