# Tag       Class Name           Scheme options

# Inputs
tutFcst     class=InputFlat       folder=tutFcst  type=forecast    offsets=0,12,24 fileFormat=%Y%m%d_%LC_%v
tutObs      class=InputFlat       folder=tutObs   type=observation offsets=0,12    fileFormat=%Y%m%d_%LC_%v
gfsSmall    class=InputGrib       folder=gfsSmall type=forecast maxCacheSize=8e9 allowTimeInterpolation  cacheOtherLocations  offsets=0:6:180
gfs         class=InputGrib       folder=gfs      type=forecast maxCacheSize=8e9 allowTimeInterpolation  cacheOtherLocations  offsets=0:6:180 fileFormat=%Y%m/%Y%m%d/gfs_4_%Y%m%d_0000_%03O fileExtension=grb2
gfsOp       class=InputNetcdf     folder=gfsOp    type=forecast maxCacheSize=8e9 allowTimeInterpolation  cacheOtherLocations  cacheOtherOffsets offsets=0:6:180
rda336      class=InputRdaNetcdf  folder=rda336   type=observation cacheOtherLocations  cacheOtherOffsets
sineFcst    class=InputSinusoidal folder=sine     type=forecast    mean=11 yearAmplitude=12 dayAmplitude=6 members=6  dayPeak=200 ensStd=1 offsets=0:24 dayCommonStd=2 dayBiasStd=0.5 dayBiasEfold=20 startDate=20100101 endDate=20140101
sineObs     class=InputSinusoidal folder=sine     type=observation mean=12 yearAmplitude=14 dayAmplitude=4 members=1  dayPeak=200 ensStd=0.4 offsets=0:24 dayCommonStd=2 startDate=20100101 endDate=20140101
lorenz63    class=InputLorenz63   folder=lorenz63 type=forecast        x0=0.9 y0=1.1 z0=0 dt=0.001 ensSize=10 xVar=0.1
lorenz63obs class=InputLorenz63   folder=lorenz63 type=observation     x0=0 y0=1 z0=0 dt=0.001 

#############
# Selectors #
#############
# Default selector. Fetches all ensemble members from the main forecast input.
sDef        class=SelectorDefault
sClim       class=SelectorClim        dayLength=15 hourLength=0  allowWrappedOffsets  allowFutureValues
# Use yesterday's obs at the same offset
sPers       class=SelectorPersistence
# Use the most recent obs for all offsets
sPers0      class=SelectorPersistence useLatest
# Ensemble of nearest neighbours
sHood4      class=SelectorDefault neighbourhoods=n4
sHood16     class=SelectorDefault neighbourhoods=n16
sHood36     class=SelectorDefault neighbourhoods=n36
# Simple analog approach, using the forecast variable as analog variable
sAnalog     class=SelectorAnalog analogMetric=1norm numAnalogs=15 averager=mean normalize=1

perf        class=SelectorPerformance num=3 detMetric=1norm

###############
# Downscalers #
###############
# Use the nearest neighbour
dNearest       class=DownscalerNeighbourhood neighbourhoods=n1
# Weight the nearest 16 neighbours using their inverse square distance
dDist16        class=DownscalerNeighbourhood neighbourhoods=n16 weightOrder=2
# Heigh-adjustment of temperatures based on a fixed lapse rate
dLapse         class=DownscalerElevation
# Uses the nearest gridpoint at the same elevation
dNearestElev   class=DownscalerNeighbourhood neighbourhoods=nNearestElev

##################
# Neighbourhoods #
##################
n1           class=NeighbourhoodNearest num=1
n4           class=NeighbourhoodNearest num=4
n16          class=NeighbourhoodNearest num=16
n36          class=NeighbourhoodNearest num=36
n225         class=NeighbourhoodNearest num=225
# Best elevation of nearest 225
nNearestElev class=NeighbourhoodElevation num=225 numBest=1
# Use elevation and terrain gradient to find 100 best points
nElevGrad    class=NeighbourhoodElevation num=225 numBest=100 elevationWeight=1 distanceWeight=0.01 gradientWeight=100000

##############
# Correctors #
##############
reg01       class=CorrectorPolynomialRegression useOrders=0,1
reg11       class=CorrectorPolynomialRegression useOrders=1,1
reg10       class=CorrectorPolynomialRegression useOrders=1,0 averager=mean
round0      class=CorrectorRound roundDownTo=0 memberSpecific
Kmeans      class=CorrectorKmeans numMeans=3
recentObs   class=CorrectorRecentObs efold=8
fixed1      class=CorrectorFixed add=1
corrClim    class=CorrectorClim

