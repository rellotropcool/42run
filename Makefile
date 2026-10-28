.PHONY: all configure build run clean

all: build

configure:
	cmake -S . -B build

build: configure
	cmake --build build -j

run: build
	./build/42run

clean:
	rm -rf build