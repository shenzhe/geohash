geohash
=======

php geohash extension  (php geohash 扩展)


编译
======

	git clone 到机器
    
    执行：
	phpize
	./configure
	make
	make install

	然后把 geohash.so 加入到php.ini中
	

方法
====

	/**
	 *  $latitude    //纬度
	 *  $longitude   //经度
	 *  $precision   //精密度, 默认是12
	 *  返回 $precision 长度的 string 
	 */
	geohash_encode($latitude, $longitude, $precision=12);  



	/**
	 *  $hash    //geohash_encode后的值
	 *  返回 array // Array
	 *					(
	 *					    [latitude] => 39.416916975752
	 *					    [longitude] => 100.92223992571
	 *					    [north] => 39.416917059571
	 *					    [east] => 100.92224009335
	 *					    [south] => 100.92223992571
	 *					    [west] => 100.92223975807
	 *					)
	 */
	geohash_decode($hash);

	/**
	 *  $hash    //geohash_encode后的值
	 *  返回 在$hash 8个 (东南西北各二个)附近的hash值
	 */
	geohash_neighbors($hash);

	/**
	 *  $precision    //精密度
	 *  返回 数组，array("width"=>12.0, "height"=>12.0) 
	 *  表示矩形的宽和高
	 */
	geohash_dimension($hash);