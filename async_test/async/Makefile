CC     = gcc
WORKDIR=../
SRCDIR = $(WORKDIR)/async
LIBDIR = $(WORKDIR)/libserver
OBJDIR = $(WORKDIR)/obj
DEPDIR = $(WORKDIR)/obj
TARGET = $(WORKDIR)/async/async
INCLUDEDIR = $(WORKDIR)/async

SRCS    = $(wildcard $(SRCDIR)/*.c)
HEADERS = $(wildcard $(INCLUDEDIR)/*.h)
OBJS   := $(patsubst %.c,%.o,$(subst $(SRCDIR),$(OBJDIR),$(SRCS)))
DEPS   := $(patsubst %.c,%.d,$(subst $(SRCDIR),$(DEPDIR),$(SRCS)))

CFLAGS = -Wall -Werror -DDEBUG -D_GNU_SOURCE -DEPOLL_MODE 

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS)

$(DEPDIR)/%.d: $(SRCDIR)/%.c
	@echo "generate $@"
	@printf "$(OBJDIR)/" > $@.tmp
	@$(CC) -MM $< $(CFLAGS) >> $@.tmp
	@mv $@.tmp $@

sinclude $(DEPS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	-rm $(OBJS) $(TARGET) $(DEPS)

clean-targer:
	-rm $(TARGET)
