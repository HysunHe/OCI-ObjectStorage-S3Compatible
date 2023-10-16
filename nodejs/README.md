# 使用 AWS JS SDK API 操作 OCI 对象存储

## 准备S3密钥

准备Customer Key:

![secret-key](secret-key.png)

## 安装AWS S3 SDK  

```shell
npm install @aws-sdk/client-s3
```

## 样例程序代码

```javascript

import {
    DeleteObjectsCommand,
    ListObjectsCommand,
    S3Client,
} from "@aws-sdk/client-s3";

export async function main() {
    const s3Client = new S3Client({
        region: 'ap-seoul-1', 
        credentials: {
            'accessKeyId': '63517899a4d4558570144a3a35e16c945f8e2476',
            'secretAccessKey': '6aZ4IG63Dgus9/LymMzqBqIjKCXxtmWHOx1OVDiIUwM=',
            'sessionToken': ''
        },
        'endpoint': 'https://sehubjapacprod.compat.objectstorage.ap-seoul-1.oraclecloud.com'
    });

    const bucketName = 'HysunS3Bucket';

    // List objects
    await s3Client.send(
        new ListObjectsCommand({
            Bucket: bucketName,
        })
    ).then(function(result){
        console.info('--- All objects in the bucket ---')
        result.Contents.forEach(element => {
            console.info(element)
        });
    })


    // Delete objects
    await s3Client.send(
        new DeleteObjectsCommand({ 
            'Bucket': bucketName, 
            'Delete': { 
                'Objects': [{ 
                    'Key': 'apex.png', 
                },{ 
                    'Key': '0211_2.png', 
                }],
                'Quiet': false,
            }, 
        })
    ).then(function(result){
        console.info('--- Deleted objects ---')
        result.Deleted.forEach(element => {
            console.info(element)
        });
    });
}

```

## 其它更多操作

AWS S3 JS SDK 参考文档：<https://docs.aws.amazon.com/sdk-for-javascript/v3/developer-guide/welcome.html>
