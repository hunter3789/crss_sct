/*******************************************************************************
**
**  연직 단면도 표출
**
**=============================================================================*
**
**     o 작성자 : 이창재 (2021.11.05.)
**
********************************************************************************/
#include "crss_sct_img.h"
#include "eccodes.h"

// 변수
struct INPUT_VAR var;  // 지점 자료
struct timeval tv0, tv1, tv2;
char   host[256];

// 함수
int color_table(gdImagePtr, int *, float *);
int topo_data_get(float *);
int plot_wd(gdImagePtr, int, int, float, float, int);
int map_ini_read();
int hh_to_pres(float);

#include "grib_io.c"
#include "calc_var.c"
#include "map_disp_lib.c"
#include "cht_afs_img.c"
/*******************************************************************************
 *
 *  MAIN
 *
 *******************************************************************************/
int main()
{
  int   err = 0;

  // 1. 초기화
  gettimeofday(&tv1, NULL);
  tv0 = tv2 = tv1;
  setvbuf(stdout, NULL, _IONBF, 0);
  alarm(60);

  printf("HTTP/1.0 200 OK\n");

  // 2. 사용자 입력 변수 분석
  if ( user_input() < 0 ) {
    printf("Content-type: text/plain\n\n");
    printf("user_input() error\n");
    return -1;
  }
  gethostname(host, sizeof(host));

  // 3. 자료 조회 및 표출
  err = crss_sct_data_get();

  if (err < 0) {
    print_error(err);
  }

  alarm(0);
  return 0;
}

/*******************************************************************************
 *  격자자료 웹 이미지 표출시 사용자 요청 분석 부분
 *******************************************************************************/
int user_input() {
  char *qs;
  char tmp[256], item[32], value[256], tm[30], tm_fc[30];
  int  iYY, iMM, iDD, iHH, iMI, iSS;
  int  iseq, i, j, k;

  // 1. 변수 초기값 : 자료별 처리 프로그램에서 각자 상황에 맞게 설정
  var.missing = -999;
  var.lon = 130; var.lon2 = 140;
  var.lat = 35;  var.lat2 = 38;
  var.map_disp = 0;
  var.flag = 0;
  var.curl = 0;
  var.grid = 1.0;
  var.min_level = 250;
  var.max_level = 1000;
  var.rain = 0;
  strcpy(var.map,     "EA_CHT");
  strcpy(var.zoom_x,  "0000000");
  strcpy(var.zoom_y,  "0000000");
  strcpy(var.model,   "ECMWF_H");
  strcpy(var.varname, "DPDK");
  strcpy(var.cht_mode, "crss_sct");

  // 2. GET 방식으로 전달된 사용자 입력변수들의 해독
  qs = getenv ("QUERY_STRING");
  if (qs == NULL) return -1;

  for (i = 0; qs[0] != '\0'; i++) {
    getword (value, qs, '&');
    getword (item, value, '=');
    if (strlen(value) == 0) continue;

    if      ( !strcmp(item,"tm"))        strcpy(tm, value);
    else if ( !strcmp(item,"tm_fc"))     strcpy(tm_fc, value);
    else if ( !strcmp(item,"model"))     strcpy(var.model, value);
    else if ( !strcmp(item,"lat"))       var.lat = atof(value);
    else if ( !strcmp(item,"lon"))       var.lon = atof(value);
    else if ( !strcmp(item,"lat2"))      var.lat2 = atof(value);
    else if ( !strcmp(item,"lon2"))      var.lon2 = atof(value);
    else if ( !strcmp(item,"min"))       var.min_level = atof(value);
    else if ( !strcmp(item,"max"))       var.max_level = atof(value);
    else if ( !strcmp(item,"flag"))      var.flag = atoi(value);
    else if ( !strcmp(item,"rain"))      var.rain = atoi(value);
    else if ( !strcmp(item,"map_disp"))  var.map_disp = atoi(value);
    else if ( !strcmp(item,"varname"))   strcpy(var.varname, value);
    else if ( !strcmp(item,"zoom_x"))    strcpy(var.zoom_x, value);
    else if ( !strcmp(item,"zoom_y"))    strcpy(var.zoom_y, value);
    else if ( !strcmp(item,"map"))       strcpy(var.map, value);

  }

  if (!strcmp(var.model,"ECMWF_H") || !strcmp(var.model,"ECMWF_1H10G1")) {
    strcpy(var.cht_name, "ecmw_lc40_asia_acptot_");
  }
  else if (!strcmp(var.model,"GDAPS") || !strcmp(var.model,"GDAPS_1H")) {
    strcpy(var.cht_name, "gdps_erly_asia_acptot_ft06");
  }
  else if (!strcmp(var.model,"GDAPS_KIM") || !strcmp(var.model,"KIM_1H")) {
    strcpy(var.cht_name, "kim_gdps_erly_asia_acptot_ft06");
  }

  // 4. 현재시간 및 재계산 지연시간 설정
  get_time(&iYY, &iMM, &iDD, &iHH, &iMI, &iSS);
  iMI = 0; iSS = 0;
  if (iHH > 9) {
    if (iHH > 21) iHH = 21;
    else iHH = 9;
    iseq = time2seq(iYY, iMM, iDD, iHH, iMI, 'm');
    seq2time(iseq - 9*60, &iYY, &iMM, &iDD, &iHH, &iMI, 'm', 'n');
  }
  else {
    iHH = 21;
    iseq = time2seq(iYY, iMM, iDD, iHH, iMI, 'm');
    seq2time(iseq - 9*60 - 24*60, &iYY, &iMM, &iDD, &iHH, &iMI, 'm', 'n');
  }

  // 5. 요청시간 설정
  if (strlen(tm) >= 10) {
    strncpy(tmp, &tm[0], 4);  tmp[4] = '\0';  iYY = atoi(tmp);
    strncpy(tmp, &tm[4], 2);  tmp[2] = '\0';  iMM = atoi(tmp);
    strncpy(tmp, &tm[6], 2);  tmp[2] = '\0';  iDD = atoi(tmp);
    strncpy(tmp, &tm[8], 2);  tmp[2] = '\0';  iHH = atoi(tmp);
    strncpy(tmp, &tm[10],2);  tmp[2] = '\0';  iMI = atoi(tmp);
  }
  var.seq = time2seq(iYY, iMM, iDD, iHH, iMI, 'm');

  if (strlen(tm_fc) >= 10) {
    strncpy(tmp, &tm_fc[0], 4);  tmp[4] = '\0';  iYY = atoi(tmp);
    strncpy(tmp, &tm_fc[4], 2);  tmp[2] = '\0';  iMM = atoi(tmp);
    strncpy(tmp, &tm_fc[6], 2);  tmp[2] = '\0';  iDD = atoi(tmp);
    strncpy(tmp, &tm_fc[8], 2);  tmp[2] = '\0';  iHH = atoi(tmp);
    strncpy(tmp, &tm_fc[10],2);  tmp[2] = '\0';  iMI = atoi(tmp);
  }
  var.seq_fc = time2seq(iYY, iMM, iDD, iHH, iMI, 'm');

  return 0;
}

/*=============================================================================*
 *  연직단면도 표출
 *=============================================================================*/
