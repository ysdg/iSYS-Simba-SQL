'''
Description: file content
Author: yuanquan
Email: yuanquan2011@qq.com
Date: 2021-02-15 20:42:55
LastEditors: yuanquan
LastEditTime: 2021-02-15 21:17:51
copyright: Copyright (c) yuanquan
'''
import adodbapi
adodbapi.adodbapi.verbose = False # adds details to the sample printout
import adodbapi.ado_consts as adc


Cfg={'server':'127.0.0.1','password':'xxxx','db':'pscitemp'}
constr = "Provider=isysSqlProvider;Integrated Security=SSPI;"
conn=adodbapi.connect(constr)

cur=conn.cursor()
sql="select * from isys_rtd where tag_name='tag1'"
cur.execute(sql)
data=cur.fetchall()
cur.close()


# if __name__==""