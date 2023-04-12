home = /home/fct/
www  = $(home)www/
src  = $(home)src/
lib  = $(home)src/lib/
bin  = $(home)www/cgi-bin/url/

lib_dir = $(www)cgi-src/lib/
cgi_dir = $(www)cgi-src/lib/
aws_dir = $(www)cgi-src/lib/
map_dir = $(src)lib/map/

cgi_lib  = $(lib)cgiutil.c
nr_lib   = $(lib)nrutil.c
time_lib = $(lib)seq_time.c
stn_lib  = $(lib)stn_inf_read.c
aws_lib  = $(lib)aws_io.c
aws2_lib = $(lib)aws2_io.c
aws3_lib = $(lib)aws3_io.c
awso_lib = $(lib)awso_io.c
vis2_lib = $(lib)vis2_io.c
url_lib  = $(lib)url_io.c
zoom_lib = $(lib_dir)grid_zooming.c
map_lib  = $(map_dir)lamcproj.c
map1_lib = $(map_dir)albeproj.c $(map_dir)lamcproj.c $(map_dir)azedproj.c $(map_dir)sterproj.c
gd_lib   = $(cgi_dir)gd_map.c
cmn_lib  = $(cgi_dir)gd_util.c $(cgi_dir)disp_htm.c

grid_inp      = $(cgi_dir)grid_inp_get.c
grid_disp_img = $(cgi_dir)grid_disp_img.c
grid_map_inf  = $(cgi_dir)grid_map_inf.c
grid_stn      = $(cgi_dir)grid_stn.c
grid_lib  = $(cgi_dir)grid_inp_get.c $(cgi_dir)grid_disp_html.c $(cgi_dir)grid_disp_img.c $(cgi_dir)grid_img.c $(cgi_dir)grid_making.c $(cgi_dir)grid_zooming.c $(cgi_dir)grid_stn.c $(cgi_dir)grid_map_img.c $(cgi_dir)grid_map_inf.c $(cgi_dir)grid_color.c
grid_data_lib = $(lib)grid_data_lib.c

INCLUDEDIRS=-I. -I/usr/include -I/usr/local/include -I/usr/local/trmm/include -I$(src)include -I$(www)cgi-src/include -I$(src)lib/map
LIBDIRS=-L. -L/usr/lib -L/usr/lib64 -L/usr/local/trmm/lib -L$(src)map
LIBS=-lm -lgd -lpng -lz 

NC_INCLUDEDIRS=-I. -I/fct/ftd/lib/netcdf-c-4.6.2/include -I/fct/ftd/lib/hdf5-1.8.21/include -I/fct/ftd/lib/zlib-1.2.8/include
NC_LIBDIRS=-L. -L/fct/ftd/lib/netcdf-c-4.6.2/lib -L/fct/ftd/lib/hdf5-1.8.21/lib -L/fct/ftd/lib/zlib-1.2.8/lib

gz_lib = /usr/local/zlib/lib/libz.a
INCLUDEDIRS2=-I. -I$(src)include -I$(www)cgi-src/include -I/usr/local/netcdf/include -I/usr/local/hdf5/include -I/usr/local/zlib/include
LIBDIRS2=-L. -L/usr/local/netcdf/lib -L/usr/local/hdf5/lib -L/usr/local/zlib/lib -Wl,-rpath,/usr/local/netcdf/lib -Wl,-rpath,/usr/local/hdf5/lib

INCLUDEDIRS3=-I. -I/usr/include -I/usr/local/include -I/usr/local/trmm/include -I$(src)include -I$(www)cgi-src/include -I$(src)lib/map -I/AFS_BK/AFS/NWA/LIB/eccodes-2.9.2/include -I/usr/local/netcdf/include -I/usr/local/hdf5/include -I/usr/local/zlib/include
LIBDIRS3=-L. -L/usr/lib -L/usr/lib64 -L/usr/local/trmm/lib -L$(src)map -L/AFS_BK/AFS/NWA/LIB/eccodes-2.9.2/lib -L/usr/local/netcdf/lib -L/usr/local/hdf5/lib -L/usr/local/zlib/lib -Wl,-rpath,/usr/local/netcdf/lib -Wl,-rpath,/usr/local/hdf5/lib
LINKS=-Wl,-rpath,/AFS_BK/AFS/NWA/LIB/eccodes-2.9.2/lib 

# AWS3 매분자료(운고,운량) 제공
aws3_min_cloud: aws3_min_cloud.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws3_min_cloud  aws3_min_cloud.c $(aws3_lib) $(url_lib) $(time_lib) $(cgi_lib) $(nr_lib) -lm -lz -lcurl

# AWS3 기반의 해면기압 산출 
aws3_ps_calc: aws3_ps_calc.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws3_ps_calc  aws3_ps_calc.c $(aws3_lib) $(url_lib) $(time_lib) $(cgi_lib) -lm -lz -lcurl

# AWS3 기반의 10분평균 풍향풍속 산출 
aws3_wd10_calc: aws3_wd10_calc.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws3_wd10_calc  aws3_wd10_calc.c $(aws3_lib) $(time_lib) $(cgi_lib) -lm -lz

# AWS3 기반의 누적강수량 산출 
aws3_rain_calc: aws3_rain_calc.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws3_rain_calc  aws3_rain_calc.c $(aws3_lib) $(time_lib) $(cgi_lib) -lm -lz

#------------------------------------------------------------------------------------------

# 앙상블기반 분석결과 지점별 자료 표출
ens_data_stns: ens_data_stns.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-ens_data_stns  ens_data_stns.c $(time_lib) $(map_lib) $(cgi_lib) $(stn_lib) $(nr_lib) $(LIBS)

# 앙상블기반 계급별 지점별 자료 표출
ens_prob_stns: ens_prob_stns.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-ens_prob_stns  ens_prob_stns.c $(time_lib) $(map_lib) $(cgi_lib) $(stn_lib) $(nr_lib) $(LIBS)

#------------------------------------------------------------------------------------------

