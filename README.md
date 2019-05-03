Aran,

If the morse code output doesn't seem correct, I hope you can forgive my misunderstanding.
If you wish to give it a look over and you don't mind pointing out where I made any mistakes \*,
`src/led.c` is where you want to go for the morse code.

There's a function `blink_morse_command` which handles each character.

Note that in the loop (`src/user.c`), after `H_E_L_L_O W_O_R_L_D` is "printed" out, there's an
approximately 5 second delay before the next iteration
(note that `_` is used in the given string to denote an inter-character pause).

Also, for some reason, in the sleep() function (see `src/common.c`), 11 cycles seems to be
too much for a single iteration. Not sure why, but using `register` on the counter declaration
lowered it down to about 5-6 cycles per iteration. This seems to work.

Some quick bullets:

- `include/common.h` provides the basic definitions/typedefs used everywhere else.

- `sysctrl.h/.c` and `flashctrl.h/.c` exist, but they aren't used for this submission so you can safely ignore them.


Thanks.

\* As far as the morse code is concerned - if it's correct but the output isn't what we want,
then I at least know the morse code portion is right.

