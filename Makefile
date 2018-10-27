CORES=4

all: src/
	make -j $(CORES) -C src/xtail 
