# Standard variables
T           units=^oC      min=-50  max=45   mean=0    std=5    description=Surface_temperature
TMin        units=^oC      min=-100 max=80   mean=0    std=5    description=Surface_temperature_minimum
TMax        units=^oC      min=-100 max=80   mean=0    std=5    description=Surface_temperature_maximum
TMin24      units=^oC      min=-100 max=80   mean=0    std=5    description=Minimum_temperature_daily
TMax24      units=^oC      min=-100 max=80   mean=0    std=5    description=Maximum_temperature_daily
Precip      units=mm       min=0    max=50   mean=2    std=10   description=Hourly_precipitation lowerDiscrete
PrecipConv  units=mm       min=0    mean=2   max=50    std=1e-5 description=Convective_precipitation_rate lowerDiscrete
TDew        units=^oC      min=-100 max=80   mean=0    std=5    description=Dewpoint_temperature
FreezeLvl   units=m        min=0    max=10000 mean=1000 std=1000
Z50         units=gpm      min=3000 mean=5400 max=12000 std=1000 description=Geopotential_height_50kpa
TWet        units=^oC      min=-100 max=80   mean=0    std=5    description=Wetbulb_temperature
BLH         units=m        min=0    max=10000 mean=600 std=200 description=Boundary_layer_height
Vort        units=1/s      min=-1e-4 mean=0 max=1e-4 std=1e-5 description=Absolute_vorticity

# Temperature
TPot        units=NoUnit   min=226.606 mean=273.9319 max=307.064 std=6.4936 description=Potential_temperature

# Humidity
RH          units=%        min=0    max=100  mean=50   std=10   cdfX=0,10,20,30,40,50,60,70,80,90,100 lowerDiscrete  upperDiscrete
SH          units=g/g      min=0    max=0.1  mean=0.001   std=0.001   cdfX=0                          lowerDiscrete
Visibility  units=m

# Pressure
P           units=kPa      min=50  mean=100 max=120 std=2 description=Pressure_surface
MSLP        units=kPa      min=92  mean=100 max=108 std=2 description=Pressure_reduced_to_mean_sea_level_pressure

# Wind
StreamFlow  units=?
U           units=m/s      min=-300 max=300  mean=0    std=5
V           units=m/s      min=-300 max=300  mean=0    std=5
W           units=m/s      min=-300 max=300  mean=0    std=5
WindChill   units=^oC      min=-50  max=50   mean=0    std=5
WindDir     units=^o       min=0    max=360  mean=180  std=30   circular
WindGust    units=m/s      min=0    max=300  mean=5    std=5 description=Wind_gust
WindSpeed   units=m/s      min=0    max=300  mean=5    std=5
W50         units=Pa/s     min=-50  max=50 mean=0    std=0.2 description=Vertical_velocity_50kPa

# Energy
Power       units=m/2      min=0    max=1e10 mean=0    std=1e5 lowerCutOffSpeed=0 upperCutOffSpeed=10 turbineRadius=20 lowerDiscrete

# Air quality
CO2         units=?        description=Carbon_dioxide
H2S         units=?        description=Hydrogen_disulphie
NO          units=?        description=Nitrogen_oxide
NS2         units=?        description=Nitrogen_dioxide
O3Total     units=Dobson   min=100 mean=300  max=1000 std=50 description=Total_ozone
O3          units=g/g      min=0   mean=1e-6 max=1e-5 std=1e-6 description=Ozone_Mixing_Ratio
PM10        units=?
PM25        units=?
SO2         units=?

# Precipitation
Precip3     units=mm       min=0    max=1000  mean=1   std=1   description=Hourly_precipitation_3hours lowerDiscrete
Precip6     units=mm       min=0    max=1000  mean=1   std=1   description=Hourly_precipitation_6hours lowerDiscrete
Precip12    units=mm       min=0    max=1000  mean=1   std=1   description=Hourly_precipitation_12hours lowerDiscrete
Precip24    units=mm       min=0    max=1000  mean=1   std=1   description=Hourly_precipitation_24hours lowerDiscrete
AccPrecip   units=mm       min=0    max=1000 mean=10   std=50   description=24h_precipitation lowerDiscrete
SnowDepth   units=m
PrecipWater units=mm       min=0    max=1000 mean=1 description=Precipitable_water lowerDiscrete

# Snow
PrecipSolid units=cm       min=0    max=1000  mean=1   std=1   description=Hourly_precipitation_snow lowerDiscrete
TSnow       units=^oC      min=-100 max=80   mean=0    std=5    description=Snow_temperature
TSnowPack   units=^oC      min=-100 max=80   mean=0    std=5    description=Snow_pack_temperature
SnowWaterEq units=?
SnowTotal   units=m

# Convective
CIN         units=J/kg     min=-1e6 mean=-5  max=1e3 std=30 description=Convective_inhibition
CAPE        units=J/kg     min=0    mean=100 max=1e6 std=300 description=Convective_available_potential_energy
SLI         units=^oC      min=-30  mean=0   max=50  std=5  description=Surface_lifted_index

# Clouds
CloudCover  units=%        min=0    max=100  mean=50   std=10   cdfX=0 cdfInv=0
CloudWater  units=kg/m^2   min=0    max=100  mean=0.1   std=10  cdfX=0 cdfInv=0

# Radiation
LWout       units=W        min=0    max=1e4  mean=50   std=50    cdfX=0 cdfInv=0
LWin        units=W        min=0    max=1e4  mean=50   std=50    cdfX=0 cdfInv=0
SWout       units=W        min=0    max=1e4  mean=100  std=100   cdfX=0 cdfInv=0
SWin        units=W        min=0    max=1500 mean=200  std=100
SWin6       units=W        min=0    max=1e4  mean=100  std=100   cdfX=0 cdfInv=0
Albedo      units=%        min=0    mean=20 max=100 std=25 description=Albedo

# Hydrology/ocean
WaterRunoff units=kg/m^2   min=0    max=1e3  mean=1    std=1    cdfX=0 cdfInv=0
WaveHeightMax units=m
SigWaveHeight units=m
TailWaterElevation units=m
WavePeriod units=s
WaterParticulate units=?
TWater  units=^oC
Inflow  units=m^3/s       min=0  max=5000 mean=50 std=20

# Topography
Elevation        units=m
ForebayElevation units=m

# Lorenz63
LorenzX min=-100 max=100
LorenzY min=-100 max=100
LorenzZ min=-100 max=100
