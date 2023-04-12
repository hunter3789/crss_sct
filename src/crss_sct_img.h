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

#define  MAP_INI_FILE   "/DATA/GIS/MAP/map.ini"    // �������� ��������
#define  INDEX_DIR      "/fct/www/ROOT/img/index"  // �ӽ� �ε������� ���� ���丮
#define  SAMPLE_DIR     "/fct/www/ROOT/img/sample" // ���� �ε������� ���� ���丮
#define  IMG_DIR        "/fct/www/ROOT/img/skew"   // �̹��� ���� ���丮
#define  TOPO_DIR       "/DATA/GIS/TOPO/KMA"       // ������ ���丮
#define  COLOR_SET_DIR  "/fct/REF/COLOR/"          // ����ǥ ���� �ӽ������
#define  CHT_INI_FILE   "/fct/www/ROOT/cht_new/cht_afs_fct.ini" // ���ձ��м� API ��ø ��������
#define  GTS_INI_FILE   "/fct/www/ROOT/gts/gts_afs_img.ini"     // ���ձ��м� API ��ø ��������(GTS)

#define  AFS_HOST_URL   "http://afs.kma.go.kr/" // ���������ý��� host url

// ����� �ѱ�TTF
#define  FONTTTF   "/fct/www/ROOT/img/fonts/NanumGothic.ttf"
#define  FONTTTF1  "/fct/www/ROOT/img/fonts/NanumGothicBold.ttf"
#define  FONTTTF2  "/fct/www/ROOT/img/fonts/NanumGothicExtraBold.ttf"
#define  FONTTTF9  "/usr/share/fonts/korean/TrueType/gulim.ttf"

// �̹��� ������ �Ϻ� �⺻��
#define  LEG_pixel    35     // ����ǥ�ÿ���
#define  LEGEND_pixel 85     // �ܸ鵵�� ����ǥ�ÿ���
#define  TITLE_pixel  50     // ����ǥ�ÿ���
#define  LEFT_pixel   45     // ���� ��ǥ�� ����
#define  TAIL_pixel   30     // �Ʒ� �Ÿ�ǥ�� ����

#define  MIN_LEVEL   250     // ��� ��(min)
#define  MAX_LEVEL   1000    // ��� ��(max)

#define  max_layer  10       // �ϱ⵵ �ִ� ��ø ���̾� ��
#define  INIT_SIZE 256

//------------------------------------------------------------------------------
// ����� �Է� ����
struct INPUT_VAR {
  // ������ �ڷ� �����
  int   seq;          // ���ؽð� SEQ(��)
  int   seq_fc;       // ���۽ð� SEQ(��) - ��ġ�� ��ǥ�ð�
  float missing;      // �� �� �����̸� ����
  char  model[16];    // ��ġ�� ����
  int   map_disp;     // ���� ǥ��
  int   flag;         // 0: �̹��� ǥ��, 1: ������ ǥ��, 2: �ڷ�ó�� �ҿ�ð� ǥ��
  int   save;
  char  layer[32];
  char  cht_name[256];      // �ϱ⵵ ����
  char  cht_mode[32];       // �ϱ⵵ ����(������, �м���, gts ��ȸ)
  int   curl;               // ���� ó��(0: X, 1: O)
  char  host[16];           // API ȣ�� host
  char  gts[16];            // SFC, ��Ÿ
  char  varname[32];        // �����ܸ鵵 �߰� ����
  int   NI, NJ, GI, GJ;
  int   OI, OJ, NX, NY, SX, SY;
  float lon, lat, lon2, lat2;
  int   gis;
  int   rain;
  int   legend_only;        // ���� �̹����� ����(0: X, 1: O)

  char  map[16];      // ����� �����ڵ�
  float grid;         // ����ũ��(km)
  char  zoom_x[16];   // X���� Ȯ��
  char  zoom_y[16];   // Y���� Ȯ��

  double startx;
  double starty;
  double endx;
  double endy;

  float min_level;
  float max_level;
};