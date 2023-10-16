import boto3
from botocore.client import Config

config = Config(
    connect_timeout=4,
    read_timeout=4,
    region_name='ap-seoul-1',
)

s3 = boto3.resource(
    's3',
    aws_access_key_id='63517899a4d4558570144a3a35e16c945f8e2476',
    aws_secret_access_key='6aZ4IG63Dgus9/LymMzqBqIjKCXxtmWHOx1OVDiIUwM=',
    config=config,
    endpoint_url='https://sehubjapacprod.compat.objectstorage.ap-seoul-1.oraclecloud.com',
)

bucket = s3.Bucket('HysunS3Bucket')
files = bucket.objects.all()
for file in files:
    print(file)
    
object_keys = {'net.png', 'apex.png'}
response = bucket.delete_objects(
    Delete={
        'Objects': [{'Key': key} for key in object_keys]
    }
)
print('Done')