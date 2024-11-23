# 
# Author: turrnut
# Copyrighted (c) turrnut under the MIT license
# ir.h
#

source_files = src/toy.c src/compiler/compiler.c src/compiler/lexer.c src/debug/debug.c src/ir/ir.c src/runtime/runtime.c src/security/memory.c src/types/object.c src/types/table.c src/types/value.c
output_binary = bin/emi

install-packages:
	sudo apt-get update
	sudo apt-get upgrade
	sudo apt-get install gcc

build:
	mkdir bin || echo
	gcc $(source_files) -o $(output_binary)

