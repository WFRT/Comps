# Standard variables
T           units=^oC      min=-50  max=45   mean=0    std=5    description=Surface_temperature  standardName=air_temperature
TMin        units=^oC      min=-100 max=80   mean=0    std=5    description=Surface_temperature_minimum standardName=air_temperature
TMax        units=^oC      min=-100 max=80   mean=0    std=5    description=Surface_temperature_maximum standardName=air_temperature
TMin24      units=^oC      min=-100 max=80   mean=0    std=5    description=Minimum_temperature_daily   standardName=air_temperature
TMax24      units=^oC      min=-100 max=80   mean=0    std=5    description=Maximum_temperature_daily   standardName=air_temperature
Precip      units=mm       min=0    max=50   mean=2    std=10   description=Hourly_precipitation lowerDiscrete  standardName=precipitation_amount
PrecipConv  units=mm       min=0    mean=2   max=50    std=1e-5 description=Convective_precipitation_rate lowerDiscrete standardName=convective_precipitation_amount
TDew        units=^oC      min=-100 max=80   mean=0    std=5    description=Dewpoint_temperature  standardName=dew_point_temperature
FreezeLvl   units=m        min=0    max=10000 mean=1000 std=1000                                  standardName=freezing_level_altitude
Z50         units=gpm      min=3000 mean=5400 max=12000 std=1000 description=Geopotential_height_50kpa   standardName=geopotential_height
TWet        units=^oC      min=-100 max=80   mean=0    std=5    description=Wetbulb_temperature          standardName=wet_bulb_temperature
BLH         units=m        min=0    max=10000 mean=600 std=200 description=Boundary_layer_height         standardName=atmosphere_boundary_layer_thickness
Vort        units=1/s      min=-1e-4 mean=0 max=1e-4 std=1e-5 description=Absolute_vorticity             standardName=atmosphere_absolute_vorticity

# Temperature
TPot        units=NoUnit   min=226.606 mean=273.9319 max=307.064 std=6.4936 description=Potential_temperature  standardName=air_potential_temperature
T0          units=^oC      min=-50  max=45   mean=0    std=5    description=Skin_temperature  standardName=air_temperature

# Humidity
RH          units=%        min=0    max=100  mean=50   std=10 lowerDiscrete  upperDiscrete  standardName=relative_humidity
SH          units=g/g      min=0    max=0.1  mean=0.001   std=0.001   lowerDiscrete         standardName=humidity_mixing_ratio
Visibility  units=m

# Pressure
P           units=kPa      min=50  mean=100 max=120 std=2 description=Pressure_surface      standardName=air_pressure
MSLP        units=kPa      min=92  mean=100 max=108 std=2 description=Pressure_reduced_to_mean_sea_level_pressure standardName=air_pressure_at_sea_level

# Wind
StreamFlow  units=?
U           units=m/s      min=-300 max=300  mean=0    std=5 standardName=eastward_wind
V           units=m/s      min=-300 max=300  mean=0    std=5 standardName=northward_wind
W           units=m/s      min=-300 max=300  mean=0    std=5
WindChill   units=^oC      min=-50  max=50   mean=0    std=5
WindDir     units=^o       min=0    max=360  mean=180  std=30   circular  standardName=wind_direction
WindGust    units=m/s      min=0    max=300  mean=5    std=5 description=Wind_gust lowerDiscrete  standardName=wind_speed_of_gust
WindSpeed   units=m/s      min=0    max=300  mean=5    std=5 lowerDiscrete                        standardName=wind_speed
W50         units=Pa/s     min=-50  max=50   mean=0    std=0.2 description=Vertical_velocity_50kPa
TKE         units=m2/s2    min=0    max=20   mean=0.1  std=0.2

# Energy
Power       units=m/2      min=0    max=1e10 mean=0    std=1e5 lowerCutOffSpeed=0 upperCutOffSpeed=10 turbineRadius=20 lowerDiscrete

# Air quality
CO2         units=kg/m3    description=Carbon_dioxide       standardName=mass_concentration_of_carbon_dioxide_in_air
H2S         units=kg/m3    description=Hydrogen_disulphie
NO          units=kg/m3    description=Nitrogen_oxide       standardName=mass_concentration_of_nitrogen_monoxide_in_air
NO2         units=kg/m3    description=Nitrogen_dioxide     standardName=mass_concentration_of_nitrogen_dixide_in_air
O3Total     units=Dobson   min=100 mean=300  max=1000 std=50 description=Colun_integrated_ozone
O3          units=g/g      min=0   mean=1e-6 max=1e-5 std=1e-6 description=Ozone_Mixing_Ratio standardName=mass_fraction_of_ozone_in_air
PM10        units=kg/m3    standardName=mass_concentration_of_particulate_organic_matter_dry_aerosol_in_air
PM25        units=kg/m3    standardName=mass_concentration_of_particulate_organic_matter_dry_aerosol_in_air
SO2         units=kg/m3    standardName=mass_concentration_of_sulfur_dixide_in_air

