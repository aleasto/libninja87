libninja87: out/libninja87.so
out/libninja87.so: ninja87.c include/**/*
	@mkdir out 2>/dev/null || true
	@gcc -c -fpic -Wall -Werror -Iinclude -o out/ninja87.o ninja87.c
	@gcc -shared -lusb-1.0 -o out/libninja87.so out/ninja87.o
	@echo ===== Made out/libninja87.so =====

toy: out/toy
out/toy: examples/toy.c include/**/* out/libninja87.so
	@gcc -Iinclude -Lout -lusb-1.0 -lninja87 -o out/toy examples/toy.c
	@echo ===== Made out/toy ======

cli: out/ninja87cli
out/ninja87cli: examples/cli.c include/**/* out/libninja87.so
	@gcc -Iinclude -Lout -lusb-1.0 -lninja87 -o out/ninja87cli examples/cli.c
	@echo ===== Made out/ninja87cli ======

clean:
	@rm -rf out/
	@echo ===== Cleared out directory =====

install: libninja87 include/ninja87/*
	cp out/libninja87.so /usr/local/lib/
	cp -r include/ninja87 /usr/local/include/
	echo /usr/local/lib > /etc/ld.so.conf.d/ninja87.conf
	@ldconfig
	@echo ===== Installed libninja87 =====

uninstall:
	-rm /usr/local/lib/libninja87
	-rm -rf /usr/local/include/ninja87
	-rm /etc/ld.so.conf.d/ninja87.conf
	@ldconfig
	@echo ===== Uninstalled libninja87 =====

.PHONY: libninja87 cli toy clean install uninstall
