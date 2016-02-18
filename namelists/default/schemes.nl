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
# Select observations at the same time of year and time of day. Uses future observations, if
# available, so this differs from what would be possible operationally. Likely is still a
# fair representation of climatology.
sClim       class=SelectorClim dayLength=15 hourLength=0  allowWrappedOffsets  allowFutureValues
# A truly fair climatology, based only on past observations.
sClimValid  class=SelectorClim dayLength=15 hourLength=0  allowWrappedOffsets  
# Use yesterday's obs at the same offset
sPers       class=SelectorPersistence
# Use the most recent obs for all offsets
sPers0      class=SelectorPersistence useLatest
# Ensemble of nearest neighbours
sHood4      class=SelectorDefault neighbourhoods=n4
sHood5      class=SelectorDefault neighbourhoods=n5
sHood16     class=SelectorDefault neighbourhoods=n16
sHood36     class=SelectorDefault neighbourhoods=n36
sHood225    class=SelectorDefault neighbourhoods=n225
# Ensemble of gridpoints at same elevation
sElev5      class=SelectorDefault neighbourhoods=n5 # Only one member
sElev16     class=SelectorDefault neighbourhoods=n16
sElev36     class=SelectorDefault neighbourhoods=n36
sElev225    class=SelectorDefault neighbourhoods=n225
# Simple analog approach, using the forecast variable as analog variable
sAnalog     class=SelectorAnalog analogMetric=1norm numAnalogs=15 averager=mean normalize=1
# Select ensemble member with best MAE (1norm)
sPerf       class=SelectorPerformance num=3 detMetric=1norm

###############
# Downscalers #
###############
# Use the nearest neighbour
dNearest       class=DownscalerNeighbourhood neighbourhoods=n1
# Use nearest X neighbours
dHood4         class=DownscalerNeighbourhood neighbourhoods=n4
dHood5         class=DownscalerNeighbourhood neighbourhoods=n5
dHood16        class=DownscalerNeighbourhood neighbourhoods=n16
dHood36        class=DownscalerNeighbourhood neighbourhoods=n36
dHood225       class=DownscalerNeighbourhood neighbourhoods=n225
# Weight the nearest 16 neighbours using their inverse square distance
dDist16        class=DownscalerNeighbourhood neighbourhoods=n16 weightOrder=2
# Heigh-adjustment of temperatures based on a fixed lapse rate
dLapse         class=DownscalerElevation
# Heigh-adjustment of temperatures based on a fixed lapse rate
dPressure      class=DownscalerElevation type=pressure
# Uses the nearest gridpoint at the same elevation
dElev5         class=DownscalerNeighbourhood neighbourhoods=nElev5
dElev16        class=DownscalerNeighbourhood neighbourhoods=nElev16
dElev36        class=DownscalerNeighbourhood neighbourhoods=nElev36
dElev225       class=DownscalerNeighbourhood neighbourhoods=nElev225

##################
# Neighbourhoods #
##################
n1           class=NeighbourhoodNearest num=1
n4           class=NeighbourhoodNearest num=4
n5           class=NeighbourhoodNearest num=5
n16          class=NeighbourhoodNearest num=16
n36          class=NeighbourhoodNearest num=36
n225         class=NeighbourhoodNearest num=225
# Use gridpoints at the same elevation
nElev5   class=NeighbourhoodElevation num=5   numBest=1
nElev16  class=NeighbourhoodElevation num=16  numBest=3
nElev36  class=NeighbourhoodElevation num=36  numBest=5
nElev225 class=NeighbourhoodElevation num=225 numBest=20
# Use elevation and terrain gradient to find 100 best points
nElevGrad    class=NeighbourhoodElevation num=225 numBest=100 elevationWeight=1 distanceWeight=0.01 gradientWeight=100000

