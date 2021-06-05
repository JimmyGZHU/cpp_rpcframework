# aux_source_directory(. SRC_LIST)

set(SRC_LIST 
    mprpcapplication.cc 
    mprpcconfig.cc 
    rpcheader.pb.cc 
    rpcprovider.cc 
    mprpcchannel.cc
    mprpccontroller.cc
    logger.cc
    zookeeperutil.cc
    )

# 编译成静态库, 动态库加SHARE即可
add_library(mprpc ${SRC_LIST})

target_link_libraries(mprpc muduo_net muduo_base pthread zookeeper_mt)
