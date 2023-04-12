/*******************************************************************************
**
**  grib 해독(eccodes)
**
**=============================================================================*
**
**     o 작성자 : 이창재(2021.11.06.)
**
********************************************************************************/
#include "grib_api_internal.h"

#define NULL_MARKER 0
#define NOT_NULL_MARKER 255
#define GRIB_MAX_OPENED_FILES 200

static int index_count;
static long values_count = 0;
static short next_id=0;
static grib_file_pool file_pool= {
        0,                    /* grib_context* context;*/
        0,                    /* grib_file* first;*/
        0,                    /* grib_file* current; */
        0,                    /* size_t size;*/
        0,                    /* int number_of_opened_files;*/
        GRIB_MAX_OPENED_FILES /* int max_opened_files; */
};

GRIB_INLINE static int grib_inline_strcmp(const char*, const char*);
void* grib_context_malloc(const grib_context*, size_t);
void* grib_context_malloc_clear(const grib_context*, size_t);
char* grib_read_string(grib_context*, FILE*, int*);
static grib_string_list* grib_read_key_values(grib_context*, FILE*, int*);
static grib_field* grib_read_field(grib_context*, FILE*, grib_file**, int*);
grib_field_tree* grib_read_field_tree(grib_context*, FILE*, grib_file**, int*);
grib_file* grib_file_new(grib_context*, const char*, int*);
grib_file* grib_get_file(const char*,int*);

static grib_string_list* grib_read_key_values(grib_context* c, FILE* fh, int* err)
{
    grib_string_list* values;
    unsigned char marker = 0;

    *err = grib_read_uchar(fh, &marker);
    if (marker == NULL_MARKER)
        return NULL;
    if (marker != NOT_NULL_MARKER) {
        *err = GRIB_CORRUPTED_INDEX;
        return NULL;
    }

    values_count++;

    values        = (grib_string_list*)grib_context_malloc_clear(c, sizeof(grib_string_list));
    values->value = grib_read_string(c, fh, err);
    if (*err)
        return NULL;

    values->next = grib_read_key_values(c, fh, err);
    if (*err)
        return NULL;

    return values;
}

static grib_index_key* grib_read_index_keys(grib_context* c, FILE* fh, int* err)
{
    grib_index_key* keys = NULL;
    unsigned char marker = 0;
    unsigned char type   = 0;

    if (!c)
        c = grib_context_get_default();

    *err = grib_read_uchar(fh, &marker);
    if (marker == NULL_MARKER)
        return NULL;
    if (marker != NOT_NULL_MARKER) {
        *err = GRIB_CORRUPTED_INDEX;
        return NULL;
    }

    keys       = (grib_index_key*)grib_context_malloc_clear(c, sizeof(grib_index_key));
    keys->name = grib_read_string(c, fh, err);
    if (*err)
        return NULL;

    *err       = grib_read_uchar(fh, &type);
    keys->type = type;
    if (*err)
        return NULL;

    values_count = 0;
    keys->values = grib_read_key_values(c, fh, err);
    if (*err)
        return NULL;

    keys->values_count = values_count;

    keys->next = grib_read_index_keys(c, fh, err);
    if (*err)
        return NULL;

    return keys;
}

static grib_file* grib_read_files(grib_context* c, FILE* fh, int* err)
{
    unsigned char marker = 0;
    short id             = 0;
    grib_file* file;
    *err = grib_read_uchar(fh, &marker);
    if (marker == NULL_MARKER)
        return NULL;
    if (marker != NOT_NULL_MARKER) {
        *err = GRIB_CORRUPTED_INDEX;
        return NULL;
    }

    file       = (grib_file*)grib_context_malloc(c, sizeof(grib_file));
    file->name = grib_read_string(c, fh, err);
    if (*err)
        return NULL;

    *err     = grib_read_short(fh, &id);
    file->id = id;
    if (*err)
        return NULL;

    file->next = grib_read_files(c, fh, err);
    if (*err)
        return NULL;

    return file;
}

/*******************************************************************************
 *  index 샘플파일 조회 && 조회할 grib 파일명 변경
 *******************************************************************************/