#
# DFS 자료 제공
#
dfs_shrt_stn: dfs_shrt_stn.c 
	gcc $(INCLUDEDIRS) -o $(bin)nph-dfs_shrt_stn  dfs_shrt_stn.c $(time_lib) $(cgi_lib) -lm -lz

dfs_shrt_stn_test: dfs_shrt_stn.c 
	gcc $(INCLUDEDIRS) -o $(bin)nph-dfs_shrt_stn_test  dfs_shrt_stn.c $(time_lib) $(cgi_lib) -lm -lz

dfs_shrt_grd_pnt: dfs_shrt_grd_pnt.c 
	gcc $(INCLUDEDIRS) -o $(bin)nph-dfs_shrt_grd_pnt  dfs_shrt_grd_pnt.c $(time_lib) $(cgi_lib) -lm -lz

dfs_shrt_grd_stn: dfs_shrt_grd_stn.c 
	gcc $(INCLUDEDIRS) -o $(bin)nph-dfs_shrt_grd_stn  dfs_shrt_grd_stn.c $(time_lib) $(cgi_lib) $(stn_lib) $(map_lib) $(url_lib) -lm -lz -lcurl

dfs_shrt_grd_stn_tmp: dfs_shrt_grd_stn.tmp.c 
	gcc $(INCLUDEDIRS) -o $(bin)nph-dfs_shrt_grd_stn_tmp  dfs_shrt_grd_stn.tmp.c $(time_lib) $(cgi_lib) $(stn_lib) $(map_lib) $(url_lib) -lm -lz -lcurl

dfs_shrt_grd_stn2: dfs_shrt_grd_stn2.c 
	gcc $(INCLUDEDIRS) -o $(bin)nph-dfs_shrt_grd_stn2  dfs_shrt_grd_stn2.c $(time_lib) $(cgi_lib) $(stn_lib) $(map_lib) $(url_lib) -lm -lz -lcurl

dfs_shrt_grd_mnt: dfs_shrt_grd_mnt.c 
	gcc $(INCLUDEDIRS) -o $(bin)nph-dfs_shrt_grd_mnt  dfs_shrt_grd_mnt.c $(time_lib) $(cgi_lib) -lm -lz

dfs_vsrt_stn: dfs_vsrt_stn.c 
	gcc $(INCLUDEDIRS) -o $(bin)nph-dfs_vsrt_stn  dfs_vsrt_stn.c $(time_lib) $(cgi_lib) -lm -lz

dfs_odam_stn: dfs_odam_stn.c 
	gcc $(INCLUDEDIRS) -o $(bin)nph-dfs_odam_stn  dfs_odam_stn.c $(time_lib) $(cgi_lib) -lm -lz

dfs_obs_var: dfs_obs_var.c 
	gcc $(INCLUDEDIRS) -o $(bin)nph-dfs_obs_var  dfs_obs_var.c $(time_lib) $(cgi_lib) -lm -lz

# DFS 격자자료 제공
dfs_shrt_grd: dfs_shrt_grd.c 
	gcc $(INCLUDEDIRS) -o $(bin)nph-dfs_shrt_grd  dfs_shrt_grd.c $(time_lib) $(cgi_lib) $(nr_lib) -lm -lz

dfs_shrt_grd_test: dfs_shrt_grd.c 
	gcc $(INCLUDEDIRS) -o $(bin)nph-dfs_shrt_grd_test  dfs_shrt_grd.c $(time_lib) $(cgi_lib) $(nr_lib) -lm -lz

# DFS 초단기QPE 격자자료 제공
dfs_qpe_grd: dfs_qpe_grd.c 
	gcc $(INCLUDEDIRS) -o $(bin)nph-dfs_qpe_grd  dfs_qpe_grd.c $(time_lib) $(cgi_lib) $(nr_lib) -lm -lz
	
# DFS 중기 격자자료 제공
dfs_medm_grd_stn: dfs_medm_grd_stn.c 
	gcc $(INCLUDEDIRS) $(NC_INCLUDEDIRS) $(NC_LIBDIRS) -o $(bin)nph-dfs_medm_grd_stn  dfs_medm_grd_stn.c $(time_lib) $(cgi_lib) $(stn_lib) $(map_lib) $(url_lib) -Wl,-rpath,/fct/ftd/lib/netcdf-c-4.6.2/lib -lm -lz -lcurl -lnetcdf

# DFS 중기 지점자료 제공
dfs_medm_stn2: dfs_medm_stn2.c 
	gcc $(INCLUDEDIRS) -o $(bin)nph-dfs_medm_stn2  dfs_medm_stn2.c $(time_lib) $(cgi_lib) $(stn_lib) $(map_lib) $(url_lib) -lm -lz -lcurl
	
#------------------------------------------------------------------------------------------

#
# MOS 자료 제공
#
mos_stn: mos_stn.c 
	gcc $(INCLUDEDIRS) -o $(bin)nph-mos_stn  mos_stn.c $(time_lib) $(cgi_lib) -lm -lz

#
# UM EPSG 자료 제공
#
um_epsg_txt: um_epsg_txt.c 
	gcc $(INCLUDEDIRS) -o $(bin)nph-um_epsg_txt  um_epsg_txt.c $(time_lib) $(cgi_lib) -lm -lz

#
# UM EPSG SVR처리자료 제공 (국내)
#
um_epsg_svrk: um_epsg_svrk.c 
	gcc $(INCLUDEDIRS) -o $(bin)nph-um_epsg_svrk  um_epsg_svrk.c $(time_lib) $(cgi_lib) -lm -lz

#
# UM EPSG SVR처리자료 제공 (국외)
#
um_epsg_svrg: um_epsg_svrg.c 
	gcc $(INCLUDEDIRS) -o $(bin)nph-um_epsg_svrg  um_epsg_svrg.c $(time_lib) $(cgi_lib) -lm -lz

#
# ECMWF EPSG 자료 제공
#
ecm_epsg_txt: ecm_epsg_txt.c 
	gcc $(INCLUDEDIRS) -o $(bin)nph-ecm_epsg_txt  ecm_epsg_txt.c $(time_lib) $(cgi_lib) -lm -lz

