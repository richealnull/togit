PROGRAM := multicaster 
OBJDIR	:= obj
SRCEXTS := .cpp
CPPFLAGS := -DNOLOG -g -Wall -std=c++0x
CFLAGS :=
CFLAGS +=
LDFLAGS := -lrt
LDFLAGS += -lm
LDFLAGS +=-L-luuid
LDFLAGS += -lpthread
LDFLAGS += -Wl,-Map,mapfile
LDFLAGS += -g
CXX = g++
RM = rm -rf

SHELL = /bin/sh
SOURCES = $(shell find ./ -name "*.cpp")
OBJS = $(foreach x,$(SRCEXTS), $(patsubst ./%$(x),$(OBJDIR)/%.o,$(filter %$(x),$(SOURCES))))
OBJDIRS	= $(sort $(dir $(OBJS)))
DEPS = $(patsubst %.o,%.d,$(OBJS))

.PHONY : all clean 

all : $(PROGRAM)

include $(DEPS)
$(OBJDIR)/%.d : %.cpp
	mkdir -p obj
	$(CXX) -o $@  -std=c++0x -MM -MD -MT '$(OBJDIR)/$(patsubst %.cpp,%.o,$<)' $<
$(OBJDIR)/%.o : %.cpp
	$(CXX) -o $@ -c $(CPPFLAGS) $<
$(PROGRAM) : $(OBJS)
	$(CXX) -o $(PROGRAM) $(OBJS) $(LDFLAGS)

clean :
	$(RM) $(OBJDIR)/* $(PROGRAM) mapfile