# Removes mean bias
cPoly       class=CorrectorPolynomialRegression useOrders=0,1
cQq         class=CorrectorQuantileQuantile
cKf         class=CorrectorKalmanFilter ratio=0.1
# Multily values by a scaling factor
cDmb        class=CorrectorDmb

# Continuous
mm2         class=ContinuousMoments  distribution=gaussian0 type=full efold=30 measure=ensVar
mm2x        class=ContinuousMoments  distribution=gaussian0 type=full measure=ensMean
mm1         class=ContinuousMoments  distribution=gaussian0 type=ens measure=ensVar
mm0         class=ContinuousMoments  distribution=gaussian0 type=const measure=ensVar
gamma       class=ContinuousGamma    distribution=gamma efold=20 estimator=maxlikeilhood
bpe0        class=ContinuousBpe
bpe         class=ContinuousBpe      interp=linear

#############
# Discretes #
#############
# Mean observed frequency
diConst     class=DiscreteConst     x=0
# Fraction of ensemble members
diConsensus class=DiscreteConsensus x=0
# Logistic regresion
diLogit1    class=DiscreteLogit useConst                    efold=10 x=0
diLogit2    class=DiscreteLogit useConst  measures=ensMean  efold=10 x=0
diLogit3    class=DiscreteLogit useConst  measures=ensMean,ensVar efold=10 x=0

# Measures
ensMean   class=MeasureEnsembleMoment moment=1
ensVar    class=MeasureEnsembleMoment moment=2
ensMedian class=MeasureEnsembleMedian moment=1

# Averager
mean        class=AveragerMean
median      class=AveragerQuantile quantile=0.5

# Calibration
pitcal      class=CalibratorPitBased numSmooth=9 efold=20 interp=linear

# Updater
pitupd      class=UpdaterPitBased

# BaseDistribution
gaussian0  class=BaseDistributionGaussian
gamma0     class=BaseDistributionGamma

# Deterministic Metrics
1norm       class=DetMetricNorm  order=1

# Smoother
triangle   class=SmootherTriangle width=5

# Output
netcdf    name=netcdf   class=OutputNetcdf
verif     name=verif    class=OutputVerif

# Parameters Ios
parMemory  class=ParameterIoMemory
parNetcdf  class=ParameterIoNetcdf

# ObsSelectors
obsNearest class=ObsSelectorNearest
obsOffset  class=ObsSelectorOffset

# Interpolators
linear   class=InterpolatorLinear

# Quality controls
qc       class=QcDefault

# Parameter regions
poolerDefault class=PoolerLocations
poolerAll     class=PoolerAll

# Metrics
mae      class=MetricNorm order=1
mse      class=MetricNorm order=2
bias     class=MetricBias
ign      class=MetricIgnorance
pit      class=MetricPit
sharp    class=MetricSharpness width=0.9
spread   class=MetricSpread
ensSpread class=MetricMeasure measure=ensVar
obs      class=MetricObs
fcst     class=MetricForecast
skill    class=MetricSelectorSkill type=min
th0      class=MetricThreshold threshold=0
th1      class=MetricThreshold threshold=1
th5      class=MetricThreshold threshold=5
pm20     class=MetricCdf threshold=-20
pm10     class=MetricCdf threshold=-10
pm5      class=MetricCdf threshold=-5
pm2      class=MetricCdf threshold=-2
pm1      class=MetricCdf threshold=-1
p0       class=MetricCdf threshold=0
p03      class=MetricCdf threshold=0.3
p1       class=MetricCdf threshold=1
p2       class=MetricCdf threshold=2
p3       class=MetricCdf threshold=3
p4       class=MetricCdf threshold=4
p5       class=MetricCdf threshold=5
p6       class=MetricCdf threshold=6
p7       class=MetricCdf threshold=7
p8       class=MetricCdf threshold=8
p9       class=MetricCdf threshold=9
p10      class=MetricCdf threshold=10
p11      class=MetricCdf threshold=11
p15      class=MetricCdf threshold=15
p19      class=MetricCdf threshold=19
p20      class=MetricCdf threshold=20
p25      class=MetricCdf threshold=25

crps     class=MetricCrps
brier5   class=MetricBrier threshold=5
brier6   class=MetricBrier threshold=60
brier10  class=MetricBrier threshold=10 anomalyAbove
hit5     class=MetricContingency threshold=5 quadrant=hit
