CORES=4

all: src/
	make -j $(CORES) -C src/xtail
	make -j $(CORES) -C src/xsplit
	make -j $(CORES) -C src/xtree
	make -j $(CORES) -C src/xdiff
	make -j $(CORES) -C src/xcat
	make -j $(CORES) -C src/xgrep
	make -j $(CORES) -C src/xinode

test: src/ tests/
	make -C tests/xtail

install: bin/
	cp bin/xtail /usr/bin/
	cp bin/xsplit /usr/bin/
	cp bin/xtree /usr/bin/
	cp bin/xdiff /usr/bin/
	cp bin/xcat /usr/bin/
	cp bin/xgrep /usr/bin/
	cp bin/xinode /usr/bin/

uninstall:
	rm /usr/bin/xtail
	rm /usr/bin/xsplit
	rm /usr/bin/xtree
	rm /usr/bin/xdiff
	rm /usr/bin/xcat
	rm /usr/bin/grep