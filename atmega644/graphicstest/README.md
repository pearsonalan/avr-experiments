
## Baseline graphics test on Arduino Uno

```
12:33:02.256 -> Using Adafruit 2.8" TFT Breakout Board Pinout
12:33:02.322 -> TFT size is 240x320
12:33:02.322 -> Found ILI9341 LCD driver
12:33:03.206 -> Benchmark                Time (microseconds)
12:33:03.239 -> Screen fill              1380000
12:33:05.087 -> Text                     356660
12:33:08.642 -> Lines                    3405764
12:33:23.533 -> Horiz/Vert Lines         144332
12:33:24.361 -> Rectangles (outline)     104224
12:33:25.217 -> Rectangles (filled)      3268568
12:33:29.312 -> Circles (filled)         922924
12:33:30.416 -> Circles (outline)        1478220
12:33:32.409 -> Triangles (outline)      735784
12:33:33.840 -> Triangles (filled)       1714192
12:33:37.062 -> Rounded rects (outline)  508636
12:33:38.280 -> Rounded rects (filled)   3419548
12:33:42.456 -> Done!
```

## Graphics test results using ATmega644pa @ 16Mhz

```
Benchmark                Time (microseconds)
Screen fill              243196
Text                     280412
Lines                    2761744
Horiz/Vert Lines         34760
Rectangles (outline)     32492
Rectangles (filled)      547548
Circles (filled)         527684
Circles (outline)        1205672
Triangles (outline)      589168
Triangles (filled)       769120
Rounded rects (outline)  375540
Rounded rects (filled)   688564
Done!
```

## Graphics test results using ATmega644pa @ 20Mhz

```
Benchmark                Time (microseconds)
Screen fill              182481
Text                     243486
Lines                    2458881
Horiz/Vert Lines         27720
Rectangles (outline)     26727
Rectangles (filled)      411336
Circles (filled)         454875
Circles (outline)        1073811
Triangles (outline)      524181
Triangles (filled)       620256
Rounded rects (outline)  332463
Rounded rects (filled)   534555
Done!
```

