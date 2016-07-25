export CC=gcc
export CXX=g++

export PROJECT_DIR := $(shell /bin/pwd)

export CXXFLAGS :=
CXXFLAGS += -ggdb -Wall -extra -Wunused-function
CXXFLAGS += -std=c++0x
CXXFLAGS += -I $(PROJECT_DIR)/src
#CXXFLAGS += -I "/usr/local/include/boost"

export LDFLAGS :=
#LDFLAGS += $(PROJECT_DIR)/src/lib/libevent.so
#LDFLAGS += $(PROJECT_DIR)/src/lib/libhiredis.so
#LDFLAGS += -lrt -lpthread -lprotobuf
LDFLAGS += -levent -lprotobuf -lpthread
LDFLAGS += -lboost_system -lboost_filesystem

export PROTOCOL_PATH = $(PROJECT_DIR)/src/protocol
export UTIL_PATH = $(PROJECT_DIR)/src/util
export OBJS_PATH = $(PROJECT_DIR)/objs
export LOCAL_OBJS_PATH = $(OBJS_PATH)/local_proxy
export PEER_OBJS_PATH = $(OBJS_PATH)/peer_proxy
export PROTOCOL_OBJS_PATH = $(OBJS_PATH)/protocol
export UTIL_OBJS_PATH = $(OBJS_PATH)/util
export LOCAL_PROXY = bin/local_proxy
export PEER_PROXY = bin/peer_proxy
