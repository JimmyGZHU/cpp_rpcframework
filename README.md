本项目是一个简易的RPC分布式通信框架，涉及到的技术点有: c++、protobuf、zookeeper、muduo，整体实现逻辑为：
客户端与服务端之间的网络通信业务基于muduo库实现，RPC结点的管理(服务配置中心)基于zookeeper实现，通信数据
的序列化/反序列化通过protobuf实现，最终将项目代码整合成一个静态库放在lib文件夹中，项目采用纯c++编写。

框架的使用方法为：
1、前提：
1）安装好protobuf 3.11.0，不同版本的protobuf会出现API不同的问题，可能会编译出错。
2）安装好zookeeper 3.4.10，并进行编译生成c/java的代码目录供我们调用，同时测试zkServer/zkCli的使用。
3）安装好muduo库并编译。

2、sh autobuild.sh开启自动化编译。

3、进入到bin目录开启服务。
   cd bin
   ./provider -i test.conf
   ./consumer -i test.conf

4、可以进入到zookeeper的目录，通过sh zkCli.sh -server 127.0.0.1:2181连接zookeeper服务端(前提是已经开启)，
   然后通过ls、get等命令查看服务结点的数据和分布情况。


Tips: 我把protobuf对应版本的压缩包放在项目根目录，可以自取并解压安装。zookeeper版本为3.4.10，文件太大无法上传，大家自行下载即可。
