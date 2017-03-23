.PHONY: all
all: opt

opt:
	$(MAKE) -C src opt

debug:
	$(MAKE) -C src debug

coverage:
	$(MAKE) -C src coverage

.PHONY: clean
clean:
	$(MAKE) -C src clean
