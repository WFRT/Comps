# These should work
confAn10    selector=an3V downscaler=test
confClim    selector=clim    downscaler=nearest averager=avg0  continuous=mm1 calibrator=nipencal correctorPool=poolAll
test        test=test test2=test
naefs       input=wfrt.naefs,wfrt.naefs test=test.test test3=test.test2,test.test,test1.test,te.q
boolean     input=wfrt.naefs,wfrt.naefs turnOnSomething test3=test.test2,test.test,test1.test,te.q

# Errors
test        1333111113=
test=test
test=test test=test
test    131test=test 324=
test    131test=test 324 test=test
test    131test=test 324 test=
test #test
q test=q test=q
test   atr1=test atr2=test atr1=test test=test
