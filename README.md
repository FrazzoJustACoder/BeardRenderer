# BeardRenderer
Just a random name for an yet another software 3D renderer, written just for fun.

I made it the more abstract I could, it supports different and variable screen sizes, bits-per-pixel formats (but only ones that are greater than or equal to 8 bits), pitch values (this makes splitscreen possible in the same buffer) and doesn't rely on almost anything (even allocations are left to the user). It can even render 3D models to a console!

I included some windows examples on how to use this library in "test" dir. "TODO make linux examples too"

If you want to include this library in your project, just run "make lib", then take libBeard.a file from bin\ and .h files from include\ (lib.h is just needed for compilation, actually). If you do so or if you take my code (partially or totally), and if you are kind enough, mention me in comments c:
