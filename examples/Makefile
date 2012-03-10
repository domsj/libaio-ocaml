test: test-byte $(if $(wildcard /usr/bin/ocamlopt),test-opt,)
	@: # Do not remove

test-byte:
	@echo "### Test byte code ###"
	$(MAKE) -C ../lib  byte-code-library
	ocamlc -I ../lib -o test.byte unix.cma bigarray.cma aio.cma test.ml
	./test.byte 2>&1 | tee test.out
	md5sum -c test.md5sum
	rm testfile test.out test.byte

test-opt:
	@echo "### Test native code ###"
	$(MAKE) -C ../lib  native-code-library
	ocamlopt -I ../lib -o test.opt unix.cmxa bigarray.cmxa aio.cmxa test.ml
	./test.opt 2>&1 | tee test.out
	md5sum -c test.md5sum
	rm testfile test.out test.opt
	@echo "### Test successfull ###"

clean:
	rm -f *.cmx *.cmi *.cmo *.o
	rm -f test.out testfile test.opt test.byte

distclean: clean

.NOTPARALLEL:
