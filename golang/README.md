# 用 AWS S3 Go SDK 读写 Oracle OCI 对象存储

## 准备S3密钥

准备Customer Key:

![secret-key](secret-key.png)

## 安装AWS S3 SDK  

```shell
go get -u github.com/aws/aws-sdk-go/aws
```

## 样例程序代码

```go

package main

import (
 "fmt"

 "github.com/aws/aws-sdk-go/aws"
 "github.com/aws/aws-sdk-go/aws/awserr"
 "github.com/aws/aws-sdk-go/aws/credentials"
 "github.com/aws/aws-sdk-go/aws/session"
 "github.com/aws/aws-sdk-go/service/s3"
)

func main() {
 fmt.Println("s3 sample")

 sess, _ := session.NewSession(&aws.Config{
  Region:      aws.String("ap-seoul-1"),
//   Credentials: credentials.NewStaticCredentials("ACCESS_KEY", "SECRET", ""),
  Credentials: credentials.NewStaticCredentials("63517899a4d4558570144a3a35e16c945f8e2476", "6aZ4IG63Dgus9/LymMzqBqIjKCXxtmWHOx1OVDiIUwM=", ""),
  Endpoint:    aws.String("https://sehubjapacprod.compat.objectstorage.ap-seoul-1.oraclecloud.com"),
 })

 client := s3.New(sess)

 list_objects(client)

 batch_delete(client)
}

func list_objects(svc *s3.S3) {
 fmt.Println("list_objects>>>")

 input := &s3.ListObjectsInput{
  Bucket:  aws.String("HysunS3Bucket"),
  MaxKeys: aws.Int64(200),
 }

 result, err := svc.ListObjects(input)
 if err != nil {
  if aerr, ok := err.(awserr.Error); ok {
   switch aerr.Code() {
   case s3.ErrCodeNoSuchBucket:
    fmt.Println(s3.ErrCodeNoSuchBucket, aerr.Error())
   default:
    fmt.Println(aerr.Error())
   }
  } else {
   // Print the error, cast err to awserr.Error to get the Code and
   // Message from an error.
   fmt.Println(err.Error())
  }
  return
 }

 fmt.Println(result)
}

func batch_delete(svc *s3.S3) {
 input := &s3.DeleteObjectsInput{
  Bucket: aws.String("HysunS3Bucket"),
  Delete: &s3.Delete{
   Objects: []*s3.ObjectIdentifier{
    {
     Key: aws.String("apex.png"),
    },
    {
     Key: aws.String("0211_2.png"),
    },
   },
   Quiet: aws.Bool(false),
  },
 }

 result, err := svc.DeleteObjects(input)
 if err != nil {
  if aerr, ok := err.(awserr.Error); ok {
   switch aerr.Code() {
   default:
    fmt.Println(aerr.Error())
   }
  } else {
   // Print the error, cast err to awserr.Error to get the Code and
   // Message from an error.
   fmt.Println(err.Error())
  }
  return
 }

 fmt.Println(result)
}

```

## 其它更多操作

AWS S3(GO SDK) 基本操作参考文档：<https://docs.aws.amazon.com/sdk-for-go/v1/developer-guide/s3-example-basic-bucket-operations.html>
