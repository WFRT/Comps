# Tag       Class Name           Scheme options

# Averager
mean        class=AveragerMeasure measure=ensMean

# Input
input        class=InputFlat      folder=test     type=forecast

# Selectors
def         class=SelectorDefault     datasets=naefs
sel01       class=SelectorMember      members=0,1
selMM5      class=SelectorMember      models=MM5

# Downscaler
nearest     class=DownscalerNearestNeighbour

# Continuous
mm0         class=ContinuousMoments  distribution=gaussian0 type=const efold=30 measure=ensVar

# Calibration

# BaseDistribution
dist       class=BaseDistributionGaussian

# EstimatorProbabilistic
mle class=EstimatorMaximumLikelihood efold=30

# Output
output       name=test      class=OutputNetcdf input=test.input

# Parameters Ios
parMemory  class=ParameterIoMemory finder=finder
parNetcdf  class=ParameterIoNetcdf finder=finder

# Finders
finder class=FinderDefault

# Metrics
mae class=MetricMae

# Transforms
abs         class=TransformAbsolute
square      class=TransformPower power=2
log10       class=TransformLog base=10
