# 文件流式上传服务

## 描述

在上传视频流时，一个视频流文件通常并不是一次性生成的，而是每次生成并上传一部分，最终通过客户端提供的文件结束标志来判断整个视频流文件是否已经完整。

这个流式上传服务用 Python 实现了一个独立的 HTTP 服务，服务部署后，会对外提供 REST 接口，客户端通过调用此接口实现文件的流式上传功能。

## 具体用法

```python

# Upload with JSON payload
# -------------------------------

binary_data = bytes('data to be encoded: 你好, ya', 'utf-8')
json_data = {
    "bucket": "test",
    "name":"json.bin.1",
    "position": 60,
    "content": base64.b64encode(binary_data).decode(encoding='utf-8'),
    "append": "1"  # "0"代表文件将上传至对象存储；"1"表示内容将追加至已有文件
}

headers = {
    'Content-Type': 'application/json',
    'X-Amz-Date': x_amz_date,
    'Authorization': auth_header
}
upload_url = '<http://158.178.240.219:5000/write-json>'

response = requests.post(url=upload_url, headers=headers, json=json_data)
if response.status_code == 200:
    print(response.json())
else:
    print(f'{response.status_code}:{response.text}')

# 返回值：
#    {'current_file_position': 30, 'location': '', 'status': 'ok'}


# Upload with bytes payload
# -------------------------------

binary_data = bytes('data to be encoded: 你好, ha', 'utf-8')
upload_url = '<http://158.178.240.219:5000/write-bytes?bucket=test&name=demo2.bin&position=0&append=1>'
headers = {
    'X-Amz-Date': x_amz_date,
    'Authorization': auth_header
}

response = requests.post(url=upload_url, headers=headers, data=binary_data)

if response.status_code == 200:
    print(response.json())
else:
    print(f'{response.status_code}:{response.text}')

# 返回值：
#    {'current_file_position': 30, 'location': '', 'status': 'ok'}

```
