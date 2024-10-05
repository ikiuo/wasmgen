# --------------
# GNUmakefile
# --------------

-include GNUmakefile.config

# --------------

prefix       ?= /usr/local
BINDIR        = $(prefix)/bin

# --------------

CXX           = clang++

CXXDEFS       =
CXXARCH       =
CXXSTD       ?= -std=c++14

CXXWARNINGS   = -W -Wall -Wextra

ifdef DEBUG
CXXLTO        =
CXXOPTIMIZES  = -O0
CXXDEBUGS     = -g3 -DDEBUG=1
else
CXXLTO        = -flto
CXXOPTIMIZES  = -O3 $(CXXLTO)
CXXDEBUGS     = -g0 -DNDEBUG=1
endif

CXXDEPS      := -MD

CXXFLAGS      = $(CXXDEFS) \
		$(CXXARCH) \
		$(CXXSTD) \
		$(CXXOPTIMIZES) \
		$(CXXDEBUGS) \
		$(CXXWARNINGS) \
		$(CXXDEPS)

# --------------

LD            = $(CXX)

LDLTO         = $(CXXLTO)

LDFLAGS       = $(LDLTO)

# --------------

SRCS          = main.cpp wmain.cpp option.cpp error.cpp \
		object.cpp string.cpp file.cpp \
		token.cpp expression.cpp instruction.cpp \
		data.cpp section.cpp module.cpp \
		lexer.cpp parser.cpp \
		$(END_SRCS)

DEPS          = $(SRCS:.cpp=.d)

OBJS          = $(SRCS:.cpp=.o)

LIBS          =

PROGRAM       = wasmgen

# --------------

RM            = rm
RMFLAGS       = -f
RMFILES       = $(PROGRAM) $(OBJS) *.[dos] *~

# --------------

.PHONY:		all clean
.PHONY:		build build_program

.SUFFIXES:	.cpp .o .s

.cpp.s:
		$(CXX) $(CXXFLAGS) -S $<

# --------------

all:		build

build:		$(PROGRAM)

install:	$(PROGRAM)
		install -m 755 -D $(BINDIR) $(PROGRAM)

clean:
		$(RM) $(RMFLAGS) $(RMFILES)

# --------------

$(PROGRAM):	$(OBJS)
		$(LD) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

# --------------

-include GNUmakefile.local
-include $(DEPS)

# --------------
