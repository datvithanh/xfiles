CORES=4

all: src/
	make -j $(CORES) -C src/xtail

test: src/ tests/
	make -C tests/xtail

install: bin/
	cp bin/xtail /usr/bin/

uninstall:
	rm /usr/bin/xtail
