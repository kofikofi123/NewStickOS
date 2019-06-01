# NewStickOS 3.0
I have learned enough mistakes (hopefully), and decided to rewrite again.

Just a student learning

# NewStickOS 3.0 (2019)
New year modification, nothing has been tested, just compiled

# NewStickOS 3.0 (March 2019)
After about 3 months, I feel I have made some good (slow, and unimportant) progress.

# NewStickOS 3.0 (April 2019)
Decided to use acpica. Remodeled my make system. OS compiles on new compiler (i686-nos)

Additonal note (04/28/19) Modified bootloader to fix an issue of not being able to load a kernel greater than the capacity of a word

# NewStickOS 3.0 (May 2019)
Spent most of my time trying to track a bug in my implementation for acpica os service layer,
ended up being unimplementated override functions. Decided to take small breaks in between them.
ACPICA table management seems to be working now.

# NewStickOS TODOS:
#### TODO before [idk] (Not in order) (Could be shifted into other deadlnes):
- ###### Add 1 MB page allocaton and paging support (support for 3-level paging, etc.)
- ###### ~~Add a basic page allocator~~
- ###### ~~Add a basic (2-level) paging system~~
- ###### ~~Rewrite kernel heap allocator~~
- ###### Improve kernel heap allocator (should not be improved beyond basic skill)
- ###### Become ACPI compliant 
- ###### Add Interrupts
- ###### Support both APIC and Legacy PIC
- ###### Add code for GDT