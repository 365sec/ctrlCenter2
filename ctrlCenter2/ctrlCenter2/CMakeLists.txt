cmake_minimum_required(VERSION 2.8)
AUX_SOURCE_DIRECTORY(. SRC_LIST)
#SET(SRC_LIST asio_service.cpp)
INCLUDE_DIRECTORIES(../../asio_service/src ../../boost_1_66_0  ../../SQLAPI/include)

LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../build)
ADD_EXECUTABLE(ctrlcenter ${SRC_LIST})
TARGET_LINK_LIBRARIES(ctrlcenter  sqlapi HardInfo  FtRock3Key  dl boost_date_time-mt boost_thread-mt boost_system-mt boost_regex-mt boost_log-mt boost_filesystem-mt pthread asio_service)
