PART1=aoc2023_day4_part1

all: $(PART1)

$(PART1): $(PART1).o
	@echo "CC $@"
	@$(CC) -O0 -ggdb -o $@ $^

%.o: %.c
	@echo "CC $@"
	@$(CC) -O0 -ggdb -c -o $@ $<

.PHONY: clean tags
clean:
	rm -rf $(PART1)
	rm -rf *.o

tags: 
	find . -name "*.[ch]" -exec ctags --append {} +

