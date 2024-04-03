# https://sourceforge.net/projects/acme-crossass/
ACME=acme

# https://github.com/mach-kernel/cadius
CADIUS=cadius

# note: flags set to decrunch backwards
EXOMIZER=exomizer mem -lnone -P23 -q

BUILDDIR=build
DISKVOLUME=PAY.DAY
LOADER=$(BUILDDIR)/LOADER.SYSTEM\#FF2000
EXE=$(BUILDDIR)/PAY.DAY\#060800
COMPRESSED=src/AUXOBJ.X
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
	$(ACME) src/pay.day.a
	$(CADIUS) REPLACEFILE "$(BUILDDISK)" "/$(DISKVOLUME)/" "$(EXE)" -C
	@touch "$@"

$(RES): $(BUILDDIR)
	$(CADIUS) REPLACEFILE "$(BUILDDISK)" "/$(DISKVOLUME)/" "$@" -C
	@touch "$@"

$(COMPRESSED):
	$(EXOMIZER) src/AUXOBJ@0x3000 -o "$@"
	$(EXOMIZER) src/OBJ@0x4000 -o res/OBJ.X\#060000

mount: $(BUILDDISK)
	@open "$(BUILDDISK)"

clean:
	rm -rf "$(BUILDDIR)"

$(BUILDDIR):
	mkdir -p "$@"
	$(CADIUS) CREATEVOLUME "$(BUILDDISK)" "$(DISKVOLUME)" 140KB -C

all: clean mount

.NOTPARALLEL:
