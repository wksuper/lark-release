# ![logo](./examples/logo.png)

[English](https://gitee.com/wksuper/lark-release/blob/master/README.md) | [简体中文](https://gitee.com/wksuper/lark-release/blob/master/README-cn.md)

## Introduction

***lark*** is a free, lite and powerful software audio DSP. It provides a flexible and scalable way to design audio route(s) with high efficiency, small footprint and low latency that enables you to build your audio system like building blocks.

### Main Features

- Support realtime manipulating audio routes
  - Load/Unload blocks in real time
  - Change routes in real time
  - Tune block parameters in real time
- Support prebuilt I/O blocks
  - file-reader, file-writer, stream-in, stream-out, alsa-capture, alsa-playback, tinyalsa-capture, tinyalsa-playback, portaudio-capture, portaudio-playback
- Support prebuilt algorithm blocks
  - gain, mixer, duplicator, multiplexer, interleave, de-interleave, format-adapter, delay, align, buffer, fade-in, fade-out
  - speexdsp algo: speex-resampler, speex-preprocessor
  - soundtouch algo: pitch, tempo, rate
  - sox-effects: vol, flanger, tremolo, echos, chorus, oops, equalizer, highpass, lowpass, etc.
- Support customizing blocks extensively
- Support up-to 32 input endpoints and 32 output endpoints for each block
- Support timestamp for each frame / each sample
- Support realtime debug tool
  - Print status
  - Print routes snapshot to a file
  - Change log level
  - Dump log to a file
  - Dump each block's audio data to files
- Cross platform
  - Linux (x86_64), MacOS (x86_64), Android (aarch64)

## Get Started

***lark*** is composed by a basic shared library `liblark.so`, various optional blocks(dynamic load libraries) e.g. `libblkmixer.so`, and an optional executable debug utility `lkdb`. Based on ***lark***, you can easily create either simple or complex audio route(s) as you want, start/stop any route in realtime, or set/get parameters to/from any block in realtime.

### Step 1

Since ***lark*** depends on the klogging library, you should install klogging at first. Go to <https://gitee.com/wksuper/klogging> to get klogging installed. Alternatively the prebuilt `libklogging.so` is also available in this repo. It will be installed along with lark installation.

### Step 2

Install ***lark*** on your Linux machine.

```bash
$ cd lark-release
$ sudo ./install.sh
$ sudo ldconfig
```

You can use `sudo ./uninstall.sh` to remove ***lark*** when you don't need it.

### Step 3

Run audio route(s).

#### Example 1: Run a single route for stereo music file playback

```
RouteA

  libblkfilereader.so     libblkalsaplayback.so
  *******************      ******************
  *                 *      *                *
  *  filereader_0   *0--->0* alsaplayback_0 *
  *                 *      *                *
  *******************      ******************
```

```bash
$ x86_64-linux-gnu/bin/larkexample1
```

If no error, the music file should be started to play.

The source code of this example is shown in [larkexample1.cpp](https://gitee.com/wksuper/lark-release/blob/master/examples/larkexample1.cpp).

#### Example 2: Run a single route for music playback where two files input are mixed with separate gain

```
RouteA

 libblkfilereader.so   libblkdeinterleave.so   libblkgain.so           libblkmixer.so       libblkinterleave.so     libblkpaplayback.so

  *****************     ******************     *************           **************       *******************      ******************
  *               *     *                *0-->0*           *0-------->0*            *0---->0*                 *      *                *
  * filereader_0  *0-->0* deinterleave_0 *1-->1*           *1---+      *   mixer_0  *       *   interleave_0  *0--->0*  paplayback_0  *
  *               *     *                *     *           *    | +-->1*            *   +->1*                 *      *                *
  *****************     ******************     *           *    | |    **************   |   *******************      ******************
                                               *   gain_0  *    | |                     |
  *****************     ******************     *           *    | |    **************   |
  *               *     *                *     *           *    +-|-->0*            *0--+
  * filereader_1  *0-->0* deinterleave_0 *0-->2*           *2-----+    *   mixer_1  *
  *               *     *                *1-->3*           *3-------->1*            *
  *****************     ******************     *************           **************
```

To run example2, the ***PortAudio*** library is needed first.

```bash
$ sudo apt install libportaudio2
```

Run example2:

```bash
$ x86_64-linux-gnu/bin/larkexample2
```

If no error, the two files mixed music should be started to play.

Launch another shell where you can adjust their volume gains via `lkdb`.

```bash
$ lkdb status     # Shows lark status
RouteA is RUNNING, 319 frames processed OK, 0 error frame
	blkmixer_1
		(I00) <-- lnk_7    48000Hz   S16_LE  1ch    960samples/frame
		(I01) <-- lnk_9    48000Hz   S16_LE  1ch    960samples/frame
		(O00) --> lnk_11   48000Hz   S16_LE  1ch    960samples/frame
	blkdeinterleave_1
		(I01) <-- lnk_3    48000Hz   S16_LE  2ch    960samples/frame
		(O00) --> lnk_4    48000Hz   S16_LE  1ch    960samples/frame
		(O01) --> lnk_5    48000Hz   S16_LE  1ch    960samples/frame
	blkmixer_0
		(I00) <-- lnk_6    48000Hz   S16_LE  1ch    960samples/frame
		(I01) <-- lnk_8    48000Hz   S16_LE  1ch    960samples/frame
		(O00) --> lnk_10   48000Hz   S16_LE  1ch    960samples/frame
	blkinterleave_0
		(I00) <-- lnk_10   48000Hz   S16_LE  1ch    960samples/frame
		(I01) <-- lnk_11   48000Hz   S16_LE  1ch    960samples/frame
		(O00) --> lnk_12   48000Hz   S16_LE  2ch    960samples/frame
	blkfilereader_0
		(O00) --> lnk_0    48000Hz   S16_LE  2ch    960samples/frame
	blkdeinterleave_0
		(I00) <-- lnk_0    48000Hz   S16_LE  2ch    960samples/frame
		(O00) --> lnk_1    48000Hz   S16_LE  1ch    960samples/frame
		(O01) --> lnk_2    48000Hz   S16_LE  1ch    960samples/frame
	blkfilereader_1
		(O00) --> lnk_3    48000Hz   S16_LE  2ch    960samples/frame
	blkgain_0
		(I00) <-- lnk_1    48000Hz   S16_LE  1ch    960samples/frame
		(I01) <-- lnk_2    48000Hz   S16_LE  1ch    960samples/frame
		(I02) <-- lnk_4    48000Hz   S16_LE  1ch    960samples/frame
		(I03) <-- lnk_5    48000Hz   S16_LE  1ch    960samples/frame
		(O00) --> lnk_6    48000Hz   S16_LE  1ch    960samples/frame
		(O01) --> lnk_7    48000Hz   S16_LE  1ch    960samples/frame
		(O02) --> lnk_8    48000Hz   S16_LE  1ch    960samples/frame
		(O03) --> lnk_9    48000Hz   S16_LE  1ch    960samples/frame
	blkpaplayback_0
		(I00) <-- lnk_12   48000Hz   S16_LE  2ch    960samples/frame

No fifo
```

```bash
$ lkdb setparam RouteA blkgain_0 1 0 0.5    # Output volume of kanr-48000_16_2.pcm left channel should be lower
$ lkdb setparam RouteA blkgain_0 1 1 0.5    # Output volume of kanr-48000_16_2.pcm right channel should be lower
$ lkdb setparam RouteA blkgain_0 1 0 0.0    # Output volume of kanr-48000_16_2.pcm left channel should be muted
$ lkdb setparam RouteA blkgain_0 1 1 0.0    # Output volume of kanr-48000_16_2.pcm right channel should be muted

$ lkdb setparam RouteA blkgain_0 1 2 0.5    # Output volume of pacificrim-48000_16_2.pcm left channel should be lower
$ lkdb setparam RouteA blkgain_0 1 3 0.5    # Output volume of pacificrim-48000_16_2.pcm right channel should be lower
$ lkdb setparam RouteA blkgain_0 1 2 0.0    # Output volume of pacificrim-48000_16_2.pcm left channel should be muted
$ lkdb setparam RouteA blkgain_0 1 3 0.0    # Output volume of pacificrim-48000_16_2.pcm right channel should be muted

$ lkdb setparam RouteA blkgain_0 1 0 1.0    # Output volume of kanr-48000_16_2.pcm left channel should be recovered back
$ lkdb setparam RouteA blkgain_0 1 1 1.0    # Output volume of kanr-48000_16_2.pcm right channel should be recovered back

$ lkdb setparam RouteA blkgain_0 1 2 1.0    # Output volume of pacificrim-48000_16_2.pcm left channel should be recovered back
$ lkdb setparam RouteA blkgain_0 1 3 1.0    # Output volume of pacificrim-48000_16_2.pcm right channel should be recovered back
```

The source code of this example is shown in [larkexample2.cpp](https://gitee.com/wksuper/lark-release/blob/master/examples/larkexample2.cpp).

#### Example 3: Run dual routes, one for music playback, one for microphone capture with keyword detection

```
                                                                                                     ****************      ******************      ***************
                                                                                        RouteA       *              *      *                *      *             *
                                                                                                     * filereader_0 *0--->0* alsaplayback_0 *0--->0* streamout_0 *
                                                                                                     *              *      *                *      *             *
                                                                                                     ****************      ******************      ********|******
                                                                                                                                                           v
========================================================================================================================================================[FIFO]=====
                                                                                                                                                         |
  **************     **************     *************     ***********************     ******************     ***********     *****************      *****v********
  *            *     *            *     *           *     *                     *     *                *     *         *     *               *      *            *
  * dummykwd_0 *0<--0*            *     *           *     *                     *0<--0* deinterleave_0 *0<--0*         *0<--0* alsacapture_0 *      * streamin_0 *
  *            *     *            *     *           *     *                     *1<--1*                *     *         *     *               *      *            *
  **************     *            *     *           *     *                     *     ******************     *         *     *****************      ******0*******
                     *            *     *           *     *                     *                            *         *                                  v
                     *  mixer_1   *0<--0* dummybf_0 *0<--0* speexpreprocessor_0 *                            * align_0 *                        **********0*******
                     *(duplicator)*     *           *1<--1* (aec, denoise, ...) *                            *         *          RouteB        * deinterleave_1 *
                     *            *     *           *     *                     *                            *         *                        ********0**1******
                     *            *     *           *     *                     *                            *         *                                v  v
****************     *            *     *           *     *                     *                            *         *     ***********      **********0**1******
*              *     *            *     *           *     *                     *16<------------------------1*         *     *         *0<---0*                  *
* filewriter_0 *0<--1*            *     *           *     *                     *                            *         *1<--0* mixer_0 *1<---1* speexresampler_0 *
*              *     *            *     *           *     *                     *                            *         *     *         *      *                  *
****************     **************     *************     ***********************                            ***********     ***********      ********************
```

To run example3, the speex libaray is needed first.

```bash
$ sudo apt install libspeexdsp-dev
```

Run example3:

```bash
# The two arguments are the alsa playback pcm name and capture pcm name.
# They vary machine by machine.
$ x86_64-linux-gnu/bin/larkexample3 plughw:0,0 plughw:0,0
```

If no error, playback and capture will be started.
The captured audio is saved in `./cap-16000_16_1.pcm`.

The source code of this example is shown in [larkexample3.cpp](https://gitee.com/wksuper/lark-release/blob/master/examples/larkexample3.cpp).

#### Example 5: Run a single playback route with ***SoundTouch*** embedded for tuning sound pitch, tempo and rate

```
RouteA

 libblkfilereader.so   libblkformatdapter.so    libblksoundtouch.so   libblkpaplayback.so
  ****************      *******************      ****************      ****************
  *              *      *                 *      *              *      *              *
  * filereader_0 *0--->0* formatadapter_0 *0--->0* soundtouch_0 *0--->0* paplayback_0 *
  *              *      *                 *      *              *      *              *
  ****************      *******************      ****************      ****************
```

To run example5, the ***SoundTouch*** library and the ***PortAudio*** library are needed first.

```bash
$ sudo apt install libsoundtouch-dev
$ sudo apt install libportaudio2
```

Run example5:

```bash
$ x86_64-linux-gnu/bin/larkexample5
```

If no error, playback will be started.

In the other shell,

```bash
$ lkdb status                                   # Shows lark status
$ lkdb setparam RouteA blksoundtouch_0 1 0.6    # Sound pitch goes lower
$ lkdb setparam RouteA blksoundtouch_0 1 1.8    # Sound pitch goes higher
$ lkdb setparam RouteA blksoundtouch_0 1 1.0    # Sound pitch goes normal
$ lkdb setparam RouteA blksoundtouch_0 2 0.5    # Sound tempo goes slower
$ lkdb setparam RouteA blksoundtouch_0 2 2.0    # Sound tempo goes faster
$ lkdb setparam RouteA blksoundtouch_0 2 1.0    # Sound tempo goes normal
$ lkdb setparam RouteA blksoundtouch_0 3 2.2    # Sound rate goes faster
$ lkdb setparam RouteA blksoundtouch_0 3 0.4    # Sound rate goes slower
$ lkdb setparam RouteA blksoundtouch_0 3 1.0    # Sound rate goes normal
```

The source code of this example is shown in [larkexample5.cpp](https://gitee.com/wksuper/lark-release/blob/master/examples/larkexample5.cpp).

#### Example 7: Run a single route with SoX effects and output to ffplay

```
RouteA

 libblkfilereader.dylib  libblkformatdapter.dylib   libblkdeinterleave.dylib  libblkmixer.dylib      libblksoxeffect.dylib       libblkinterleave.dylib  libblkfilewriter.dylib

  *******************     **********************     *********************     **************     ***************************     *******************     *******************
  *                 *     *                    *     *                   *     *            *0-->0* blksoxeffect_highpass_0 *0-->0*                 *     *                 *
  *                 *     *                    *     *                   *0-->0*            *     ***************************     *                 *     *                 *
  * blkfilereader_0 *0-->0* blkformatadapter_0 *0-->0* blkdeinterleave_0 *     * blkmixer_0 *                                     * blkinterleave_0 *0-->0* blkfilewriter_0 *
  *                 *     *                    *     *                   *1-->1*            *     ***************************     *                 *     *     (stdout)    *
  *                 *     *                    *     *                   *     *            *1-->0* blksoxeffect_lowpass_0  *0-->1*                 *     *                 *
  *******************     **********************     *********************     **************     ***************************     *******************     *******************
```

To run example7, the ***SoX*** library and ***ffmpeg*** are needed first.

```bash
$ brew install sox
$ brew install ffmpeg
```

Run example7:

```bash
$ x86_64-apple-darwin/bin/larkexample7 | ffplay -i pipe:0 -f s32le -ar 48000 -ac 2 -autoexit
```

If no error, playback will be started, and on the ffplay screen, audio spectrum is being drawed like this.

![larkexample7-1](./examples/larkexample7-1.png)

In the other shell,

```bash
$ lkdb status    # Shows lark status. Found that the name of higpass/lowpass filter block is 'blksoxeffect_highpass_0'/'blksoxeffect_lowpass_0' respectively.
```

```bash
$ lkdb setparam RouteA blksoxeffect_highpass_0 0 800    # Only higher than 800Hz music signals go to left speaker
```

The audio spectrum is being drawed like this.

![larkexample7-2](./examples/larkexample7-2.png)

> Tips: In ffplay spectrum, red means left channel and green means right channel. If grey is displayed on a certain frequency at a certain time, it means the power of left channel and of right channel on that frequency at that time is same.

```bash
$ lkdb setparam RouteA blksoxeffect_lowpass_0 0 800    # Only lower than 800Hz music signals go to right speaker
```

The audio spectrum is being drawed like this.

![larkexample7-3](./examples/larkexample7-3.png)

The real-time change of ffplay spectrum color verified the sox-effects on lark route are taking effect in real time.

The source code of this example is shown in [larkexample7.cpp](https://gitee.com/wksuper/lark-release/blob/master/examples/larkexample7.cpp).

### Furthermore Examples

[Author's blog: <My Engineer Daddy - Audio Application DIY Series>](https://blog.csdn.net/weixin_44278307?type=blog) demonstrated by using ***lark*** how to deal with the daily audio related affairs of life through a series of short stories.

## Debug with `lkdb` in Realtime

When ***lark*** is running in a process, the debug utility `lkdb` can communicate with it to get info from ***lark*** and send command to ***lark***.

```
Usage:
  lkdb status [--dot]
    - Print the lark running status
    - Print dot code if enabled '--dot' option
  lkdb newroute ROUTENAME
    - Create a named route
  lkdb newblock ROUTENAME SOLIB ISFIRST ISLAST [ARGS]
    - Create a block on the route from the dynamic load library
  lkdb newlink ROUTENAME RATE FORMAT CHNUM FRAMESIZEINSAMPLES SRCBLKNAME SRCBLKOUTEPIDX SINKBLKNAME SINKBLKINEPIDX
    - Create a link to link the source block and sink block on the route
  lkdb start ROUTENAME
    - Start the route
  lkdb stop ROUTENAME
    - Stop the route
  lkdb setparam ROUTENAME BLOCKNAME PARAMID [PARAMS]
    - Set parameter to the block in the route
  lkdb getparam ROUTENAME BLOCKNAME PARAMID
    - Get parameter from the block in the route
  lkdb delete ROUTENAME LINKNAME
    - Delete the link from the route
  lkdb delete ROUTENAME BLOCKNAME
    - Delete the block and associated link(s) from the route
  lkdb delete ROUTENAME !
    - Delete the entire route
  lkdb loglevel LEVEL
    - Set the logging LEVEL: 0=off 1=fatal 2=error 3=warning 4=info(default) 5=debug 6=verbose
  lkdb setdump DIRECTORY
    - Enable dumping log & data when DIRECTORY is a valid path
    - Disable dumping log & data when DIRECTORY is --
```

Besides real-time printing status, setting parameters shown in the examples above, `lkdb` can also manipulate audio routes, take a snapshot of the routes, change logging level, setting audio data dump, etc.

### No coding needed to run the same route as example1's via `lkdb`

In one shell,

```bash
$ x86_64-linux-gnu/bin/larkexample0    # Run lark
```

In the other shell,

```bash
$ lkdb newroute RouteA
Created RouteA
$ lkdb newblock RouteA libblkfilereader.so true false examples/kanr-48000_16_2.pcm
Created blkfilereader_0 from libblkfilereader.so on RouteA
$ lkdb newblock RouteA libblkalsaplayback.so false true
Created blkalsaplayback_0 from libblkalsaplayback.so on RouteA
$ lkdb newlink RouteA 48000 S16_LE 2 960 blkfilereader_0 0 blkalsaplayback_0 0
Created lnk_0 on RouteA
$ lkdb start RouteA      # The music playback should be started
Started RouteA
$ lkdb stop RouteA
Stopped RouteA
$ lkdb delete RouteA !
Deleted RouteA
```

The source code of this example is shown in [larkexample0.cpp](https://gitee.com/wksuper/lark-release/blob/master/examples/larkexample0.cpp).

### Take a snapshot of example3's routes via `lkdb`

While running example3,

```bash
$ lkdb status --dot | dot -Tpng -o larkexample3.png
```

The routes snapshot will be saved to `larkexample3.png`.

![larkexample3.png](./examples/larkexample3.png)

Note: This requires graphviz(dot) to be installed on your machine.

```bash
$ sudo apt install graphviz
```

## Make Your Own Audio Route(s)

### Step 1

Design the route(s) in your mind. The prebuilt blocks(like gain, mixer, etc.) can be used directly. If you need custom block(s), you can compile the so library file(s) by yourself. [BlkPassthrough.cpp](https://gitee.com/wksuper/lark-release/blob/master/examples/BlkPassthrough.cpp) is an example of block source code. See [MANUAL.md - 4 Build Your Own Block](https://gitee.com/wksuper/lark-release/blob/master/MANUAL.md#4-build-your-own-block) for more detail.

### Step 2

Call ***lark*** APIs to make your route(s) implemented in your process. [Examples](https://gitee.com/wksuper/lark-release/tree/master/examples) have been listed. Refer to [User Manual](https://gitee.com/wksuper/lark-release/blob/master/MANUAL.md) for detail.

## FAQs

**Q1**: What is "block"? What is "endpoint"? What is "link"? What is "route"?

**A**: A "block" is a software DSP processor unit. It is created from a dynamic load so library and managed by ***lark*** route. It accepts up-to 32 inputs(called input endpoints) data, processes them, and fills the processed data into the output endpoints (up-to 32 outputs). Usually, one endpoint represents one audio channel. However, one endpoint can also carry multi-channels. e.g. The output endpoint of "interleave" block would carry multi-channels. A "link" links an output endpoint of a block as the source to an input endpoint of the other block as the sink. A "route" is a blocks-linked chain(or graph).

**Q2**: On one machine, how many ***lark***s can I run? Within one ***lark***, how many routes can I create? Within one route, how many blocks can I create? How many first blocks and last blocks can I create?

**A**: On one machine, only one process is allowed to create the ***lark***. i.e. One machine runs one ***lark***. `lkdb` will communicate with the unique ***lark***. Within one ***lark***, 26 routes can be created at maximum, with the name "RouteA", "RouteB", ..., "RouteZ" specified manually. For each route, name is required and should be fixed to "Route#"(case sensitive) where "#" stands for [A-Z]. Within one route, no amount limitation to create the blocks. All the blocks/links created from a route belong to this route, which means their resources are managed by this route. Also, no amount limitation to create first blocks and last blocks.

**Q3**: What is "first block"? What is "last block"?

**A**: In one route, at least one "first block" and one "last block" have to be created with the "first" flag and the "last" flag specified separately. So that, the route can know from which block(s) to begin processing and from which block(s) to end processing. From the first block(s) to the last block(s), blocks should be linked without cut-off. The route can only start to run when the links are complete. If there is cut-off in the route, it is allowed, but the route won't be able to run. If you insist to run, the route will stay in BROKEN_RUNNING state until the cut-off is linked up.

**Q4**: What's the difference between "multi-routes" and "multi-first-blocks in one route"? If I have multiple inputs as the first blocks, how to select between "multi-routes" and "multi-first-blocks in one route"?

**A**: One route has one thread to process data. Normally "multi-first-blocks in one route" can work well. In this case, the multiple inputs are able to provide frames at the same pace, and they shouldn't be blocked by each other. For example, one input is alsacapture, one input is filereader. The scenario that needs multi-routes is, if the multiple inputs running in one route have chance to block each other, then they need to be separated into multi-routes. For example, one input is alsacapture, one input is echo-reference.

## lark-based Open Source Applications

- [kplay](https://gitee.com/wksuper/kplay) - A WAV File Player with Real-Time Sound Tuning
