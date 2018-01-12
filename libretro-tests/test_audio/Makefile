all: libretro native

libretro:
	$(MAKE) -f Makefile.libretro

native:
	$(MAKE) -f Makefile.native

clean: clean-libretro clean-native

clean-libretro:
	$(MAKE) -f Makefile.libretro clean

clean-native:
	$(MAKE) -f Makefile.native clean