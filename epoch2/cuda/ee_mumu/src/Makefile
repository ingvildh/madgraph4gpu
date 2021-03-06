INCFLAGS = -I.
OPTFLAGS = -O3
OMPFLAGS?= -fopenmp
CXXFLAGS = $(OPTFLAGS) -std=c++11 $(INCFLAGS) $(USE_NVTX) -Wall -Wshadow -Wextra $(OMPFLAGS) $(MGONGPU_CONFIG)
CXXFLAGS+= -ffast-math # see issue #117
###CXXFLAGS+= -Ofast # performance is not different from --fast-math
LIBDIR   = ../lib
LIBFLAGS = -L$(LIBDIR) -l$(MODELLIB)
CXX     ?= g++

# If CUDA_HOME is not set, try to set it from the location of nvcc
ifndef CUDA_HOME
  NVCC ?= $(shell which nvcc 2>/dev/null)
  ifneq ($(NVCC),)
    # NVCC is in the PATH or set explicitly
    CUDA_HOME  = $(patsubst %bin/nvcc,%,$(NVCC))
    CUDA_HOME := $(warning No CUDA_HOME exported. Using "$(CUDA_HOME)") $(CUDA_HOME)
  endif
endif

ifneq ($(wildcard $(CUDA_HOME)/bin/nvcc),)
  NVCC = $(CUDA_HOME)/bin/nvcc
  CUINC = -I$(CUDA_HOME)/include/
endif

# Enable ccache if USECCACHE=1
ifeq ($(USECCACHE)$(shell echo $(CXX) | grep ccache),1)
override CXX:=ccache $(CXX)
endif
ifeq ($(USECCACHE)$(shell echo $(AR) | grep ccache),1)
override AR:=ccache $(AR)
endif
ifeq ($(USECCACHE)$(shell echo $(NVCC) | grep ccache),1)
override NVCC:=ccache $(NVCC)
endif

# Assuming uname is available, detect if architecture is power
#UNAME_P := $(shell uname -p)
#ifeq ($(UNAME_P),ppc64le)
#  CUFLAGS+= -Xcompiler -mno-float128
#endif

target=$(LIBDIR)/libmodel_sm.a
cxx_objects=Parameters_sm.o read_slha.o rambo.o 
cu_objects= # NB grambo.cu must be included by gcheck.cu (no rdc)

all: $(target)

debug: OPTFLAGS = -g -O0 -DDEBUG2
#debug: CUFLAGS := $(filter-out -lineinfo,$(CUFLAGS))
#debug: CUFLAGS += -G
debug: $(target)

# NB: cuda includes are needed in the C++ code for curand.h
%.o : %.cc *.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(CUINC) -c $< -o $@

#%.o : %.cu *.h
#	$(NVCC) $(CPPFLAGS) $(CUFLAGS) -c $< -o $@

$(target): $(cxx_objects) #$(cu_objects)
	if [ ! -d $(LIBDIR) ]; then mkdir -p $(LIBDIR); fi
	$(AR) cru $@ $(cxx_objects) #$(cu_objects)
	ranlib $(target)

.PHONY: clean

clean:
	rm -f $(target)
	rm -f $(cxx_objects) $(cu_objects)
