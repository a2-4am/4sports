# default configuration inherited from Improbable engine
# adapted for Strategic Craps

# 1, 2, or 3 correspond to Apple Pascal v1.1, v1.2, or v1.3
# note that 0 is an invalid value
version=3

# boolean, 0 or 1 (normally 1, use 0 for 48K Pascal e.g. Wizardry)
is_64kb=1

# boolean, 0 or 1 (normally 0, use 1 for 128K Pascal e.g. Widespread)
# use 0 for disks that boot 64K Pascal then manually use auxmem e.g. Jostens Learning
is_128kb=0

# boolean, 0 or 1 (normally 1, use 0 if you need write support)
read_only=1

# boolean, 0 or 1 (normally 1, use 0 if you know in advance that game clobbers auxmem)
backup_prodos=1

# integer, 1+ (number of disks)
# multi-disk games will only work OOTB if they support 2 drives
# otherwise the disk images will need to be patched to support 'swapping' disks
# see Angelsoft adventures for examples of magic swapping
disks=1

# boolean, 0 or 1 (normally 0, use 1 for Pascal v1.2 variant used by Reading Workshop)
is_reading=0

# boolean, 0 or 1 (normally 0, use 1 for Pascal v1.1 variant used by Apple Presents Apple)
is_apa=0

# boolean, 0 or 1 (normally 0, use 1 for Pascal v1.1 variant used by The Dark Heart of Uukrul)
is_uukrul=0

# boolean, 0 or 1 (normally 0, use 1 for Pascal v1.2 variant used by Sundog Frozen Legacy)
is_sundog=0

#-----------------------------------------------------------------------

# https://sourceforge.net/projects/acme-crossass/
ACME=acme

# https://github.com/mach-kernel/cadius
CADIUS=cadius

BUILDDIR=build
SOURCES=$(wildcard src/*.a)
EXE=$(BUILDDIR)/STRATEGIC.CRAPS\#FF2000
DATA=$(wildcard res/*)
DISKVOLUME=STRATEGIC.CRAPS
BUILDDISK=$(BUILDDIR)/$(DISKVOLUME).po

.PHONY: clean mount all

$(BUILDDISK): $(DATA) $(EXE)

$(EXE): $(SOURCES) | $(BUILDDIR)
	$(ACME) -r build/improbable.lst \
		-Dversion=$(version) \
		-Dis_64kb=$(is_64kb) \
		-Dis_128kb=$(is_128kb) \
		-Dread_only=$(read_only) \
		-Dbackup_prodos=$(backup_prodos) \
		-Ddisks=$(disks) \
		-Dis_reading=$(is_reading) \
		-Dis_apa=$(is_apa) \
		-Dis_uukrul=$(is_uukrul) \
		-Dis_sundog=$(is_sundog) \
		src/improbable.a
	$(CADIUS) REPLACEFILE "$(BUILDDISK)" "/$(DISKVOLUME)/" "$(EXE)" -C
	@touch "$@"

$(DATA): $(BUILDDIR)
	$(CADIUS) REPLACEFILE "$(BUILDDISK)" "/$(DISKVOLUME)/" "$@" -C
	@touch "$@"

mount: $(BUILDDISK)
	@open "$(BUILDDISK)"

clean:
	rm -rf "$(BUILDDIR)"

$(BUILDDIR):
	mkdir -p "$@"
	$(CADIUS) CREATEVOLUME "$(BUILDDISK)" "$(DISKVOLUME)" 800KB -C

all: clean mount

.NOTPARALLEL:
