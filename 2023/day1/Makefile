PART1=aoc2023_day1_part1
PART2=aoc2023_day1_part2

all: $(PART1) $(PART2)

$(PART1): $(PART1).o
	@echo "CC $@"
	@$(CC) -O0 -ggdb -o $@ $^

$(PART2): $(PART2).o
	@echo "CC $@"
	@$(CC) -O0 -ggdb -o $@ $^

%.o: %.c
	@echo "CC $@"
	@$(CC) -O0 -ggdb -c -o $@ $<

.PHONY: clean tags
clean:
	rm -rf $(PART1) $(PART2)
	rm -rf *.o

tags: 
	find . -name "*.[ch]" -exec ctags --append {} +

