project_name = LEDCubeDriver

compiler_settings = -Wall -O3
libraries = -lpthread -lwiringPi

source_files = main.c illuminator.c communicator.c


all: release

release: clean
	gcc -o$(project_name) $(compiler_settings) $(libraries) $(source_files)

clean:
	rm $(project_name) || true
