'''
Description: file content
Author: yuanquan
Email: yuanquan2011@qq.com
Date: 2021-02-15 20:42:55
LastEditors: yuanquan
LastEditTime: 2021-02-16 13:52:48
copyright: Copyright (c) yuanquan
'''
import pyodbc

def Run():
	# 建立连接
	conn = pyodbc.connect('DSN=isysSqlDSII;ISYS_ADDR=127.0.0.1;ISYS_PORT=5150;')
	cursor = conn.cursor()

	# 执行SQL
	cursor.execute("SELECT * FROM isys_rtd WHERE tag_name  IN ('tag1', 'tag2')")

	# 获取并打印数据
	row = cursor.fetchone()
	while row:
		print(row)
		row = cursor.fetchone()

	conn.close()


if __name__ == "__main__":
	Run()