int crss_sct_img(int varSize, int levelSize, int xznums, double ***xz, long int *levels, double *xx)
{
  FILE* fp = NULL;
  gdImagePtr im;
  float  data_lvl[256], data_contour[256];
  int    color_lvl[256];
  int    num_color, num_contour, i, j, iy, ix, k, ivar;
  float  v1, v2, vv, dd, di, dj;
  float  ***g, *topo_x;
  char   txt[256], imgname[512], varname[64], value[64];
  //int    pa[10] = {1000, 925, 850, 800, 700, 600, 500, 400, 300, 250};
  int    pa[23] = {1000, 975, 950, 925, 900, 875, 850, 825, 800, 750, 700, 650, 600, 550, 500, 450, 400, 350, 300, 250, 200, 150, 100};
  int    styleDashed[4];
  int    YY, MM, DD, HH, MI, YY2, MM2, DD2, HH2, MI2;
  struct lamc_parameter map;
  float  zm = 1.0, xo = 0.0, yo = 0.0, rate;
  float  lon, lat, x, y, lon2, lat2, x1, y1, len;
  int    zx, zy, nx, ny;
  float  a1, a2, vfact;
  int    R, G, B;
  int    num_color_rdr, color_rdr[40];
  float  data_rdr[40];
  char   fname[120];

  // 1. 이미지 영역 설정
  len = acos(sin(var.lat * DEGRAD) * sin(var.lat2 * DEGRAD) + cos(var.lat * DEGRAD) * cos(var.lat2 * DEGRAD) * cos(var.lon * DEGRAD - var.lon2 * DEGRAD)) * RADDEG * 60 * 1.1515 * 1.609344;
  var.NI = (int)len;
  if (var.NI < 600) var.NI = 600;
  else if (var.NI > 1500) var.NI = 1500;
  var.NJ = var.max_level - var.min_level;
  var.OI = LEFT_pixel;
  var.OJ = TITLE_pixel;
  var.GI = var.NI + var.OI + LEGEND_pixel;
  if (var.rain == 1) {
    var.GJ = var.NJ + var.OJ + TAIL_pixel + 60;
  }
  else {
    var.GJ = var.NJ + var.OJ + TAIL_pixel;
  }

  // 2. 이미지 구조체 설정 및 색상표 읽기
  im = gdImageCreateTrueColor(var.GI, var.GJ);
  num_color = color_table(im, color_lvl, data_lvl);
  gdImageFilledRectangle(im, 0, 0, var.GI, var.GJ, color_lvl[241]);

  a1 = log(var.max_level*100.);
  a2 = log(var.min_level*100.);
  vfact = var.NJ/(a1-a2);

  // 3. 이미지 크기에 맞는 데이터 배열 만들기
  g = (float ***) malloc((unsigned) (varSize)*sizeof(float **));
  for (j = 0; j < varSize; j++) {
    g[j] = (float **) malloc((unsigned) (var.NJ+1)*sizeof(float *));
  }   

  for (j = 0; j < varSize; j++) {
    for (i = 0; i <= var.NJ; i++) {
      g[j][i] = (float *) malloc((unsigned) (var.NI+1)*sizeof(float));
    }
  }  

  for (j = 0; j <= var.NJ; j++) {
    for (k = 0; k < levelSize; k++) {
      //if (j*(float)(var.max_level-var.min_level)/(float)(var.NJ)+var.min_level < levels[k]) {
      if (exp((float)j/vfact + a2)/100. < levels[k]) {
        iy = k;
        break;
      }
    }

    //dj = (float)((float)(levels[iy]-(j*(float)(var.max_level-var.min_level)/(float)(var.NJ)+var.min_level))/(float)(levels[iy]-levels[iy-1]));
    dj = (float)((float)(levels[iy]-(exp((float)j/vfact + a2)/100.))/(float)(levels[iy]-levels[iy-1]));

    for (i = 0; i <= var.NI; i++) {
      di = (float)i/((float)(var.NI)/(float)xznums) - (int)(i/((float)(var.NI)/(float)xznums));
      ix = (int)(i/((float)(var.NI)/(float)xznums));
      for (ivar = 0; ivar < varSize; ivar++) {
        v1 = xz[ivar][iy][ix]*(1-di) + xz[ivar][iy][ix+1]*(di);
        v2 = xz[ivar][iy-1][ix]*(1-di) + xz[ivar][iy-1][ix+1]*(di);
        vv = v1*(1-dj) + v2*(dj);
        g[ivar][j][i] = vv;
      }
    }
  }

  // 4. 등치면 표출
  if (!strcmp(var.varname, "PVEL") && (!strcmp(var.model, "GDAPS") || !strcmp(var.model, "GDAPS_KIM") || !strcmp(var.model, "GDAPS_1H") || !strcmp(var.model, "KIM_1H"))) ivar = 4;
  else ivar = 3;

  grid_smooth(g[ivar], var.NI, var.NJ, (int)(var.missing));
  for (j = 0; j <= var.NJ; j++) {
    for (i = 0; i <= var.NI; i++) {
      for (k = 0; k < num_color; k++) {
        if (g[ivar][j][i] < data_lvl[k]) {
          gdImageSetPixel(im, i+var.OI, j+var.OJ, color_lvl[k]);
          break;
        }
      }
    }
  }

  // 5. 지형 표출
  topo_x = (float *) malloc((unsigned) (var.NI+1)*sizeof(float));
  topo_data_get(topo_x);
  for (i = 0; i <= var.NI; i++) {
    //gdImageLine(im, var.OI+i, var.OJ+var.NJ, var.OI+i, var.OJ+var.NJ-(int)topo_x[i]/((float)(var.max_level-var.min_level)/(float)(var.NJ))/10, color_lvl[240]);
    if (topo_x[i] < 0) {
    }
    else {
      gdImageLine(im, var.OI+i, var.OJ+var.NJ, var.OI+i, var.OJ+(int)((log(topo_x[i]*100)-a2)*vfact), color_lvl[240]);
    }
  }
  free(topo_x);

  // 8. 제목 표출
  seq2time(var.seq_fc, &YY, &MM, &DD, &HH, &MI, 'm', 'n');
  seq2time(var.seq+9*60, &YY2, &MM2, &DD2, &HH2, &MI2, 'm', 'n');
  if (!strcmp(var.varname, "DPDK")) {
    strcpy(varname, "T-Td");
    strcpy(value, "(C)");
  }
  else if (!strcmp(var.varname, "TD")) {
    strcpy(varname, "Td");
    strcpy(value, "(C)");
  }
  else if (!strcmp(var.varname, "PVEL")) {
    strcpy(varname, "P-velocity");
    strcpy(value, "(hPa/hr)");
  }
  else if (!strcmp(var.varname, "WSPD")) {
    strcpy(varname, "wind speed");
    strcpy(value, "(kt)");
  }
  else if (!strcmp(var.varname, "EPOT")) {
    strcpy(varname, "Equiv.P.Temp");
    strcpy(value, "(K)");
  }
  else if (!strcmp(var.varname, "CONV")) {
    strcpy(varname, "Convergence");
    strcpy(value, "(E-6/s)");
  }

  sprintf(txt, "VALID: %04d.%02d.%02d.%02dKST[+%03dh] / Issued at %04d.%02d.%02d.%02dUTC / %s / Horizontal wind, Temp, %s", YY2, MM2, DD2, HH2, (var.seq-var.seq_fc)/60, YY, MM, DD, HH, var.model, varname);
  gdImageString(im, gdFontSmall, 50, 4, txt, color_lvl[242]);
  sprintf(txt, "Lat1: %.1f, Lon1: %.1f ~ Lat2: %.1f, Lon2: %.1f / Distance: %.1fkm", var.lat, var.lon, var.lat2, var.lon2, len);
  gdImageString(im, gdFontSmall, 50, 18, txt, color_lvl[242]);

  // 9. A, B, 등압선 표출
  styleDashed[0] = color_lvl[240];
  styleDashed[1] = color_lvl[240];
  styleDashed[2] = gdTransparent;
  styleDashed[3] = gdTransparent;
  gdImageSetStyle(im, styleDashed, 4);

  for (k = 0; k < 23; k++) {
    if (pa[k] > var.max_level || pa[k] < var.min_level) {
      continue;
    }

    sprintf(txt, "%dhPa", (int)(pa[k]));
    //gdImageString(im, gdFontSmall, 8, var.OJ+(pa[i]-var.min_level)*(float)(var.NJ)/(float)(var.max_level-var.min_level)-8, txt, color_lvl[242]);
    //gdImageLine(im, var.OI, var.OJ+(pa[i]-var.min_level)*(float)(var.NJ)/(float)(var.max_level-var.min_level), var.OI+var.NI, var.OJ+(pa[i]-var.min_level)*(float)(var.NJ)/(float)(var.max_level-var.min_level), gdStyled);
    gdImageString(im, gdFontSmall, 8, var.OJ+(int)((log(pa[k]*100)-a2)*vfact)-8, txt, color_lvl[242]);
    gdImageLine(im, var.OI, var.OJ+(int)((log(pa[k]*100)-a2)*vfact), var.OI+var.NI, var.OJ+(int)((log(pa[k]*100)-a2)*vfact), gdStyled);
  }

  gdImageStringFT(im, NULL, color_lvl[247], FONTTTF1, 17.0, 0.0, var.OI+10, var.OJ+var.NJ-10, "A");
  gdImageStringFT(im, NULL, color_lvl[247], FONTTTF1, 17.0, 0.0, var.OI+var.NI-20, var.OJ+var.NJ-10, "B");

  for (i = 1; i <= 4; i++) {
    gdImageLine(im, var.OI+var.NI*(float)i/(float)5, var.OJ, var.OI+var.NI*(float)i/(float)5, var.OJ+var.NJ, gdStyled);
    sprintf(txt, "%d", i);
    gdImageStringFT(im, NULL, color_lvl[247], FONTTTF1, 17.0, 0.0, var.OI+var.NI*(float)i/(float)5-6, var.OJ+var.NJ-10, txt);
  }

  // 6. 등치선 표출
  num_contour = 100;
  for (i = 0; i < 100; i++) {
    data_contour[i] = -100 + 2*(float)(i);
  }
  plot_contour(im, g[0], num_contour, data_contour, color_lvl[246]);
  gdImageSetThickness(im, 1);

  // 7. 바람 표출
  for (k = 0; k < 23; k++) {
    if (pa[k] > var.max_level || pa[k] < var.min_level) {
      continue;
    }

    //j = (int)((pa[k]-var.min_level)*(float)(var.NJ)/(float)(var.max_level-var.min_level));
    j = (int)((log(pa[k]*100)-a2)*vfact);
    for (i = 40; i <= var.NI; i = i + 25) {
      plot_wd(im, i+var.OI, j+var.OJ, g[1][j][i], g[2][j][i], color_lvl[244]);
    }
  }

  // 10. 범례 표출
  gdImageString(im, gdFontSmall, var.OI+var.NI+34, var.OJ-16, value, color_lvl[244]);

  for (i = 0; i < num_color; i++) {
    if (i != num_color-1) {
      gdImageFilledRectangle(im, var.OI+var.NI+34, var.OJ+(float)(var.NJ)*((float)(i)/(float)(num_color-1)), var.OI+var.NI+44, var.OJ+(float)(var.NJ)*((float)(i+1)/(float)(num_color-1)), color_lvl[num_color-i-1]);
      gdImageRectangle(im, var.OI+var.NI+34, var.OJ+(float)(var.NJ)*((float)(i)/(float)(num_color-1)), var.OI+var.NI+44, var.OJ+(float)(var.NJ)*((float)(i+1)/(float)(num_color-1)), color_lvl[244]);
    }

    if ((int)(fabs(data_lvl[i])*10 + 0.5) % 10 == 0) {
      if (data_lvl[i] >= 0) sprintf(value, "%d", (int)(data_lvl[i]+0.5));
      else sprintf(value, "%d", (int)(data_lvl[i]-0.5));
    }
    else if ((int)(fabs(data_lvl[i])*100 + 0.5) % 10 == 0) {
      sprintf(value, "%.1f", data_lvl[i]);
    }
    else {
      sprintf(value, "%.2f", data_lvl[i]);
    }
    if (data_lvl[i] != fabs(var.missing)) gdImageString(im, gdFontSmall, var.OI+var.NI+50, var.OJ+(float)(var.NJ)*((float)(num_color-i-1)/(float)(num_color-1))-6, value, color_lvl[244]);
  }

  // 11. 배열 초기화
  for (j = varSize-1; j >= 0; j--) {
    for (i = var.NJ; i >= 0; i--) {
      free(g[j][i]);
    }
  }
  for (j = varSize-1; j >= 0; j--) free(g[j]);
  free(g);

  // 강수 표출
  if (var.rain == 1) {
    sprintf(txt, "RAIN");
    gdImageString(im, gdFontSmall, 8, var.NJ+var.OJ+24, txt, color_lvl[242]);

    sprintf(txt, "(mm)");
    gdImageString(im, gdFontSmall, 8, var.NJ+var.OJ+34, txt, color_lvl[242]);

    // 레이더 색상표
    sprintf(fname, "%s/color_rdr_echo.rgb", COLOR_SET_DIR);    // 레이더 색상표 

    if ((fp = fopen(fname,"r")) == NULL) return -1;
    num_color_rdr = 0;
    while (fscanf(fp, "%d %d %d %f\n", &R, &G, &B, &v1) != EOF) {
      color_rdr[num_color_rdr] = gdImageColorAllocate(im, R, G, B);
      data_rdr[num_color_rdr] = v1;
      num_color_rdr++;
      if (num_color_rdr >= 120) break;
    }
    fclose(fp);

    for (i = 0; i <= xznums; i++) {
      for (k = 0; k < num_color_rdr; k++) {
        if (xx[i] < data_rdr[k]) {
          gdImageFilledRectangle(im, var.OI+(float)var.NI*(float)i/(float)(xznums+1), var.NJ+var.OJ+25, var.OI+(float)var.NI*(float)(i+1)/(float)(xznums+1), var.NJ+var.OJ+50, color_rdr[k]);
          break;
        }
      }
    }


    // 강수 범례
    for (i = 0; i < num_color_rdr-1; i++) {
      gdImageFilledRectangle(im, var.OI+(float)var.NI*(float)i/(float)(num_color_rdr-1), var.NJ+var.OJ+60, var.OI+(float)var.NI*(float)(i+1)/(float)(num_color_rdr-1), var.NJ+var.OJ+70, color_rdr[i]);
      gdImageRectangle(im, var.OI+(float)var.NI*(float)i/(float)(num_color_rdr-1), var.NJ+var.OJ+60, var.OI+(float)var.NI*(float)(i+1)/(float)(num_color_rdr-1), var.NJ+var.OJ+70, color_lvl[244]);

      if ((int)(fabs(data_rdr[i])*10 + 0.5) % 10 == 0) {
        if (data_rdr[i] >= 0) sprintf(value, "%d", (int)(data_rdr[i]+0.5));
        else sprintf(value, "%d", (int)(data_rdr[i]-0.5));
      }
      else if ((int)(fabs(data_rdr[i])*100 + 0.5) % 10 == 0) {
        sprintf(value, "%.1f", data_rdr[i]);
      }
      else {
        sprintf(value, "%.2f", data_rdr[i]);
      }
      if (data_rdr[i] != fabs(var.missing)) gdImageString(im, gdFontSmall, var.OI+(float)var.NI*(float)(i)/(float)(num_color_rdr-1), var.NJ+var.OJ+70, value, color_lvl[244]);
    }
  }

  // 12. 지도 표출
  if (var.map_disp == 1) {
    map_ini_read();
    map_edge();
    var.NI = 150;  
    if (!strcmp(var.map, "EA_CHT")) {
      var.NJ = (int)((float)(var.NI)/(float)854*598);
    }
    else if (!strcmp(var.map, "TP")) {
      var.NJ = (int)((float)(var.NI)/(float)854*525);
    }
    else if (!strcmp(var.map, "E10")) {
      var.NJ = var.NI = 140;
    }

    var.OI = var.GI - LEGEND_pixel - var.NI;
    rate = (float)(var.NI)/(float)(var.NX);
    gdImageFilledRectangle(im, var.OI, var.OJ, var.OI+var.NI, var.OJ+var.NJ, color_lvl[241]);
    //cht_afs_disp(im, color_lvl);
    //topo_disp(im, 1.0, 0, 0, color_lvl[0]);
    map_disp(im, "map", 1.0, color_lvl[244], color_lvl[244]);
    gdImageRectangle(im, var.OI, var.OJ, var.OI+var.NI, var.OJ+var.NJ, color_lvl[244]);

    for (i = 0; i < 7; i++, zm *= 1.5) {
      zx = var.zoom_x[i]-'0';
      zy = var.zoom_y[i]-'0';
      if (zx == 0 || zy == 0) break;
      xo += (float)(var.NX)/24.0*(zx-1)/zm;
      yo += (float)(var.NY)/24.0*(zy-1)/zm;
    }

    map.Re    = 6371.00877;
    map.grid  = 1.0/(zm*rate);
    map.slat1 = 30.0;    map.slat2 = 60.0;
    map.olon  = 126.0;   map.olat  = 38.0;
    map.xo = (float)(var.SX - xo)*(zm*rate);
    map.yo = (float)(var.SY - yo)*(zm*rate);
    map.first = 0;
 
    lon = var.lon;   lat = var.lat;
    lon2 = var.lon2; lat2 = var.lat2;

    lamcproj_ellp(&lon, &lat, &x, &y, 0, &map);
    lamcproj_ellp(&lon2, &lat2, &x1, &y1, 0, &map);

    if (sqrt((x1-x)*(x1-x) + (y1-y)*(y1-y)) > 20) {
      for (i = 1; i <= 4; i++) {
        gdImageFilledArc(im, var.OI + x + (x1-x)*(float)i/(float)5, var.OJ + (var.NJ - (y + (y1-y)*(float)i/(float)5)), 5, 5, 0, 360, color_lvl[247], gdArc);
        sprintf(txt, "%d", i);
        gdImageString(im, gdFontSmall, var.OI + x + (x1-x)*(float)i/(float)5 - 1, var.OJ + (var.NJ - (y + (y1-y)*(float)i/(float)5)) + 1, txt, color_lvl[247]);
      }
    }

    gdImageFilledArc(im, var.OI + x, var.OJ + (var.NJ - y), 5, 5, 0, 360, color_lvl[246], gdArc);
    gdImageString(im, gdFontSmall, var.OI + x - 1, var.OJ + (var.NJ - y) + 1, "A", color_lvl[246]);
    gdImageFilledArc(im, var.OI + x1, var.OJ + (var.NJ - y1), 5, 5, 0, 360, color_lvl[246], gdArc);
    gdImageString(im, gdFontSmall, var.OI + x1 - 1, var.OJ + (var.NJ - y1) + 1, "B", color_lvl[246]);
  }

  // 13. 이미지 전송
  if (var.flag == 0) {
    printf("Content-type: image/png\n\n");
    gdImagePng(im, stdout);


    if (var.map_disp == 1) {
      sprintf(imgname, "%s/crss_sct_%s_lon=%.2f_lat=%.2f_lon2=%.2f_lat2=%.2f_varname=%s_min=%d_max=%d_mapdisp=%d_zoomx=%s_zoomy=%s_s%03d_%04d%02d%02d%02d.png",
              IMG_DIR, var.model, var.lon, var.lat, var.lon2, var.lat2, var.varname, (int)var.min_level, (int)var.max_level, var.map_disp, var.zoom_x, var.zoom_y, (var.seq-var.seq_fc)/60, YY, MM, DD, HH);
    }
    else {
      sprintf(imgname, "%s/crss_sct_%s_lon=%.2f_lat=%.2f_lon2=%.2f_lat2=%.2f_varname=%s_min=%d_max=%d_rain=%d_s%03d_%04d%02d%02d%02d.png",
              IMG_DIR, var.model, var.lon, var.lat, var.lon2, var.lat2, var.varname, (int)var.min_level, (int)var.max_level, var.rain, (var.seq-var.seq_fc)/60, YY, MM, DD, HH);
    }
    if ((fp = fopen(imgname, "wb"))) {
      gdImagePng(im, fp);
      fclose(fp);
    }
  }
  gdImageDestroy(im);

  return 0;
}

