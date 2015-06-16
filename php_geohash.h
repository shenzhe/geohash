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

#ifndef PHP_GEOHASH_H
#define PHP_GEOHASH_H

extern zend_module_entry geohash_module_entry;
#define phpext_geohash_ptr &geohash_module_entry

#define PHP_GEOHASH_VERSION "1.1" /* Replace with version number for your extension */

#ifdef PHP_WIN32
#	define PHP_GEOHASH_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_GEOHASH_API __attribute__ ((visibility("default")))
#else
#	define PHP_GEOHASH_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif


#define MAX_LAT             90.0
#define MIN_LAT             -90.0

#define MAX_LONG            180.0
#define MIN_LONG            -180.0

#define MAX_HASH_LENGTH 22

#define NORTH               0
#define EAST                1
#define SOUTH               2
#define WEST                3

#define LENGTH_OF_DEGREE  111100        // meters

typedef struct IntervalStruct {
    
    double high;
    double low;
    
} Interval;

typedef struct GeoBoxDimensionStruct {
  
  double height;
  double width;

} GeoBoxDimension;

typedef struct GeoCoordStruct {
    
    double latitude;
    double longitude;
    
    double north;
    double east;
    double south;
    double west;

  GeoBoxDimension dimension;
    
} GeoCoord;

PHP_MINIT_FUNCTION(geohash);
PHP_MSHUTDOWN_FUNCTION(geohash);
PHP_RINIT_FUNCTION(geohash);
PHP_RSHUTDOWN_FUNCTION(geohash);
PHP_MINFO_FUNCTION(geohash);

PHP_FUNCTION(geohash_encode);	
PHP_FUNCTION(geohash_decode);	
PHP_FUNCTION(geohash_neighbors); 
PHP_FUNCTION(geohash_dimension); 

/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(geohash)
	long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(geohash)
*/

/* In every utility function you add that needs to use variables 
   in php_geohash_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as GEOHASH_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define GEOHASH_G(v) TSRMG(geohash_globals_id, zend_geohash_globals *, v)
#else
#define GEOHASH_G(v) (geohash_globals.v)
#endif

#endif	/* PHP_GEOHASH_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
