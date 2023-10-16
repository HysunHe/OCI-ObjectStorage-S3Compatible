/**
 * 使用 AWS Rust SDK API 操作 OCI 对象存储
 * - 2023/10/13 By Hysun He: Initial version.
*/

use aws_config::ConfigLoader;
use aws_sdk_s3::{config::Credentials, config::Region, meta::PKG_VERSION, Client, Error};
use aws_sdk_s3::types::{Delete, ObjectIdentifier};

#[tokio::main]
async fn main() -> Result<(), Error> {
    println!();
    println!("OCI Object Storage Rust Sample >>>");
    println!("S3 client version: {}", PKG_VERSION);
    println!();
    
    
    let credential = Credentials::new(
        "63517899a4d4558570144a3a35e16c945f8e2476",
        "6aZ4IG63Dgus9/LymMzqBqIjKCXxtmWHOx1OVDiIUwM=",
        None,
        None,
        "OCI_CRED",
    );
    
    let config = ConfigLoader::default()
        .region(Region::new("ap-seoul-1"))
        .credentials_provider(credential)
        .endpoint_url("https://sehubjapacprod.compat.objectstorage.ap-seoul-1.oraclecloud.com")
        .load()
        .await;
    
    let bucket = "HysunS3Bucket";
    
    let client = Client::new(&config);

    list_objects(&client, bucket).await?;

    batch_delete(&client, bucket).await?;

    Ok(())

}

async fn list_objects(client: &Client, bucket: &str) -> Result<(), Error> {
    let resp = client.list_objects_v2().bucket(bucket).send().await?;

    for object in resp.contents().unwrap_or_default() {
        println!("{}", object.key().unwrap_or_default());
    }

    Ok(())
}

async fn batch_delete(client: &Client, bucket: &str) -> Result<(), Error> {
    println!();
    println!("Deleting objects >>> ");

    let objects = vec![String::from("apex.png"), String::from("0211_2.png")];

    let mut delete_objects: Vec<ObjectIdentifier> = vec![];

    for obj in objects {
        let obj_id = ObjectIdentifier::builder().set_key(Some(obj)).build();
        delete_objects.push(obj_id);
    }

    let delete = Delete::builder().set_objects(Some(delete_objects)).build();

    client
        .delete_objects()
        .bucket(bucket)
        .delete(delete)
        .send()
        .await?;

    println!("Objects deleted.");

    Ok(())
}
