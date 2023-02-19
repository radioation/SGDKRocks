# NOTES
The ["ShmupBoss](https://github.com/radioation/SGDKRocks/tree/main/ShmupBoss) example was originally meant to have two separate rotations occuring in `BG_A`.   Dual rotation did work, but started to stutter after adding serveral sprites.  So I removed the second rotation from `BG_A`.  

This example fixes the stutter by optimizing the code and moving sprites to SGDK's low level sprite API.
