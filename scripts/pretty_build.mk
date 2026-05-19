# Converts any path to relative from TOPDIR (without realpath).
toprelpath = $(patsubst $(TOPDIR)/%,%,$(abspath $(1)))

define pretty_build
	@printf "  %-8s %s\n" "$(1)" "$(call toprelpath,$(2))"
endef