#------------------------------------------------------------------------------------------

# GRIB-1 헤더정보 표출
nwp_grb1_inf: nwp_grb1_inf.c 
	gcc $(INCLUDEDIRS) -o $(bin)nph-nwp_grb1_inf  nwp_grb1_inf.c $(time_lib) $(cgi_lib) $(nr_lib) -lm -lz

# 수치모델 GRIB-1 자료 해독 및 제공 (2차원 격자자료)
nwp_grb1_xy_txt1: nwp_grb1_xy_txt1.c nwp_grb1_xy_lib.c nwp_grb1_io.c 
	gcc $(INCLUDEDIRS) -o $(bin)nph-nwp_grb1_xy_txt1  nwp_grb1_xy_txt1.c nwp_grb1_xy_lib.c nwp_grb1_io.c $(time_lib) $(map_lib) $(cgi_lib) $(nr_lib) -lm -lz

# 수치모델 GRIB-2 자료 해독 및 제공 (2차원 격자자료) 1에서 출력 수정
nwp_grb1_xy_txt2: nwp_grb1_xy_txt2.c nwp_grb1_xy_lib.c nwp_grb1_io.c 
	gcc $(INCLUDEDIRS) -o $(bin)nph-nwp_grb1_xy_txt2  nwp_grb1_xy_txt2.c nwp_grb1_xy_lib.c nwp_grb1_io.c $(time_lib) $(map_lib) $(cgi_lib) $(nr_lib) -lm -lz

nwp_grb1_xy_img1: nwp_grb1_xy_img1.c nwp_grb1_xy_lib.c nwp_grb1_io.c 
	gcc $(INCLUDEDIRS) -o $(bin)nph-nwp_grb1_xy_img1  nwp_grb1_xy_img1.c nwp_grb1_xy_lib.c nwp_grb1_io.c $(time_lib) $(map_lib) $(cgi_lib) $(nr_lib) $(LIBS)

# 수치모델 GRIB 자료 해독 및 제공 (지점자료)
nwp_grb1_pt_txt1: nwp_grb1_pt_txt1.c nwp_grb1_pt_lib.c nwp_grb1_io.c 
	gcc $(INCLUDEDIRS) -o $(bin)nph-nwp_grb1_pt_txt1  nwp_grb1_pt_txt1.c nwp_grb1_pt_lib.c nwp_grb1_io.c $(time_lib) $(map_lib) $(cgi_lib) $(nr_lib) -lm -lz

# UM수치모델 GRIB 자료 해독 및 제공 (단면도)
nwp_grb1_xz_txt1: nwp_grb1_xz_txt1.c nwp_grb1_xy_lib.c nwp_grb1_io.c 
	gcc $(INCLUDEDIRS) -o $(bin)nph-nwp_grb1_xz_txt1  nwp_grb1_xz_txt1.c nwp_grb1_xy_lib.c nwp_grb1_io.c $(time_lib) $(map_lib) $(cgi_lib) $(nr_lib) -lm -lz

#------------------------------------------------------------------------------------------

# UM모델 GRIB 헤더정보 표출
########################################################################################################
um_grib_inf: um_grib_inf.c 
	gcc $(INCLUDEDIRS) -o $(bin)nph-um_grib_inf  um_grib_inf.c $(time_lib) $(map_lib) $(cgi_lib) $(nr_lib) -lm -lz

# UM수치모델 GRIB 자료 해독 및 제공 (2차원 격자자료)
um_grib_xy_txt1: um_grib_xy_txt1.c um_grib_xy_lib.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-um_grib_xy_txt1  um_grib_xy_txt1.c um_grib_xy_lib.c um_grib_io.c $(time_lib) $(map_lib) $(cgi_lib) $(nr_lib) -lm -lz

# UM수치모델 GRIB 자료 해독 및 제공 (2차원 GRIB자료)
um_grib_file_down: um_grib_file_down.c 
	gcc $(INCLUDEDIRS) -o $(bin)um_grib_file_down  um_grib_file_down.c $(time_lib) $(map_lib) $(cgi_lib) $(nr_lib) -lm -lz

um_grib_xy_img1: um_grib_xy_img1.c um_grib_xy_lib.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-um_grib_xy_img1  um_grib_xy_img1.c um_grib_xy_lib.c um_grib_io.c $(time_lib) $(map_lib) $(cgi_lib) $(nr_lib) $(LIBS)

# UM수치모델 GRIB 자료 해독 및 제공 (지점자료)
um_grib_pt_txt1: um_grib_pt_txt1.c um_grib_pt_lib.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-um_grib_pt_txt1  um_grib_pt_txt1.c um_grib_pt_lib.c um_grib_io.c $(time_lib) $(map_lib) $(cgi_lib) $(nr_lib) -lm -lz

# UM수치모델 GRIB 자료 해독 및 제공 (단면도)
um_grib_xz_txt1: um_grib_xz_txt1.c um_grib_xy_lib.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-um_grib_xz_txt1  um_grib_xz_txt1.c um_grib_xy_lib.c um_grib_io.c $(time_lib) $(map_lib) $(cgi_lib) $(nr_lib) -lm -lz

#  UM수치모델 GRIB의 헤더정보 분리
um_grib_header: um_grib_header.c
	gcc $(INCLUDEDIRS) -o um_grib_header  um_grib_header.c $(time_lib) $(map_lib) $(cgi_lib) $(nr_lib) -lm -lz
#======================================================================================================#
um_grib_inf_test: um_grib_inf.c 
	gcc $(INCLUDEDIRS) -o $(bin)nph-um_grib_inf_test  um_grib_inf.c $(time_lib) $(map_lib) $(cgi_lib) $(nr_lib) -lm -lz

# UM수치모델 GRIB 자료 해독 및 제공 (2차원 격자자료)
um_grib_xy_txt1_test: um_grib_xy_txt1.c um_grib_xy_lib.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-um_grib_xy_txt1_test  um_grib_xy_txt1.c um_grib_xy_lib.c um_grib_io.c $(time_lib) $(map_lib) $(cgi_lib) $(nr_lib) -lm -lz

