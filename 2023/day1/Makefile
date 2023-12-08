TARGET=aoc_2023_day1

all: $(TARGET)

$(TARGET): $(TARGET).o
	@echo "CC $@"
	@$(CC) -O0 -ggdb -o $@ $^

%.o: %.c
	@echo "CC $@"
	@$(CC) -O0 -ggdb -c -o $@ $<

.PHONY: clean
clean:
	rm -rf $(TARGET)
	rm -rf $(TARGET).o
