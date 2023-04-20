# miniKMS

include ../config.mk

CFLAGS=$(shell pkg-config oehost-$(COMPILER) --cflags)
LDFLAGS=$(shell pkg-config oehost-$(COMPILER) --libs)
INCDIR=$(shell pkg-config oehost-$(COMPILER) --variable=includedir)

build:
	@ echo "Compilers used: $(CC), $(CXX)"
	oeedger8r ../kms.edl --untrusted \
		--search-path $(INCDIR) \
		--search-path $(INCDIR)/openenclave/edl/sgx
	$(CXX) -g -c $(CFLAGS) host.cpp
	$(CC) -g -c $(CFLAGS) kms_u.c
	$(CXX) -o kms_host kms_u.o host.o $(LDFLAGS)

clean:
	rm -f kms_host host.o kms_u.o kms_u.c kms_u.h kms_args.h
