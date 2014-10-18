COMPS
=====

The component-based post-processing system is a tool for improving weather forecasts through
post-processing. The system uses a modular approach, where components operate on
weather-model and observation datasets to produce optimized deterministic, en- semble, and
probabilistic forecasts. COMPS has post-processing components such as downscaling, ensemble
bias-correction, creating probabilistic forecast by representing forecast uncertainty, and
probability calibration. The system processes datasets in gridded or point form and can read
commonly used data formats such as GRIB and NetCDF.

The system is under development. A stable version has not been released yet.

## Installation
Full details here: http://wfrt.github.io/Comps/tutorials/basics/installing.html

Short summary:

1. csh initSchemes.csh
2. scons comps.exe
