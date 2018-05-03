import requests
import csv
import random
import time
import socket
import http.client
from bs4 import BeautifulSoup
import string


def get_content(url, data = None):
    header = {
                 'Accept': 'text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8',
                 'Accept-Encoding': 'gzip, deflate',
                 'Accept-Language': 'zh-CN,zh;q = 0.9',
                 'Connection': 'keep-alive',
                 'User-Agent': 'Mozilla/5.0(Windows NT 6.1; Win64; x64) AppleWebKit/537.36(KHTML, like Gecko)\
                  Chrome/65.0.3325.181 Safari/537.36'
    }

    timeout = random.choice(range(80,180))
    while True:
        try:
            rep = requests.get(url, headers = header, timeout = timeout)
            rep.encoding = 'utf-8'
            break
        except socket.timeout as e:
            print('3:', e)
            time.sleep(random.choice(range(8,15)))
        except socket.error as e:
            print('4:', e)
            time.sleep(random.choice(range(20,60)))
        except http.client.BadStatusLine as e:
            print('5:', e)
            time.sleep(random.choice(range(30,80)))
        except http.client.IncompleteRead as e:
            print('6:', e)
            time.sleep(random.choice(range(5,15)))

    return rep.text

def get_data(html_text):
    final = []
    flag = True
    bs = BeautifulSoup(html_text, "html.parser")
    body = bs.body
    data = body.find('div', {'id': '7d'})
    ul = data.find('ul')
    li = ul.find_all('li')

    final.append(["日期 ","天气 ","最高温度 ","最低温度"])
    for day in li:
        temp = []
        date = day.find('h1').string

        temp.append(date)

        inf = day.find_all('p')

        temp.append((inf[0]).string)
        if inf[1].find('span') is None:
            temperature_highest = None
        else:
            temperature_highest = inf[1].find('span').string
            temperature_highest = temperature_highest.replace('℃','')
        temperature_lowest = inf[1].find('i').string
        temperature_lowest = temperature_lowest.replace('℃','')
        temp.append(temperature_highest)
        temp.append(temperature_lowest)
        final.append(temp)
    print(final)
    return final

def write_data(data, name):
    file__name = name
    with open(file__name, 'w', errors='ignore', newline='') as f:
        f_csv = csv.writer(f)
        f_csv.writerows(data)

if __name__ == '__main__':
    url = 'http://www.weather.com.cn/weather/101200105.shtml'
    html = get_content(url)
    result = get_data(html)
    write_data(result, 'weather.csv')