DIR_LIB = ./lib
DIR_APP = ./app


DOXYGEN    = doxygen
DOXYFILE   = Doxyfile

all: build_lib build_app


build_lib:
	$(MAKE) -C $(DIR_LIB)


build_app: build_lib
	$(MAKE) -C $(DIR_APP)

doc :
	$(DOXYGEN) $(DOXYFILE)

clean :
	$(MAKE) -C $(DIR_APP) clean
	$(MAKE) -C $(DIR_LIB) clean

.PHONY: all build_lib build_app clean