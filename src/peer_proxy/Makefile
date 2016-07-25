PEER_OBJS = $(PEER_OBJS_PATH)/main.o $(PEER_OBJS_PATH)/config.o \
			$(PEER_OBJS_PATH)/server.o

PROTOCOL_OBJS = $(PROTOCOL_OBJS_PATH)/packet.o \
				$(PROTOCOL_OBJS_PATH)/proxy.pb.o
UTIL_OBJS = $(UTIL_OBJS_PATH)/log.o

$(PEER_PROXY) : $(PEER_OBJS) $(PROTOCOL_OBJS) $(UTIL_OBJS)
	${CXX} -o $(PROJECT_DIR)/$(PEER_PROXY) $(LDFLAGS) $(PEER_OBJS) \
		$(PROTOCOL_OBJS) $(UTIL_OBJS)

$(PEER_OBJS_PATH)/%.o : %.cc
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(PROTOCOL_OBJS_PATH)/%.o : $(PROTOCOL_PATH)/%.cc
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(UTIL_OBJS_PATH)/%.o : $(UTIL_PATH)/%.cc
	$(CXX) -c $(CXXFLAGS) $< -o $@

.PHONY : clean
clean :
	rm -f $(PROJECT_DIR)/$(PEER_PROXY)
