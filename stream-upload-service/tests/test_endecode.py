""" Hysun He (hysun.he@oracle.com) @ 2023/07/04 """
import base64

src_str = 'data to be encoded: 你好'
print(f'src_str: {src_str}')
src_bytes = bytes('data to be encoded: 你好', 'utf-8')
print(f'src_bytes: {src_bytes}')
src_bytes_de = src_bytes.decode('utf-8')
print(f'src_bytes_de: {src_bytes_de}')
image_base4 = base64.b64encode (bytes('data to be encoded: 你好', 'utf-8'))
print(f'image_base4: {image_base4}')
image_base4_decode = image_base4.decode(encoding='utf-8')
print(f'image_base4_decode: {image_base4_decode}')
image_base4_en_de = base64.b64decode(image_base4).decode(encoding='utf-8')   
print(f'image_base4_en_de: {image_base4_en_de}')
image_base4_de_de = base64.b64decode(image_base4_decode).decode(encoding='utf-8')   
print(f'image_base4_de_de: {image_base4_de_de}')

from pathlib import Path
file_name="ccc/ddd/aaa.txt"
tmp_dir = file_name
while True:
    tmp_dir = Path(tmp_dir)
    tmp_dir_parent = tmp_dir.parent
    print(f'dir: {tmp_dir}')
    if str(tmp_dir) in ('.', '~', '/'):
        break
    tmp_dir = tmp_dir_parent