CPP=$(HOME)/dartino/sdk/third_party/clang/linux/bin/clang++

#CFLAGS=--std=c++11 -g -O0 -Wall -Werror -fno-strict-aliasing -DDEBUG=1
CFLAGS=--std=c++11 -O3 -Wall -Werror -fno-strict-aliasing

HFILES=allocation.h assert.h builder.h globals.h hash_map.h hash_set.h hash_table.h list.h list_builder.h os.h pair.h parser.h pretty_printer.h scanner.h source.h string_buffer.h test_case.h tokens.h tree.h trie.h utils.h void_hash_table.h zone.h

OFILES=allocation.o assert.o builder.o os.o parser.o pretty_printer.o scanner.o source.o string_buffer.o tokens.o tree.o utils.o void_hash_table.o zone.o

TESTOFILES=assert_test.o builder_test.o globals_test.o hash_table_test.o list_test.o parser_test.o scanner_test.o test_case.o utils_test.o zone_test.o

%.o: %.cc $(HFILES) Makefile
	$(CPP) $(CFLAGS) -c -I.. $<

test: $(OFILES) $(TESTOFILES) Makefile
	$(CPP) $(CFLAGS) -o test $(TESTOFILES) $(OFILES)

clean:
	rm -f *.o
