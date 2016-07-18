export CC=gcc
export CXX=g++

export PROJECT_DIR := $(shell /bin/pwd)

export CXXFLAGS :=
CXXFLAGS += -ggdb -Wall -extra -Wunused-function
CXXFLAGS += -std=c++0x
CXXFLAGS += -I $(PROJECT_DIR)/src/include
#CXXFLAGS += -I "/usr/local/include/boost"

export LDFLAGS :=
#LDFLAGS += $(PROJECT_DIR)/src/lib/libevent.so
#LDFLAGS += $(PROJECT_DIR)/src/lib/libhiredis.so
#LDFLAGS += -lrt -lpthread -lprotobuf
#LDFLAGS += -lboost_system -lboost_filesystem

export OBJS_PATH = $(PROJECT_DIR)/objs
export LOCAL_OBJS_PATH = $(OBJS_PATH)/local_proxy
export PEER_OBJS_PATH = $(OBJS_PATH)/peer_proxy
export LOCAL_PROXY = local_proxy
export PEER_PROXY = peer_proxy
