OCAMLMAKEFILE = ../OCamlMakefile

SOURCES   = aio_buffer.ml aio.mli aio.ml aio_stubs.c aio_buffer_stubs.c
CFLAGS    = -O2 -g -W -Wall
CLIBS	  = aio
RESULT    = aio

all: byte-code-library $(if $(wildcard /usr/bin/ocamlopt),native-code-library,)

install: $(if $(wildcard /usr/bin/ocamlopt),libinstall,libinstall-byte-code)
uninstall: libuninstall

distclean: clean
	rm -f .depend

-include $(OCAMLMAKEFILE)
