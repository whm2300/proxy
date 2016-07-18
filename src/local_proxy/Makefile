OBJS = $(LOCAL_OBJS_PATH)/main.o

$(LOCAL_PROXY) : $(OBJS)
	${CXX} -o $(PROJECT_DIR)/$(LOCAL_PROXY) $(LDFLAGS) $(OBJS)

$(LOCAL_OBJS_PATH)/%.o : %.cc
	$(CXX) -c $(CXXFLAGS) $< -o $@

.PHONY : clean
clean :
	rm -f $(PROJECT_DIR)/$(LOCAL_PROXY)
