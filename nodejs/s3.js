/***
 * 使用 AWS JS SDK API 操作 OCI 对象存储
 * - 2023/10/13 By Hysun He: Initial version.
***/

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

import { fileURLToPath } from "url";
if (process.argv[1] === fileURLToPath(import.meta.url)) {
  main();
}