/*=============================================================================*
 *  연직단면 자료 추출
 *=============================================================================*/
int crss_sct_data_get()
{
    FILE* fp = NULL;
    int err = 0, err_point;
    char fname[512], idxname[512], fname2[512], idxname2[512], fname3[512], idxname3[512], imgname[512], idxsample[512];
    long numPoints, nlat, nlon, *levels, nlat1, nlon1;
    double slat, slon, dlat, dlon, elat, elon;
    double slat1, slon1, dlat1, dlon1, elat1, elon1;
    codes_index* index = NULL;
    codes_index* index2 = NULL;
    codes_index* index3 = NULL;
    codes_handle* h;
    codes_handle* h2;
    codes_handle* h3;
    double *g, *gu = NULL, *gv = NULL, ***xz, *g2, *g3, *xx;
    float  ***uuu, ***vvv, ***conv;
    int    i, j, k, ilev, ivar, code, code2, code3, nlev, di, dj;
    struct stat st;
    size_t levelSize, start[1], start_4d[4], count_4d[4], start_3d[3], count_3d[3];
    float  v, v1, v2;
    float  x, y, dx, dy;
    float  len, dd, x1, x2, y1, y2, x3, x4, y3, y4;
    float  lon1, lon2, lat1, lat2, buf;
    int    nd, xznums = 0, varSize, varSize2;
    int    YY, MM, DD, HH, MI;
    int    nc_id, var_id, dim_id, nc_id2;
    size_t sizep, nelemsp;
    float  preemptionp;
    int    seq_bk, seq_bk2, seq_bk3;
 
    seq_bk  = time2seq(2018, 6, 7, 0, 0, 'm');
    seq_bk2 = time2seq(2016, 7, 1, 0, 0, 'm');
    seq_bk3 = time2seq(2011, 6, 1, 0, 0, 'm');

    // 1. 조회할 변수 설정
    if (!strcmp(var.varname,"PVEL") && (!strcmp(var.model,"GDAPS") || !strcmp(var.model,"GDAPS_KIM") || !strcmp(var.model, "GDAPS_1H") || !strcmp(var.model, "KIM_1H"))) {
      varSize = 5; varSize2 = 5;
    }
    else if (!strcmp(var.varname,"WSPD") || !strcmp(var.varname,"CONV")) {
      varSize = 3; varSize2 = 4;
    }
    else {
      varSize = 4; varSize2 = 4;
    }

    char   varname[varSize][32];
    long   discipline[varSize], parameterCategory[varSize], parameterNumber[varSize];

    for (i = 0; i < varSize; i++) {
      switch (i) {
        case 0:  
          if (!strcmp(var.model,"GDAPS_KIM")) strcpy(varname[i], "T");
          else strcpy(varname[i], "t");
          break;
        case 1:  strcpy(varname[i], "u");  break;
        case 2:  strcpy(varname[i], "v");  break;
        case 3:  
          if (!strcmp(var.varname,"PVEL") && (!strcmp(var.model,"ECMWF_H") || !strcmp(var.model,"ECMWF_1H10G1"))) {
            strcpy(varname[i], "w");
          }
          else {
            if (!strcmp(var.model,"GDAPS_KIM")) strcpy(varname[i], "rh");
            else strcpy(varname[i], "r");
          }
          break;
        case 4:  
          if (!strcmp(var.varname,"PVEL")) {
            if (!strcmp(var.model,"GDAPS") || !strcmp(var.model,"GDAPS_1H") || !strcmp(var.model, "KIM_1H")) strcpy(varname[i], "wz");
            else if (!strcmp(var.model,"GDAPS_KIM")) strcpy(varname[i], "w");
          }
          break;
      }

      if (!strcmp(var.model,"GDAPS")) {
        switch (i) {
          case 0:  
            discipline[i] = 0; parameterCategory[i] = 0; parameterNumber[i] = 0;
            break;
          case 1:  
            discipline[i] = 0; parameterCategory[i] = 2; parameterNumber[i] = 2;
            break;
          case 2:  
            discipline[i] = 0; parameterCategory[i] = 2; parameterNumber[i] = 3;
            break;
          case 3:  
            discipline[i] = 0; parameterCategory[i] = 1; parameterNumber[i] = 1;
            break;
        }
      }
    }

    // 2. 파일명 체크
    seq2time(var.seq_fc, &YY, &MM, &DD, &HH, &MI, 'm', 'n');
    if (!strcmp(var.model,"ECMWF_H")) {
      sprintf(fname,   "/ARCV/GRIB/MODL/ECMW/T127/%04d%02d/%02d%02d/e025_v025_nhem_h%03d.%04d%02d%02d%02d00.gb1", YY, MM, DD, HH, (var.seq - var.seq_fc)/60, YY, MM, DD, HH);
      sprintf(idxname, "%s/e025_v025_nhem_h%03d.%04d%02d%02d%02d00.gbx8", INDEX_DIR, (var.seq - var.seq_fc)/60, YY, MM, DD, HH);
    }
    else if (!strcmp(var.model,"ECMWF_1H10G1")) {
      sprintf(fname,   "/ARCV/GRIB/MODL/ECMW/T127/%04d%02d/%02d%02d/e010_v025_hfp_asia_h%03d.%04d%02d%02d%02d00.gb1", YY, MM, DD, HH, (var.seq - var.seq_fc)/60, YY, MM, DD, HH);
      sprintf(idxname, "%s/e010_v025_hfp_asia_h%03d.%04d%02d%02d%02d00.gbx8", INDEX_DIR, (var.seq - var.seq_fc)/60, YY, MM, DD, HH);
    }
    else if (!strcmp(var.model,"GDAPS")) {
      if (var.seq_fc >= seq_bk) {
        sprintf(fname,   "/ARCV/GRIB/MODL/GDPS/N128/%04d%02d/%02d/g128_v070_ergl_pres_h%03d.%04d%02d%02d%02d.gb2", YY, MM, DD, (var.seq - var.seq_fc)/60, YY, MM, DD, HH);
        sprintf(idxname, "%s/g128_v070_ergl_pres_h%03d.%04d%02d%02d%02d.gbx8", INDEX_DIR, (var.seq - var.seq_fc)/60, YY, MM, DD, HH);
        sprintf(idxsample, "%s/g128_v070_ergl_pres_h000.2021010800.gbx8", SAMPLE_DIR);
      }
      else if (var.seq_fc >= seq_bk2) {
        sprintf(fname,   "/ARCV/GRIB/MODL/GDPS/N768/%04d%02d/%02d/g768_v070_ergl_pres_h%03d.%04d%02d%02d%02d.gb2", YY, MM, DD, (var.seq - var.seq_fc)/60, YY, MM, DD, HH);
        sprintf(idxname, "%s/g768_v070_ergl_pres_h%03d.%04d%02d%02d%02d.gbx8", INDEX_DIR, (var.seq - var.seq_fc)/60, YY, MM, DD, HH);
      }
      else if (var.seq_fc >= seq_bk3) {
        sprintf(fname,   "/ARCV/GRIB/MODL/GDPS/N512/%04d%02d/%02d/g512_v070_ergl_pres_h%03d.%04d%02d%02d%02d.gb2", YY, MM, DD, (var.seq - var.seq_fc)/60, YY, MM, DD, HH);
        sprintf(idxname, "%s/g512_v070_ergl_pres_h%03d.%04d%02d%02d%02d.gbx8", INDEX_DIR, (var.seq - var.seq_fc)/60, YY, MM, DD, HH);
      }
      else {
        sprintf(fname,   "/ARCV/GRIB/MODL/GDPS/N320/%04d%02d/%02d/g320_v050_ergl_pres_h%03d.%04d%02d%02d%02d.gb2", YY, MM, DD, (var.seq - var.seq_fc)/60, YY, MM, DD, HH);
        sprintf(idxname, "%s/g320_v050_ergl_pres_h%03d.%04d%02d%02d%02d.gbx8", INDEX_DIR, (var.seq - var.seq_fc)/60, YY, MM, DD, HH);
      }
    }
    else if (!strcmp(var.model,"GDAPS_1H")) {
      sprintf(fname,   "/ARCV/GRIB/MODL/GDPS/N128/%04d%02d/%02d/g128_hkor_h%03d.%04d%02d%02d%02d.gb2", YY, MM, DD, (var.seq - var.seq_fc)/60, YY, MM, DD, HH);
      sprintf(idxname, "%s/g128_hkor_h%03d.%04d%02d%02d%02d.gbx8", INDEX_DIR, (var.seq - var.seq_fc)/60, YY, MM, DD, HH);
    }
    else if (!strcmp(var.model,"GDAPS_KIM")) {
      sprintf(fname,   "/ARCV/RAWD/MODL/GDPS/NE36/%04d%02d/%02d/%02d/ERLY/FCST/post/prs.ft%03d.nc", YY, MM, DD, HH, (var.seq - var.seq_fc)/60);
    }
    else if (!strcmp(var.model,"KIM_1H")) {
      sprintf(fname,   "/ARCV/GRIB/MODL/GDPS/NE36/%04d%02d/%02d/kim_g128_hkor_pres_h%03d.%04d%02d%02d%02d.gb2", YY, MM, DD, (var.seq - var.seq_fc)/60, YY, MM, DD, HH);
      sprintf(idxname, "%s/kim_g128_hkor_pres_h%03d.%04d%02d%02d%02d.gbx8", INDEX_DIR, (var.seq - var.seq_fc)/60, YY, MM, DD, HH);
    }

    // 2.2. 파일명 체크(지상 변수)
    if (!strcmp(var.model,"ECMWF_H")) {
      sprintf(fname2,   "/ARCV/GRIB/MODL/ECMW/T127/%04d%02d/%02d%02d/e025_v025_nhem_h%03d.%04d%02d%02d%02d00.gb1", YY, MM, DD, HH, (var.seq - var.seq_fc)/60, YY, MM, DD, HH);
      sprintf(idxname2, "%s/e025_v025_nhem_h%03d.%04d%02d%02d%02d00.gbx8", INDEX_DIR, (var.seq - var.seq_fc)/60, YY, MM, DD, HH);

      if((var.seq-var.seq_fc)/60 > 144) {
        sprintf(fname3,   "/ARCV/GRIB/MODL/ECMW/T127/%04d%02d/%02d%02d/e025_v025_nhem_h%03d.%04d%02d%02d%02d00.gb1", YY, MM, DD, HH, (var.seq - var.seq_fc)/60-6, YY, MM, DD, HH);
        sprintf(idxname3, "%s/e025_v025_nhem_h%03d.%04d%02d%02d%02d00.gbx8", INDEX_DIR, (var.seq - var.seq_fc)/60-6, YY, MM, DD, HH);
      }
      else {
        sprintf(fname3,   "/ARCV/GRIB/MODL/ECMW/T127/%04d%02d/%02d%02d/e025_v025_nhem_h%03d.%04d%02d%02d%02d00.gb1", YY, MM, DD, HH, (var.seq - var.seq_fc)/60-3, YY, MM, DD, HH);
        sprintf(idxname3, "%s/e025_v025_nhem_h%03d.%04d%02d%02d%02d00.gbx8", INDEX_DIR, (var.seq - var.seq_fc)/60-3, YY, MM, DD, HH);
      }
    }
    else if (!strcmp(var.model,"ECMWF_1H10G1")) {
      sprintf(fname2,   "/ARCV/GRIB/MODL/ECMW/T127/%04d%02d/%02d%02d/e010_v025_hfp_asia_h%03d.%04d%02d%02d%02d00.gb1", YY, MM, DD, HH, (var.seq - var.seq_fc)/60, YY, MM, DD, HH);
      sprintf(idxname2, "%s/e010_v025_hfp_asia_h%03d.%04d%02d%02d%02d00.gbx8", INDEX_DIR, (var.seq - var.seq_fc)/60, YY, MM, DD, HH);

      if((var.seq-var.seq_fc)/60 > 144) {
        sprintf(fname3,   "/ARCV/GRIB/MODL/ECMW/T127/%04d%02d/%02d%02d/e010_v025_hfp_asia_h%03d.%04d%02d%02d%02d00.gb1", YY, MM, DD, HH, (var.seq - var.seq_fc)/60-6, YY, MM, DD, HH);
        sprintf(idxname3, "%s/e010_v025_hfp_asia_h%03d.%04d%02d%02d%02d00.gbx8", INDEX_DIR, (var.seq - var.seq_fc)/60-6, YY, MM, DD, HH);
      }
      else if((var.seq-var.seq_fc)/60 > 90) {
        sprintf(fname3,   "/ARCV/GRIB/MODL/ECMW/T127/%04d%02d/%02d%02d/e010_v025_hfp_asia_h%03d.%04d%02d%02d%02d00.gb1", YY, MM, DD, HH, (var.seq - var.seq_fc)/60-3, YY, MM, DD, HH);
        sprintf(idxname3, "%s/e010_v025_hfp_asia_h%03d.%04d%02d%02d%02d00.gbx8", INDEX_DIR, (var.seq - var.seq_fc)/60-3, YY, MM, DD, HH);
      }
      else {
        sprintf(fname3,   "/ARCV/GRIB/MODL/ECMW/T127/%04d%02d/%02d%02d/e010_v025_hfp_asia_h%03d.%04d%02d%02d%02d00.gb1", YY, MM, DD, HH, (var.seq - var.seq_fc)/60-1, YY, MM, DD, HH);
        sprintf(idxname3, "%s/e010_v025_hfp_asia_h%03d.%04d%02d%02d%02d00.gbx8", INDEX_DIR, (var.seq - var.seq_fc)/60-1, YY, MM, DD, HH);
      }
    }
    else if (!strcmp(var.model,"GDAPS")) {
      if (var.seq_fc >= seq_bk) {
        sprintf(fname2,   "/ARCV/GRIB/MODL/GDPS/N128/%04d%02d/%02d/g128_v070_ergl_unis_h%03d.%04d%02d%02d%02d.gb2", YY, MM, DD, (var.seq - var.seq_fc)/60, YY, MM, DD, HH);
        sprintf(idxname2, "%s/g128_v070_ergl_unis_h%03d.%04d%02d%02d%02d.gbx8", INDEX_DIR, (var.seq - var.seq_fc)/60, YY, MM, DD, HH);
      }
      else if (var.seq_fc >= seq_bk2) {
        sprintf(fname2,   "/ARCV/GRIB/MODL/GDPS/N768/%04d%02d/%02d/g768_v070_ergl_unis_h%03d.%04d%02d%02d%02d.gb2", YY, MM, DD, (var.seq - var.seq_fc)/60, YY, MM, DD, HH);
        sprintf(idxname2, "%s/g768_v070_ergl_unis_h%03d.%04d%02d%02d%02d.gbx8", INDEX_DIR, (var.seq - var.seq_fc)/60, YY, MM, DD, HH);
      }
      else if (var.seq_fc >= seq_bk3) {
        sprintf(fname2,   "/ARCV/GRIB/MODL/GDPS/N512/%04d%02d/%02d/g512_v070_ergl_unis_h%03d.%04d%02d%02d%02d.gb2", YY, MM, DD, (var.seq - var.seq_fc)/60, YY, MM, DD, HH);
        sprintf(idxname2, "%s/g512_v070_ergl_unis_h%03d.%04d%02d%02d%02d.gbx8", INDEX_DIR, (var.seq - var.seq_fc)/60, YY, MM, DD, HH);
      }
      else {
        sprintf(fname2,   "/ARCV/GRIB/MODL/GDPS/N320/%04d%02d/%02d/g320_v050_ergl_unis_h%03d.%04d%02d%02d%02d.gb2", YY, MM, DD, (var.seq - var.seq_fc)/60, YY, MM, DD, HH);
        sprintf(idxname2, "%s/g320_v050_ergl_unis_h%03d.%04d%02d%02d%02d.gbx8", INDEX_DIR, (var.seq - var.seq_fc)/60, YY, MM, DD, HH);
      }
    }
    else if (!strcmp(var.model,"GDAPS_1H")) {
      sprintf(fname2,   "/ARCV/GRIB/MODL/GDPS/N128/%04d%02d/%02d/g128_hkor_h%03d.%04d%02d%02d%02d.gb2", YY, MM, DD, (var.seq - var.seq_fc)/60, YY, MM, DD, HH);
      sprintf(idxname2, "%s/g128_hkor_h%03d.%04d%02d%02d%02d.gbx8", INDEX_DIR, (var.seq - var.seq_fc)/60, YY, MM, DD, HH);
    }
    else if (!strcmp(var.model,"GDAPS_KIM")) {
      sprintf(fname2,   "/ARCV/RAWD/MODL/GDPS/NE36/%04d%02d/%02d/%02d/ERLY/FCST/post/sfc.ft%03d.nc", YY, MM, DD, HH, (var.seq - var.seq_fc)/60);
      sprintf(fname3,   "/ARCV/RAWD/MODL/GDPS/NE36/%04d%02d/%02d/%02d/ERLY/FCST/post/sfc.ft%03d.nc", YY, MM, DD, HH, (var.seq - var.seq_fc)/60-3);
    }
    else if (!strcmp(var.model,"KIM_1H")) {
      sprintf(fname2,   "/ARCV/GRIB/MODL/GDPS/NE36/%04d%02d/%02d/kim_g128_hkor_unis_h%03d.%04d%02d%02d%02d.gb2", YY, MM, DD, (var.seq - var.seq_fc)/60, YY, MM, DD, HH);
      sprintf(idxname2, "%s/kim_g128_hkor_unis_h%03d.%04d%02d%02d%02d.gbx8", INDEX_DIR, (var.seq - var.seq_fc)/60, YY, MM, DD, HH);

      if((var.seq-var.seq_fc)/60 > 135) {
        sprintf(fname3,   "/ARCV/GRIB/MODL/GDPS/NE36/%04d%02d/%02d/kim_g128_hkor_unis_h%03d.%04d%02d%02d%02d.gb2", YY, MM, DD, (var.seq - var.seq_fc)/60-3, YY, MM, DD, HH);
        sprintf(idxname3, "%s/kim_g128_hkor_unis_h%03d.%04d%02d%02d%02d.gbx8", INDEX_DIR, (var.seq - var.seq_fc)/60-3, YY, MM, DD, HH);
      }
      else {
        sprintf(fname3,   "/ARCV/GRIB/MODL/GDPS/NE36/%04d%02d/%02d/kim_g128_hkor_unis_h%03d.%04d%02d%02d%02d.gb2", YY, MM, DD, (var.seq - var.seq_fc)/60-1, YY, MM, DD, HH);
        sprintf(idxname3, "%s/kim_g128_hkor_unis_h%03d.%04d%02d%02d%02d.gbx8", INDEX_DIR, (var.seq - var.seq_fc)/60-1, YY, MM, DD, HH);
      }
    }

    if (stat(fname2, &st) != 0) {
      var.rain = 0;
    }

    if (!strcmp(var.model,"ECMWF_H") || !strcmp(var.model,"ECMWF_1H10G1") || !strcmp(var.model,"KIM_1H") || !strcmp(var.model,"GDAPS_KIM")) {
      if (stat(fname3, &st) != 0) {
        var.rain = 0;
      }
    }

    if (var.flag != 0) printf("Content-type: text/plain\n\n");

    // 3. 파일 열기 및 인덱스 파일 조회
    if (!strcmp(var.model,"GDAPS_KIM")) {
      code = nc_open(fname, NC_NOWRITE, &nc_id);
      if (code != NC_NOERR) return -1;

      nc_inq_dimid(nc_id, "lons", &dim_id);
      nc_inq_dimlen(nc_id, dim_id, &nlon);
      nc_inq_dimid(nc_id, "lats", &dim_id);
      nc_inq_dimlen(nc_id, dim_id, &nlat);
      nc_inq_dimid(nc_id, "levs", &dim_id);
      nc_inq_dimlen(nc_id, dim_id, &levelSize);

      nc_inq_varid(nc_id, "lats", &var_id);
      start[0] = 0;
      nc_get_var1_double(nc_id, var_id, start, &slat);
      start[0] = nlat-1;
      nc_get_var1_double(nc_id, var_id, start, &elat);

      nc_inq_varid(nc_id, "lons", &var_id);
      start[0] = 0;
      nc_get_var1_double(nc_id, var_id, start, &slon);
      start[0] = nlon-1;
      nc_get_var1_double(nc_id, var_id, start, &elon);
      dlat = (elat - slat)/(float)(nlat-1);
      dlon = (elon - slon)/(float)(nlon-1);

      nc_inq_varid(nc_id, "levs", &var_id);
      levels = (long*)malloc(sizeof(long) * levelSize);
      nlev = 0;
      for (i = 0; i < levelSize; i++) {
        start[0] = levelSize-i-1;
        nc_get_var1_float(nc_id, var_id, start, &buf);
        levels[i] = (int)buf;
        if (levels[i] >= (int)var.min_level) nlev++;
      }

      if (var.flag == 2) {
        gettimeofday(&tv2, NULL);
        printf("# 인덱스 처리 소요시간 = %f\n", (double)((tv2.tv_sec-tv1.tv_sec)*1000000+(tv2.tv_usec-tv1.tv_usec))/1000000.0);
        tv1 = tv2;
      }
    }
    else {
      //fp = fopen(fname, "rb");

      //if (!fp) {
      //    perror("ERROR: unable to input file");
      //    return -1;
      //}

      code = stat(idxname, &st);
      if (code == 0) {
        //index = codes_index_read(0, idxname, &err);
      }
      else {
        //if (!strcmp(var.model,"GDAPS")) {
        //  //index = codes_index_new_from_file(0, fname, "shortName:s,level:l", &err);
        //  index = grib_index_read_switch_file(0, idxsample, fname, &err);
        //}
        //else {
          index = codes_index_new_from_file(0, fname, "shortName,level", &err);
          codes_index_write(index, idxname);
        //}
      }

      if (var.rain == 1) {
        code2 = stat(idxname2, &st);
        if (code2 == 0) {
          //index2 = codes_index_read(0, idxname2, &err);
        }
        else {
          index2 = codes_index_new_from_file(0, fname2, "shortName,level", &err);
          codes_index_write(index2, idxname2);
        }

        if (!strcmp(var.model,"ECMWF_H") || !strcmp(var.model,"ECMWF_1H10G1") || !strcmp(var.model,"KIM_1H")) {
          code3 = stat(idxname3, &st);
          if (code3 == 0) {
            //index3 = codes_index_read(0, idxname3, &err);
          }
          else {
            index3 = codes_index_new_from_file(0, fname3, "shortName,level", &err);
            codes_index_write(index3, idxname3);
          }
        }
      }

      if (code == 0) {
        index = codes_index_read(0, idxname, &err);
      }

      if (var.rain == 1) {
        if (code2 == 0) {
          index2 = codes_index_read(0, idxname2, &err);
        }

        if (!strcmp(var.model,"ECMWF_H") || !strcmp(var.model,"ECMWF_1H10G1") || !strcmp(var.model,"KIM_1H")) {
          if (code3 == 0) {
            index3 = codes_index_read(0, idxname3, &err);
          }
        }
      }

      if (!strcmp(var.model,"GDAPS_1H")) {
        int pa[22] = {1000, 975, 950, 925, 900, 875, 850, 800, 750, 700, 650, 600, 550, 500, 450, 400, 350, 300, 250, 200, 150, 100};
        levelSize = sizeof(pa) / sizeof(int);
        levels = (long*)malloc(sizeof(long) * levelSize);
        for (i = 0; i < levelSize; i++) {
          levels[i] = pa[levelSize-i-1];
        }
      }
      else {
        CODES_CHECK(codes_index_get_size(index, "level", &levelSize), 0);
        levels = (long*)malloc(sizeof(long) * levelSize);
        CODES_CHECK(codes_index_get_long(index, "level", levels, &levelSize), 0);
      }

      if (!strcmp(var.model,"GDAPS")) get_grib_size(1, index, &h, varname[0], levels[levelSize-1], &numPoints, &nlat, &nlon, &slat, &slon, &dlat, &dlon, &elat, &elon, discipline[0], parameterCategory[0], parameterNumber[0]);
      else get_grib_size(0, index, &h, varname[0], levels[levelSize-1], &numPoints, &nlat, &nlon, &slat, &slon, &dlat, &dlon, &elat, &elon, 0, 0, 0);
      codes_handle_delete(h);

      if (var.flag == 2) {
        gettimeofday(&tv2, NULL);
        printf("# 인덱스 처리 소요시간 = %f\n", (double)((tv2.tv_sec-tv1.tv_sec)*1000000+(tv2.tv_usec-tv1.tv_usec))/1000000.0);
        tv1 = tv2;
      }
    }

    // 4. (X1,Y1)~(X2,Y2)간에 점들 설정
    err_point = 0;
    if (elon > slon) {
      if (elat > slat) {
        if (var.lon < (slon + dlon) || var.lon > (elon - dlon) || var.lat < (slat + dlat) || var.lat > (elat - dlat)) {
          err_point++;
        }

        if (var.lon2 < (slon + dlon) || var.lon2 > (elon - dlon) || var.lat2 < (slat + dlat) || var.lat2 > (elat - dlat)) {
          err_point++;
        }

        if (var.lon < (slon + dlon)) var.lon = slon + dlon;
        else if (var.lon > (elon - dlon)) var.lon = elon - dlon;

        if (var.lat < (slat + dlat)) var.lat = slat + dlat;
        else if (var.lat > (elat - dlat)) var.lat = elat - dlat;

        if (var.lon2 < (slon + dlon)) var.lon2 = slon + dlon;
        else if (var.lon2 > (elon - dlon)) var.lon2 = elon - dlon;

        if (var.lat2 < (slat + dlat)) var.lat2 = slat + dlat;
        else if (var.lat2 > (elat - dlat)) var.lat2 = elat - dlat;
      }
      else {
        if (var.lon < (slon + dlon) || var.lon > (elon - dlon) || var.lat < (elat - dlat) || var.lat > (slat + dlat)) {
          err_point++;
        }

        if (var.lon2 < (slon + dlon) || var.lon2 > (elon - dlon) || var.lat2 < (elat - dlat) || var.lat2 > (slat + dlat)) {
          err_point++;
        }

        if (var.lon < (slon + dlon)) var.lon = slon + dlon;
        else if (var.lon > (elon - dlon)) var.lon = elon - dlon;

        if (var.lat < (elat - dlat)) var.lat = elat - dlat;
        else if (var.lat > (slat + dlat)) var.lat = slat + dlat;

        if (var.lon2 < (slon + dlon)) var.lon2 = slon + dlon;
        else if (var.lon2 > (elon - dlon)) var.lon2 = elon - dlon;

        if (var.lat2 < (elat - dlat)) var.lat2 = elat - dlat;
        else if (var.lat2 > (slat + dlat)) var.lat2 = slat + dlat;
      }
    }
    else {
      if (elat > slat) {
        if (var.lon < (elon - dlon) || var.lon > (slon + dlon) || var.lat < (slat + dlat) || var.lat > (elat - dlat)) {
          err_point++;
        }

        if (var.lon2 < (elon - dlon) || var.lon2 > (slon + dlon) || var.lat2 < (slat + dlat) || var.lat2 > (elat - dlat)) {
          err_point++;
        }

        if (var.lon < (elon - dlon)) var.lon = elon - dlon;
        else if (var.lon > (slon + dlon)) var.lon = slon + dlon;

        if (var.lat < (slat + dlat)) var.lat = slat + dlat;
        else if (var.lat > (elat - dlat)) var.lat = elat - dlat;

        if (var.lon2 < (elon - dlon)) var.lon2 = elon - dlon;
        else if (var.lon2 > (slon + dlon)) var.lon2 = slon + dlon;

        if (var.lat2 < (slat + dlat)) var.lat2 = slat + dlat;
        else if (var.lat2 > (elat - dlat)) var.lat2 = elat - dlat;
      }
      else {
        if (var.lon < (elon - dlon) || var.lon > (slon + dlon) || var.lat < (elat - dlat) || var.lat > (slat + dlat)) {
          err_point++;
        }

        if (var.lon2 < (elon - dlon) || var.lon2 > (slon + dlon) || var.lat2 < (elat - dlat) || var.lat2 > (slat + dlat)) {
          err_point++;
        }

        if (var.lon < (elon - dlon)) var.lon = elon - dlon;
        else if (var.lon > (slon + dlon)) var.lon = slon + dlon;

        if (var.lat < (elat - dlat)) var.lat = elat - dlat;
        else if (var.lat > (slat + dlat)) var.lat = slat + dlat;

        if (var.lon2 < (elon - dlon)) var.lon2 = elon - dlon;
        else if (var.lon2 > (slon + dlon)) var.lon2 = slon + dlon;

        if (var.lat2 < (elat - dlat)) var.lat2 = elat - dlat;
        else if (var.lat2 > (slat + dlat)) var.lat2 = slat + dlat;
      }
    }

    if (err_point > 0) {
      err = -1;
      return err;
    }

    lon1 = var.lon;  lon2 = var.lon2;
    lat1 = var.lat;  lat2 = var.lat2;

    if (lon1 < 0) lon1 += 360;
    x1 = (lon1 - slon) / dlon;
    y1 = (lat1 - slat) / dlat;
  
    if (lon2 < 0) lon2 += 360;
    x2 = (lon2 - slon) / dlon;
    y2 = (lat2 - slat) / dlat;

    dd = 1.;
    len = sqrt( (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) );
    nd = (int)(len / dd) + 1;
    xznums = nd;

    xz = (double ***) malloc((unsigned) (varSize2)*sizeof(double **));
    for (j = 0; j < varSize2; j++) {
      xz[j] = (double **) malloc((unsigned) (levelSize)*sizeof(double *));
    }   

    for (j = 0; j < varSize2; j++) {
      for (i = 0; i < levelSize; i++) {
        xz[j][i] = (double *) malloc((unsigned) (xznums+1)*sizeof(double));
      }
    }  

    // 5. 변수 조회용 배열 설정
    if (y2 < y1) {
      y3 = y2; y4 = y1;
    }
    else {
      y3 = y1; y4 = y2;
    }

    if (x2 < x1) {
      x3 = x2; x4 = x1;
    }
    else {
      x3 = x1; x4 = x2;
    }

    start_4d[0] = 0; start_4d[1] = 0; start_4d[2] = (int)y3; start_4d[3] = (int)x3;
    count_4d[0] = 1; count_4d[1] = nlev; count_4d[2] = (int)(y4-y3+1)+1; count_4d[3] = (int)(x4-x3+1)+1;

    if (!strcmp(var.model,"GDAPS_KIM")) {
      g = (double *) malloc((count_4d[0]*count_4d[1]*count_4d[2]*count_4d[3])*sizeof(double));
    }
    else {
      g = (double *) malloc((unsigned) (numPoints)*sizeof(double));
    }

    if (!strcmp(var.varname, "CONV")) {
      uuu  = (float ***) malloc((unsigned) (levelSize)*sizeof(float **));
      vvv  = (float ***) malloc((unsigned) (levelSize)*sizeof(float **));
      conv = (float ***) malloc((unsigned) (levelSize)*sizeof(float **));
      for (j = 0; j < levelSize; j++) {
        uuu[j]  = (float **) malloc((unsigned) (count_4d[2])*sizeof(float *));
        vvv[j]  = (float **) malloc((unsigned) (count_4d[2])*sizeof(float *));
        conv[j] = (float **) malloc((unsigned) (count_4d[2])*sizeof(float *));
      }   

      for (j = 0; j < levelSize; j++) {
        for (i = 0; i < count_4d[2]; i++) {
          uuu[j][i]  = (float *) malloc((unsigned) (count_4d[3])*sizeof(float));
          vvv[j][i]  = (float *) malloc((unsigned) (count_4d[3])*sizeof(float));
          conv[j][i] = (float *) malloc((unsigned) (count_4d[3])*sizeof(float));
        }
      }  
    }

    // 6. 변수 조회
    for (ivar = 0; ivar < varSize; ivar++) {
      // 자료 읽기
      if (!strcmp(var.model,"GDAPS_KIM")) {
        nc_inq_varid(nc_id, varname[ivar], &var_id);
        nc_get_var_chunk_cache(nc_id, var_id, &sizep, &nelemsp, &preemptionp);
        nc_set_var_chunk_cache(nc_id, var_id, nlat*nlon*sizeof(double), nelemsp, preemptionp);
        nc_get_vara_double(nc_id, var_id, start_4d, count_4d, g);
        if (!strcmp(var.varname, "CONV")) {
          if (ivar == 1 || ivar == 2) {
            for (ilev = 0; ilev < levelSize; ilev++) {
              if (levels[ilev] < (int)var.min_level) continue;
              for (dj = 0; dj < count_4d[2]; dj++) {
                for (di = 0; di < count_4d[3]; di++) {
                  if (ivar == 1) {
                    uuu[ilev][dj][di] = g[(levelSize-ilev-1)*(count_4d[2]*count_4d[3])+dj*count_4d[3]+di];
                  }
                  else if (ivar == 2) {
                    vvv[ilev][dj][di] = g[(levelSize-ilev-1)*(count_4d[2]*count_4d[3])+dj*count_4d[3]+di];
                  }
                }
              }

              if (ivar == 2) calc_conv(count_4d[3], count_4d[2], uuu[ilev], vvv[ilev], conv[ilev], slon+dlon*(float)start_4d[3], slat+dlat*(float)start_4d[2], dlon, dlat);
            }
          }
        }

        for (k = 0; k <= xznums; k++) {
          if (x2 < x1) {
            x = (x1 - x2) - (x1 - x2) * k / xznums;
          }
          else x = (x2 - x1) * k / xznums;

          if (y2 < y1) {
            y = (y1 - y2) - (y1 - y2) * k / xznums;
          }
          else y = (y2 - y1) * k / xznums;

          i = (int)x;
          j = (int)y;

          dx = x - i;
          dy = y - j;

          for (ilev = 0; ilev < levelSize; ilev++) {
            if (levels[ilev] < (int)var.min_level) continue;

            v1 = (g[(levelSize-ilev-1)*(count_4d[2]*count_4d[3])+j*count_4d[3]+i])*(1-x+i) + (g[(levelSize-ilev-1)*(count_4d[2]*count_4d[3])+j*count_4d[3]+(i+1)])*(x-i);
            v2 = (g[(levelSize-ilev-1)*(count_4d[2]*count_4d[3])+(j+1)*count_4d[3]+i])*(1-x+i) + (g[(levelSize-ilev-1)*(count_4d[2]*count_4d[3])+(j+1)*count_4d[3]+(i+1)])*(x-i);

            xz[ivar][ilev][k] = v1*(1-y+j) + v2*(y-j);

            if (!strcmp(var.varname, "CONV") && ivar == 2) {
              v1 = conv[ilev][j][i]*(1-x+i) + conv[ilev][j][i+1]*(x-i);
              v2 = conv[ilev][j+1][i]*(1-x+i) + conv[ilev][j+1][i+1]*(x-i);
              xz[3][ilev][k] = v1*(1-y+j) + v2*(y-j);
            }
          }
        }
        if (var.flag == 2) {
          gettimeofday(&tv2, NULL);
          printf("# %d번째 변수(%s) %d개 층 처리 소요시간 = %f, ", ivar+1, varname[ivar], nlev, levels[ilev], (double)((tv2.tv_sec-tv1.tv_sec)*1000000+(tv2.tv_usec-tv1.tv_usec))/1000000.0);
          printf("xznums=%d, numPoints=%d\n", xznums+1, count_4d[0]*count_4d[1]*count_4d[2]*count_4d[3]);
          tv1 = tv2;
        }
      }
      else {
        for (ilev = 0; ilev < levelSize; ilev++) {
          if (levels[ilev] < (int)var.min_level) continue;

          if (!strcmp(var.model,"GDAPS")) {
            //get_grib_size(0, index, &h, varname[ivar], levels[ilev], &numPoints, &nlat, &nlon, &slat, &slon, &dlat, &dlon, &elat, &elon, discipline[ivar], parameterCategory[ivar], parameterNumber[ivar]);
            get_grib_size(0, index, &h, varname[ivar], levels[ilev], &numPoints, &nlat1, &nlon1, &slat1, &slon1, &dlat1, &dlon1, &elat1, &elon1, discipline[ivar], parameterCategory[ivar], parameterNumber[ivar]);
          }
          else get_grib_size(0, index, &h, varname[ivar], levels[ilev], &numPoints, &nlat1, &nlon1, &slat1, &slon1, &dlat1, &dlon1, &elat1, &elon1, 0, 0, 0);

          if (!strcmp(var.model,"GDAPS_1H") && !strcmp(varname[ivar],"u")) {
            if (gu == NULL) {
              gu = (double *) malloc((unsigned) (numPoints)*sizeof(double));
            }
            codes_get_double_array(h, "values", gu, &numPoints);

            for (dj = 0; dj < nlat; dj++) {
              for (di = 0; di < nlon; di++) {
                if (di == nlon-1) {
                  g[dj*nlon+di] = gu[dj*nlon1+di];
                }
                else {
                  g[dj*nlon+di] = (gu[dj*nlon1+di] + gu[dj*nlon1+di+1])/2.;
                }
              }
            }
          }
          else if (!strcmp(var.model,"GDAPS_1H") && !strcmp(varname[ivar],"v")) {
            if (gv == NULL) {
              gv = (double *) malloc((unsigned) (numPoints)*sizeof(double));
            }
            codes_get_double_array(h, "values", gv, &numPoints);

            for (dj = 0; dj < nlat; dj++) {
              for (di = 0; di < nlon; di++) {
                g[dj*nlon+di] = (gv[dj*nlon1+di] + gv[(dj+1)*nlon1+di])/2.;
              }
            }
          }
          else codes_get_double_array(h, "values", g, &numPoints);
          // 에러 체크해야하는데...

          if (!strcmp(var.varname, "CONV")) {
            if (ivar == 1 || ivar == 2) {
              for (dj = 0; dj < count_4d[2]; dj++) {
                for (di = 0; di < count_4d[3]; di++) {
                  if (ivar == 1) {
                    uuu[ilev][dj][di] = g[(start_4d[2]+dj)*nlon+start_4d[3]+di];
                  }
                  else if (ivar == 2) {
                    vvv[ilev][dj][di] = g[(start_4d[2]+dj)*nlon+start_4d[3]+di];
                  }
                }
              }
              if (ivar == 2) calc_conv(count_4d[3], count_4d[2], uuu[ilev], vvv[ilev], conv[ilev], slon+dlon*(float)start_4d[3], slat+dlat*(float)start_4d[2], dlon, dlat);
            }
          }

          for (k = 0; k <= xznums; k++) {
            x = (x2 - x1) * k / xznums + x1;
            y = (y2 - y1) * k / xznums + y1;

            i = (int)x;
            j = (int)y;

            dx = x - i;
            dy = y - j;

            v1 = (g[j*nlon+i])*(1-x+i) + (g[j*nlon+(i+1)])*(x-i);
            v2 = (g[(j+1)*nlon+i])*(1-x+i) + (g[(j+1)*nlon+(i+1)])*(x-i);

            xz[ivar][ilev][k] = v1*(1-y+j) + v2*(y-j);

            if (!strcmp(var.varname, "CONV") && ivar == 2) {
              if (x2 < x1) {
                x = (x1 - x2) - (x1 - x2) * k / xznums;
              }
              else x = (x2 - x1) * k / xznums;

              if (y2 < y1) {
                y = (y1 - y2) - (y1 - y2) * k / xznums;
              }
              else y = (y2 - y1) * k / xznums;

              i = (int)x;
              j = (int)y;

              dx = x - i;
              dy = y - j;

              v1 = conv[ilev][j][i]*(1-x+i) + conv[ilev][j][i+1]*(x-i);
              v2 = conv[ilev][j+1][i]*(1-x+i) + conv[ilev][j+1][i+1]*(x-i);
              xz[3][ilev][k] = v1*(1-y+j) + v2*(y-j);
            }
          }

          codes_handle_delete(h);

          if (var.flag == 2) {
            gettimeofday(&tv2, NULL);
            printf("# %d번째 변수(%s), %dhPa 처리 소요시간 = %f, xznums=%d, numPoints=%d\n", ivar+1, varname[ivar], levels[ilev], (double)((tv2.tv_sec-tv1.tv_sec)*1000000+(tv2.tv_usec-tv1.tv_usec))/1000000.0, xznums+1, numPoints);
            tv1 = tv2;
          }
        }
        if (var.flag == 2) printf("\n");
      }
    }

    for (ivar = 0; ivar < varSize2; ivar++) {
      if (ivar == 1 || ivar == 2) continue;

      for (ilev = 0; ilev < levelSize; ilev++) {
        if (levels[ilev] < (int)var.min_level) continue;

        for (k = 0; k <= xznums; k++) {
          // Temperature: Kelvin to Celsius
          if (ivar == 0) { 
            xz[ivar][ilev][k] -= 273.15;
          }
          else if (ivar == 3) { 
            if (!strcmp(var.varname, "DPDK")) {
              // Dew point Temperature
              xz[ivar][ilev][k] = xz[0][ilev][k] - get_dew_temp(xz[0][ilev][k]+273.15, xz[ivar][ilev][k], (float)(levels[ilev]));
            }
            else if (!strcmp(var.varname, "TD")) {
              // Dew point Temperature
              xz[ivar][ilev][k] = get_dew_temp(xz[0][ilev][k]+273.15, xz[ivar][ilev][k], (float)(levels[ilev]));
            }
            else if (!strcmp(var.varname, "WSPD")) {
              // Wind Speed
              xz[ivar][ilev][k] = sqrt(xz[1][ilev][k]*xz[1][ilev][k] + xz[2][ilev][k]*xz[2][ilev][k])*1.94384;
            }
            else if (!strcmp(var.varname, "EPOT")) {
              // Eqiuv.P.Temp
              xz[ivar][ilev][k] = get_ept(xz[0][ilev][k], get_dew_temp(xz[0][ilev][k]+273.15, xz[ivar][ilev][k], (float)(levels[ilev])), (float)(levels[ilev]));
            }
            else if (!strcmp(var.varname, "PVEL") && (!strcmp(var.model,"ECMWF_H") || !strcmp(var.model,"ECMWF_1H10G1"))) {
              // P-velocity
              xz[ivar][ilev][k] *= 36.;
            }
          }
          else if (ivar == 4) { 
            if (!strcmp(var.varname, "PVEL")) {
              // P-velocity
              xz[ivar][ilev][k] = calc_w_to_omega(xz[4][ilev][k], xz[0][ilev][k]+273.15, xz[3][ilev][k], (float)(levels[ilev]));
            }
          }

          //printf("%d, value[%d][%d][%d]: %f\n", levels[ilev], ivar, ilev, k, xz[ivar][ilev][k]);
        }
      }
    }

    // 9. 파일 종료
    if (!strcmp(var.model,"GDAPS_KIM")) {
      nc_close(nc_id);
    } 
    else {
      //if (code != 0) codes_index_write(index, idxname);
      codes_index_delete(index);
    }


    // 지상 변수(강수량) 확인
    if (var.rain == 1) {
      // 1. 조회할 변수 설정
      if (!strcmp(var.model,"GDAPS") || !strcmp(var.model, "GDAPS_1H") || !strcmp(var.model, "KIM_1H")) {
        varSize = 4;
      }
      else if (!strcmp(var.model,"GDAPS_KIM")) {
        varSize = 2;
      }
      else {
        varSize = 2;
      }

      char   varname2[varSize][32];
      long   levels2[varSize];
      long   discipline2[varSize], parameterCategory2[varSize], parameterNumber2[varSize];

      for (i = 0; i < varSize; i++) {
        switch (i) {
          case 0:  
            if (!strcmp(var.model,"ECMWF_H") || !strcmp(var.model,"ECMWF_1H10G1")) {
              strcpy(varname2[i], "lsp");
              levels2[i] = 0;
            }
            else if (!strcmp(var.model,"GDAPS_KIM")) {
              strcpy(varname2[i], "precl");
              levels2[i] = 0;
            }
            else if (!strcmp(var.model,"KIM_1H")) {
              strcpy(varname2[i], "lswp");
            }
            else {
              strcpy(varname2[i], "ncpcp");
              levels2[i] = 0;
            }
            break;
          case 1:
            if (!strcmp(var.model,"ECMWF_H") || !strcmp(var.model,"ECMWF_1H10G1")) {
              strcpy(varname2[i], "cp");
            }
            else if (!strcmp(var.model,"GDAPS_KIM")) {
              strcpy(varname2[i], "precc");
            }
            else if (!strcmp(var.model,"KIM_1H")) {
              strcpy(varname2[i], "cwp");
            }
            else {
              strcpy(varname2[i], "acpcp");
            }
            break;
          case 2:  strcpy(varname2[i], "snol");  break;
          case 3:  strcpy(varname2[i], "snoc");  break;
        }

        if (!strcmp(var.model,"GDAPS")) {
          switch (i) {
            case 0:  
              discipline2[i] = 0; parameterCategory2[i] = 1; parameterNumber2[i] = 9;
              break;
            case 1:  
              discipline2[i] = 0; parameterCategory2[i] = 1; parameterNumber2[i] = 10;
              break;
            case 2:  
              discipline2[i] = 0; parameterCategory2[i] = 1; parameterNumber2[i] = 15;
              break;
            case 3:  
              discipline2[i] = 0; parameterCategory2[i] = 1; parameterNumber2[i] = 14;
              break;
          }
        }
      }

      // 3. 파일 열기
      if (!strcmp(var.model,"GDAPS_KIM")) {
        code = nc_open(fname2, NC_NOWRITE, &nc_id);
        if (code != NC_NOERR) return -1;

        code2 = nc_open(fname3, NC_NOWRITE, &nc_id2);
        if (code2 != NC_NOERR) return -1;
      }

      // 4. (X1,Y1)~(X2,Y2)간에 점들 설정
      xx = (double *) malloc((unsigned) (xznums+1)*sizeof(double));

      // 5. 변수 조회용 배열 설정
      start_3d[0] = 0; start_3d[1] = (int)y3; start_3d[2] = (int)x3;
      count_3d[0] = 1; count_3d[1] = (int)(y4-y3+1)+1; count_3d[2] = (int)(x4-x3+1)+1;

      if (!strcmp(var.model,"GDAPS_KIM")) {
        g2 = (double *) malloc((count_3d[0]*count_3d[1]*count_3d[2])*sizeof(double));
        g3 = (double *) malloc((count_3d[0]*count_3d[1]*count_3d[2])*sizeof(double));
      }
      else {
        g2 = (double *) malloc((unsigned) (numPoints)*sizeof(double));
        g3 = (double *) malloc((unsigned) (numPoints)*sizeof(double));
      }

      // 6. 변수 조회
      for (ivar = 0; ivar < varSize; ivar++) {
        // 자료 읽기
        if (!strcmp(var.model,"GDAPS_KIM")) {
          nc_inq_varid(nc_id, varname2[ivar], &var_id);
          nc_get_var_chunk_cache(nc_id, var_id, &sizep, &nelemsp, &preemptionp);
          nc_set_var_chunk_cache(nc_id, var_id, nlat*nlon*sizeof(double), nelemsp, preemptionp);
          nc_get_vara_double(nc_id, var_id, start_3d, count_3d, g2);

          nc_inq_varid(nc_id2, varname2[ivar], &var_id);
          nc_set_var_chunk_cache(nc_id2, var_id, nlat*nlon*sizeof(double), nelemsp, preemptionp);
          nc_get_vara_double(nc_id2, var_id, start_3d, count_3d, g3);

          for (k = 0; k <= xznums; k++) {
            if (x2 < x1) {
              x = (x1 - x2) - (x1 - x2) * k / xznums;
            }
            else x = (x2 - x1) * k / xznums;

            if (y2 < y1) {
              y = (y1 - y2) - (y1 - y2) * k / xznums;
            }
            else y = (y2 - y1) * k / xznums;

            i = (int)x;
            j = (int)y;

            dx = x - i;
            dy = y - j;

            v1 = ((g2[j*count_3d[2]+i])*(1-x+i) + (g2[j*count_3d[2]+(i+1)])*(x-i)) - ((g3[j*count_3d[2]+i])*(1-x+i) + (g3[j*count_3d[2]+(i+1)])*(x-i));
            v2 = ((g2[(j+1)*count_3d[2]+i])*(1-x+i) + (g2[(j+1)*count_3d[2]+(i+1)])*(x-i)) - ((g3[(j+1)*count_3d[2]+i])*(1-x+i) + (g3[(j+1)*count_3d[2]+(i+1)])*(x-i));

            if (ivar == 0) {
              xx[k] = v1*(1-y+j) + v2*(y-j);
            }
            else {
              xx[k] += v1*(1-y+j) + v2*(y-j);
            }
          }

          if (var.flag == 2) {
            gettimeofday(&tv2, NULL);
            printf("# %d번째 변수(%s) 처리 소요시간 = %f, ", ivar+1, varname2[ivar], (double)((tv2.tv_sec-tv1.tv_sec)*1000000+(tv2.tv_usec-tv1.tv_usec))/1000000.0);
            printf("xznums=%d, numPoints=%d\n", xznums+1, count_4d[0]*count_4d[1]*count_4d[2]*count_4d[3]);
            tv1 = tv2;
          }
        }
        else {
          if (!strcmp(var.model,"GDAPS")) {
            get_grib_size(0, index2, &h2, varname2[ivar], 0, &numPoints, &nlat1, &nlon1, &slat1, &slon1, &dlat1, &dlon1, &elat1, &elon1, discipline[ivar], parameterCategory[ivar], parameterNumber[ivar]);
          }
          else {
            get_grib_size(0, index2, &h2, varname2[ivar], 0, &numPoints, &nlat1, &nlon1, &slat1, &slon1, &dlat1, &dlon1, &elat1, &elon1, 0, 0, 0);
          }
          codes_get_double_array(h2, "values", g2, &numPoints);
          if (!strcmp(var.model,"ECMWF_H") || !strcmp(var.model,"ECMWF_1H10G1") || !strcmp(var.model,"KIM_1H")) {
            get_grib_size(0, index3, &h3, varname2[ivar], 0, &numPoints, &nlat1, &nlon1, &slat1, &slon1, &dlat1, &dlon1, &elat1, &elon1, 0, 0, 0);
            codes_get_double_array(h3, "values", g3, &numPoints);
          }

          for (k = 0; k <= xznums; k++) {
            x = (x2 - x1) * k / xznums + x1;
            y = (y2 - y1) * k / xznums + y1;

            i = (int)x;
            j = (int)y;

            dx = x - i;
            dy = y - j;

            if (!strcmp(var.model,"ECMWF_H") || !strcmp(var.model,"ECMWF_1H10G1") || !strcmp(var.model,"KIM_1H")) {
              v1 = ((g2[j*nlon+i])*(1-x+i) + (g2[j*nlon+(i+1)])*(x-i)) - ((g3[j*nlon+i])*(1-x+i) + (g3[j*nlon+(i+1)])*(x-i));
              v2 = ((g2[(j+1)*nlon+i])*(1-x+i) + (g2[(j+1)*nlon+(i+1)])*(x-i)) - ((g3[(j+1)*nlon+i])*(1-x+i) + (g3[(j+1)*nlon+(i+1)])*(x-i));

              if (!strcmp(var.model,"ECMWF_H") || !strcmp(var.model,"ECMWF_1H10G1")) {
                v1 *= 1000.;
                v2 *= 1000.;
              }
            }
            else {
              v1 = ((g2[j*nlon+i])*(1-x+i) + (g2[j*nlon+(i+1)])*(x-i));
              v2 = ((g2[(j+1)*nlon+i])*(1-x+i) + (g2[(j+1)*nlon+(i+1)])*(x-i));
            }

            if (ivar == 0) {
              xx[k] = v1*(1-y+j) + v2*(y-j);
            }
            else {
              xx[k] += v1*(1-y+j) + v2*(y-j);
            }
          }

          codes_handle_delete(h2);
          if (!strcmp(var.model,"ECMWF_H") || !strcmp(var.model,"ECMWF_1H10G1") || !strcmp(var.model,"KIM_1H")) {
            codes_handle_delete(h3);
          }

          if (var.flag == 2) {
            gettimeofday(&tv2, NULL);
            printf("# %d번째 변수(%s) 처리 소요시간 = %f, xznums=%d, numPoints=%d\n", ivar+1, varname2[ivar], (double)((tv2.tv_sec-tv1.tv_sec)*1000000+(tv2.tv_usec-tv1.tv_usec))/1000000.0, xznums+1, numPoints);
            tv1 = tv2;
          }
        }
      }

      // 9. 파일 종료
      if (!strcmp(var.model,"GDAPS_KIM")) {
        nc_close(nc_id);
        nc_close(nc_id2);
      } 
      else {
        codes_index_delete(index2);

        if (!strcmp(var.model,"ECMWF_H") || !strcmp(var.model,"ECMWF_1H10G1") || !strcmp(var.model,"KIM_1H")) {
          codes_index_delete(index3);
        }
      }
    }


    if (var.flag == 2) {
      printf("\n# 전체 소요시간 = %f\n", (double)((tv2.tv_sec-tv0.tv_sec)*1000000+(tv2.tv_usec-tv0.tv_usec))/1000000.0);
    }

    // 7. 이미지 생성
    if (var.flag == 0 || var.flag == 1) crss_sct_img(varSize2, levelSize, xznums, xz, levels, xx);

    // 8. 배열 해제
    free((char*) (g));
    if (!strcmp(var.model,"GDAPS_1H")) {
      free((char*) (gu));
      free((char*) (gv));
    }

    if (var.rain == 1) {
      free((char*) (g2));
      free((char*) (g3));
    }

    for (j = varSize2-1; j >= 0; j--) {
      for (i = levelSize-1; i >= 0; i--) {
        free(xz[j][i]);
      }
    }
    for (j = varSize2-1; j >= 0; j--) free(xz[j]);
    free(xz);

    if (var.rain == 1) {
      free(xx);
    }

    if (!strcmp(var.varname, "CONV")) {
      for (j = levelSize-1; j >= 0; j--) {
        for (i = count_4d[2]-1; i >= 0; i--) {
          free(uuu[j][i]);
          free(vvv[j][i]);
          free(conv[j][i]);
        }
      }
      for (j = levelSize-1; j >= 0; j--) {
        free(uuu[j]);
        free(vvv[j]);
        free(conv[j]);
      }
      free(uuu);
      free(vvv);
      free(conv);
    }

    return 0;
}

//=====================
// 지형 자료 얻기
//=====================
int topo_data_get(float *topo_x)
{
  FILE* fp = NULL;
  int err = 0;
  char fname[512], idxname[512], imgname[512];  
  short  **topo;
  float  *topo_xz;
  int    nx, ny;
  float  slat, slon, dlat, dlon, elat, elon;
  int    i, j, k, ix;
  float  v, v1, v2, vv;
  float  x, y, dx, dy, di;
  float  len, dd, x1, x2, y1, y2, lon1, lon2, lat1, lat2;
  int    nd, xznums;

  nx = 3600; ny = 1800;
  slon = (float)0;   dlon = (float)360/(float)nx;
  slat = (float)-90; dlat = (float)180/(float)ny;

  sprintf(fname, "%s/gmted_mealatlon.bin", TOPO_DIR);
  if ((fp = fopen(fname, "rb")) == NULL) return -1;

  topo = smatrix(0, ny, 0, nx);
  fseek(fp, (long)64, SEEK_SET);    // 해더(64byts) skip
  for (j = 0; j <= ny; j++) {
    fread(topo[j], 2, nx+1, fp);
  }
  fclose(fp);

  lon1 = var.lon;  lon2 = var.lon2;
  lat1 = var.lat;  lat2 = var.lat2;

  if (lon1 < 0) lon1 += 360;
  x1 = (lon1 - slon) / dlon;
  y1 = (lat1 - slat) / dlat;
  
  if (lon2 < 0) lon2 += 360;
  x2 = (lon2 - slon) / dlon;
  y2 = (lat2 - slat) / dlat;

  dd = 1.;
  len = sqrt( (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) );
  nd = (int)(len / dd) + 1;
  xznums = nd;

  topo_xz = (float *) malloc((unsigned) (xznums+1)*sizeof(float));
  for (k = 0; k <= xznums; k++) {
    x = (x2 - x1) * k / xznums + x1;
    y = (y2 - y1) * k / xznums + y1;

    i = (int)x;
    j = (int)y;

    dx = x - i;
    dy = y - j;

    v1 = ((float)topo[j][i])*(1-x+i)   + ((float)topo[j][i+1])*(x-i);
    v2 = ((float)topo[j+1][i])*(1-x+i) + ((float)topo[j+1][i+1])*(x-i);

    topo_xz[k] = v1*(1-y+j) + v2*(y-j);
  }
  free_smatrix(topo, 0, ny, 0, nx);

  for (i = 0; i <= var.NI; i++) {
    di = (float)i/((float)(var.NI)/(float)xznums) - (int)(i/((float)(var.NI)/(float)xznums));
    ix = (int)(i/((float)(var.NI)/(float)xznums));
    vv = topo_xz[ix]*(1-di) + topo_xz[ix+1]*(di);
    topo_x[i] = hh_to_pres(vv);
  }
  
  free(topo_xz);

  return;
}

