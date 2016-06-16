# IsletModel

This version is in the middle of an overhaul. It currently works and
produces output that has been validated against the latest Dropbox
version. The janus-build script is not up to date, and won't work. 
However, it can easily be modified to run accurate simulations.

The overhaul has several goals:
1.	Convert the program from procedural to objective
		a. This will allow new modules to be added painlessly, and old
			modules to be modified without compromising current code.
		b. It will also allow a fairly simple integration with other code
			components, such as sphere packing, islet generation, and
			random number generation.
2. Make a dynamic user input system that automates output paths
	and provides documentation based on runtime parameters.
		a. This is mostly to free up user brainpower to focus on more
			important things, like what they're going to simulate, and
			what the data means.
		b. It will also allow the user to automatically run a fairly complex
			series of simulations without a bunch of manual setup.
3. Separate the code from the executable.
		a. The final version will include a build that can be run as-is
			on Janus, and won't need code modifications and
			re-compilation when the user wants to run a new simulations.
			
			
			
To build run it, go make sure cygwin is installed with gcc packages.
Download Boost and put it in your cygwin home directory (V.54 
works, but there have been problems with other versions). Update
the cygwin_build.sh script to reflect path changes. Finally, run it
from the cygwin terminal with "./cygwin_build.sh" from the code
directory.

Future versions may not build in cygwin, unfortunately. That's
because there are compatibility issues in cygwin when it links to
certain boost libraries, including the ones that I'll be using for
automatic file-system management.

- WLF