""" Hysun He (hysun.he@oracle.com) @ 2023/07/04 """
import requests
import base64

import sys
import os

current = os.path.dirname(os.path.realpath(__file__))
parent = os.path.dirname(current)
sys.path.append(parent)

import my_utils

def upload_json_with_binary(url, json_data):
    x_amz_date = '20230724T023712Z'
    auth_header = my_utils.gen_auth_md5(x_amz_date)
    headers = {
        'Content-Type': 'application/json',
        'X-Amz-Date': x_amz_date,
        'Authorization': auth_header
    }
    response = requests.post(url, headers=headers, json=json_data)
    # print(response.content)
    if response.status_code == 200:
        print(response.json())
    else:
        print(f'{response.status_code}:{response.text}')

binary_data = bytes('data to be encoded: 你好, ya', 'utf-8')
json_data = {
    "bucket": "test", 
    "name":"hhh/json.bin.x2",
    "position": 30,
    "content": base64.b64encode(binary_data).decode(encoding='utf-8'),
    "append": "0"
}
# upload_url = 'http://132.226.236.106:5000/write-json'
upload_url = 'http://158.178.240.219:5000/write-json'
upload_json_with_binary(upload_url, json_data)
