""" Hysun He (hysun.he@oracle.com) @ 2023/07/04 """
import oci
import env_config


class OciConf:
    """ docstring """
    _OCI_CONF = None
    _NAMESPACE = None

    @classmethod
    def oci_config(cls):
        """ docstring """
        if cls._OCI_CONF is None:
            cls._OCI_CONF = oci.config.from_file(
                file_location=env_config.OCI_CONFIG_FILE)
        return cls._OCI_CONF

    @classmethod
    def core_client(cls):
        """ docstring """
        return oci.core.ComputeClient(cls.oci_config())

    @classmethod
    def object_storage_client(cls):
        """ docstring """
        return oci.object_storage.ObjectStorageClient(cls.oci_config())

    @classmethod
    def get_region(cls):
        """ docstring """
        return cls.oci_config()['region']  #pylint: disable=unsubscriptable-object

    @classmethod
    def get_namespace(cls):
        """ docstring """
        if cls._NAMESPACE is None:
            response = cls.object_storage_client().get_namespace(
                compartment_id=cls.oci_config()['tenancy'])  #pylint: disable=unsubscriptable-object
            cls._NAMESPACE = response.data
        return cls._NAMESPACE

    @classmethod
    def get_accesskey(cls) -> str:
        """ docstring """
        access_key = cls.oci_config()['access_key']  #pylint: disable=unsubscriptable-object
        assert access_key is not None, 'access_key field is required'
        return access_key

    @classmethod
    def get_secretkey(cls) -> str:
        """ docstring """
        secret_key = cls.oci_config()['secret_key']  #pylint: disable=unsubscriptable-object
        assert secret_key is not None, 'secret_key field is required'
        return secret_key
