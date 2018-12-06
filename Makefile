CORES=4

all: src/
	make -j $(CORES) -C src/xtail
	make -j $(CORES) -C src/xsplit
	make -j $(CORES) -C src/xtree

test: src/ tests/
	make -C tests/xtail

install: bin/
	cp bin/xtail /usr/bin/
	cp bin/xsplit /usr/bin/
	cp bin/xtree /usr/bin/

uninstall:
	rm /usr/bin/xtail
	rm /usr/bin/xsplit
	rm /usr/bin/xtree
