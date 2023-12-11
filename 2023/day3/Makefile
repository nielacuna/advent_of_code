PARTS=aoc2023_day3_part1 \
      aoc2023_day3_part2

all: $(PARTS)

aoc2023_day3_part%: aoc2023_day3_part%.o
	@echo "CC $@"
	@$(CC) -O0 -ggdb -o $@ $^

%.o: %.c
	@echo "CC $@"
	@$(CC) -O0 -ggdb -c -o $@ $<

.PHONY: clean tags
clean:
	rm -rf $(PARTS)
	rm -rf *.o

tags: 
	find . -name "*.[ch]" -exec ctags --append {} +