##############
# Correctors #
##############
cRecentObs  class=CorrectorRecentObs efold=8
# Weigh forecast and climatology
cClim       class=CorrectorClim
# Linear regression with forecast and climatology-anomaly
cClimReg    class=CorrectorClimReg
# Removes mean bias:  a + fcst
cPoly       class=CorrectorPolynomialRegression useOrders=1,0
cMeanBias   class=CorrectorPolynomialRegression useOrders=1,0
# Full linear regression: a + b * fcst
cReg        class=CorrectorPolynomialRegression useOrders=1,1
cQq         class=CorrectorQuantileQuantile
cKf         class=CorrectorKalmanFilter ratio=0.1
# Multily values by a scaling factor
cDmb        class=CorrectorDmb
# Round values below a threshold down to 0 (used for precip)
cRound0     class=CorrectorRound roundDownTo=0 memberSpecific
# Adds 1 to the forecast (for testing)
cFixed1     class=CorrectorFixed add=1
# Reduces ensemble to 3 members, based on kmeans clustering
cKmeans3    class=CorrectorKmeans numMeans=3

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
R         name=R        class=OutputR validOnly

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
p05      class=MetricCdf threshold=0.5
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
q10      class=MetricQuantile quantile=0.1
q20      class=MetricQuantile quantile=0.2
q30      class=MetricQuantile quantile=0.3
q40      class=MetricQuantile quantile=0.4
q50      class=MetricQuantile quantile=0.5
q60      class=MetricQuantile quantile=0.6
q70      class=MetricQuantile quantile=0.7
q80      class=MetricQuantile quantile=0.8
q90      class=MetricQuantile quantile=0.9

crps     class=MetricCrps
brier5   class=MetricBrier threshold=5
brier6   class=MetricBrier threshold=60
brier10  class=MetricBrier threshold=10 anomalyAbove
hit5     class=MetricContingency threshold=5 quadrant=hit

#############
# Variables #
#############
# Standard variables
T           class=VariableDefault units=^oC      min=-90  max=60   mean=0    std=5    description=Surface_temperature  standardName=air_temperature
T_Min       class=VariableDefault units=^oC      min=-100 max=80   mean=0    std=5    description=Surface_temperature_minimum_from_model_start standardName=air_temperature
T_Max       class=VariableDefault units=^oC      min=-100 max=80   mean=0    std=5    description=Surface_temperature_maximum_from_model_start standardName=air_temperature
T_Min24     class=VariableDefault units=^oC      min=-100 max=80   mean=0    std=5    description=Minimum_temperature_daily   standardName=air_temperature
T_Max24     class=VariableDefault units=^oC      min=-100 max=80   mean=0    std=5    description=Maximum_temperature_daily   standardName=air_temperature
Precip      class=VariableDefault units=mm       min=0    max=50   mean=2    std=10   description=Hourly_precipitation lowerDiscrete  standardName=precipitation_amount
PrecipConv  class=VariableDefault units=mm       min=0    mean=2   max=50    std=1e-5 description=Convective_precipitation_rate lowerDiscrete standardName=convective_precipitation_amount
TDew        class=VariableDefault units=^oC      min=-100 max=80   mean=0    std=5    description=Dewpoint_temperature  standardName=dew_point_temperature
FreezeLvl   class=VariableDefault units=m        min=-10000    max=10000 mean=1000 std=1000 description=Freezing_level  standardName=freezing_level_altitude
Z50         class=VariableDefault units=gpm      min=3000 mean=5400 max=12000 std=1000 description=Geopotential_height_50kpa   standardName=geopotential_height
TWet        class=VariableDefault units=^oC      min=-100 max=80   mean=0    std=5    description=Wetbulb_temperature          standardName=wet_bulb_temperature
BLH         class=VariableDefault units=m        min=0    max=10000 mean=600 std=200 description=Boundary_layer_height         standardName=atmosphere_boundary_layer_thickness
Vort        class=VariableDefault units=1/s      min=-1e-4 mean=0 max=1e-4 std=1e-5 description=Absolute_vorticity             standardName=atmosphere_absolute_vorticity