# Precipitation
Precip3     units=mm       min=0    max=1000  mean=1   std=1   description=Hourly_precipitation_3hours lowerDiscrete   standardName=precipitation_amount 
Precip6     units=mm       min=0    max=1000  mean=1   std=1   description=Hourly_precipitation_6hours lowerDiscrete   standardName=precipitation_amount 
Precip12    units=mm       min=0    max=1000  mean=1   std=1   description=Hourly_precipitation_12hours lowerDiscrete  standardName=precipitation_amount 
Precip24    units=mm       min=0    max=1000  mean=1   std=1   description=Hourly_precipitation_24hours lowerDiscrete  standardName=precipitation_amount 
PrecipAcc   units=mm       min=0    max=1000 mean=10   std=50  description=24h_precipitation lowerDiscrete            standardName=precipitation_amount 
SnowDepth   units=m
PrecipWater units=mm       min=0    max=1000 mean=1 description=Precipitable_water lowerDiscrete

# Snow
PrecipSolid units=cm       min=0    max=1000  mean=1   std=1   description=Hourly_precipitation_snow lowerDiscrete  standardName=snowfall_amount
TSnow       units=^oC      min=-100 max=80   mean=0    std=5   description=Snow_temperature                        standardName=snow_temperature
TSnowPack   units=^oC      min=-100 max=80   mean=0    std=5   description=Snow_pack_temperature
SnowWaterEq units=?
SnowTotal   units=m

# Convective
CIN         units=J/kg     min=-1e6 mean=-5  max=1e3 std=30    description=Convective_inhibition                       standardName=atmosphere_convective_inhibition
CAPE        units=J/kg     min=0    mean=100 max=1e6 std=300   description=Convective_available_potential_energy      
SLI         units=^oC      min=-30  mean=0   max=50  std=5     description=Surface_lifted_index

# Clouds
CloudCover  units=1        min=0    max=1    mean=0.5  std=0.1 standardName=cloud_area_fraction
CloudWater  units=kg/m^2   min=0    max=100  mean=0.1  std=10
CloudHigh   units=1        min=0    max=1    mean=0.5  std=0.1 standardName=cloud_area_fraction
CloudMedium units=1        min=0    max=1    mean=0.5  std=0.1 standardName=cloud_area_fraction
CloudLow    units=1        min=0    max=1    mean=0.5  std=0.1 standardName=cloud_area_fraction
CloudFog    units=1        min=0    max=1    mean=0.5  std=0.1 standardName=fog_area_fraction # Is the standard name correct?

# Radiation
LWout       units=W        min=0    max=1e4  mean=50   std=50  standardName=surface_upwelling_longwave_flux_in_air
LWin        units=W        min=0    max=1e4  mean=50   std=50  standardName=surface_downwelling_longwave_flux_in_air
SWout       units=W        min=0    max=1e4  mean=100  std=100 standardName=surface_upwelling_shortwave_flux_in_air
SWin        units=W        min=0    max=1500 mean=200  std=100 standardName=surface_upwelling_shortwave_flux_in_air
SWin6       units=W        min=0    max=1e4  mean=100  std=100 standardName=surface_upwelling_shortwave_flux_in_air
Albedo      units=%        min=0    mean=20 max=100 std=25 description=Albedo standardName=albedo

# Hydrology/ocean
WaterRunoff units=kg/m^2   min=0    max=1e3  mean=1    std=1
WaveHeightMax units=m
SigWaveHeight units=m
TailWaterElevation units=m
WavePeriod units=s                 standardName=sea_surface_wind_wave_period
WaterParticulate units=?
TWater  units=^oC                  standardName=sea_water_temperature
Inflow  units=m^3/s       min=0  max=5000 mean=50 std=20

# Topography
Elevation        units=m      standardName=surface_altitude
ForebayElevation units=m

# Lorenz63
LorenzX min=-100 max=100
LorenzY min=-100 max=100
LorenzZ min=-100 max=100
