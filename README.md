# Summary

Using only C++ (and so far only 'ncurses' as any third party code)
rendering a FPS environment you can move around in (like the classics
Wolfenstein 3d or DOOM 1 and 2).

Primarily following [this youtube video](https://www.youtube.com/watch?v=xW8skO7MFYw&t)
as guidance in this project.

# How to build and run

* Build: ```make```
    * (you will have to install ncurses if you don't have it)
* Run: ```./a.out $(stty size)```
    * The reason we run it like this is so the screen size is set based
      on current size of your terminal window and your font size, which
      is information coming from the 'stty size' command.
    * You can also manually provide screen height and width like this:
      ```./a.out 40 80```, in this case we provided 40 height and 80 width.
      the unit represents the number of characters in each dimension.
      * !!! NOTE !!! if terminal window is not big enough or font size is to big to
        fit what screen size is set to, the rendering will look all messed up.
    * If you just run ```./a.out``` it will use hardcoded default values for
      screen size