# Temperature
TPot        class=VariableDefault units=NoUnit   min=226.606 mean=273.9319 max=307.064 std=6.4936 description=Potential_temperature  standardName=air_potential_temperature
T0          class=VariableDefault units=^oC      min=-50  max=45   mean=0    std=5    description=Skin_temperature  standardName=air_temperature

# Humidity
RH          class=VariableDefault units=%        min=0    max=100  mean=50   std=10 lowerDiscrete  upperDiscrete  standardName=relative_humidity
SH          class=VariableDefault units=g/g      min=0    max=0.1  mean=0.001   std=0.001   lowerDiscrete         standardName=humidity_mixing_ratio
Visibility  class=VariableDefault units=m

# Pressure
P           class=VariableDefault units=kPa      min=50  mean=100 max=120 std=2 description=Pressure_surface      standardName=air_pressure
MSLP        class=VariableDefault units=kPa      min=92  mean=100 max=108 std=2 description=Pressure_reduced_to_mean_sea_level_pressure standardName=air_pressure_at_sea_level
QNH         class=VariableDefault units=kPa      min=92  mean=100 max=108 std=2 description=Pressure_reduced_to_mean_sea_level_pressure_ICAO standardName=air_pressure_at_sea_level_ICAO

# Wind
StreamFlow  class=VariableDefault units=?
U           class=VariableDefault units=m/s      min=-300 max=300  mean=0    std=5 standardName=eastward_wind
V           class=VariableDefault units=m/s      min=-300 max=300  mean=0    std=5 standardName=northward_wind
W           class=VariableDefault units=m/s      min=-300 max=300  mean=0    std=5
WindChill   class=VariableDefault units=^oC      min=-50  max=50   mean=0    std=5
WindDir     class=VariableDefault units=^o       min=0    max=360  mean=180  std=30   circular  standardName=wind_direction
WindGust    class=VariableDefault units=m/s      min=0    max=300  mean=5    std=5 description=Wind_gust lowerDiscrete  standardName=wind_speed_of_gust
WindSpeed   class=VariableDefault units=m/s      min=0    max=300  mean=5    std=5 lowerDiscrete                        standardName=wind_speed
W50         class=VariableDefault units=Pa/s     min=-50  max=50   mean=0    std=0.2 description=Vertical_velocity_50kPa
TKE         class=VariableDefault units=m2/s2    min=0    max=20   mean=0.1  std=0.2

# Energy
Power       class=VariableDefault units=m/2      min=0    max=1e10 mean=0    std=1e5 lowerCutOffSpeed=0 upperCutOffSpeed=10 turbineRadius=20 lowerDiscrete

# Aviation
QNH         class=VariableDefault units=kPa      min=50    mean=100 max=120    std=2 description=Pressure_reduced_to_sea_level_using_standard_atmosphere

# Air quality
CO2         class=VariableDefault units=kg/m3    description=Carbon_dioxide       standardName=mass_concentration_of_carbon_dioxide_in_air
H2S         class=VariableDefault units=kg/m3    description=Hydrogen_disulphie
NO          class=VariableDefault units=kg/m3    description=Nitrogen_oxide       standardName=mass_concentration_of_nitrogen_monoxide_in_air
NO2         class=VariableDefault units=kg/m3    description=Nitrogen_dioxide     standardName=mass_concentration_of_nitrogen_dixide_in_air
O3Total     class=VariableDefault units=Dobson   min=100 mean=300  max=1000 std=50 description=Colun_integrated_ozone
O3          class=VariableDefault units=g/g      min=0   mean=1e-6 max=1e-5 std=1e-6 description=Ozone_Mixing_Ratio standardName=mass_fraction_of_ozone_in_air
PM10        class=VariableDefault units=kg/m3    standardName=mass_concentration_of_particulate_organic_matter_dry_aerosol_in_air
PM25        class=VariableDefault units=kg/m3    standardName=mass_concentration_of_particulate_organic_matter_dry_aerosol_in_air
SO2         class=VariableDefault units=kg/m3    standardName=mass_concentration_of_sulfur_dixide_in_air

