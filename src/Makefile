CPP=$(HOME)/dartino/sdk/third_party/clang/linux/bin/clang++

CFLAGS=-g -O0 -Wall -Werror -fno-strict-aliasing

HFILES=allocation.h assert.h builder.h globals.h list.h list_builder.h os.h parser.h pretty_printer.h scanner.h source.h string_buffer.h test_case.h tokens.h tree.h trie.h utils.h zone.h

OFILES=allocation.o assert.o builder.o os.o parser.o pretty_printer.o scanner.o source.o string_buffer.o tokens.o tree.o utils.o zone.o

TESTOFILES=assert_test.o builder_test.o globals_test.o list_test.o parser_test.o scanner_test.o test_case.o utils_test.o zone_test.o

%.o: %.cc $(HFILES) Makefile
	$(CPP) $(CFLAGS) -c -I.. $<

test: $(OFILES) $(TESTOFILES) Makefile
	$(CPP) $(CFLAGS) -o test $(TESTOFILES) $(OFILES)

clean:
	rm -f *.o
