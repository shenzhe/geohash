<?php
	$hash = geohash_encode(39.4169170000,100.92224000000);
    $arr = geohash_decode($hash);
    print_r($arr);
    $neighbors = geohash_neighbors($hash);
    var_dump($neighbors);

    var_dump(geohash_dimension(12));

    var_dump(geohash_dimension(5));
