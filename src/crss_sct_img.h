#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iconv.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>
#include <math.h>
#include <zlib.h>
#include <zconf.h>
#include <gd.h>
#include <gdfontt.h>
#include <gdfonts.h>
#include <gdfontmb.h>
#include <gdfontl.h>
#include <gdfontg.h>
#include <hdf5.h>
#include <netcdf.h>
#include <curl/curl.h>

#include "cgiutil.h"
#include "nrutil.h"
#include "stn_inf.h"
#include "map_ini.h"
#include "url_io.h"
#include "projects.h"
#include "emess.h"
#include "cJSON.h"

#define  CELKEL   273.15
#define  DEGRAD   0.01745329251994329576923690768489    // PI/180
#define  RADDEG   57.295779513082320876798154814105     // 180/PI

#define  MAP_INI_FILE   "/DATA/GIS/MAP/map.ini"    // 지도영역 정보파일
#define  INDEX_DIR      "/fct/www/ROOT/img/index"  // 임시 인덱스파일 저장 디렉토리
#define  SAMPLE_DIR     "/fct/www/ROOT/img/sample" // 샘플 인덱스파일 저장 디렉토리
#define  IMG_DIR        "/fct/www/ROOT/img/skew"   // 이미지 저장 디렉토리
#define  TOPO_DIR       "/DATA/GIS/TOPO/KMA"       // 지형고도 디렉토리
#define  COLOR_SET_DIR  "/fct/REF/COLOR/"          // 색상표 파일 임시저장소
#define  CHT_INI_FILE   "/fct/www/ROOT/cht_new/cht_afs_fct.ini" // 통합기상분석 API 중첩 정보파일
#define  GTS_INI_FILE   "/fct/www/ROOT/gts/gts_afs_img.ini"     // 통합기상분석 API 중첩 정보파일(GTS)

#define  AFS_HOST_URL   "http://afs.kma.go.kr/" // 선진예보시스템 host url

// 사용할 한글TTF
#define  FONTTTF   "/fct/www/ROOT/img/fonts/NanumGothic.ttf"
#define  FONTTTF1  "/fct/www/ROOT/img/fonts/NanumGothicBold.ttf"
#define  FONTTTF2  "/fct/www/ROOT/img/fonts/NanumGothicExtraBold.ttf"
#define  FONTTTF9  "/usr/share/fonts/korean/TrueType/gulim.ttf"

// 이미지 영역중 일부 기본값
#define  LEG_pixel    35     // 범례표시영역
#define  LEGEND_pixel 85     // 단면도용 범례표시영역
#define  TITLE_pixel  50     // 제목표시영역
#define  LEFT_pixel   45     // 왼쪽 고도표시 영역
#define  TAIL_pixel   30     // 아래 거리표시 영역

#define  MIN_LEVEL   250     // 계산 고도(min)
#define  MAX_LEVEL   1000    // 계산 고도(max)

#define  max_layer  10       // 일기도 최대 중첩 레이어 수
#define  INIT_SIZE 256

//------------------------------------------------------------------------------
// 사용자 입력 변수
struct INPUT_VAR {
  // 지점별 자료 추출용
  int   seq;          // 기준시각 SEQ(분)
  int   seq_fc;       // 시작시각 SEQ(분) - 수치모델 발표시각
  float missing;      // 이 값 이하이면 오류
  char  model[16];    // 수치모델 종류
  int   map_disp;     // 지도 표출
  int   flag;         // 0: 이미지 표출, 1: 데이터 표출, 2: 자료처리 소요시간 표출
  int   save;
  char  layer[32];
  char  cht_name[256];      // 일기도 종류
  char  cht_mode[32];       // 일기도 형태(예상장, 분석장, gts 조회)
  int   curl;               // 병렬 처리(0: X, 1: O)
  char  host[16];           // API 호출 host
  char  gts[16];            // SFC, 기타
  char  varname[32];        // 연직단면도 추가 변수
  int   NI, NJ, GI, GJ;
  int   OI, OJ, NX, NY, SX, SY;
  float lon, lat, lon2, lat2;
  int   gis;
  int   rain;
  int   legend_only;        // 범례 이미지만 생성(0: X, 1: O)

  char  map[16];      // 사용할 지도코드
  float grid;         // 격자크기(km)
  char  zoom_x[16];   // X방향 확대
  char  zoom_y[16];   // Y방향 확대

  double startx;
  double starty;
  double endx;
  double endy;

  float min_level;
  float max_level;
};