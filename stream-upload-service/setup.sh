which conda > /dev/null 2>&1 || (wget -nc https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh && bash Miniconda3-latest-Linux-x86_64.sh -b && eval "$(~/miniconda3/bin/conda shell.bash hook)" && conda init && conda config --set auto_activate_base true);
pip install -r requirements-dev.txt

pid=`ps -ef | grep -i "main.py" | grep -v grep | awk '{print $2}'`
if [[ ! -z "$pid" ]]
then
	echo "Kill old one: $pid"
	kill -9 $pid > /dev/null 2>&1
fi

cd ~/oss-append > /dev/null

export OSS_BUCKET="test"
export NO_UPDATE_TIMEOUT_SECONDS=30
export BUF_FILE_DIR="/var/tmp/ossappend"
export OCI_CLI_SUPPRESS_FILE_PERMISSIONS_WARNING=True
export MAX_QUEUE_SIZE=2000000
export SYNCER_DOP=8
export SERVER_HOST="0.0.0.0"
export SERVER_LISTEN_PORT=5000
export OCI_CONFIG_FILE="~/.oci/config"
export DEBUG_LEVEL=20

nohup python main.py < /dev/null > ~/app.out 2>&1 &

cd - > /dev/null

echo "Started."