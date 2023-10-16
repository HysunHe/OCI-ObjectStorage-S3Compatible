# 使用 AWS S3 SDK API 或 REST API 操作 OCI 对象存储以及流式上传实现

## 概述

相比其它云厂商，AWS S3 是最早流行的对象存储云服务，因此，很多用户的应用程序早已是基于 S3 客户端实现。Oracle OCI 的对象存储服务也支持S3客户端协议，可以直接使用 AWS S3 SDK API 或 AWS S3 REST API 对 OCI 对象存储进行操作。

目前提供了使用 **10** 种语言的S3 SDK/API操作OCI对象存储的样例，包括 C，C++, Python, Java, Go, Nodejs, Rust, .Net, Objective-C, PHP

另外，由于 AWS S3 SDK 没有提供类似阿里云对象存储的流式上传接口，因此，我们实现一个单独的服务并向外提供 REST 接口，客户端直接调用此接口实现流式上传（如视频流文件的上传）。

## 多语言实现

### Python语言样例

Python 样例程序位于 [python/](python/) 子目录下。Python 样例程序演示了如何使用OCI信息配置AWS S3 Python SDK，并且实现了从 Bucket 获取对象列表和批量删除两个功能。关键是如何利用OCI信息配置AWS S3 SDK，如果需要实现更多的功能，那么直接参考AWS S3 SDK的官方文档中的实例代码即可。

### Go语言样例

Go 样例程序位于 [golang/](golang/) 子目录下。Go 样例程序演示了如何使用OCI信息配置AWS S3 Go SDK，并且实现了从 Bucket 获取对象列表和批量删除两个功能。关键是如何利用OCI信息配置AWS S3 SDK，如果需要实现更多的功能，那么直接参考AWS S3 SDK的官方文档中的实例代码即可。

### Java语言样例

Java 样例程序位于 [java/](java/) 子目录下。Java 样例程序演示了如何使用OCI信息配置AWS S3 Java SDK，并且实现了从 Bucket 获取上传和批量删除两个功能。关键是如何利用OCI信息配置AWS S3 SDK，如果需要实现更多的功能，那么直接参考AWS S3 SDK的官方文档中的实例代码即可。

### Nodejs样例

Nodejs 样例程序位于 [nodejs/](nodejs/) 子目录下。Nodejs 样例程序演示了如何使用OCI信息配置AWS S3 JavaScript SDK，并且实现了从 Bucket 获取对象列表和批量删除两个功能。关键是如何利用OCI信息配置AWS S3 SDK，如果需要实现更多的功能，那么直接参考AWS S3 SDK的官方文档中的实例代码即可。

### Rust语言样例

Nodejs 样例程序位于 [rust/](rust/) 子目录下。Rust 样例程序演示了如何使用OCI信息配置AWS S3 Rust SDK，并且实现了从 Bucket 获取对象列表和批量删除两个功能。关键是如何利用OCI信息配置AWS S3 SDK，如果需要实现更多的功能，那么直接参考AWS S3 SDK的官方文档中的实例代码即可。

### .Net语言样例

.Net 样例程序位于 [dotnet/](dotnet/) 子目录下。.net 样例程序演示了如何使用OCI信息配置AWS S3 .net SDK，并且实现了获取和显示 Bucket 列表的功能。关键是如何利用OCI信息配置AWS S3 SDK，如果需要实现更多的功能，那么直接参考AWS S3 SDK的官方文档中的实例代码即可。

### C语言样例

C 样例程序位于 [c/](c/OSS-Demo/ObjectStorageDemo/) 子目录下。C 样例程序演示了如何实现 HTTP 请求签名，以及如何通过 CURL 调用 S3 的 REST API 来实现想要的功能。样例实现了文件下载、流式下载、文件删除、批量删除、流式上传等功能。关键是如何实现请求的签名，如果需要实现更多的功能，那么直接调用相应的 S3 REST API即可。

### C++语言样例

C++ 样例程序位于 [cpp/](cpp/) 子目录下。C++ 样例程序演示了如何使用OCI信息配置AWS S3 C++ SDK，并且实现了文件下载、文件上传、分块上传的功能。关键是如何利用OCI信息配置AWS S3 SDK，如果需要实现更多的功能，那么直接参考AWS S3 SDK的官方文档中的实例代码即可。

### Objective-C语言样例

Objective-C 样例程序位于 [object-c/](object-c/ocis3test/) 子目录下。Objective-C 样例程序演示了如何使用OCI信息配置AWS S3 iOS SDK，并且实现了文件下载、文件上传、批量删除等功能。关键是如何利用OCI信息配置AWS S3 SDK，如果需要实现更多的功能，那么直接参考AWS S3 SDK的官方文档中的实例代码即可。

### PHP语言样例

PHP 样例程序位于 [php/](php/) 子目录下。PHP 样例程序演示了如何使用OCI信息配置AWS S3 .net SDK，并且实现了文件上传和下载功能。关键是如何利用OCI信息配置AWS S3 SDK，如果需要实现更多的功能，那么直接参考AWS S3 SDK的官方文档中的实例代码即可。

## 流式上传实现

流式上传实现 位于 [stream-upload-service/](stream-upload-service/) 子目录下。流式上传用 Python 实现了一个独立的 HTTP 服务，服务部署后，会对外提供 REST 接口，客户端通过调用此接口实现文件的流式上传功能。上述 [C语言样例](C语言样例) 通过调用 REST 接口实现了流式上传。
