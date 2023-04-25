NAME := ft_nm
NAME_DEBUG := $(NAME)_debug
CC := cc
CFLAGS := -Wall -Wextra -Werror -no-pie
LIBFTDIR := libft
LIBFT := $(LIBFTDIR)/libft.a
LIBFTINC := $(LIBFTDIR)/includes
LIBS := -L $(LIBFTDIR) -lft

# if make debug add -g
ifneq ($(filter debug, $(MAKECMDGOALS)),)
	CFLAGS += -g
endif

# Directories
SRCSDIR := srcs
OBJSDIR_RELEASE := .build/release/
OBJSDIR_DEBUG := .build/debug/
INCDIR := includes


SRCS += main.c \
		file.c \
		symbols_set.c 
		

OBJS_RELEASE := $(addprefix $(OBJSDIR_RELEASE)/, $(SRCS:.c=.o))
OBJS_DEBUG := $(addprefix $(OBJSDIR_DEBUG)/, $(SRCS:.c=.o))

DEPS_RELEASE := $(addprefix $(OBJSDIR_RELEASE)/, $(SRCS:.c=.d))
DEPS_DEBUG := $(addprefix $(OBJSDIR_DEBUG)/, $(SRCS:.c=.d))

DFLAGS	= -MMD -MF $(@:.o=.d)

all: $(NAME)

$(LIBFT):
	$(MAKE) -C $(LIBFTDIR)

$(NAME): $(LIBFT) $(OBJS_RELEASE)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS_RELEASE) $(LIBS)

$(NAME_DEBUG): $(LIBFT) $(OBJS_DEBUG)
	$(CC) $(CFLAGS) -o $(NAME_DEBUG) $(OBJS_DEBUG) $(LIBS)

debug: $(NAME_DEBUG)

-include $(DEPS_RELEASE)
$(OBJSDIR_RELEASE)/%.o: $(SRCSDIR)/%.c
	@mkdir -p $(OBJSDIR_RELEASE)
	$(CC) $(CFLAGS) -I $(INCDIR) -I $(LIBFTINC) $(DFLAGS) -c $< -o $@

-include $(DEPS_DEBUG)
$(OBJSDIR_DEBUG)/%.o: $(SRCSDIR)/%.c
	@mkdir -p $(OBJSDIR_DEBUG)
	$(CC) $(CFLAGS) -I $(INCDIR) -I $(LIBFTINC) $(DFLAGS) -c $< -o $@

clean:
	make -C $(LIBFTDIR) clean
	rm -rf .build

fclean: clean
	make -C $(LIBFTDIR) fclean
	rm -f $(NAME) $(NAME_DEBUG)

re: fclean all

redebug: fclean debug

.PHONY: all clean fclean re debug redebug