
CROSS_COMPILE ?= arm-linux-
CC	= $(CROSS_COMPILE)gcc
CPP	= $(CROSS_COMPILE)cpp
AS	= $(CROSS_COMPILE)as
LD	= $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
export CC CPP AS LD OBJCOPY OBJDUMP

# CPU architecture
CPU = $(subst --with-tune=,,$(filter --with-tune=%,$(shell $(CC) -v 2>&1)))
export CPU

c_flags = -Wp,-MD,$(depfile),-MT,$(@) -I$(dir $@) -I$(CURDIR) -I$(srctree) $(_c_flags)
ASFLAGS = --noexecstack,-mcpu=$(CPU),-EL,--MD,$(depfile-tmp)
#ASFLAGS = --noexecstack,-mcpu=$(CPU),-EL
LDFLAGS += -nostdlib -Bstatic -T $(lds)
export c_flags ASFLAGS


###################### from Linux kernel kbuild ###############################

##### Makefile #####

# Beautify output
# ---------------------------------------------------------------------------
#
# Normally, we echo the whole command before executing it. By making
# that echo $($(quiet)$(cmd)), we now have the possibility to set
# $(quiet) to choose other forms of output instead, e.g.
#
#         quiet_cmd_cc_o_c = Compiling $(RELDIR)/$@
#         cmd_cc_o_c       = $(CC) $(c_flags) -c -o $@ $<
#
# If $(quiet) is empty, the whole command will be printed.
# If it is set to "quiet_", only the short version will be printed.
# If it is set to "silent_", nothing will be printed at all, since
# the variable $(silent_cmd_cc_o_c) doesn't exist.
#
# A simple variant is to prefix commands with $(Q) - that's useful
# for commands that shall be hidden in non-verbose mode.
#
#	$(Q)ln $@ :<
#
# If KBUILD_VERBOSE equals 0 then the above command will be hidden.
# If KBUILD_VERBOSE equals 1 then the above command is displayed.

ifdef V
  ifeq ("$(origin V)", "command line")
    KBUILD_VERBOSE = $(V)
  endif
endif
ifndef KBUILD_VERBOSE
  KBUILD_VERBOSE = 0
endif

ifeq ($(KBUILD_VERBOSE),1)
  quiet =
  Q =
else
  quiet=quiet_
  Q = @
endif

# If the user is running make -s (silent mode), suppress echoing of
# commands

ifneq ($(findstring s,$(MAKEFLAGS)),)
  quiet=silent_
endif

export quiet Q KBUILD_VERBOSE




##### Kbuild.include #####

# Convenient variables
comma   := ,
squote  := '
empty   :=
space   := $(empty) $(empty)

###
# Name of target with a '.' as filename prefix. foo/bar.o => foo/.bar.o
dot-target = $(dir $@).$(notdir $@)

###
# The temporary file to save gcc -MD generated dependencies must not
# contain a comma
depfile = $(subst $(comma),_,$(dot-target).d)
depfile-tmp = $(subst $(comma),_,$(dot-target).d.tmp)

###
# Escape single quote for use in echo statements
escsq = $(subst $(squote),'\$(squote)',$1)





##### Makefile.build #####

# C (.c) files
# ---------------------------------------------------------------------------
# The C file is compiled and updated dependency information is generated.
# (See cmd_cc_o_c + relevant part of rule_cc_o_c)

quiet_cmd_cc_o_c = CC     $@
      cmd_cc_o_c = $(CC) $(c_flags) -c -o $@ $<

define rule_cc_o_c
	$(call echo-cmd,cc_o_c) $(cmd_cc_o_c);				  \
	echo 'cmd_$@ := $(make-cmd)' > $(dot-target).cmd
endef

# Built-in and composite module parts
#$(obj)/%.o: $(src)/%.c FORCE
%.o: %.c FORCE
	$(call if_changed_rule,cc_o_c)


# Compile assembler sources (.S)
# ---------------------------------------------------------------------------

quiet_cmd_as_s_S = CPP  $@
#cmd_as_s_S       = $(CPP) $(a_flags)   -o $@ $<
cmd_as_s_S       = $(CPP) -Wa,$(ASFLAGS) $(c_flags) -o $@ $<

#$(obj)/%.s: $(src)/%.S FORCE
#	$(call if_changed_dep,as_s_S)
%.s: %.S FORCE
	$(call if_changed,as_s_S)

quiet_cmd_as_o_S = AS     $@
#     cmd_as_o_S = $(CC) -Wa,$(ASFLAGS) $(c_flags) -c -o $@ $<
# for dependency of directive ".incbin"
      cmd_as_o_S = $(CC) -Wa,$(ASFLAGS) $(c_flags) -c -o $@ $<; \
                   sed -n -e 's/[a-zA-Z0-9\/]*\.s//g' -e 'p' $(depfile-tmp) >> $(depfile); rm -f $(depfile-tmp)

