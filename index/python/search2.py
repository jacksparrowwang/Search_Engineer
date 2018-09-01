#!/usr/bin/python
#coding:utf-8

#Python库，提供一系列针对url的操作方法
import urllib2
#Re正则表达式库 提供了一系列针对正则表达式的方法
import re
#BeautifulSoup库
from bs4 import BeautifulSoup

import sys                                                                                                               
reload(sys)
sys.setdefaultencoding("utf8")

def filter_tags(htmlstr):
    #先过滤CDATA
    re_cdata=re.compile('//<!\[CDATA\[[^>]*//\]\]>',re.I) #匹配CDATA
    re_script=re.compile('<\s*script[^>]*>[^<]*<\s*/\s*script\s*>',re.I)#Script
    re_style=re.compile('<\s*style[^>]*>[^<]*<\s*/\s*style\s*>',re.I)#style
    re_br=re.compile('<br\s*?/?>')#处理换行
    re_h=re.compile('</?\w+[^>]*>')#HTML标签
    re_comment=re.compile('<!--[^>]*-->')#HTML注释
    s=re_cdata.sub('',htmlstr)#去掉CDATA
    s=re_script.sub('',s) #去掉SCRIPT
    s=re_style.sub('',s)#去掉style

    # 此处由于需要让一个文档只占一行, 把 <br> 替换成空格
    s=re_br.sub(' ',s)

    s=re_h.sub('',s) #去掉HTML 标签
    s=re_comment.sub('',s)#去掉HTML注释
    #去掉多余的空行
    blank_line=re.compile('\n+')

    # 此处把多个空行合并成一个空格
    s=blank_line.sub(' ',s)

    s=replaceCharEntity(s)#替换实体
    return s

def replaceCharEntity(htmlstr):
    CHAR_ENTITIES={'nbsp':' ','160':' ',
                   'lt':'<','60':'<',
                   'gt':'>','62':'>',
                   'amp':'&','38':'&',
                   'quot':'"','34':'"',}

    re_charEntity=re.compile(r'&#?(?P<name>\w+);')
    sz=re_charEntity.search(htmlstr)
    while sz: 
        key=sz.group('name')#去除&;后entity,如&gt;为gt
        try:
            htmlstr=re_charEntity.sub(CHAR_ENTITIES[key],htmlstr,1)
            sz=re_charEntity.search(htmlstr)
        except KeyError:                                                                                                 
            #以空串代替
            htmlstr=re_charEntity.sub('',htmlstr,1)
            sz=re_charEntity.search(htmlstr)
    return htmlstr


#获取页面内容
def OpenPage(url):
    MyHeaders = {}
    #urllib2.Request 填写两个参数,第一个参数是url,第二个参数是我们的请求头headers
    request = urllib2.Request(url,headers=MyHeaders)
    #类文件对象 
    #urlopen发送指定请求
    f = urllib2.urlopen(request)
    #f.read()读取响应对象的内容
    data = f.read()
    #编码解码方法 decode 解码 encode编码
    #ignore replace
    return data.decode("GBK",errors="ignore").encode("utf-8")
    #return data
#测试访问页面内容
def Test1():
    # print OpenPage("http://www.shengxu6.com/book/2967.html")
    print OpenPage("http://shannxi.cncn.com/jingdian/1-1-0-7.html")
#解析主页内容,获取到url列表
def ParseMainPage(page):
    #调用BeatifulSoup库解析页面 
    soup = BeautifulSoup(page,"html.parser")
    #find_all方法查询所有的指定内容
    #包含read字符串的href链接 通过正则表达式
    list_charts = soup.find_all(href=re.compile(".cncn.com/jingdian"))

    list_tmp = []
    for item in list_charts:
        #print type(item)
        #每一个item是一个tag标签类的实例化对象
        #通过item['href'] 可以获取到href的值
        # print item["href"]
        if item["href"] not in list_tmp:
            list_tmp.append(item["href"].encode("utf-8"))
        # url_list.append("http://www.shengxu6.com" + item['href'])
    list_tmp = list_tmp[1:11]
    # for it in list_tmp:
    #     print it
    return list_tmp

def Test2():
    page = OpenPage("http://shannxi.cncn.com/jingdian/1-1-0-7.html")
    # print ParseMainPage(page)
    ParseMainPage(page)

def ParseMainPage2(page, url):
    soup = BeautifulSoup(page,"html.parser")
    url = url[7:-1]
    yulin = "sxyulin.cncn.com/jingdian"
    if url != yulin:
        url = url.capitalize()
    list_charts = soup.find_all(href=re.compile(url))

    list_chart = []
    for item in list_charts:
        if item not in list_chart:
            list_chart.append(item["href"].encode("utf-8"))
    return list_chart

def TestMain2():
    url = "http://baoji.cncn.com/jingdian/"
    page = OpenPage(url)
    # print ParseMainPage(page)
    ParseMainPage2(page, url)


# 解析具体的网页信息
def ParseDetailPage(page, url):
    #BeautifulSoup 解析响应内容
    soup = BeautifulSoup(page,"html.parser")
     
    title = soup.find('title').string
    url_tmp = url[:30]
    if url_tmp == "http://baoji.cncn.com/jingdian":
        content = soup.find_all(class_="type")[0].get_text() 
    else:
        content = soup.find_all(class_="box670 details")[0].get_text() 
    return title,content

def Test3():
    page = OpenPage("http://ankang.cncn.com/jingdian/bashanqiuchi/profile")
    # title,content = ParseDetailPage(page)
    title, content = ParseDetailPage(page, "http://ankang.cncn.com/jingdian/bashanqiuchi/profile")
    print title
    print content

#把获取到的内容写入文件中
def WriteDataToFile(data):
    f = open("./search/raw_input","w")
    f.write(data)
    f.close()
    #with open(file_path,"a+") as f:
    #    f.write(data)
def Test4():
    WriteDataToFile("tmp.txt","asnodinasdion\n")

if __name__ == "__main__":
    #小说的主页
    url0 = "http://shannxi.cncn.com/jingdian/1-1-0-7.html"
    #获取主页内容
    main_page = OpenPage(url0)
     # 获取主页中的url
    url_list = ParseMainPage(main_page)
    f = open("./search/raw_input","w")
    for url in url_list:
        # 打开页面获取内容
        main2_page = OpenPage(url)
        # 解析内容, 获取url
        url2_list = ParseMainPage2(main2_page, url)
        for url2 in url2_list:
            url2 = url2+"profile"
            if url2 == "http://Tongchuan.cncn.com/jingdian/zhaojinhuaxuechang/profile":
                continue
            if url2 == "http://sxyulin.cncn.com/jingdian/profile":
                continue
            if url2 == "http://Ankang.cncn.com/jingdian/nvwamiao/profile":
                continue
            detail_page = OpenPage(url2)
            title,cont = ParseDetailPage(detail_page, url2)
            #profile
            content = filter_tags(cont)
            data = url2[:-7] + "\3" + title + "\3" + content + "\n"
            data = data.encode("utf-8")
            f.write(data)
            print url2 
    f.close()
    print "爬取完成"

