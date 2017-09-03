.PHONY: all
all: opt

opt:
	$(MAKE) -C libraries/EightBit/src opt
	$(MAKE) -C libraries/EightBit/Intel8080/src opt
	$(MAKE) -C src opt

debug:
	$(MAKE) -C libraries/EightBit/src debug
	$(MAKE) -C libraries/EightBit/Intel8080/src debug
	$(MAKE) -C src debug

coverage:
	$(MAKE) -C libraries/EightBit/src coverage
	$(MAKE) -C libraries/EightBit/Intel8080/src coverage
	$(MAKE) -C src coverage

.PHONY: clean
clean:
	$(MAKE) -C libraries/EightBit/src clean
	$(MAKE) -C libraries/EightBit/Intel8080/src clean
	$(MAKE) -C src clean
