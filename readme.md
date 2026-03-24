# ESCH_PIXEL

This offers a dynamic visual of the 'twisting' of a zoom-periodic image that occurs in Escher's [Print Gallery](<https://en.wikipedia.org/wiki/Print_Gallery_(M._C._Escher)>). It has been inspired by [this](https://youtu.be/ldxFjLJ3rVY?si=-3u8K7pAUULMriUS) 3blue1brown lecture (which is based on [this](https://www.ams.org/notices/200304/fea-escher.pdf) article).

Only square pictures are supported. It has been tested only with 'small' pixel art pictures.

It works not only with a self-repeating image, but also with a number of 'nested' images, that are inserted in each other by the software. This makes it possible to achieve a significant 'zoom-period' using a number of images (all of the same size) with sane resolutions, instead of one huge picture. This applies in particular to pixel art, as in the examples.

You can load your own images changing the 'options' file (I'm to lazy to code any sort of gui... forgive me).

The displayed image is dinamically rendered using shaders, and can be changed:

- up and down arrow keys zoom in and out
- wasd change the real (a,d) and imaginary (w,s) part of the constant 'gamma' that regulates the 'twisting'
- c changes from the twisted mode to a standard self-repeating mode.

## Compile instructions

This software was built using version 5.5 of [raylib](https://github.com/raysan5/raylib).

On linux (where i have developed it), the easiest way to compile this is probably to download the raylib executable from [here](https://github.com/raysan5/raylib/releases/tag/5.5) and run something like

```
gcc main.c -L path/to/ray/lib -l:libraylib.a -lm -o esch
```

On other platforms, you should follow the instructions [here](https://github.com/raysan5/raylib).

## License

Both the code and the pictures are distributed under the zlib license (see LICENSE file).
