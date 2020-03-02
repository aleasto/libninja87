libninja87: out/libninja87.so
out/libninja87.so: ninja87.c include/**/*
	@mkdir out 2>/dev/null || true
	@gcc -o out/ninja87.o -c -fpic -Wall -Werror -Iinclude ninja87.c
	@gcc -o out/libninja87.so -shared out/ninja87.o -lusb-1.0
	@echo ===== Made out/libninja87.so =====

toy: out/toy
out/toy: examples/toy.c include/**/* out/libninja87.so
	@gcc -o out/toy -Iinclude -Lout examples/toy.c -lninja87 -lusb-1.0
	@echo ===== Made out/toy ======

cli: out/ninja87cli
out/ninja87cli: examples/cli.c include/**/* out/libninja87.so
	@gcc -o out/ninja87cli -Iinclude -Lout examples/cli.c -lninja87 -lusb-1.0
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
