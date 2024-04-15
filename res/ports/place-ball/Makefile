# https://sourceforge.net/projects/acme-crossass/
ACME=acme

# https://github.com/mach-kernel/cadius
CADIUS=cadius

EXOMIZER=exomizer mem -lnone -P23 -f -q

BUILDDIR=build
DISKVOLUME=PLACE.BALL
LOADER=$(BUILDDIR)/LOADER.SYSTEM\#FF2000
EXE=$(BUILDDIR)/PLACE.BALL\#068000
COMPRESSED1=src/OBJ1.X
COMPRESSED2=src/OBJ2.X
SOURCES=$(wildcard src/*.a) $(COMPRESSED1) $(COMPRESSED2)
RES=$(wildcard res/*) $(wildcard ../common/res/*)
BUILDDISK=$(BUILDDIR)/$(DISKVOLUME).po

.PHONY: clean mount all

$(BUILDDISK): $(EXE) $(LOADER) $(RES)

$(LOADER): $(SOURCES) | $(BUILDDIR)
	$(ACME) src/loader.a
	$(CADIUS) REPLACEFILE "$(BUILDDISK)" "/$(DISKVOLUME)/" "$(LOADER)" -C
	@touch "$@"

$(EXE): $(SOURCES) $(COMPRESSED1) $(COMPRESSED2) | $(BUILDDIR)
	$(ACME) src/place.ball.a
	$(CADIUS) REPLACEFILE "$(BUILDDISK)" "/$(DISKVOLUME)/" "$(EXE)" -C
	@touch "$@"

$(RES): $(BUILDDIR)
	$(CADIUS) REPLACEFILE "$(BUILDDISK)" "/$(DISKVOLUME)/" "$@" -C
	@touch "$@"

$(COMPRESSED1):
	$(EXOMIZER) src/OBJ1@0x0900 -o "$@"

$(COMPRESSED2):
	$(EXOMIZER) src/OBJ2@0x5000 -o "$@"

mount: $(BUILDDISK)
	@open "$(BUILDDISK)"

clean:
	rm -rf "$(BUILDDIR)"

$(BUILDDIR):
	mkdir -p "$@"
	$(CADIUS) CREATEVOLUME "$(BUILDDISK)" "$(DISKVOLUME)" 140KB -C

all: clean mount

.NOTPARALLEL:
