/*******************************************************************************
**
**  변수 계산
**
**=============================================================================*
**
**     o 작성자 : 이창재(2021.11.06.)
**
********************************************************************************/
//=====================
// 수증기량(혼합비) 계산(온도는 섭씨로 넣는다.기압은 hPa로 넣는다.)
//=====================
float get_mixing_ratio(float ta, float hm, float pres)
{
  float  tmp, evap, mixr, result;

  tmp = 0.;
  evap = 0.;
  mixr = 0.;

  tmp = -2.30259 * 2937.4 / (ta + CELKEL) - 4.9283 * log(ta + CELKEL) + 2.30259 * 23.5518;
  evap = exp(tmp);
  mixr = 0.622 * evap / (pres - evap) * hm / 100.0;
  result = mixr * 1000.;

  return result;
}

//=====================
// 수증기량(혼합비) 계산
//=====================
float get_mixing_ratio2(float ta, float td, float pres)
{
  float  tmp, evap, mixr, result;

  evap = 0.;
  mixr = 0.;

  //CKVN /273.15/
  //TMP : 기온(섭씨), Tdew(K) : 이슬점온도, PRES : 기압
  //Evap  = 6.11*exp((17.269*(Tdew+CKVN)-4717.3) / ((Tdew+CKVN)-35.86))
  //rmix  = (0.622*Evap)/(PRES-Evap) !Mixing Ratio (kg/kg)

  evap = 6.11*exp((17.269*(td+CELKEL)-4717.3) / ((td+CELKEL)-35.86));
  mixr = (0.622*evap)/(pres-evap); //!Mixing Ratio (kg/kg)
  result = mixr * 1000.;

  return result;
}

//=====================
// 온위를 구한다. (Potential Temperature)
// val_Theta =  val_Tsur * Math.pow( (1000./press_PSL), (const_Rgas/const_Cp) ); // Potential Temperature [K]
//=====================
float get_potential_temp(float ta, float pres)
{
  float  result;

  result = (ta + CELKEL) * (pow((1000 / pres), 0.285857));

  return result;
}

//=====================
// 기온과 이슬점온도로 상대습도 구한다.
//=====================
float get_relative_humidity(float ta, float td)
{
  float  es, e, rh, result;

  es = 6.112 * exp(((17.67 * ta)/(ta + 243.5)));
  e = 6.112 * exp(((17.67 * td)/(td + 243.5)));

  rh = (e/es) * 100;
  result = rh;

  return result;
}

//=====================
// 기온(켈빈 온도임.)과 습도와 압력(hPa)로 이슬점온도 구한다.
//=====================
float get_dew_temp(float ta, float rh, float pres)
{
  float  eps, eZero, eslCon1, eslCon2, result;
  float  pEvaps, shs, specificHumidity, pe, peLog;

  // 상수 정의
  eps = 0.622;
  eZero = 6.112;
  eslCon1 = 17.67;
  eslCon2 = 29.65;

  if (rh <= 0.0) {
    rh = 0.1;
  } 
  else if (rh > 100.0) {
    rh = 100.0;
  }

  // 계산
  pEvaps = eZero * exp(eslCon1 * (ta - CELKEL) / (ta - eslCon2));
  shs = (eps * pEvaps) / (pres - pEvaps);
  specificHumidity = rh * shs / 100.0;

  pe = specificHumidity * pres / (eps + specificHumidity);
  peLog = log(pe / eZero);
  result = (243.5 * peLog) / (eslCon1 - peLog);

  return result;
}

//=====================
// 습구온도를 구한다. (Wet Bulb Temperature) //2019.11.19. 이창재
//=====================
float get_wetbulb_temp(float ta, float rh)
{
  float  result;

  result = ta * atan(0.151977 * sqrt(rh + 8.313659)) + atan(ta + rh) - atan(rh - 1.676331) + 0.00391838 * pow(rh, 1.5) * atan(0.023101 * rh) - 4.68035;

  return result;
}

//=====================
// LCL 온도 계산
//=====================
float get_tlcl(float ta, float td)
{
  float  s, dlt, result;

  s = ta - td;
  dlt = s*(1.2185 + 1.278 * pow(10,-3) * ta + s * (-2.19 * pow(10,-3) + 1.173 * pow(10,-5) * s - 5.2 * pow(10,-6) * ta));
  result = ta - dlt;

  return result;
}