# UM수치모델 GRIB 자료 해독 및 제공 (2차원 GRIB자료)
um_grib_file_down_test: um_grib_file_down.c 
	gcc $(INCLUDEDIRS) -o $(bin)um_grib_file_down_test  um_grib_file_down.c $(time_lib) $(map_lib) $(cgi_lib) $(nr_lib) -lm -lz

um_grib_xy_img1_test: um_grib_xy_img1.c um_grib_xy_lib.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-um_grib_xy_img1_test  um_grib_xy_img1.c um_grib_xy_lib.c um_grib_io.c $(time_lib) $(map_lib) $(cgi_lib) $(nr_lib) $(LIBS)

# UM수치모델 GRIB 자료 해독 및 제공 (지점자료)
um_grib_pt_txt1_test: um_grib_pt_txt1.c um_grib_pt_lib.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-um_grib_pt_txt1_test  um_grib_pt_txt1.c um_grib_pt_lib.c um_grib_io.c $(time_lib) $(map_lib) $(cgi_lib) $(nr_lib) -lm -lz

# UM수치모델 GRIB 자료 해독 및 제공 (단면도)
um_grib_xz_txt1_test: um_grib_xz_txt1.c um_grib_xy_lib.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-um_grib_xz_txt1_test  um_grib_xz_txt1.c um_grib_xy_lib.c um_grib_io.c $(time_lib) $(map_lib) $(cgi_lib) $(nr_lib) -lm -lz

#  UM수치모델 GRIB의 헤더정보 분리
um_grib_header_test: um_grib_header.c
	gcc $(INCLUDEDIRS) -o um_grib_header_test  um_grib_header.c $(time_lib) $(map_lib) $(cgi_lib) $(nr_lib) -lm -lz
########################################################################################################

um_test: um_test.c
	gcc $(INCLUDEDIRS) -o um_test  um_test.c $(time_lib) $(map_lib) $(cgi_lib) $(nr_lib) -lm -lz

# 수치모델 GRIB 자료 해독 및 제공
nwp_grib: nwp_grib.c
	gcc $(INCLUDEDIRS) -o $(bin)nwp_grib  nwp_grib.c $(time_lib) $(cgi_lib) -lm -lz

#------------------------------------------------------------------------------------------

# RTSM 격자점별 자료 제공
rtsm_grib_pt_txt1: rtsm_grib_pt_txt1.c rtsm_grib_pt_lib.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-rtsm_grib_pt_txt1  rtsm_grib_pt_txt1.c rtsm_grib_pt_lib.c $(time_lib) $(map_lib) $(cgi_lib) $(nr_lib) -lm -lz

# 2018.07.06 grib2 read test(rtsm_um12_*)
rtsm_grib_pt_txt1_test: rtsm_grib_pt_txt1.c rtsm_grib_pt_lib.20180706.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-rtsm_grib_pt_txt1_test  rtsm_grib_pt_txt1.c rtsm_grib_pt_lib.20180706.c $(time_lib) $(map_lib) $(cgi_lib) $(nr_lib) -lm -lz

#------------------------------------------------------------------------------------------