# Precipitation
Precip_3    class=VariableDefault units=mm       min=0    max=1000  mean=1   std=1   description=Hourly_precipitation_3hours lowerDiscrete   standardName=precipitation_amount 
Precip_6    class=VariableDefault units=mm       min=0    max=1000  mean=1   std=1   description=Hourly_precipitation_6hours lowerDiscrete   standardName=precipitation_amount 
Precip_12   class=VariableDefault units=mm       min=0    max=1000  mean=1   std=1   description=Hourly_precipitation_12hours lowerDiscrete  standardName=precipitation_amount 
Precip_24   class=VariableDefault units=mm       min=0    max=1000  mean=1   std=1   description=Hourly_precipitation_24hours lowerDiscrete  standardName=precipitation_amount 
Precip_Acc  class=VariableDefault units=mm       min=0    max=1000 mean=10   std=50  description=24h_precipitation lowerDiscrete            standardName=precipitation_amount 
SnowDepth   class=VariableDefault units=m
PrecipWater class=VariableDefault units=mm       min=0    max=1000 mean=1 description=Precipitable_water lowerDiscrete
Phase       class=VariableDefault units=type     min=0    max=3    mean=1 description=Precipitation_phase

# Snow
PrecipSolid class=VariableDefault units=cm       min=0    max=1000  mean=1   std=1   description=Hourly_precipitation_snow lowerDiscrete  standardName=snowfall_amount
PrecipSolid_Acc class=VariableDefault units=cm       min=0    max=1000  mean=1   std=1   description=Hourly_precipitation_snow lowerDiscrete  standardName=snowfall_amount
TSnow       class=VariableDefault units=^oC      min=-100 max=80   mean=0    std=5   description=Snow_temperature                        standardName=snow_temperature
TSnowPack   class=VariableDefault units=^oC      min=-100 max=80   mean=0    std=5   description=Snow_pack_temperature
SnowWaterEq class=VariableDefault units=?
SnowTotal   class=VariableDefault units=m

# Convective
CIN         class=VariableDefault units=J/kg     min=-1e6 mean=-5  max=1e3 std=30    description=Convective_inhibition                       standardName=atmosphere_convective_inhibition
CAPE        class=VariableDefault units=J/kg     min=0    mean=100 max=1e6 std=300   description=Convective_available_potential_energy      
SLI         class=VariableDefault units=^oC      min=-30  mean=0   max=50  std=5     description=Surface_lifted_index

# Clouds
CloudCover  class=VariableDefault units=1        min=0    max=1    mean=0.5  std=0.1 standardName=cloud_area_fraction
CloudWater  class=VariableDefault units=kg/m^2   min=0    max=100  mean=0.1  std=10
CloudHigh   class=VariableDefault units=1        min=0    max=1    mean=0.5  std=0.1 standardName=cloud_area_fraction
CloudMedium class=VariableDefault units=1        min=0    max=1    mean=0.5  std=0.1 standardName=cloud_area_fraction
CloudLow    class=VariableDefault units=1        min=0    max=1    mean=0.5  std=0.1 standardName=cloud_area_fraction
CloudFog    class=VariableDefault units=1        min=0    max=1    mean=0.5  std=0.1 standardName=fog_area_fraction # Is the standard name correct?

