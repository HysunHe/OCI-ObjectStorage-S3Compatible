//
//  AppDelegate.m
//  ocis3test
//
//  Created by Alex Huang on 2023/7/24.
//

#import "AppDelegate.h"
#import <AWSCore/AWSCore.h>
#import <AWSS3/AWSS3.h>

@interface AppDelegate ()

@end

@implementation AppDelegate


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    // Override point for customization after application launch.
    
    NSString *accessKey = @"7890a67c6305622a8ddf34fa1aa149f6eb0459b6";
    NSString *secretKey = @"zineJ+pN9vHayIPVxn7kXZ+vStFyw+01eMydPzNv47s=";
    // Set up your S3 bucket name and object key (file name)


    NSString *bucketName = @"s3test";
    NSString *objectKey = @"Jenkinsfile.rtf";
    // Configure AWS service
    AWSStaticCredentialsProvider *credentialsProvider = [[AWSStaticCredentialsProvider alloc] initWithAccessKey:accessKey secretKey:secretKey];
    
    AWSEndpoint *customEndpoint = [[AWSEndpoint alloc]initWithURLString: @"https://idnvvogqipdy.compat.objectstorage.us-ashburn-1.oraclecloud.com"];
    AWSServiceConfiguration *configuration = [[AWSServiceConfiguration alloc] initWithRegion:AWSRegionUSEast1 endpoint:customEndpoint credentialsProvider:credentialsProvider];
    
    [AWSServiceManager defaultServiceManager].defaultServiceConfiguration = configuration;
    [AWSS3 registerS3WithConfiguration:configuration forKey:@"USEast1S3"];

    AWSS3 *S3 = [AWSS3 S3ForKey:@"USEast1S3"];
    
    // Create the folder
    AWSS3PutObjectRequest *request = [AWSS3PutObjectRequest new];
    request.bucket = bucketName;
    request.key = @"test/";
    request.body = nil;
    [S3 putObject:request completionHandler:^(AWSS3PutObjectOutput * _Nullable response, NSError * _Nullable error) {
        if (error != nil) {
            NSLog(@"Error: %@", error);
        } else {
            NSLog(@"Folder created successfully");
        }
    }];
    
    //Deletefile
    AWSS3DeleteObjectRequest *deleteRequest = [AWSS3DeleteObjectRequest new];
    deleteRequest.bucket = bucketName;
    deleteRequest.key = objectKey;

    // Delete the file from the S3 bucket
    [[S3 deleteObject:deleteRequest] continueWithBlock:^id(AWSTask *deletetask) {
        if (deletetask.error) {
            NSLog(@"Error deleting file: %@", deletetask.error);
        } else {
            NSLog(@"File deleted successfully!");
        }
        return nil;
    }];
    
    //Download file
    //NSString *bucketName = @"YOUR_BUCKET_NAME";
    NSString *downloadsPath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) firstObject];
    NSString *dlobjectKey = @"tmux-client-93710.log";
    NSString *localFilePath = [downloadsPath stringByAppendingPathComponent:@"tmux-client-93710.log"]; // 文件保存在Downloads目录中的文件名
    //NSString *localFilePath = @"tmux-client-93710.log"; // 本地存储下载文件的路径

    AWSS3GetObjectRequest *dlrequest = [AWSS3GetObjectRequest new];
    dlrequest.bucket = bucketName;
    dlrequest.key = dlobjectKey;

    [[S3 getObject:dlrequest] continueWithBlock:^id(AWSTask *dltask) {
        if (dltask.error) {
            NSLog(@"Error downloading object: %@", dltask.error);
        } else if (dltask.result) {
            AWSS3GetObjectOutput *output = dltask.result;
            
            // 获取文件内容
            NSData *fileData = output.body;
            
            // 将文件内容保存到本地文件
            [fileData writeToFile:localFilePath atomically:YES];
            
            NSLog(@"文件下载成功，保存在路径：%@", localFilePath);
        }
        return nil;
    }];
    
    //Streaming download
    /*
    NSString *sdobjectKey = @"tmux-server-93712.log";
    NSString *sddownloadsPath = [NSSearchPathForDirectoriesInDomains(NSDownloadsDirectory, NSUserDomainMask, YES) firstObject];
        NSString *sdlocalFilePath = [sddownloadsPath stringByAppendingPathComponent:sdobjectKey];

    // 创建S3GetObjectRequest对象
    AWSS3GetObjectRequest *sdrequest = [AWSS3GetObjectRequest new];
    sdrequest.bucket = bucketName;
    sdrequest.key = sdobjectKey;

    // 设置下载进度回调
    sdrequest.downloadProgress = ^(int64_t bytesWritten, int64_t totalBytesWritten, int64_t totalBytesExpectedToWrite) {
        float progress = (float)totalBytesWritten / (float)totalBytesExpectedToWrite;
        NSLog(@"下载进度：%f", progress);
    };

    // 创建下载任务
    AWSTask *downloadTask = [S3 getObject:sdrequest];

    [downloadTask continueWithBlock:^id(AWSTask *task) {
        if (task.error) {
            NSLog(@"Error downloading object: %@", task.error);
        } else {
            // 获取文件内容
            AWSS3GetObjectOutput *output = task.result;
            NSData *fileData = output.body;
            
            // 将文件内容保存到本地
            [fileData writeToFile:sdlocalFilePath atomically:YES];
            NSLog(@"文件下载成功，保存在路径：%@", sdlocalFilePath);
        }
        return nil;
    }];
    */

    //批量删除
    // 要删除的目录
    /*
    NSString *directoryPath = @"test1/"; // 注意在结尾加上斜杠


    AWSS3Remove *delete = [AWSS3Remove new];
    delete.quiet = @NO; // 如果设置为@YES，则不会返回删除对象的结果
    // 创建并配置AWSS3DeleteObjectsRequest对象
    AWSS3DeleteObjectsRequest *btdeleteRequest = [AWSS3DeleteObjectsRequest new];
    btdeleteRequest.bucket = bucketName;
    

    // 创建并配置AWSS3Delete *objectsRequest对象

    NSMutableArray *objects = [NSMutableArray array];

    // 获取指定目录下的文件列表
    AWSS3ListObjectsRequest *listObjectsRequest = [AWSS3ListObjectsRequest new];
    listObjectsRequest.bucket = bucketName;
    listObjectsRequest.prefix = directoryPath;

    [[S3 listObjects:listObjectsRequest] continueWithBlock:^id(AWSTask *listtask) {
        if (listtask.error) {
            NSLog(@"Error listing objects: %@", listtask.error);
        } else {
            AWSS3ListObjectsOutput *listObjectsOutput = listtask.result;
            NSArray<AWSS3Object *> *objectSummaries = listObjectsOutput.contents;
            //NSLog(@"Delete Objects: %@", objectSummaries);
            
            for (AWSS3Object *objectSummary in objectSummaries) {
                if (![objectSummary.key isEqualToString:directoryPath]) {
                    AWSS3ObjectIdentifier *objectIdentifier = [AWSS3ObjectIdentifier new];
                    objectIdentifier.key = objectSummary.key;
                    [objects addObject:objectIdentifier];
                }
            }
            
            delete.objects = objects;
            btdeleteRequest.remove = delete;

            // 调用删除操作
            [[S3 deleteObjects:btdeleteRequest] continueWithBlock:^id(AWSTask *btdeltask) {
                if (btdeltask.error) {
                    NSLog(@"Error deleting objects: %@", btdeltask.error);
                } else {
                    NSLog(@"目录下的文件删除成功！");
                }
                return nil;
            }];
        }
        return nil;
    }];
*/

    //文件上传
    NSURL *fileURL = [NSURL fileURLWithPath:localFilePath];
    NSLog(@"File %@",fileURL);
    // Create an AWSS3PutObjectRequest to upload the data
    AWSS3PutObjectRequest *putObjectRequest = [AWSS3PutObjectRequest new];
    putObjectRequest.bucket = bucketName;
    putObjectRequest.key = @"upload-file.txt";
    putObjectRequest.body = fileURL;
    putObjectRequest.contentType = @"text/plain";
    
    // Optionally, you can set other properties such as ACL or ContentType if needed.
    // For example, to set the content type:
    // putObjectRequest.contentType = @"text/plain";

    // Upload the data to S3
    [[S3 putObject:putObjectRequest] continueWithBlock:^id(AWSTask *puttask) {
        if (puttask.error) {
            NSLog(@"Error listing objects: %@", puttask.error);
        } else {
            // Data successfully uploaded
            NSLog(@"文件上传成功");
        }
        return nil;
    }];




    return YES;
}


#pragma mark - UISceneSession lifecycle


- (UISceneConfiguration *)application:(UIApplication *)application configurationForConnectingSceneSession:(UISceneSession *)connectingSceneSession options:(UISceneConnectionOptions *)options {
    // Called when a new scene session is being created.
    // Use this method to select a configuration to create the new scene with.
    return [[UISceneConfiguration alloc] initWithName:@"Default Configuration" sessionRole:connectingSceneSession.role];
}


- (void)application:(UIApplication *)application didDiscardSceneSessions:(NSSet<UISceneSession *> *)sceneSessions {
    // Called when the user discards a scene session.
    // If any sessions were discarded while the application was not running, this will be called shortly after application:didFinishLaunchingWithOptions.
    // Use this method to release any resources that were specific to the discarded scenes, as they will not return.
}


@end
