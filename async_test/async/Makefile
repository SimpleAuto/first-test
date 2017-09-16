CC     = gcc
WORKDIR=../
SRCDIR = $(WORKDIR)/async
LIBDIR = $(WORKDIR)/libserver
OBJDIR = $(WORKDIR)/obj
DEPDIR = $(WORKDIR)/obj
TARGET = $(WORKDIR)/async/AsyncServer
INCLUDEDIR = $(WORKDIR)/async

SRCS    = $(wildcard $(SRCDIR)/*.c)
HEADERS = $(wildcard $(INCLUDEDIR)/*.h)
OBJS   := $(patsubst %.c,%.o,$(subst $(SRCDIR),$(OBJDIR),$(SRCS)))
DEPS   := $(patsubst %.c,%.d,$(subst $(SRCDIR),$(DEPDIR),$(SRCS)))

CFLAGS = -Wall -Werror -DDEBUG -D_GNU_SOURCE -DEPOLL_MODE -DUSE_TLOG 
#CFLAGS = -Wall -DDEBUG -D_GNU_SOURCE -DEPOLL_MODE 
LIBS   = -lpthread -ldl

all: $(TARGET) clean

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

$(DEPDIR)/%.d: $(SRCDIR)/%.c
	@echo "generate $@"
	@printf "$(OBJDIR)/" > $@.tmp
	@$(CC) -MM $< $(CFLAGS) >> $@.tmp
	@mv $@.tmp $@

sinclude $(DEPS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c $< -o $@ $(CFLAGS) $(LIBS)

.PHONY:clean

clean:
	-rm $(OBJS) $(DEPS)

clean-targer:
	-rm $(TARGET)