/*=============================================================================*
 *  최대 지도영역 확인
 *=============================================================================*/
int map_ini_read()
{
  FILE  *fp;
  char  map_cd[8], buf[300], tmp[300];
  float max_sx, max_sy, max_dx, max_dy, max_nx = 0;
  int   i, j, k, n = 0;

  // 1. 지도설정파일 열기
  if ((fp = fopen(MAP_INI_FILE, "r")) == NULL) return -1;
  
  // 2. 해당되는 지도 정보 읽기
  while (fgets(buf, 300, fp) != NULL) {
    if (buf[0] == '#') continue;

    getword(map_cd, buf, ':');
    if (!strcmp(map_cd, var.map)) {
      getword(tmp, buf, ':');  var.NX = atof(tmp);
      getword(tmp, buf, ':');  var.NY = atof(tmp);
      getword(tmp, buf, ':');  var.SX = atof(tmp);
      getword(tmp, buf, ':');  var.SY = atof(tmp);
    }
  }
  fclose(fp);

  return 0;
}

/*=============================================================================*
 *  색상표
 *=============================================================================*/
int color_table(gdImagePtr im, int color_lvl[], float data_lvl[])
{
  FILE  *fp;
  char  color_file[120], fname[256];
  float v1;
  int   R, G, B, num_color;

  sprintf(fname, "%s/color_crss_sct_%s.rgb", COLOR_SET_DIR, var.varname);    // 레이더 색상표 

  if ((fp = fopen(fname,"r")) == NULL) return -1;
  num_color = 0;
  while (fscanf(fp, "%d %d %d %f\n", &R, &G, &B, &v1) != EOF) {
    color_lvl[num_color] = gdImageColorAllocate(im, R, G, B);
    data_lvl[num_color] = v1;
    num_color++;
    if (num_color >= 120) break;
  }
  fclose(fp);

  // 5. 기타 색상표 설정
  color_lvl[236] = gdImageColorAllocate(im, 30, 180, 30);     // 녹색1
  color_lvl[237] = gdImageColorAllocate(im, 150, 150, 150);   // 테두리색
  color_lvl[238] = gdImageColorAllocate(im, 250, 250, 00);    // 노란색
  //color_lvl[239] = gdImageColorAllocate(im, 30, 180, 30);     // 녹색
  color_lvl[239] = gdImageColorAllocate(im, 0, 180, 0);     // 녹색2
  //color_lvl[240] = gdImageColorAllocate(im, 180, 180, 180);   // 배경색1
  color_lvl[240] = gdImageColorAllocate(im, 150, 150, 150);   // 배경색1
  color_lvl[241] = gdImageColorAllocate(im, 255, 255, 255);   // 배경색2
  color_lvl[242] = gdImageColorAllocate(im, 30, 30, 30);      // 지도색
  color_lvl[243] = gdImageColorAllocate(im, 12, 28, 236);     // 제목
  color_lvl[244] = gdImageColorAllocate(im, 0, 0, 0);         // 검은색
  color_lvl[245] = gdImageColorAllocate(im, 240, 240, 240);
  color_lvl[246] = gdImageColorAllocate(im, 255, 0, 0);
  color_lvl[247] = gdImageColorAllocate(im, 0, 0, 255);
  color_lvl[248] = gdImageColorAllocate(im, 160, 160, 160);   // 배경색3
  color_lvl[249] = gdImageColorAllocate(im, 110, 110, 110);   // 시군경계
  color_lvl[250] = gdImageColorAllocate(im, 220, 230, 242);   // 배경색4
  color_lvl[251] = gdImageColorAllocate(im, 255, 0, 172);     // 단면선

  return num_color;
}

