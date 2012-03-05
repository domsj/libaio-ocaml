all:
	$(MAKE) -C lib $@

%:
	$(MAKE) -C lib $@

clean:
	$(MAKE) -C lib $@
	$(MAKE) -C examples $@

test:
	$(MAKE) -C examples $@
