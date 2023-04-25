CC=gcc
CFLAGS=-Wall -Wstrict-prototypes -Werror -fPIC

LD_SONAME=-Wl,-soname,libnss_create.so.2
LIBRARY=libnss_create.so.2.0
LINKS=libnss_create.so.2 libnss_create.so

DESTDIR=/
PREFIX=$(DESTDIR)usr
LIBDIR=$(PREFIX)/lib64
BUILD=.libs

default: build
build: nss_create

nss_create_build_dir:
	[ -d $(BUILD) ] || mkdir $(BUILD)

nss_create-passwd:
	$(CC) $(CFLAGS) -c nss_create-passwd.c -o $(BUILD)/nss_create-passwd.o

nss_create_services: nss_create-passwd

nss_create: nss_create_build_dir nss_create_services
	$(CC) -shared $(LD_SONAME) -o $(BUILD)/$(LIBRARY) \
		$(BUILD)/nss_create-passwd.o

clean:
	rm -rf $(BUILD)

install:
	[ -d $(LIBDIR) ] || install -d $(LIBDIR)
	install $(BUILD)/$(LIBRARY) $(LIBDIR)
	cd $(LIBDIR); for link in $(LINKS); do ln -sf $(LIBRARY) $$link ; done
	cp nss_create /sbin
	sed -i.bak 's/^passwd:.*sss files$$/passwd:     sss files create files/g' /etc/nsswitch.conf
	systemctl restart sshd

remove:
	sed -i.bak 's/^passwd:.*sss files create files$$/passwd:     sss files/g' /etc/nsswitch.conf
	rm -rf /sbin/nss_create $(LIBDIR)/$(LIBRARY)
	cd $(LIBDIR); for link in $(LINKS); do unlink $$link ; done
	systemctl restart sshd


.PHONY: clean install nss_create_build_dir nss_create