/*******************************************************************************
 *  wd : 풍향(degree), ws : 풍속 (m/s)
 *******************************************************************************/
int plot_wd(
  gdImagePtr im,    // 이미지영역
  int xs,  
  int ys,  
  float uu1, 
  float vv1,
  int color_wd
)
{
  float WR = 16.0;
  float wr_s, wd_s, ws, wd, adj_wd;
  int x1, y1, x2, y2, x3, y3;
  gdPoint xy[3], xy2[3], xy3[3], xy4[3], xy5[3];
  char   txt[128], txt_utf[128];
  double font_size = 8.0;
  int    brect[8];
  int    i;

  ws = sqrt(uu1*uu1 + vv1*vv1);
  wd = 180./3.141592 * atan(uu1/vv1);
  if ( vv1  > 0.0)                  wd = wd + 180;
  if ((vv1  < 0.0) && (uu1 >= 0.0)) wd = wd + 360;
  if ((vv1 == 0.0) && (uu1 >  0.0)) wd = 270.;
  if ((vv1 == 0.0) && (uu1 <  0.0)) wd = 90.;

  if (wd > 360) wd -= 360;
  else if (wd < 0) wd += 360;

  if (wd > 360) wd -= 360;
  if (wd < 0 || wd > 360) return;
  if (ws < 0 || ws > 100) return;

  // 풍향
  wd *= DEGRAD;

  x1 = xs;
  y1 = ys;
  x2 = xs + (int)(WR*sin(wd) + 0.5);
  y2 = ys - (int)(WR*cos(wd) + 0.5);
  if (ws > 0.2) {
    gdImageLine(im, x1, y1, x2, y2, color_wd);
  }

  // 풍속깃
  while (ws > 0.0)
  {
    if (ws < 5)
      wr_s = 2.0*ws;
    else
      wr_s = 10.0;

    wd_s = wd + 60.0*DEGRAD;
    x1 = x2 + (int)(wr_s*sin(wd_s) + 0.5);
    y1 = y2 - (int)(wr_s*cos(wd_s) + 0.5);

    if (ws >= 25) {
      WR -= 2.5;
      x3 = xs + (int)(WR*sin(wd) + 0.5);
      y3 = ys - (int)(WR*cos(wd) + 0.5);

      xy[0].x = x1; xy2[0].x = x1;   xy3[0].x = x1;   xy4[0].x = x1-1; xy5[0].x = x1+1;
      xy[0].y = y1; xy2[0].y = y1-1; xy3[0].y = y1+1; xy4[0].y = y1;   xy5[0].y = y1;
      xy[1].x = x2; xy2[1].x = x2;   xy3[1].x = x2;   xy4[1].x = x2-1; xy5[1].x = x2+1;
      xy[1].y = y2; xy2[1].y = y2-1; xy3[1].y = y2+1; xy4[1].y = y2;   xy5[1].y = y2;
      xy[2].x = x3; xy2[2].x = x3;   xy3[2].x = x3;   xy4[2].x = x3-1; xy5[2].x = x3+1;
      xy[2].y = y3; xy2[2].y = y3-1; xy3[2].y = y3+1; xy4[2].y = y3;   xy5[2].y = y3;
      gdImageFilledPolygon(im, xy, 3, color_wd);

      ws -= 25.0;
      x2 = x3;
      y2 = y3;
    }
    else {
      gdImageLine(im, x1, y1, x2, y2, color_wd);
      ws -= 5.0;
      WR -= 2.5;
      x2 = xs + (int)(WR*sin(wd) + 0.5);
      y2 = ys - (int)(WR*cos(wd) + 0.5);
    }
  }

  //gdImageFilledArc(im, xs, ys, 1, 1, 0, 360, color_wd, gdArc);
  return;
}

