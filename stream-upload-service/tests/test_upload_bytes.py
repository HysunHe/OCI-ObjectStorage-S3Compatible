""" Hysun He (hysun.he@oracle.com) @ 2023/07/04 """
import sys
import os
import requests

current = os.path.dirname(os.path.realpath(__file__))
parent = os.path.dirname(current)
sys.path.append(parent)

import my_utils

def upload_binary_data(url, data):
    x_amz_date = '20230724T023712Z'
    auth_header = my_utils.gen_auth_md5(x_amz_date)
    headers = {
        'X-Amz-Date': x_amz_date,
        'Authorization': auth_header
    }
    response = requests.post(url=url, headers=headers, data=data)
    # print(response.content)
    if response.status_code == 200:
        print(response.json())
    else:
        print(f'{response.status_code}:{response.text}')

binary_data = bytes('data to be encoded: 你好, ha', 'utf-8')

upload_url = 'http://158.178.240.219:5000/write-bytes?bucket=test&name=yyy/demo3.bin&position=0&append=1'

upload_binary_data(upload_url, binary_data)