grib_index* grib_index_read_switch_file(grib_context* c, const char* idxfilename, char* switchfilename, int* err)
{
    grib_file *file, *f;
    grib_file** files;
    grib_index* index    = NULL;
    unsigned char marker = 0;
    char* identifier     = NULL;
    int max              = 0;
    FILE* fh             = NULL;
    ProductKind product_kind = PRODUCT_GRIB;

    if (!c)
        c = grib_context_get_default();

    fh = fopen(idxfilename, "r");
    if (!fh) {
        grib_context_log(c, (GRIB_LOG_ERROR) | (GRIB_LOG_PERROR),
                         "Unable to read file %s", idxfilename);
        perror(idxfilename);
        *err = GRIB_IO_PROBLEM;
        return NULL;
    }

    identifier = grib_read_string(c, fh, err);
    if (!identifier) {
        fclose(fh);
        return NULL;
    }

    if (strcmp(identifier, "BFRIDX1")==0) product_kind = PRODUCT_BUFR;
    grib_context_free(c, identifier);

    *err = grib_read_uchar(fh, &marker);
    if (marker == NULL_MARKER) {
        fclose(fh);
        return NULL;
    }
    if (marker != NOT_NULL_MARKER) {
        *err = GRIB_CORRUPTED_INDEX;
        fclose(fh);
        return NULL;
    }

    file = grib_read_files(c, fh, err);
    if (*err)
        return NULL;

    strcpy(file->name, switchfilename);

    f = file;
    while (f) {
        if (max < f->id)
            max = f->id;
        f = f->next;
    }

    files = (grib_file**)grib_context_malloc_clear(c, sizeof(grib_file) * (max + 1));

    f = file;
    while (f) {
        grib_file_open(f->name, "r", err);
        if (*err)
            return NULL;
        files[f->id] = grib_get_file(f->name, err); // fetch from pool
        f            = f->next;
    }

    while (file) {
        f    = file;
        file = file->next;
        grib_context_free(c, f->name);
        grib_context_free(c, f);
    }

    index          = (grib_index*)grib_context_malloc_clear(c, sizeof(grib_index));
    index->context = c;
    index->product_kind = product_kind;

    index->keys = grib_read_index_keys(c, fh, err);
    if (*err)
        return NULL;

    index_count   = 0;
    index->fields = grib_read_field_tree(c, fh, files, err);
    if (*err)
        return NULL;

    index->count = index_count;

    fclose(fh);
    grib_context_free(c, files);
    return index;
}

/*******************************************************************************
 *  grib_size 얻기
 *******************************************************************************/
int get_grib_size(
  int mode,
  codes_index* idx,
  codes_handle* *h,
  char* varname,
  long level,
  long *numPoints,
  long *nlat,
  long *nlon,
  double *slat,
  double *slon,
  double *dlat,
  double *dlon,
  double *elat,
  double *elon, 
  long discipline, 
  long parameterCategory, 
  long parameterNumber
)
{
  int err = 0;
  codes_index_select_string(idx,"shortName",varname);
  codes_index_select_long(idx,"level",level);
  if (mode != 0) {
    codes_index_select_long(idx,"discipline",discipline);
    codes_index_select_long(idx,"parameterCategory",parameterCategory);
    codes_index_select_long(idx,"parameterNumber",parameterNumber);
  }
  *h = codes_handle_new_from_index(idx,&err);
  codes_get_long(*h,"numberOfDataPoints",numPoints);
  codes_get_long(*h,"Ni",nlon);
  codes_get_long(*h,"Nj",nlat);
  codes_get_double(*h,"longitudeOfFirstGridPointInDegrees",slon);
  codes_get_double(*h,"latitudeOfFirstGridPointInDegrees",slat);
  codes_get_double(*h,"longitudeOfLastGridPointInDegrees",elon);
  codes_get_double(*h,"latitudeOfLastGridPointInDegrees",elat);
  codes_get_double(*h,"iDirectionIncrementInDegrees",dlon);
  codes_get_double(*h,"jDirectionIncrementInDegrees",dlat);
  if (*slon < 0) *slon += 360.;
  if (*elon < 0) *elon += 360.;
  if (*elat < *slat) *dlat = -1. * *dlat;
  if (*elon < *slon) *dlon = -1. * *dlon;
}

/*******************************************************************************
 *  grib_data 얻기
 *******************************************************************************/
int get_grib_data_2d(
  codes_handle* h,
  long numPoints,
  long nlat,
  long nlon,
  double **values_2d
)
{
  int err = 0, i, j;
  double *values;

  values = (double *) malloc((unsigned) (numPoints)*sizeof(double));
  codes_get_double_array(h, "values", values, &numPoints);

  for (j=0; j<nlat; j++) {
    for (i=0; i<nlon; i++) {
      values_2d[j][i] = values[j*nlon+i];
    }
  }

  free((char*) (values));
}

/*
!=======================================================================
SUBROUTINE get_grib_latlon(igrib,numPoints,olats,olons)
  use eccodes

  integer,intent(in)                           :: numPoints,igrib
  real,dimension(:),allocatable                :: lats,lons
  real,dimension(numPoints),intent(out)        :: olats,olons

  allocate(lats(numPoints),lons(numPoints))
  call codes_get(igrib,'latitudes',lats)
  call codes_get(igrib,'longitudes',lons)  

  olats = lats
  olons = lons

  deallocate(lats,lons)

END SUBROUTINE get_grib_latlon
!=======================================================================
SUBROUTINE get_grib_latlon_2d(igrib,nlat,nlon,olats2d,olons2d,olats,olons)
  use eccodes

  integer,intent(in)                           :: nlat,nlon,igrib
  real,dimension(nlon*nlat)                    :: olats_in,olons_in
  real,dimension(nlon,nlat),intent(out)        :: olats2d,olons2d
  real,dimension(nlat),intent(out)             :: olats
  real,dimension(nlon),intent(out)             :: olons
  integer                                      :: i,j

  call get_grib_latlon(igrib,nlon*nlat,olats_in,olons_in)

  do i=1,nlon
   do j=1,nlat
    olats2d(i,j) = olats_in((j-1)*nlon+i)
    olons2d(i,j) = olons_in((j-1)*nlon+i)

    if (i.eq.1) then
      olats(j) = olats_in((j-1)*nlon+i)
    endif
    if (j.eq.1) then
      olons(i) = olons_in((j-1)*nlon+i)
    endif
   enddo
  enddo

END SUBROUTINE get_grib_latlon_2d
!========================================================================
*/