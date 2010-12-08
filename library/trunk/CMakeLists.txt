# cmake build file for Mathomatic, originally contributed by Jonathan Stark.
# Produces the normal version of Mathomatic without readline support.
# Do not use unless you wish create a project with cmake.

cmake_minimum_required(VERSION 2.6)

project(mathomatic)

file(READ VERSION FV)
string(STRIP ${FV} MATHOMATIC_VERSION)
message(STATUS VERSION: ${MATHOMATIC_VERSION})
add_definitions(-DUNIX -DVERSION="${MATHOMATIC_VERSION}")

add_executable(mathomatic
	altproto.h
	complex.c	
	externs.h
	globals.c
	license.h
	poly.c
	super.c
	am.c
	complex.h
	factor.c
	help.c	
	list.c	
	proto.h	
	unfactor.c
	am.h
	complex_lib.c
	factor_int.c
	includes.h
	main.c	
	simplify.c
	cmds.c	
	diff.c	
	gcd.c	
	integrate.c
	parse.c	
	solve.c
)

target_link_libraries(mathomatic -lm)
