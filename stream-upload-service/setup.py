""" docstring """
from setuptools import setup

setup(
    name='oss_append',
    version='1.0',
    description='oss_append',
    author='hysun',
    author_email='hysunhe@foxmail.com',
    url='https://github.com/HysunHe/PythonLearn',
    license='XXX License 2.0',
    classifiers=[
        'Operating System :: OS Independent',
        'Programming Language :: Python 3.8',
        'Programming Language :: Python 3.9',
        'Programming Language :: Python 3.10',
    ],
    # packages=['devlog'], # packages参数时，不会递归的打包子package！只打包当前package！
    # packages=find_packages(), #find_packages只会打包内含__init__.py的package
    # packages=find_namespace_packages(),
    include_package_data=True,  #默认值为True。当为True时，将根据http://MANIFEST.in文件来打包分发库。
    python_requires='>=3.8, <3.11',
    install_requires=[
        'arrow==1.2.3',
        'astroid==2.6.6',
        'blinker==1.6.2',
        'certifi==2023.5.7',
        'cffi==1.15.1',
        'charset-normalizer==3.1.0',
        'circuitbreaker==1.4.0',
        'click==8.1.3',
        'cryptography==39.0.2',
        'Flask==2.3.2',
        'idna==3.4',
        'isort==5.12.0',
        'itsdangerous==2.1.2',
        'Jinja2==3.1.2',
        'jmespath==0.10.0',
        'lazy-object-proxy==1.9.0',
        'markdown-it-py==3.0.0',
        'MarkupSafe==2.1.3',
        'mccabe==0.6.1',
        'mdurl==0.1.2',
        'numpy==1.25.0',
        'opencv-python==4.8.0.74',
        'pathlib2==2.3.7.post1',
        'prompt-toolkit==3.0.29',
        'pycparser==2.21',
        'Pygments==2.15.1',
        'pylev==1.4.0',
        'pyOpenSSL==23.2.0',
        'python-dateutil==2.8.2',
        'pytz==2023.3',
        'PyYAML==6.0',
        'requests==2.31.0',
        'rich==13.4.2',
        'setuptools==67.8.0',
        'six==1.16.0',
        'terminaltables==3.1.0',
        'urllib3==2.0.3',
        'wcwidth==0.2.6',
        'Werkzeug==2.3.6',
        'wheel==0.38.4',
        'wrapt==1.12.1',
        'oci>=2.104,<3',
        'oci-cli>=3.29,<4',
    ],
    entry_points={  #把python文件中的函数自动生成为可执行脚本
        'console_scripts': [  # key值为console_scripts
            'ossappend = main:run'
        ]
    },
    scripts=['main.py']  #把.sh、.py等可执行脚本生成到系统path中
)
