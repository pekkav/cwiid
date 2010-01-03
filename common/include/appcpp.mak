###
#   Copyright (C) 2009  Pekka Vanhoja <pekka.vanhoja@gmail.com>
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.
##

OBJECTS = $(SOURCES:.cpp=.o)
DEPS    = $(SOURCES:.cpp=.d)

INST_DIR ?= /usr/local/bin

DEST_INST_DIR = $(ROOTDIR)$(INST_DIR)

all: $(APP_NAME)

$(APP_NAME): $(OBJECTS)
	$(CPP) -o $@ $(OBJECTS) $(LDFLAGS) $(LDLIBS)

install: $(APP_NAME)
	install -D $(APP_NAME) $(DEST_INST_DIR)/$(APP_NAME)

clean:
	rm -f $(APP_NAME) $(OBJECTS) $(DEPS)

uninstall:
	rm -f $(DEST_INST_DIR)/$(APP_NAME)

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distclean)
include $(COMMON)/include/depcpp.mak
-include $(DEPS)
endif
endif

.PHONY: all install uninstall clean
