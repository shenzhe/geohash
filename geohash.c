/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2013 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_geohash.h"

#define SET_BIT(bits, mid, range, value, offset) \
mid = ((range)->high + (range)->low) / 2.0; \
if ((value) >= mid) { \
    (range)->low = mid; \
    (bits) |= (0x1 << (offset)); \
} else { \
    (range)->high = mid; \
    (bits) |= (0x0 << (offset)); \
}


static char char_map[32] =  "0123456789bcdefghjkmnpqrstuvwxyz";

/*
 *  The follow character maps were created by Dave Troy and used in his Javascript Geohashing
 *  library. http://github.com/davetroy/geohash-js
 */
static char *even_neighbors[] = {"p0r21436x8zb9dcf5h7kjnmqesgutwvy",
                                "bc01fg45238967deuvhjyznpkmstqrwx", 
                                "14365h7k9dcfesgujnmqp0r2twvyx8zb",
                                "238967debc01fg45kmstqrwxuvhjyznp"
                                };

static char *odd_neighbors[] = {"bc01fg45238967deuvhjyznpkmstqrwx", 
                               "p0r21436x8zb9dcf5h7kjnmqesgutwvy",
                                "238967debc01fg45kmstqrwxuvhjyznp",
                               "14365h7k9dcfesgujnmqp0r2twvyx8zb"    
                                };

static char *even_borders[] = {"prxz", "bcfguvyz", "028b", "0145hjnp"};
static char *odd_borders[] = {"bcfguvyz", "prxz", "0145hjnp", "028b"};
static unsigned int index_for_char(char c, char *string);
static char* get_neighbor(char *hash, int direction);
static char* _geohash_encode(double lat, double lng, long precision);
static GeoCoord _geohash_decode(char *hash);
static char** _geohash_neighbors(char *hash);
static GeoBoxDimension geohash_dimensions_for_precision(long precision);

/* If you declare any globals in php_geohash.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(geohash)
*/

/* True global resources - no need for thread safety here */
static int le_geohash;

/* {{{ geohash_functions[]
 *
 * Every user visible function must have an entry in geohash_functions[].
 */
const zend_function_entry geohash_functions[] = {
    PHP_FE(geohash_encode,  NULL)       
    PHP_FE(geohash_decode,  NULL)   
    PHP_FE(geohash_neighbors,  NULL)   
    PHP_FE(geohash_dimension,  NULL)   
    PHP_FE_END  /* Must be the last line in geohash_functions[] */
};
/* }}} */

/* {{{ geohash_module_entry
 */
zend_module_entry geohash_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    "geohash",
    geohash_functions,
    PHP_MINIT(geohash),
    PHP_MSHUTDOWN(geohash),
    PHP_RINIT(geohash),     /* Replace with NULL if there's nothing to do at request start */
    PHP_RSHUTDOWN(geohash), /* Replace with NULL if there's nothing to do at request end */
    PHP_MINFO(geohash),
#if ZEND_MODULE_API_NO >= 20010901
    PHP_GEOHASH_VERSION,
#endif
    STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_GEOHASH