/*============================================================================*
 *  data --> level
 *============================================================================*/
int data_level(float data, int num_level, float data_lvl[])
{
    int  v = num_level-1, i;

    for (i = 0; i < num_level-1; i++) {
        if (data < data_lvl[i]) {
            v = i;
            break;
        }
    }
    return v;
}

/*=============================================================================*
 *  minimum & maximum of level
 *  int *v      : inp:입력 자료들
 *  int n       : inp:자료의 수
 *  int *imax   : inp:자료중 최대값
 *  int *imin   : inp:자료중 최소값
 *=============================================================================*/
int level_max_min(int *v, int n, int *imax, int *imin)
{
    int i;

    *imax = v[0];
    *imin = v[0];

    for (i = 1; i < n; i++) {
        if (v[i] > *imax) *imax = v[i];
        if (v[i] < *imin) *imin = v[i];
    }
    return 0;
}

/*=============================================================================*
 *  등치선
 *=============================================================================*/
int plot_contour(gdImagePtr im, float **g, int num_data, float data_lvl[], int color)
{
  float  q1, q2, q3, q4;
  float  x, y, f;
  int    v[5], vmax, vmin, v1, styleDashed[4];
  int    x1, y1, x2, y2, legend[3][num_data];
  int    i, j, i1, j1, i2, j2, n, m, k;
  char   txt[256];

  for (j = 0; j < 3; j++) {
    for (i = 0; i < num_data; i++) {
      legend[j][i] = 0;
    }
  }

  //gdImageSetAntiAliased(im, color);
  //printf("Content-type: text/plain\n\n");

  //styleDashed[0] = color;
  //styleDashed[1] = color;
  //styleDashed[2] = gdTransparent;
  //styleDashed[3] = gdTransparent;
  //gdImageSetStyle(im, styleDashed, 4);

  j2 = -1;
  for (j = 0; j < var.NJ; j++) {
    j1 = (int)(j);
    if (j1 < 0 || j1 > var.NJ - 1) continue;
    if (j1 == j2) continue; //같은 격자 중복 계산 안하도록...
    j2 = j1;
    i2 = -1;
    for (i = 0; i < var.NI; i++) {
      i1 = (int)(i);
      if (i1 < 0 || i1 > var.NI - 1) continue;
      if (i1 == i2) continue; //같은 격자 중복 계산 안하도록...
      i2 = i1;
      q1 = g[j1][i1];
      q2 = g[j1][i1+1];
      q3 = g[j1+1][i1+1];
      q4 = g[j1+1][i1];

      if (q1 > var.missing && q2 > var.missing && q3 > var.missing && q4 > var.missing) {
        v[0] = data_level(q1, num_data, data_lvl);
        v[1] = data_level(q2, num_data, data_lvl);
        v[2] = data_level(q3, num_data, data_lvl);
        v[3] = data_level(q4, num_data, data_lvl);
        v[4] = v[0];
        level_max_min(v, 4, &vmax, &vmin);

        if (vmax != vmin) {
          for (v1 = vmin; v1 < vmax; v1++) {
            f = data_lvl[v1];
            m = 0;

            for (n = 0; n < 4; n++) {
              if (v[n] != v[n+1]) {
                switch (n) {
                  case 0:  x = (f-q1)/(q2-q1);  y = 0.0;             break;
                  case 1:  x = 1.0;             y = (f-q2)/(q3-q2);  break;
                  case 2:  x = (f-q4)/(q3-q4);  y = 1.0;             break;
                  case 3:  x = 0.0;             y = (f-q1)/(q4-q1);  break;
                }

                if (x >= 0.0 && x <= 1.0 && y >= 0.0 && y <= 1.0) {
                  m++;
                  if (m == 1) {
                    x1 = var.OI + (int)(x + i1);
                    y1 = var.OJ + (int)(y + j1);
                  }
                  else {
                    x2 = var.OI + (int)(x + i1);
                    y2 = var.OJ + (int)(y + j1);
                  }
                }
              }
            }

            if (m >= 2) {
              if (x1 < var.OI+5) {
                k = data_level(f, num_data, data_lvl);
                if (legend[0][k] == 0) {
                  sprintf(txt, "%d", (int)(f));
                  if ((int)(f/2) % 2 == 0) gdImageString(im, gdFontSmall, var.OI-strlen(txt)*6, y1-8, txt, color);
                  legend[0][k]++;
                }
              }

              if (y1 > var.OJ+var.NJ-2) {
                k = data_level(f, num_data, data_lvl);
                if (legend[1][k] == 0) {
                  sprintf(txt, "%d", (int)(f));
                  gdImageString(im, gdFontSmall, x1, var.OJ+var.NJ+2, txt, color);
                  legend[1][k]++;
                }
              }

              if (x1 > var.OI+var.NI-5) {
                k = data_level(f, num_data, data_lvl);
                if (legend[2][k] == 0) {
                  sprintf(txt, "%d", (int)(f));
                  if ((int)(f/2) % 2 != 0) gdImageString(im, gdFontSmall, var.OI+var.NI+4, y1-8, txt, color);
                  legend[2][k]++;
                }
              }

              if (x1 == x2 && y1 == y2) {
              }
              else {
                if ((int)(f/2) % 4 == 0) gdImageSetThickness(im, 2);
                else gdImageSetThickness(im, 1);
                //gdImageLine(im, x1, y1, x2, y2, gdAntiAliased);
                gdImageLine(im, x1, y1, x2, y2, color);
              }
              //printf("%d, %d, %d, %d\n", x1, y1, x2, y2);
            }
          }
        }
      }
    }
  }

  return 0;
}

/*=============================================================================*
 *  에러 출력
 *=============================================================================*/
int print_error(int error)
{
  gdImagePtr im;
  float  data_lvl[256], data_contour[256];
  int    color_lvl[256];
  int    num_color, num_contour, i, j;
  struct lamc_parameter map;
  float  zm = 1.0, xo = 0.0, yo = 0.0, rate;
  float  lon, lat, x, y, lon2, lat2, x1, y1, len;
  int    zx, zy, nx, ny;

  char   txt[256], txt_utf[256];
  double font_size = 10.0;
  int    brect[8];

  // 1. 이미지 영역 설정
  var.GI = 500;
  var.GJ = 500;

  // 2. 이미지 구조체 설정 및 색상표 읽기
  im = gdImageCreate(var.GI, var.GJ);
  num_color = color_table(im, color_lvl, data_lvl);
  gdImageFilledRectangle(im, 0, 0, var.GI, var.GJ, color_lvl[241]);

  // 3. 에러 표출
  if (error == -1) sprintf(txt, "에러: 선택하신 지점이 수치모델 영역을 벗어났습니다.");
  for (i = 0; i < 256; i++) txt_utf[i] = 0;
  euckr2utf(txt, txt_utf);
  gdImageStringFT(im, &brect[0], color_lvl[244], FONTTTF, font_size, 0.0, 20, 30, txt_utf);

  // 4. 이미지 전송
  printf("Content-type: image/png\n\n");
  gdImagePng(im, stdout);
  gdImageDestroy(im);

  return 0;
}

/*=============================================================================*
 *  지도 모서리 정보 확인
 *=============================================================================*/
int map_edge()
{
  int   i, j, k;
  struct lamc_parameter map;
  struct ster_parameter map2;
  struct eqdc_parameter map3;
  float  zm = 1.0, xo = 0.0, yo = 0.0;
  int    zx, zy;
  float  lon, lat, x, y, lon1, lat1, x1, y1;

  //if (!strcmp(var.map, "NHEM")) {
  //  map2.Re    = 6371.00877;
  //  map2.grid  = var.grid;
  //  var.NX = (int)((float)(map2.Re*2*PI/2)/map2.grid);  var.NY = (int)((float)(map2.Re*2*PI/2)/map2.grid);
  //  var.SX = (int)((float)(map2.Re*PI/2)/map2.grid);    var.SY = (int)((float)(map2.Re*PI/2)/map2.grid);
  //}
  //else map_ini_read();

  // 1. 확대시, 중심위치와 확대비율 계산
  for (i = 0; i < 7; i++, zm *= 1.5) {
    zx = var.zoom_x[i]-'0';
    zy = var.zoom_y[i]-'0';
    if (zx == 0 || zy == 0) break;
    xo += (float)(var.NX)/24.0*(zx-1)/zm;
    yo += (float)(var.NY)/24.0*(zy-1)/zm;
  }

  // 2. 지도 투영법
  if (!strcmp(var.map, "NHEM")) {
    map2.Re    = 6371.00877;
    map2.grid  = var.grid/(zm);
    map2.slon  = 120.0;
    map2.slat  = 90.0;
    map2.olon  = 120.0;
    map2.olat  = 90.0;
    map2.xo = (float)(var.SX - xo)*(zm);
    map2.yo = (float)(var.SY - yo)*(zm);
    map2.first = 0;
    map2.polar = 0;
  }
  else if (!strcmp(var.map, "WORLD")) {
    map3.Re    = 6371.00877;
    map3.grid  = var.grid/(zm);
    map3.slon  = 126.0;
    map3.slat  = 0.0;
    map3.olon  = 126.0;
    map3.olat  = 0.0;
    map3.xo = (float)(var.SX - xo)*(zm);
    map3.yo = (float)(var.SY - yo)*(zm);
    map3.first = 0;
  }
  else {
    map.Re    = 6371.00877;
    map.grid  = var.grid/(zm);
    map.slat1 = 30.0;    map.slat2 = 60.0;
    map.olon  = 126.0;   map.olat  = 38.0;
    map.xo = (float)(var.SX - xo)*(zm);
    map.yo = (float)(var.SY - yo)*(zm);
    map.first = 0;
  }

  x = 0;  y = (float)(var.NY);
  x1 = (float)(var.NX);  y1 = 0;
  if (!strcmp(var.map, "NHEM")) {
    sterproj_ellp(&lon, &lat, &x, &y, 1, &map2);
    sterproj_ellp(&lon1, &lat1, &x1, &y1, 1, &map2);
  }
  else if (!strcmp(var.map, "WORLD")) {
    eqdcproj(&lon, &lat, &x, &y, 1, &map3);
    eqdcproj(&lon1, &lat1, &x1, &y1, 1, &map3);

    if (lat > 90) lat = 90.;
    else if (lat < -90) lat = -90.;

    if (lat1 > 90) lat1 = 90.;
    else if (lat1 < -90) lat1 = -90.;
  }
  else {
    lamcproj_ellp(&lon, &lat, &x, &y, 1, &map);
    lamcproj_ellp(&lon1, &lat1, &x1, &y1, 1, &map);
  }

  var.startx = lon;  var.starty = lat;
  var.endx = lon1;   var.endy = lat1;

  return 0;
}

/*=============================================================================*
 *  고도 -> 표준등압면 변환
 *=============================================================================*/
int hh_to_pres(float hh)
{
  float pres;

  if (hh < 0) {
    pres = -999.;
  }
  else {
    pres = 1013.25 * exp(log(1.-hh/44308.) / 0.19023);
    if (pres < 234.52) {
      pres = 234.52 / exp((hh-10769.)/6381.6);
    }
  }

  if (pres > var.max_level) {
    pres = var.max_level;
  }

  return pres;
}