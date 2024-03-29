# https://sourceforge.net/projects/acme-crossass/
ACME=acme

# https://github.com/mach-kernel/cadius
CADIUS=cadius

EXOMIZER=exomizer mem -lnone -P23 -f -q

BUILDDIR=build
DISKVOLUME=MU.TORERE
LOADER=$(BUILDDIR)/LOADER.SYSTEM\#FF2000
EXE=$(BUILDDIR)/MU.TORERE\#068000
COMPRESSED=src/OBJ.X
SOURCES=$(wildcard src/*.a) $(COMPRESSED)
RES=$(wildcard res/*) $(wildcard ../common/res/*)
BUILDDISK=$(BUILDDIR)/$(DISKVOLUME).po

.PHONY: clean mount all

$(BUILDDISK): $(EXE) $(LOADER) $(RES)

$(LOADER): $(SOURCES) | $(BUILDDIR)
	$(ACME) src/loader.a
	$(CADIUS) REPLACEFILE "$(BUILDDISK)" "/$(DISKVOLUME)/" "$(LOADER)" -C
	@touch "$@"

$(EXE): $(SOURCES) $(COMPRESSED) | $(BUILDDIR)
	$(ACME) src/mu.torere.a
	$(CADIUS) REPLACEFILE "$(BUILDDISK)" "/$(DISKVOLUME)/" "$(EXE)" -C
	@touch "$@"

$(RES): $(BUILDDIR)
	$(CADIUS) REPLACEFILE "$(BUILDDISK)" "/$(DISKVOLUME)/" "$@" -C
	@touch "$@"

$(COMPRESSED):
	$(EXOMIZER) src/OBJ@0x0900 -o "$@"

mount: $(BUILDDISK)
	@open "$(BUILDDISK)"

clean:
	rm -rf "$(BUILDDIR)"

$(BUILDDIR):
	mkdir -p "$@"
	$(CADIUS) CREATEVOLUME "$(BUILDDISK)" "$(DISKVOLUME)" 140KB -C

all: clean mount

.NOTPARALLEL:
