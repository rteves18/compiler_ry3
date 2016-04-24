
# $Id: Makefile,v 1.11 2016-03-24 14:40:28-07 - - $
#Partner Name: Ryan Teves Username: rteves
#Partner Name: Matthew Kim Username: madkim

MKFILE     = Makefile
DEPFILE    = Makefile.dep
NOINCLUDE = ci clean spotless
NEEDINCL  = ${filter ${NOINCLUDE}, ${MAKECMDGOALS}}
GCC        = g++ -g -O0 -Wall -Wextra -std=gnu++14
MKDEP      = g++ -MM -std=gnu++14
VALGRIND   = valgrind --leak-check=full --show-reachable=yes
BISON      = bison --defines=${HYGEN} --output=${CYGEN}

SOURCES    = auxlib.cpp main.cpp stringset.cpp astree.cpp lyutils.cpp
HEADERS    = auxlib.h stringset.h astree.h lyutils.h
OTHERSRC   = README ${MKFILE} ${DEPFILE}
LSOURCES   = scanner.l
YSOURCES   = parser.y
CLGEN      = yylex.cc
HYGEN      = yyparse.h
CYGEN      = yyparse.cc
CGENS      = ${CLGEN} ${CYGEN}
ALLGENS    = ${HYGEN} ${CGENS}
EXECBIN    = oc
OBJECTS    = ${SOURCES:.cpp=.o} ${CGENS:.cc=.o}
ALLCSRC    = ${SOURCES} ${CGENS}
LREPORT    = yylex.output
YREPORT    = yyparse.output
REPORT     = ${LREPORT} ${YREPORT}
SRCFILES   = ${HEADERS} ${SOURCES} ${LSOURCES} ${YSOURCES} ${MKFILE}
SMALLFILES = ${DEPFILE} foo.oc foo1.oh foo2.oh
CHECKINS   = ${SRCFILES} ${SMALLFILES}
LISTING    = Listing.ps
ALLSRC     = ${OTHERSRC} ${YSOURCES} ${LSOURCES} ${HEADERS} ${SOURCES}
TESTINS    = ${wildcard test*.in}
EXECTEST   = ${EXECBIN} -ly
LISTSRC    = ${ALLSRC} ${HYGEN}

all : ${EXECBIN}

${EXECBIN} : ${OBJECTS}
	${GCC} -o${EXECBIN} ${OBJECTS}

${OBJECTS} : ${SOURCES} ${CGENS}
	${GCC} -c ${SOURCES}
	${GCC} -c ${CGENS}

${CLGEN} : ${LSOURCES}
		flex --outfile=${CLGEN} ${LSOURCES} 2>${LREPORT}
		- grep -v '^  ' ${LREPORT}

${CYGEN} ${HYGEN} : ${YSOURCES}
			bison --defines=${HYGEN} --output=${CYGEN} ${YSOURCES}

%.o : %.cpp
	${GCC} -c $<

ci :
	cid + ${CHECKINS}
	checksource ${CHECKINS}

lis : ${LISTSRC} tests
	mkpspdf List.source.ps ${LISTSRC}
	mkpspdf List.output.ps ${REPORT} \
		${foreach test, ${TESTINS:.in=}, \
		${patsubst %, ${test}.%, in out err log}}

clean :
	- rm ${OBJECTS} ${ALLGENS} ${REPORT} ${DEPFILE} core
	- rm ${foreach test, ${TESTINS:.in=}, \
		${patsubst %, ${test}.%, out err log}}

spotless : clean
	- rm ${EXECBIN} List.*.ps List.*.pdf

deps : ${ALLCSRC}
	${MKDEP} ${ALLCSRC} >>${DEPFILE}

${DEPFILE} :
	@ touch ${DEPFILE}
	${MAKE} --no-print-directory deps

test : ${EXECBIN}
	${VALGRIND} ${EXECBIN} foo.oc 1>test.out 2>test.err

%.out %.err : %.in
	${GRIND} --log-file=$*.log ${EXECTEST} $< 1>$*.out 2>$*.err; \
	echo EXIT STATUS = $$? >>$*.log

misc.lis : ${DEPFILE} foo.oc foo1.oh foo2.oh
	catnv ${DEPFILE} foo.oc foo1.oh foo2.oh >misc.lis

ifeq "${NEEDINCL}" ""
include ${DEPFILE}
endif