# Radiation
LWout       class=VariableDefault units=W/m2     min=0    mean=50   std=50  standardName=surface_upwelling_longwave_flux_in_air
LWin        class=VariableDefault units=W/m2     min=0    mean=50   std=50  standardName=surface_downwelling_longwave_flux_in_air
SWout       class=VariableDefault units=W/m2     min=0    mean=100  std=100 standardName=surface_upwelling_shortwave_flux_in_air
SWin        class=VariableDefault units=W/m2     min=0    mean=200  std=100 standardName=surface_upwelling_shortwave_flux_in_air
SWin6       class=VariableDefault units=W/m2     min=0    mean=100  std=100 standardName=surface_upwelling_shortwave_flux_in_air
Albedo      class=VariableDefault units=%        min=0    mean=20  max=100   std=25 description=Albedo standardName=albedo
LWnet       class=VariableDefault units=W/m2     mean=0    std=50  standardName=surface_net_upward_longwave_flux
SWnet       class=VariableDefault units=W/m2     mean=0    std=50  standardName=surface_net_upward_shortwave_flux
SHF         class=VariableDefault units=W/m2     mean=0    std=1e4 standardName=surface_upward_sensible_heat_flux
LWout_Acc   class=VariableDefault units=W/m2     min=0    mean=50   std=50
LWin_Acc    class=VariableDefault units=W/m2     min=0    mean=50   std=50
SWout_Acc   class=VariableDefault units=W/m2     min=0    mean=100  std=100
SWin_Acc    class=VariableDefault units=W/m2     min=0    mean=200  std=100
LWnet_Acc   class=VariableDefault units=W/m2     min=0    mean=50   std=50
SWnet_Acc   class=VariableDefault units=W/m2     min=0    mean=50   std=50
SHF_Acc     class=VariableDefault units=W/m2     mean=0    std=1e4

# Hydrology/ocean
WaterRunoff class=VariableDefault units=kg/m^2   min=0    max=1e3  mean=1    std=1
WaveHeightMax class=VariableDefault units=m
SigWaveHeight class=VariableDefault units=m
TailWaterElevation class=VariableDefault units=m
WavePeriod class=VariableDefault units=s                 standardName=sea_surface_wind_wave_period
WaterParticulate class=VariableDefault units=?
TWater  class=VariableDefault units=^oC                  standardName=sea_water_temperature
Inflow  class=VariableDefault units=m^3/s       min=0  max=5000 mean=50 std=20

# Dynamics
MFluxN  class=VariableDefault units=N/m2 min=-1e8 max=1e8 mean=0 std=1e4 standardName=downward_northward_momentum_flux_in_air
MFluxE  class=VariableDefault units=N/m2 min=-1e8 max=1e8 mean=0 std=1e4 standardName=downward_eastward_momentum_flux_in_air


# Topography
Elevation        class=VariableDefault units=m      standardName=surface_altitude
ForebayElevation class=VariableDefault units=m

# Lorenz63
LorenzX class=VariableDefault min=-100 max=100
LorenzY class=VariableDefault min=-100 max=100
LorenzZ class=VariableDefault min=-100 max=100

Fraction class=VariableDefault units=unitless min=0 max=1 std=0.5
PrecipSolid_Fraction class=VariableDefault units=unitless min=0 max=1 std=0.5

#####################
# Derived variables #
#####################
vPrecip           class=VariableDeaccumulate baseVariable=Precip_Acc
vPrecipSolid      class=VariableDeaccumulate baseVariable=PrecipSolid_Acc
vPrecip_Acc       class=VariableAccumulate   baseVariable=Precip
vPrecipSolid_Acc  class=VariableAccumulate   baseVariable=PrecipSolid
vPrecip_6         class=VariableAccumulate   baseVariable=Precip timeWindow=6
vPrecip_24        class=VariableAccumulate   baseVariable=Precip timeWindow=24

vQNH              class=VariableQnh

vT_Min24          class=VariableMinimum      baseVariable=T  timeWindow=24
vT_Max24          class=VariableMaximum      baseVariable=T  timeWindow=24
vTDew             class=VariableTDew
vTWet             class=VariableTWet

vWindSpeed        class=VariableWindSpeed
vWindDir          class=VariableWindDir
