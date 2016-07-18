include build.mk

all:
	mkdir -p $(OBJS_PATH)
	mkdir -p $(LOCAL_OBJS_PATH)
	mkdir -p $(PEER_OBJS_PATH)
	cd src/local_proxy && $(MAKE) 
	cd src/peer_proxy && $(MAKE) 

.PHONY : clean
clean:
	rm -rf $(OBJS_PATH)
	cd src/local_proxy; make clean
	cd src/peer_proxy; make clean
