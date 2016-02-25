
# note: build order dependency: ch14 -> ch13

DIRS=	lib \
	ch1  ch2  ch3  ch4  ch5  ch6  ch7  ch8  ch9  ch10 \
	ch11 ch12 ch14 ch13 ch15 ch16 ch17 ch18 ch19 ch20 \
	ch16+

all:
	(for X in $(DIRS); do make -C $$X all || return 1 ; done)

cleanobj:
	(for X in $(DIRS); do make -C $$X cleanobj ; done)

clean:
	(for X in $(DIRS); do make -C $$X clean ; done)