//=====================
// 상당온위 계산
//=====================
float get_ept(float ta, float td, float pres)
{
  float  w, tlcl, tk, tl, pt, result;

  w = get_mixing_ratio2(ta, td, pres);

  tlcl = get_tlcl(ta, td);
  tk = ta + CELKEL;
  tl = tlcl + CELKEL;
  pt = tk*pow((1000./pres), (0.2854*(1.0 - 0.00028*w)));
  result = pt * exp((3.376/tl-0.00254)*w*(1.0 + 0.00081*w));

  return result;
}

//=====================
// w(m/s) -> omega(hPa/hr)
//=====================
float calc_w_to_omega(float wz, float ta, float rh, float pres)
{
  float  eps, eZero, eslCon1, eslCon2, result;
  float  pEvaps, shs, specificHumidity, pe, peLog;
  float  grav, rgas, virtual_ta;

  // 상수 정의
  eps = 0.622;
  eZero = 6.112;
  eslCon1 = 17.67;
  eslCon2 = 29.65;
  grav = 9.81;           // m/s**2
  rgas = 287.04;         // J/K/kg

  if (rh <= 0.0) {
    rh = 0.1;
  } 
  else if (rh > 100.0) {
    rh = 100.0;
  }

  // 계산
  pEvaps = eZero * exp(eslCon1 * (ta - CELKEL) / (ta - eslCon2));
  shs = (eps * pEvaps) / (pres - pEvaps);
  specificHumidity = rh * shs / 100.0;
  virtual_ta = ta*(eps+specificHumidity)/(eps*(1.+specificHumidity));
  result = -grav*pres*100./(rgas*virtual_ta)*wz*36.;
  
  return result;
}

//=====================
// 수렴/발산 계산
//=====================
float calc_conv(int nlon, int nlat, float **u, float **v, float **conv, float slon, float slat, float dlon, float dlat)
{
  float scale, du, dv, dx, dy, dist, rearth;
  int i, j;

  scale = 1.0*pow(10.,6.);
  rearth = 6371.00877;
  dist = 40.;

  // CALCULATE CONVERGENCE
  for (j = 0; j < nlat; j++) {
    for (i = 0; i < nlon; i++) {
      if (j == nlat-1 && i == nlon-1) {
        conv[j][i] = conv[j-1][i-1];
      }
      else if (j == nlat - 1) {
        conv[j][i] = conv[j-1][i];
      }
      else if (i == nlon - 1) {
        conv[j][i] = conv[j][i-1];
      }
      else {
        du = ((u[j+1][i+1]+u[j][i+1]) - (u[j+1][i]+u[j][i]))/2.;
        dv = ((v[j+1][i]+v[j+1][i+1]) - (v[j][i]+v[j][i+1]))/2.;
        dx = rearth*1000.*cos((slat+dlat*j)*DEGRAD)*dlon*DEGRAD;
        dy = rearth*1000.*dlat*DEGRAD;
        conv[j][i] = (du/dx + dv/dy)*scale;
      }
    }
  }

/*
  // FILL BOUNDARY TO BE SAME AS NEXT BOUNDARY -------------------
  for (i = 1; i < nlon-1; i++) {
    conv[0][i] = conv[1][i]*2. - conv[2][i];
    conv[nlat-1][i] = conv[nlat-2][i]*2. - conv[nlat-3][i];
  }

  for (j = 1; j < nlat-1; j++) {
    conv[j][0] = conv[j][1]*2. - conv[j][2];
    conv[j][nlon-1] = conv[j][nlon-2]*2. - conv[j][nlon-3];
  }

  // ADJUST FOUR CORNERS -----------------------------------------
  conv[0][0] = conv[1][0] + conv[0][1] - conv[1][1];
  conv[nlat-1][0] = conv[nlat-1][1] + conv[nlat-2][0] + conv[nlat-2][1];
  conv[0][nlon-1] = conv[1][nlon-1] + conv[0][nlon-2] - conv[1][nlon-2];
  conv[nlat-1][nlon-1] = conv[nlat-2][nlon-1] + conv[nlat-1][nlon-2] + conv[nlat-2][nlon-2];
*/
  return;
}
