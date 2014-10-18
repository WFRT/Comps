Albedo      name=al_sfc_0
BLH         name=hpbl_sfc_0
CAPE        name=cape_sfc_0
CIN         name=cin_sfc_0
LWin        name=dlwrf_sfc_0
LWout       name=ulwrf_sfc_0
MSLP        name=prmsl_sfc_0 scale=0.001
O3Total     name=tozne_200_0
P           name=sp_sfc_0 scale=0.001
Precip      name=prate_sfc_0
PrecipConv  name=cprat_sfc_0
PrecipWater name=pwat_200_0
RH          name=rh_sfc_0
SH          name=q_sfc_2
SLI         name=lftx_sfc_0
SWin        name=dswrf_sfc_0
SWout       name=uswrf_sfc_0
T           name=2t_sfc_2 offset=-273.15
T_Max3      name=tmax_sfc_2
T_Min3      name=tmin_sfc_2
TPot        name=pt_104_1
U           name=10u_sfc_10
Vort        name=absv_pl_500
W50         name=w_pl_500
WaterRunoff name=watr_sfc_0
WindGust    name=gust_sfc_0
Z50         name=gh_pl_500

# Other GFS variables that haven't been define in COMPS
# V doesn't work, perhaps U,V are coupled?
#V name=10v_sfc_10 
#O3          name=o3mr_pl_100
#U1 name=crain_sfc_0
#U3 name=U-component_of_wind
#U4 name=U-component_of_wind_altitude_above_msl
#U5 name=U-component_of_wind_height_above_ground
#U6 name=U-component_of_wind_potential_vorticity_surface
#U7 name=u_6_0
#U9 name=u_108_3000
#U10 name=U-component_of_wind_sigma
#U11 name=v_7_0
#U12 name=4lftx_sfc_undef
#U14 name=cfrzr_sfc_undef
#U16 name=wilt_sfc_undef
#U18 name=tp_sfc_undef
#U19 name=icec_sfc_undef
#U21 name=ustm_sfc_6000
#U22 name=gflux_sfc_undef
#U25 name=lhtfl_sfc_undef
# Total cloud cover boundary layer:
#U26 name=tcc_211_0
#U27 name=tcc_244_0
#U28 name=tcc_200_0
#U29 name=tcc_234_0
#U30 name=tcc_214_0
#U31 name=tcc_224_0
#U33 name=uflx_sfc_undef
#U36 name=cin_108_18000
# There are several messages at the same level
#U37 name=soilw_sfc_0
#U38 name=clwmr_pl_1000
#U39 name=sdwe_sfc_undef
#U40 name=5wava_pl_500
#U41 name=SunShine_Duration
#U43 name=csnow_sfc_undef
#U45 name=q_108_3000
#U52 name=acpcp_sfc_undef
#U53 name=5wavh_pl_500
# Is this one at the surface?:
#U54 name=vwsh_pv_2
#U55 name=vwsh_7_0
#U56 name=u-gwd_sfc_undef
#U58 name=gpa_pl_1000
#U59 name=cwat_200_0
#U60 name=shtfl_sfc_undef
# Updrd long wave rad flux nominal top (not surface):
#U63 name=ulwrf_ntop_0
#U64 name=cicep_sfc_undef
#U65 name=v-gwd_sfc_undef
#U66 name=vstm_sfc_6000
#U69 name=uswrf_ntop_0
# Storm relative helicity at 1000m:
#U70 name=hlcy_sfc_1000
#U74 name=pevpr_sfc_0
#U76 name=cwork_200_0
#U79 name=vflx_sfc_undef
#U80 name=V-component_of_wind
#U81 name=V-component_of_wind_altitude_above_msl
#U82 name=V-component_of_wind_height_above_ground
#U83 name=V-component_of_wind_potential_vorticity_surface
#U84 name=v_6_0
#U86 name=v_108_3000
#U87 name=V-component_of_wind_sigma
#U88 name=v_7_0
#U90 name=cape_108_18000
#U91 name=icaht_6_0
#U92 name=icaht_7_0
#U93 name=fldcp_sfc_0
# Vertical velocity at 1000:
#U95 name=w_104_1
