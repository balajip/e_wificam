# makefile for installing Lua
# see INSTALL for installation instructions
# see src/Makefile and src/luaconf.h for further customization

# == CHANGE THE SETTINGS BELOW TO SUIT YOUR ENVIRONMENT =======================

# Your platform. See PLATS for possible values.
PLAT= linux

# Where to install. The installation starts in the src directory, so take care
# if INSTALL_TOP is not an absolute path. (Man pages are installed from the
# doc directory.) You may want to make these paths consistent with LUA_ROOT,
# LUA_LDIR, and LUA_CDIR in luaconf.h (and also with etc/lua.pc).
#
INSTALL_TOP= /usr/local
INSTALL_BIN= $(INSTALL_TOP)/bin
INSTALL_INC= $(INSTALL_TOP)/include
INSTALL_LIB= $(INSTALL_TOP)/lib
INSTALL_MAN= $(INSTALL_TOP)/man/man1
INSTALL_LMOD= $(INSTALL_TOP)/share/lua/$V
INSTALL_CMOD= $(INSTALL_TOP)/lib/lua/$V

# How to install. You may prefer "install" instead of "cp" if you have it.
# To remove debug information from binaries, use "install -s" in INSTALL_EXEC.
#
INSTALL_EXEC= $(CP)
INSTALL_DATA= $(CP)
#INSTALL_EXEC= $(INSTALL) -m 0755
#INSTALL_DATA= $(INSTALL) -m 0644

# Utilities.
CP= cp
FIND= find
INSTALL= install
MKDIR= mkdir
RANLIB= ranlib

# == END OF USER SETTINGS. NO NEED TO CHANGE ANYTHING BELOW THIS LINE =========

# Convenience platforms targets.
PLATS= aix ansi bsd freebsd generic linux macosx mingw posix solaris

# What to install.
TO_BIN= lua luac
TO_INC= lua.h luaconf.h lualib.h lauxlib.h ../etc/lua.hpp
TO_LIB= liblua.a
TO_MAN= lua.1 luac.1

# Lua version and release.
V= 5.1
R= 5.1.2

all: lualib
	
lualib:
	cd src; make $(PLAT)

clean:
	cd src && $(MAKE) $@

# list targets that do not create files (but not all makes understand .PHONY)
.PHONY: all $(PLATS) clean test install local none dummy echo pecho lecho

# (end of Makefile)
