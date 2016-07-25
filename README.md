代理程序。TCP透传。

功能说明：
两个代理程序配合运行，peer_proxy运行在有公网IP的服务器上，local_proxy运行在无公网IP的服务器上。将无公网IP TCP端口映射到有公网IP服务器上。

依赖库：
libevent-2.0.22-stable
protobuf-2.5
boost-1.53

centos 7.0 boost安装:yum install boost-devel