# AWS 시간별 풍향,풍속(수치모델 및 가이던스 검증용)
afs_aws_hr_wd: afs_aws_hr_wd.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-afs_aws_hr_wd  afs_aws_hr_wd.c $(aws2_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz

afs_aws_hr_wd_aws3: afs_aws_hr_wd.aws3.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-afs_aws_hr_wd_aws3  afs_aws_hr_wd.aws3.c $(aws3_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz

#------------------------------------------------------------------------------------------

# AWS 시간통계:습도 계산
aws2_hr_hm: aws2_hr_hm.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws2_hr_hm  aws2_hr_hm.c $(aws2_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz

aws2_hr_hm_aws3: aws2_hr_hm.aws3.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws2_hr_hm_aws3  aws2_hr_hm.aws3.c $(aws3_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz

#------------------------------------------------------------------------------------------

# AWS 매분자료 제공
aws_min: aws_min.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws_min  aws_min.c $(aws_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz

# AWS2 매분자료 제공
aws2_min: aws2_min.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws2_min  aws2_min.c $(aws2_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz

# AWS2 매분자료 제공
aws2_min_aws3: aws2_min.aws3.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws2_min_aws3  aws2_min.aws3.c $(aws3_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz

# AWS3 매분자료 제공
aws3_min: aws3_min.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws3_min  aws3_min.c $(aws3_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz

# AWS3 매분자료(원시자료) 제공
aws3_min_raw: aws3_min_raw.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws3_min_raw  aws3_min_raw.c $(aws3_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz

# AWS2 매분자료(원시자료) 제공
aws2_min_raw: aws2_min_raw.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws2_min_raw  aws2_min_raw.c $(aws2_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz

aws2_min_raw_aws3: aws2_min_raw.aws3.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws2_min_raw_aws3  aws2_min_raw.aws3.c $(aws3_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz

aws2_min_obs: aws2_min_obs.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws2_min_obs  aws2_min_obs.c $(aws2_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz

aws2_min_obs_aws3: aws2_min_obs.aws3.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws2_min_obs_aws3  aws2_min_obs.aws3.c $(aws3_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz

# AWS2 매분자료(운고,운량) 제공
aws2_min_cloud: aws2_min_cloud.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws2_min_cloud  aws2_min_cloud.c $(aws2_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz

aws2_min_cloud_aws3: aws2_min_cloud.aws3.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws2_min_cloud_aws3  aws2_min_cloud.aws3.c $(aws3_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz

aws2_min_ca2: aws2_min_ca2.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws2_min_ca2  aws2_min_ca2.c $(aws2_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz

aws2_min_ca2_aws3: aws2_min_ca2.aws3.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws2_min_ca2_aws3  aws2_min_ca2.aws3.c $(aws3_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz

aws2_min_ca3: aws2_min_ca3.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws2_min_ca3  aws2_min_ca3.c $(aws2_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz

aws2_min_ca3_aws3: aws2_min_ca3.aws3.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws2_min_ca3_aws3  aws2_min_ca3.aws3.c $(aws3_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz

# AWS2 매분자료(일사,일조) 제공
aws2_min_si: aws2_min_si.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws2_min_si  aws2_min_si.c $(aws2_lib) $(vis2_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz

aws2_min_si_aws3: aws2_min_si.aws3.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws2_min_si_aws3  aws2_min_si.aws3.c $(aws3_lib) $(vis2_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz

# AWS2 매분자료(시정) 제공
aws2_min_vis: aws2_min_vis.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws2_min_vis  aws2_min_vis.c $(aws2_lib) $(vis2_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz

aws2_min_vis_test: aws2_min_vis.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws2_min_vis_test  aws2_min_vis.c $(aws2_lib) $(vis2_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz

aws2_min_vis2: aws2_min_vis2.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws2_min_vis2  aws2_min_vis2.c $(aws2_lib) $(vis2_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz

aws2_min_vis2_test: aws2_min_vis2.20190408.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws2_min_vis2_test  aws2_min_vis2.20190408.c $(aws2_lib) $(vis2_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz

aws2_min_vis3: aws2_min_vis3.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws2_min_vis3  aws2_min_vis3.c $(aws2_lib) $(vis2_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz
	
aws2_min_vis3_test: aws2_min_vis3.20190408.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws2_min_vis3_test  aws2_min_vis3.20190408.c $(aws2_lib) $(vis2_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz

aws3_min_vis3_test: aws3_min_vis3_test.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws3_min_vis3_test  aws3_min_vis3_test.c $(aws3_lib) $(vis2_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz	

# AWS 시정관련 데이터 추출(기상속보 자동화용)
aws2_vis_sum3: aws2_vis_sum3.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws2_vis_sum3  aws2_vis_sum3.c $(aws2_lib) $(vis2_lib) $(url_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz -lcurl
	
aws3_vis_sum3_test: aws3_vis_sum3_test.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws3_vis_sum3_test  aws3_vis_sum3_test.c $(aws3_lib) $(vis2_lib) $(url_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz -lcurl	

# AWS2 적설자료 제공
aws2_min_snow: aws2_min_snow.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws2_min_snow  aws2_min_snow.c $(aws2_lib) $(url_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz -lcurl

aws2_min_snow_aws3: aws2_min_snow.aws3.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws2_min_snow_aws3  aws2_min_snow.aws3.c $(aws3_lib) $(url_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz -lcurl

# AWS2 적설자료 제공(기상속보 자동화용)
aws2_min_snow2: aws2_min_snow2.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws2_min_snow2  aws2_min_snow2.c $(aws2_lib) $(url_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz -lcurl

aws2_min_snow2_aws3: aws2_min_snow2.aws3.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws2_min_snow2_aws3  aws2_min_snow2.aws3.c $(aws3_lib) $(url_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz -lcurl

aws2_min_snow_test: aws2_min_snow.20190312.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws2_min_snow_test  aws2_min_snow.20190312.c $(aws2_lib) $(url_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz -lcurl

# AWS2 적설자료 제공(시계열 표출용)
aws2_min_snow3: aws2_min_snow3.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws2_min_snow3  aws2_min_snow3.c $(aws2_lib) $(url_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz -lcurl

aws2_min_snow3_aws3: aws2_min_snow3.aws3.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws2_min_snow3_aws3  aws2_min_snow3.aws3.c $(aws3_lib) $(url_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz -lcurl

# AWS2 적설자료 제공(시계열 표출용) - 신적설계산방식수정
aws2_min_snow4: aws2_min_snow4.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws2_min_snow4  aws2_min_snow4.c $(aws2_lib) $(url_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz -lcurl

aws2_min_snow4_aws3: aws2_min_snow4.aws3.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws2_min_snow4_aws3  aws2_min_snow4.aws3.c $(aws3_lib) $(url_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz -lcurl

# AWS2 매분자료 (초상,지면,지주온도) 제공
aws2_min_lst: aws2_min_lst.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws2_min_lst  aws2_min_lst.c $(aws2_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz

aws2_min_lst_aws3: aws2_min_lst.aws3.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws2_min_lst_aws3  aws2_min_lst.aws3.c $(aws3_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz

# AWS2 현천자료 제공
aws2_min_ww1: aws2_min_ww1.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws2_min_ww1  aws2_min_ww1.c $(aws2_lib) $(vis2_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz

aws2_min_ww2: aws2_min_ww2.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws2_min_ww2  aws2_min_ww2.c $(aws2_lib) $(vis2_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz

aws2_min_ww3: aws2_min_ww3.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws2_min_ww3  aws2_min_ww3.c $(aws2_lib) $(vis2_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz

# AWS2 매분자료 (농업기상관측) 제공
aws2_min_agr: aws2_min_agr.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws2_min_agr  aws2_min_agr.c $(aws2_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz

# AWS2 매분자료 (농업기상관측) 제공
aws2_min_agr_aws3: aws2_min_agr.aws3.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws2_min_agr_aws3  aws2_min_agr.aws3.c $(aws3_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz

# AWS2 매분자료에서 임의 시간간격의 평균값을 제공
aws2_min_avg: aws2_min_avg.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws2_min_avg  aws2_min_avg.c $(aws2_lib) $(aws_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz

# AWS2 누적강수량 제공 (2018.07.17)
aws2_min_rain: aws2_min_rain.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws2_min_rain  aws2_min_rain.c $(aws2_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz

aws2_min_rain_aws3: aws2_min_rain.aws3.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws2_min_rain_aws3  aws2_min_rain.aws3.c $(aws3_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz

# AWS2 매분자료 제공(기상속보 자동화용)
aws2_txt_min3: aws2_txt_min3.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws2_txt_min3  aws2_txt_min3.c $(aws_lib) $(aws2_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz

aws2_txt_min3_aws3: aws2_txt_min3.aws3.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws2_txt_min3_aws3  aws2_txt_min3.aws3.c $(aws_lib) $(aws3_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz

#------------------------------------------------------------------------------------------

# 유관기관 매분자료 제공
awso_min: awso_min.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-awso_min  awso_min.c $(awso_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz

awso_min_test: awso_min.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-awso_min_test  awso_min.c $(aws3_lib) $(awso_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz

#------------------------------------------------------------------------------------------

# 레이더합성 강수량
rdr_cmp_rain: rdr_cmp_rain.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-rdr_cmp_rain  rdr_cmp_rain.c  $(zoom_lib) $(time_lib) $(cgi_lib) $(nr_lib) -lm -lz

# 레이더합성자료 제공
rdr_cmp_data: rdr_cmp_data.c
	gcc $(INCLUDEDIRS) -o $(bin)rdr_cmp_data  rdr_cmp_data.c $(time_lib) $(cgi_lib) -lm -lz

#  레이더 UF 파일 정보 추출
rdr_uf_inf: rdr_uf_inf.c 
	gcc $(INCLUDEDIRS) -o $(bin)nph-rdr_uf_inf  rdr_uf_inf.c $(time_lib) $(cgi_lib) $(LIBDIRS) -lrsl -lm -lz 

#  레이더 UF 파일 자료 제공
rdr_uf_data: rdr_uf_data.c 
	gcc $(INCLUDEDIRS) -o $(bin)nph-rdr_uf_data  rdr_uf_data.c $(time_lib) $(cgi_lib) $(LIBDIRS) -lrsl -lm -lz

#------------------------------------------------------------------------------------------

# 레이더 합성 신규포맷 자료(500m해상도,5분간격)
rdr_cmp1_api: rdr_cmp1_api.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-rdr_cmp1_api  rdr_cmp1_api.c $(time_lib) $(cgi_lib) $(nr_lib) -lm -lz
	
# 레이더 합성 경계값 이상 자료 조회
rdr_cmp1_thrhld_api: rdr_cmp1_thrhld_api.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-rdr_cmp1_thrhld_api  rdr_cmp1_thrhld_api.c $(time_lib) $(cgi_lib) $(nr_lib) $(map_lib) -lm -lz

# 레이더 합성 신규포맷 정보 추출
rdr_cmp_inf: rdr_cmp_inf.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-rdr_cmp_inf  rdr_cmp_inf.c $(time_lib) $(cgi_lib) $(LIBDIRS) -lm -lz

# 레이더 합성 여러 지점에 대한 시계열 자료 추출
rdr_stns: rdr_stns.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-rdr_stns  rdr_stns.c $(map_lib) $(stn_lib) $(time_lib) $(cgi_lib) $(nr_lib) -lm -lz -lcurl
	
# 레이더 합성 알람
rdr_cmp_search: rdr_cmp_search.c
	gcc $(INCLUDEDIRS) -I/fct/ftd/lib/proj-4.4.5/include -L. -L/fct/ftd/lib/proj-4.4.5/lib -o $(bin)nph-rdr_cmp_search  rdr_cmp_search.c $(time_lib) $(cgi_lib) $(nr_lib) $(map_lib) -Wl,-rpath,/fct/ftd/lib/proj-4.4.5/lib -lm -lz -lproj

#------------------------------------------------------------------------------------------

# 위성 NWPN 격자자료 제공
sat_data: sat_data.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-sat_data  sat_data.c $(time_lib) $(cgi_lib) $(nr_lib) -lm -lz

sat_data2: sat_data2.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-sat_data2  sat_data2.c $(time_lib) $(cgi_lib) $(nr_lib) -lm -lz
	
sat_search: sat_search.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-sat_search  sat_search.c $(time_lib) $(cgi_lib) $(nr_lib) -lm -lz

#초단기 격자자료
vsrt_data: vsrt_data.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-vsrt_data  vsrt_data.c $(time_lib) $(cgi_lib) $(nr_lib) -lm -lz

# 천리안 1개 위경도 지점에 대한 시계열 자료 추출
coms_pnt: coms_pnt.c 
	gcc $(INCLUDEDIRS) -o $(bin)nph-coms_pnt  coms_pnt.c $(map_lib) $(stn_lib) $(time_lib) $(cgi_lib) $(nr_lib) -lm -lz -lcurl

# 천리안 여러 지점에 대한 시계열 자료 추출
coms_stns: coms_stns.c 
	gcc $(INCLUDEDIRS) -o $(bin)nph-coms_stns  coms_stns.c $(map_lib) $(stn_lib) $(time_lib) $(cgi_lib) $(nr_lib) -lm -lz -lcurl

# 천리안기반 지점별 운량자료 산출
coms_stn_ca: coms_stn_ca.c 
	gcc $(INCLUDEDIRS) -o $(bin)nph-coms_stn_ca  coms_stn_ca.c $(map_lib) $(stn_lib) $(time_lib) $(cgi_lib) $(nr_lib) -lm -lz -lcurl

# 천리안기반 지점별 운고자료 산출
coms_stn_ch: coms_stn_ch.c 
	gcc $(INCLUDEDIRS) -o $(bin)nph-coms_stn_ch  coms_stn_ch.c $(map_lib) $(stn_lib) $(time_lib) $(cgi_lib) $(nr_lib) -lm -lz -lcurl

# 히마와리 1개 위경도 지점에 대한 시계열 자료 추출
hima_pnt: hima_pnt.c 
	gcc $(INCLUDEDIRS) -o $(bin)nph-hima_pnt  hima_pnt.c $(map_lib) $(stn_lib) $(time_lib) $(cgi_lib) $(nr_lib) -lm -lz -lcurl

# 히마와리 여러 지점에 대한 시계열 자료 추출
hima_stns: hima_stns.c 
	gcc $(INCLUDEDIRS) -o $(bin)nph-hima_stns  hima_stns.c $(map_lib) $(stn_lib) $(time_lib) $(cgi_lib) $(nr_lib) -lm -lz -lcurl

# 위성: 여러 지점에 대한 운량 자료 추정
sat_ca: sat_ca.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-sat_ca  sat_ca.c $(map_lib) $(stn_lib) $(time_lib) $(cgi_lib) $(nr_lib) $(url_lib) -lm -lz -lcurl

# 위성: 여러 지점에 대한 critical index 시계열 자료 추출
coms_stn_cridx: coms_stn_cridx.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-coms_stn_cridx  coms_stn_cridx.c $(map_lib) $(stn_lib) $(time_lib) $(cgi_lib) $(nr_lib) -lm -lz -lcurl

# GK2A 위성: 여러 지점에 대한 critical index 시계열 자료 추출
gk2a_stn_cridx: gk2a_stn_cridx.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-gk2a_stn_cridx  gk2a_stn_cridx.c $(map_lib) $(stn_lib) $(time_lib) $(cgi_lib) $(nr_lib) -lm -lz -lcurl

#
# KIM(한국형 수치모델) 자료 제공
#

# KIM수치모델 nc 자료 해독 및 제공 (텍스트/2차원 격자자료)
kim_nc_xy_txt1: kim_nc_xy_txt1.c kim_nc_xy_lib.c
	gcc $(INCLUDEDIRS) $(NC_INCLUDEDIRS) $(NC_LIBDIRS) -o $(bin)nph-kim_nc_xy_txt1  kim_nc_xy_txt1.c kim_nc_xy_lib.c kim_nc_io.c $(time_lib) $(map_lib) $(cgi_lib) $(nr_lib) -Wl,-rpath,/fct/ftd/lib/netcdf-c-4.6.2/lib -Wl,-rpath,/fct/ftd/lib/hdf5-1.8.21/lib -lnetcdf -lhdf5_hl -lhdf5 -ldl -lm -lz -lcurl

# KIM수치모델 nc 자료 해독 및 제공 (이미지/2차원 격자자료)
kim_nc_xy_img1: kim_nc_xy_img1.c kim_nc_xy_lib.c
	gcc $(INCLUDEDIRS) $(NC_INCLUDEDIRS) $(NC_LIBDIRS) -o $(bin)nph-kim_nc_xy_img1  kim_nc_xy_img1.c kim_nc_xy_lib.c kim_nc_io.c $(time_lib) $(map_lib) $(cgi_lib) $(nr_lib) -Wl,-rpath,/fct/ftd/lib/netcdf-c-4.6.2/lib -Wl,-rpath,/fct/ftd/lib/hdf5-1.8.21/lib -lnetcdf -lhdf5_hl -lhdf5 -ldl -lm -lz -lcurl -lpng -lgd

# KIM수치모델 nc 자료 해독 및 제공 (지점자료)
kim_nc_pt_txt1: kim_nc_pt_txt1.c kim_nc_pt_lib.c
	gcc $(INCLUDEDIRS) $(NC_INCLUDEDIRS) $(NC_LIBDIRS) -o $(bin)nph-kim_nc_pt_txt1  kim_nc_pt_txt1.c kim_nc_pt_lib.c kim_nc_pt_io.c $(time_lib) $(map_lib) $(cgi_lib) $(nr_lib) -Wl,-rpath,/fct/ftd/lib/netcdf-c-4.6.2/lib -Wl,-rpath,/fct/ftd/lib/hdf5-1.8.21/lib -lnetcdf -lhdf5_hl -lhdf5 -ldl -lm -lz -lcurl


# 블랜딩 QPF
qpf_ana_img: qpf_ana_img.c city_img_float.c 
	gcc $(INCLUDEDIRS) $(LIBDIRS) $(NC_INCLUDEDIRS) $(NC_LIBDIRS) -o $(bin)nph-qpf_ana_img  qpf_ana_img.c city_img_float.c $(grid_data_lib) $(map_lib) $(time_lib) $(cgi_lib) $(url_lib) $(nr_lib) $(gz_lib) -Wl,-rpath,/fct/ftd/lib/netcdf-c-4.6.2/lib -Wl,-rpath,/fct/ftd/lib/hdf5-1.8.21/lib -Wl,-rpath,/fct/ftd/lib/zlib-1.2.8/lib -lm -lgd -lpng -lcurl -lnetcdf -lhdf5 -lhdf5_hl

# 초단기예보 10분 강수량 추출
prcp_blnd_pt_txt1: prcp_blnd_pt_txt1.c prcp_blnd_pt_lib.c
	gcc $(INCLUDEDIRS) $(LIBDIRS) $(NC_INCLUDEDIRS) $(NC_LIBDIRS) -o $(bin)nph-prcp_blnd_pt_txt1 prcp_blnd_pt_txt1.c prcp_blnd_pt_lib.c aws_io.c aws2_io.c aws3_io.c $(SRCS) $(time_lib) $(url_lib) $(cgi_lib) $(map_lib) -Wl,-rpath,/fct/ftd/lib/netcdf-c-4.6.2/lib -Wl,-rpath,/fct/ftd/lib/hdf5-1.8.21/lib -lnetcdf -lhdf5_hl -lhdf5 -ldl -lm -lz -lcurl -lgd -lpng

# 초단기예보 10분 강수량 경계값 이상 지역 추출
prcp_blnd_thrhld_txt1: prcp_blnd_thrhld_txt1.c prcp_blnd_thrhld_lib.c prcp_blnd_thrhld_grid.c
	gcc $(INCLUDEDIRS) $(LIBDIRS) $(NC_INCLUDEDIRS) $(NC_LIBDIRS) -o $(bin)nph-prcp_blnd_thrhld_txt1 prcp_blnd_thrhld_txt1.c prcp_blnd_thrhld_lib.c prcp_blnd_thrhld_grid.c $(SRCS) $(time_lib) $(nr_lib) $(url_lib) $(cgi_lib) $(map_lib) -Wl,-rpath,/fct/ftd/lib/netcdf-c-4.6.2/lib -Wl,-rpath,/fct/ftd/lib/hdf5-1.8.21/lib -lnetcdf -lhdf5_hl -lhdf5 -ldl -lm -lz -lcurl -lgd -lpng

# gis 배경지도
cht_ana_img: cht_ana_img.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-cht_ana_img cht_ana_img.c $(map_lib) $(url_lib) $(time_lib) $(cgi_lib) $(nr_lib) $(gz_lib) $(LIBDIRS) $(met_lib) -lm -lz -lgd -lpng -lcurl

# 지도(위성/레이더/낙뢰 표출 포함)
#map_ana_img: map_ana_img.c rdr_sat_lgt_img.c gts_cht_stn_lib.c
#	gcc $(INCLUDEDIRS2) -o $(bin)nph-map_ana_img map_ana_img.c sterproj.c $(map_lib) $(url_lib) $(time_lib) $(cgi_lib) $(nr_lib) $(gz_lib) $(LIBDIRS2) $(met_lib) -lnetcdf -lhdf5_hl -lhdf5 -lm -lz -lgd -lpng -lcurl
map_ana_img: map_ana_img.c rdr_sat_lgt_img.c gts_cht_stn_lib.c cht_afs_img.c
	gcc $(INCLUDEDIRS3) -I/fct/ftd/lib/proj-4.4.5/include -L. -L/fct/ftd/lib/proj-4.4.5/lib -o $(bin)nph-map_ana_img map_ana_img.c cJSON.c sterproj.c eqdcproj.c $(map_lib) $(url_lib) $(time_lib) $(cgi_lib) $(nr_lib) $(gz_lib) $(LIBDIRS3) $(met_lib) $(LINKS) -Wl,-rpath,/fct/ftd/lib/proj-4.4.5/lib -lnetcdf -lhdf5_hl -lhdf5 -lm -lz -lgd -lpng -lcurl -lproj -leccodes

map_ana_img_test: map_ana_img.c rdr_sat_lgt_img.c gts_cht_stn_lib.c cht_afs_img.c
	gcc $(INCLUDEDIRS3) -I/fct/ftd/lib/proj-4.4.5/include -L. -L/fct/ftd/lib/proj-4.4.5/lib -o $(bin)nph-map_ana_img_test map_ana_img.c cJSON.c sterproj.c eqdcproj.c $(map_lib) $(url_lib) $(time_lib) $(cgi_lib) $(nr_lib) $(gz_lib) $(LIBDIRS3) $(met_lib) $(LINKS) -Wl,-rpath,/fct/ftd/lib/proj-4.4.5/lib -lnetcdf -lhdf5_hl -lhdf5 -lm -lz -lgd -lpng -lcurl -lproj -leccodes

# 수치모델 BIAS 산출(GTS 자료 비교)
nwp_bias_gts_txt1: nwp_bias_gts_txt1.c calc_var.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-nwp_bias_gts_txt1  nwp_bias_gts_txt1.c $(time_lib) $(url_lib) $(map_lib) $(cgi_lib) $(nr_lib) -lm -lz -lcurl

#------------------------------------------------------------------------------------------

# AWS 정렬검색
aws_sort: aws_sort.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws_sort aws_sort.c $(aws_lib) $(aws2_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz

# AWS 정렬검색2
aws_sort2: aws_sort2.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws_sort2 aws_sort2.c $(aws_lib) $(aws2_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz

# AWS 체감온도검색
aws_tc: aws_tc.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws_tc aws_tc.c $(aws_lib) $(aws2_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz

# AWS 일극값 정렬검색
aws_day: aws_day.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-aws_day aws_day.c $(aws_lib) $(aws2_lib) $(stn_lib) $(time_lib) $(cgi_lib) -lm -lz

# PROJ4 변환
proj4: proj4.c
	gcc -I. -I/usr/include -I/usr/local/include -I/usr/local/trmm/include -I/home/fct/src/include -I/home/fct/www/cgi-src/include -I/fct/ftd/lib/proj-4.4.5/include -L. -L/fct/ftd/lib/proj-4.4.5/lib -o $(bin)nph-proj4 proj4.c sterproj.c -Wl,-rpath,/fct/ftd/lib/proj-4.4.5/lib $(cgi_lib) $(nr_lib) $(map_lib) -lproj -lm

# 단열선도 표출
skew_img: skew_img.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-skew_img  skew_img.c cJSON.c $(map1_lib) $(url_lib) $(time_lib) $(cgi_lib) $(nr_lib) $(LIBDIRS) -lm -lz -lgd -lpng -lcurl

skew_img_tmp: skew_img.tmp.c
	gcc $(INCLUDEDIRS) -o $(bin)nph-skew_img_tmp  skew_img.tmp.c cJSON.c $(map1_lib) $(url_lib) $(time_lib) $(cgi_lib) $(nr_lib) $(LIBDIRS) -lm -lz -lgd -lpng -lcurl

# 연직단면도 표출
crss_sct_img: crss_sct_img.c
	gcc $(INCLUDEDIRS3) $(LIBDIRS3) -I/fct/ftd/lib/proj-4.4.5/include -L. -L/fct/ftd/lib/proj-4.4.5/lib -o $(bin)nph-crss_sct_img crss_sct_img.c cJSON.c sterproj.c eqdcproj.c $(map_lib) $(url_lib) $(time_lib) $(cgi_lib) $(nr_lib) $(zoom_lib) $(LINKS) -Wl,-rpath,/fct/ftd/lib/proj-4.4.5/lib -lnetcdf -lhdf5_hl -lhdf5 -leccodes -lm -lz -lgd -lpng -lcurl -lproj

# AMDAR(GTS) 자료 조회
amdar_bufr: amdar_bufr.c
	gcc $(INCLUDEDIRS3) $(LIBDIRS3) -o $(bin)nph-amdar_bufr amdar_bufr.c $(map_lib) $(url_lib) $(time_lib) $(cgi_lib) $(nr_lib) $(zoom_lib) $(LINKS) -leccodes -lm -lz -lgd -lpng -lcurl

# BUFR 해독
bufr_decode: bufr_decode.c
	gcc $(INCLUDEDIRS3) $(LIBDIRS3) -o $(bin)nph-bufr_decode bufr_decode.c $(map_lib) $(url_lib) $(time_lib) $(cgi_lib) $(nr_lib) $(zoom_lib) $(LINKS) -leccodes -lm -lz -lgd -lpng -lcurl