ZEND_GET_MODULE(geohash)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("geohash.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_geohash_globals, geohash_globals)
    STD_PHP_INI_ENTRY("geohash.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_geohash_globals, geohash_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_geohash_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_geohash_init_globals(zend_geohash_globals *geohash_globals)
{
    geohash_globals->global_value = 0;
    geohash_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(geohash)
{
    /* If you have INI entries, uncomment these lines 
    REGISTER_INI_ENTRIES();
    */
    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(geohash)
{
    /* uncomment this line if you have INI entries
    UNREGISTER_INI_ENTRIES();
    */
    return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(geohash)
{
    return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(geohash)
{
    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(geohash)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "geohash support", "enabled");
    php_info_print_table_row(2, "Version", PHP_GEOHASH_VERSION);
    php_info_print_table_row(2, "Author", "shenzhe[shenzhe163@gmail.com]");
    php_info_print_table_end();

    /* Remove comments if you have entries in php.ini
    DISPLAY_INI_ENTRIES();
    */
}
/* }}} */


/* Remove the following function when you have successfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_geohash_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(geohash_encode)
{
    double latitude, longitude;
    long precision = 12;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dd|l", &latitude, &longitude, &precision) == FAILURE) {
        return;
    }


    if(latitude>90.0 || latitude<-90.0){
        php_error_docref(NULL TSRMLS_CC, E_NOTICE, "latitude range -90.0 to 90.0, now: %f", latitude);
        RETURN_NULL();
    }

    if(longitude>180.0 || longitude<-180.0){
        php_error_docref(NULL TSRMLS_CC, E_NOTICE, "longitude range -180.0 to 180.0, now: %f", longitude);
        RETURN_NULL();
    }

    char *hash;
    hash = _geohash_encode(latitude, longitude, precision);

    RETURN_STRING(hash, 0);

    efree(hash);
}

PHP_FUNCTION(geohash_decode)
{
    char *hash;
    int hash_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &hash, &hash_len) == FAILURE) {
            return;
    }
    
    GeoCoord area =  _geohash_decode(hash);

    array_init(return_value);
    add_assoc_double(return_value,"latitude",area.latitude);
    add_assoc_double(return_value,"longitude",area.longitude);
    add_assoc_double(return_value,"north",area.north);
    add_assoc_double(return_value,"east",area.east);
    add_assoc_double(return_value,"south",area.longitude);
    add_assoc_double(return_value,"west",area.west);

}


PHP_FUNCTION(geohash_neighbors)
{
    char *hash;
    int hash_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &hash, &hash_len) == FAILURE) {
            return;
    }
    
    char **list = _geohash_neighbors(hash);
    if(list == NULL)
        RETURN_FALSE;
    int len = sizeof(list);
    int i;
    array_init(return_value);
    for(i=0; i<len; i++) {
        add_next_index_string(return_value, list[i], 1);
        efree(list[i]);
    }
    efree(list);
}

PHP_FUNCTION(geohash_dimension)
{
    long precision;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &precision) == FAILURE) {
            return;
    }
    
    GeoBoxDimension dimension =  geohash_dimensions_for_precision(precision);

    array_init(return_value);
    add_assoc_double(return_value,"width", dimension.width);
    add_assoc_double(return_value,"height", dimension.height);

}

static unsigned int 
index_for_char(char c, char *string) 
{
    
    int index = -1;
    int string_amount = strlen(string);
    int i;
    for(i = 0; i < string_amount; i++) {
        
        if(c == string[i]) {
            
            index = i; 
            break;
        }
        
    }
    
    return index;
}

static char* 
get_neighbor(char *hash, int direction) 
{
    
    int hash_length = strlen(hash);
    
    char last_char = hash[hash_length - 1];
    
    int is_odd = hash_length % 2;
    char **border = is_odd ? odd_borders : even_borders;
    char **neighbor = is_odd ? odd_neighbors : even_neighbors; 
    
    char *base = emalloc(sizeof(char) * 1);
    base[0] = '\0';
    strncat(base, hash, hash_length - 1);
    
    if(index_for_char(last_char, border[direction]) != -1)
        base = get_neighbor(base, direction);
    
    int neighbor_index = index_for_char(last_char, neighbor[direction]);
    last_char = char_map[neighbor_index];
        
    char *last_hash = emalloc(sizeof(char) * 2);
    last_hash[0] = last_char;
    last_hash[1] = '\0';
    strcat(base, last_hash);
    efree(last_hash);
    
    return base;
}

static char*
_geohash_encode(double lat, double lon, long len)
{
    unsigned int i;
    char *hash;
    unsigned char bits = 0;
    double mid;
    Interval lat_range = {  90,  -90 };
    Interval lon_range = { 180, -180 };

    double val1, val2, val_tmp;
    Interval *range1, *range2, *range_tmp;

    assert(lat >= -90.0);
    assert(lat <= 90.0);
    assert(lon >= -180.0);
    assert(lon <= 180.0);
    assert(len <= MAX_HASH_LENGTH);

    hash = (char *)emalloc(sizeof(char) * (len + 1));
    if (hash == NULL)
        return NULL;

    val1 = lon; range1 = &lon_range;
    val2 = lat; range2 = &lat_range;

    for (i=0; i < len; i++) {

        bits = 0;
        SET_BIT(bits, mid, range1, val1, 4);
        SET_BIT(bits, mid, range2, val2, 3);
        SET_BIT(bits, mid, range1, val1, 2);
        SET_BIT(bits, mid, range2, val2, 1);
        SET_BIT(bits, mid, range1, val1, 0);

        hash[i] = char_map[bits];

        val_tmp   = val1;
        val1      = val2;
        val2      = val_tmp;
        range_tmp = range1;
        range1    = range2;
        range2    = range_tmp;
    }

    hash[len] = '\0';
    return hash;
}

static GeoCoord 
_geohash_decode(char *hash) 
{
    
    GeoCoord coordinate = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    
    if(hash) {
        
        int char_amount = strlen(hash);
        
        if(char_amount) {
            
            unsigned int char_mapIndex;
            Interval lat_interval = {MAX_LAT, MIN_LAT};
            Interval lng_interval = {MAX_LONG, MIN_LONG};
            Interval *interval;
        
            int is_even = 1;
            double delta;
            int i, j;
            for(i = 0; i < char_amount; i++) {
            
                char_mapIndex = index_for_char(hash[i], (char*)char_map);
                
                if(char_mapIndex < 0)
                    break;
            
                // Interpret the last 5 bits of the integer
                for(j = 0; j < 5; j++) {
                
                    interval = is_even ? &lng_interval : &lat_interval;
                
                    delta = (interval->high - interval->low) / 2.0;
                
                    if((char_mapIndex << j) & 0x0010)
                        interval->low += delta;
                    else
                        interval->high -= delta;
                
                    is_even = !is_even;
                }
            
            }
            
            coordinate.latitude = lat_interval.high - ((lat_interval.high - lat_interval.low) / 2.0);
            coordinate.longitude = lng_interval.high - ((lng_interval.high - lng_interval.low) / 2.0);
            
            coordinate.north = lat_interval.high;
            coordinate.east = lng_interval.high;
            coordinate.south = lat_interval.low;
            coordinate.west = lng_interval.low;
        }
    }
    
    return coordinate;
}


static char** 
_geohash_neighbors(char *hash) 
{

    char** neighbors = NULL;
    
    if(hash) {
        
        // N, NE, E, SE, S, SW, W, NW
        neighbors = (char**)emalloc(sizeof(char*) * 8);
        
        neighbors[0] = get_neighbor(hash, NORTH);
        neighbors[1] = get_neighbor(neighbors[0], EAST);
        neighbors[2] = get_neighbor(hash, EAST);
        neighbors[3] = get_neighbor(neighbors[2], SOUTH);
        neighbors[4] = get_neighbor(hash, SOUTH);
        neighbors[5] = get_neighbor(neighbors[4], WEST);                
        neighbors[6] = get_neighbor(hash, WEST);
        neighbors[7] = get_neighbor(neighbors[6], NORTH);        

    }
    return neighbors;
}

static GeoBoxDimension 
geohash_dimensions_for_precision(long precision) 
{
    
    GeoBoxDimension dimensions = {0.0, 0.0};
    
    if(precision > 0) {
    
        int lat_times_to_cut = precision * 5 / 2;
        int lng_times_to_cut = precision * 5 / 2 + (precision % 2 ? 1 : 0);
    
        double width = 360.0;
        double height = 180.0;
    
        int i;
        for(i = 0; i < lat_times_to_cut; i++)
            height /= 2.0;
        
        for(i = 0; i < lng_times_to_cut; i++)
            width /= 2.0;
        
        dimensions.width = width;
        dimensions.height = height;
        
    }
    
    return dimensions;
}

/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */

