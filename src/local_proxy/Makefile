LOCAL_OBJS = $(LOCAL_OBJS_PATH)/main.o $(LOCAL_OBJS_PATH)/config.o \
			 $(LOCAL_OBJS_PATH)/server.o

PROTOCOL_OBJS = $(PROTOCOL_OBJS_PATH)/packet.o \
				$(PROTOCOL_OBJS_PATH)/proxy.pb.o
UTIL_OBJS = $(UTIL_OBJS_PATH)/log.o

$(LOCAL_PROXY) : $(LOCAL_OBJS) $(PROTOCOL_OBJS) $(UTIL_OBJS)
	${CXX} -o $(PROJECT_DIR)/$(LOCAL_PROXY) $(LDFLAGS) $(LOCAL_OBJS) \
		$(PROTOCOL_OBJS) $(UTIL_OBJS)

$(LOCAL_OBJS_PATH)/%.o : %.cc
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(PROTOCOL_OBJS_PATH)/%.o : $(PROTOCOL_PATH)/%.cc
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(UTIL_OBJS_PATH)/%.o : $(UTIL_PATH)/%.cc
	$(CXX) -c $(CXXFLAGS) $< -o $@

.PHONY : clean
clean :
	rm -f $(PROJECT_DIR)/$(LOCAL_PROXY)
