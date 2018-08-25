# BeardRenderer
Just a random name for an yet another software renderer, written just for fun.

I made it the more abstract I could, it supports different and variable screen sizes, bits-per-pixel formats (but only ones that are greater than or equal to 8 bits), pitch values (this makes splitscreen possible in the same buffer) and doesn't rely on almost anything (even allocations are left to the user)
