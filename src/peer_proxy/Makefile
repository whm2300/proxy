OBJS = $(PEER_OBJS_PATH)/main.o

$(PEER_PROXY) : $(OBJS)
	${CXX} -o $(PROJECT_DIR)/$(PEER_PROXY) $(LDFLAGS) $(OBJS)

$(PEER_OBJS_PATH)/%.o : %.cc
	$(CXX) -c $(CXXFLAGS) $< -o $@

.PHONY : clean
clean :
	rm -f $(PROJECT_DIR)/$(PEER_PROXY)
