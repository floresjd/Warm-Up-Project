
SEGFAULT_ID=139


lab0: lab0.o
	$gcc -o lab0 lab0.o -Wall -Wextra -g

check:
	@echo "***Checking for Segfault"
	@./lab0 --segfault ; \
	if [ $$? -eq $$SEGFAULT_ID ];\
	then\
		@echo "Success in creating segfault...GOOD"\
	else\
		@echo "Did not succesfully create segfault...BAD"\
	fi

clean:
	rm -rf up  lab0.o lab0 lab0-404474130.tar.gz compare#lab0.c backtrace.png breakpoint.png README Makefile

dist:
	tar -zcvf lab0-404474130.tar.gz README lab0.c backtrace.png breakpoint.png Makefile runcheck