%.o: %.S FORCE
	$(call if_changed,as_o_S)

# Linker scripts preprocessor (.lds.S -> .lds)
# ---------------------------------------------------------------------------
quiet_cmd_cpp_lds_S = LDS    $@
      cmd_cpp_lds_S = $(CPP) $(c_flags) -D__ASSEMBLY__ -Wp,-P -o $@ $<

%.lds: %.lds.S FORCE
	$(call if_changed,cpp_lds_S)

# Build the compiled-in targets
# ---------------------------------------------------------------------------

#
# Rule to compile a set of .o files into one .o file
#
quiet_cmd_link_o_target = GEN    $@.bin
# If the list of objects to link is empty, just create an empty built-in.o
cmd_link_o_target = $(LD)  $(LDFLAGS) $(obj-y) -Map $(@).map -o $@; \
	$(OBJCOPY) -O binary $(@) $(@).bin; \
	$(OBJDUMP) -D $@ > $@.ds

# Add FORCE to the prequisites of a target to force it to be always rebuilt.
FORCE:
PHONY += FORCE


# Read all saved command lines and dependencies for the $(targets) we
# may be building above, using $(if_changed{,_dep}). As an
# optimization, we don't need to read them if the target does not
# exist, we will rebuild anyway in that case.

targets += $(TARGETS) $(obj-y) $(lds)
targets := $(wildcard $(sort $(targets)))

dep_files := $(wildcard $(foreach f,$(targets),$(dir $(f)).$(notdir $(f)).d))

ifneq ($(dep_files),)
  include $(dep_files)
endif

cmd_files := $(wildcard $(foreach f,$(targets),$(dir $(f)).$(notdir $(f)).cmd))

ifneq ($(cmd_files),)
  include $(cmd_files)
endif

# Declare the contents of the .PHONY variable as phony.  We keep that
# information in a variable se we can use it in if_changed and friends.

.PHONY: $(PHONY)

# echo command.
# Short version is used, if $(quiet) equals `quiet_', otherwise full one.
echo-cmd = $(if $($(quiet)cmd_$(1)),\
	echo '  $(call escsq,$($(quiet)cmd_$(1)))$(echo-why)';)

###
# if_changed      - execute command if any prerequisite is newer than
#                   target, or command line has changed
# if_changed_dep  - as if_changed, but uses fixdep to reveal dependencies
#                   including used config symbols
# if_changed_rule - as if_changed but execute rule instead
# See Documentation/kbuild/makefiles.txt for more info

ifneq ($(KBUILD_NOCMDDEP),1)
# Check if both arguments has same arguments. Result is empty string if equal.
# User may override this check using make KBUILD_NOCMDDEP=1
arg-check = $(strip $(filter-out $(cmd_$(1)), $(cmd_$@)) \
                    $(filter-out $(cmd_$@),   $(cmd_$(1))) )
endif

# >'< substitution is for echo to work,
# >$< substitution to preserve $ when reloading .cmd file
# note: when using inline perl scripts [perl -e '...$$t=1;...']
# in $(cmd_xxx) double $$ your perl vars
make-cmd = $(subst \#,\\\#,$(subst $$,$$$$,$(call escsq,$(cmd_$(1)))))

# Find any prerequisites that is newer than target or that does not exist.
# PHONY targets skipped in both cases.
any-prereq = $(filter-out $(PHONY),$?) $(filter-out $(PHONY) $(wildcard $^),$^)

# Execute command if command has changed or prerequisite(s) are updated.
#
if_changed = $(if $(strip $(any-prereq) $(arg-check)),                       \
	@set -e;                                                             \
	$(echo-cmd) $(cmd_$(1));                                             \
	echo 'cmd_$@ := $(make-cmd)' > $(dot-target).cmd)

# Execute the command and also postprocess generated .d dependencies file.
if_changed_dep = $(if $(strip $(any-prereq) $(arg-check) ),                  \
	@set -e;                                                             \
	$(echo-cmd) $(cmd_$(1));                                             \
	scripts/basic/fixdep $(depfile) $@ '$(make-cmd)' > $(dot-target).tmp;\
	rm -f $(depfile);                                                    \
	mv -f $(dot-target).tmp $(dot-target).cmd)

# Usage: $(call if_changed_rule,foo)
# Will check if $(cmd_foo) or any of the prerequisites changed,
# and if so will execute $(rule_foo).
if_changed_rule = $(if $(strip $(any-prereq) $(arg-check) ),                 \
	@set -e;                                                             \
	$(rule_$(1)))
