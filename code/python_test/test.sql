SELECT 
	* 
FROM 
	isys_his 
WHERE 
	(tag_name='tag1' OR tag_name='tag2' ) AND 
	((time_stamp BETWEEN '2021-02-26 06:05:57' AND '2021-02-26 06:15:57')  OR (time_stamp = '2021-02-26 10:15:57'))
ORDER BY time_stamp
;


SELECT 
	* 
FROM 
	isys_his 
WHERE 
	(tag_name='tag1' OR tag_name='tag2' ) AND 
	time_stamp > '2021-02-26 06:05:57' AND time_stamp < '2021-02-26 06:15:57'
ORDER BY time_stamp