# lark

[English](https://gitee.com/wksuper/lark-release/blob/master/README.md) | [简体中文](https://gitee.com/wksuper/lark-release/blob/master/README-cn.md)

***lark*** is a lite but powerful software audio DSP owned by Wang Kui. It provides a flexible and scalable way to design audio route(s) with high performance, low MCPS and low latency.
Main features (as of v0.2):

- Support realtime manipulating audio routes
  - Load/Unload blocks in real time
  - Change routes in real time
  - Tune block parameters in real time
  - Set log level in real time
  - Dump data of each block in real time
- Support prebuilt I/O blocks
  - file-reader, file-writer, stream-in, stream-out, alsa-capture, alsa-playback, portaudio-playback
- Support prebuilt algorithm blocks
  - gain, mixer(duplicator), interleave, de-interleave, format-adapter, delay, align, buffer, speex-resampler, speex-preprocessor, soundtouch
- Support customizing blocks extensively
- Support up-to 32 input endpoints and 32 output endpoints for each block
- Support timestamp for each frame / each sample
- Support realtime debug tool
- Support for multiple operating systems
  - Linux (x86_64), MacOS (x86_64)

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

  libblkfilereader.so      libblkinterleave.so     libblkalsaplayback.so
  *******************      *******************      ******************
  *                 *0--->0*                 *      *                *
  *  filereader_0   *1--->1*  interleave_0   *0--->0* alsaplayback_0 *
  *                 *      *                 *      *                *
  *******************      *******************      ******************
```

```bash
$ x86_64-linux-gnu/bin/larkexample1
```

If no error, the music file should be started to play.

The source code of this example is shown in [larkexample1.cpp](https://gitee.com/wksuper/lark-release/blob/master/examples/larkexample1.cpp).

#### Example 2: Run a single route for music playback where two files input are mixed with separate gain

```
RouteA

 libblkfilereader.so     libblkgain.so           libblkmixer.so       libblkinterleave.so     libblkalsaplayback.so

  *****************      *************           **************       *******************      ******************
  *               *0--->0*           *0-------->0*            *0---->0*                 *      *                *
  * filereader_0  *1--->1*   gain_0  *1---+      *   mixer_0  *       *   interleave_0  *0--->0* alsaplayback_0 *
  *               *      *           *    | +-->1*            *   +->1*                 *      *                *
  *****************      *************    | |    **************   |   *******************      ******************
                                          | |                     |
  *****************      *************    | |    **************   |
  *               *      *           *    +-|-->0*            *0--+
  * filereader_1  *0--->0*   gain_1  *0-----+    *   mixer_1  *
  *               *1--->1*           *1-------->1*            *
  *****************      *************           **************
```

```bash
$ x86_64-linux-gnu/bin/larkexample2
```

If no error, the two files mixed music should be started to play.

The source code of this example is shown in [larkexample2.cpp](https://gitee.com/wksuper/lark-release/blob/master/examples/larkexample2.cpp).

#### Example 3: Run dual routes, one for music playback, one for microphone capture with keyword detection

```
                                                                                ****************      ****************      ******************      ***************
                                                                   RouteA       *              *0--->0*              *      *                *      *             *
                                                                                * filereader_0 *1--->1* interleave_0 *0--->0* alsaplayback_0 *0--->0* streamout_0 *
                                                                                *              *      *              *      *                *      *             *
                                                                                ****************      ****************      ******************      ********|******
                                                                                                                                                            v
=========================================================================================================================================================[FIFO]=====
                                                                                                                                                          |
  **************     **************      *************     ***********************     ******************     ***********     *****************      *****v********
  *            *     *            *      *           *     *                     *     *                *     *         *     *               *      *            *
  * dummykwd_0 *0<--0*            *      *           *     *                     *0<--0* deinterleave_0 *0<--0*         *0<--0* alsacapture_0 *      * streamin_0 *
  *            *     *            *      *           *     *                     *1<--1*                *     *         *     *               *      *            *
  **************     *            *      *           *     *                     *     ******************     *         *     *****************      ******0*******
                     *            *      *           *     *                     *                            *         *                                  v
                     *  mixer_1   *0<---0* dummybf_0 *0<--0* speexpreprocessor_0 *                            * align_0 *                        **********0*******
                     *(duplicator)*      *           *1<--1* (aec, denoise, ...) *                            *         *          RouteB        * deinterleave_1 *
                     *            *      *           *     *                     *                            *         *                        ********0**1******
                     *            *      *           *     *                     *                            *         *                                v  v
****************     *            *      *           *     *                     *                            *         *     ***********      **********0**1******
*              *     *            *      *           *     *                     *16<------------------------1*         *     *         *0<---0*                  *
* filewriter_0 *0<--1*            *      *           *     *                     *                            *         *1<--0* mixer_0 *1<---1* speexresampler_0 *
*              *     *            *      *           *     *                     *                            *         *     *         *      *                  *
****************     **************      *************     ***********************                            ***********     ***********      ********************
```

To run example3, the speex libaray is needed first.

```bash
$ sudo apt install libspeexdsp-dev
```

Run example3:

```bash
$ x86_64-linux-gnu/bin/larkexample3
```

If no error, playback and capture will be started.
The captured audio is saved in `./cap-16000_16_1.pcm`.

The source code of this example is shown in [larkexample3.cpp](https://gitee.com/wksuper/lark-release/blob/master/examples/larkexample3.cpp).

#### Example 5: Run a single playback route with SoundTouch embedded for tuning sound pitch, tempo and rate

```
RouteA

 libblkfilereader.so   libblkinterleave.so   libblkformatdapter.so    libblksoundtouch.so   libblkpaplayback.so
  ****************      ****************      *******************      ****************      ****************
  *              *0--->0*              *      *                 *      *              *      *              *
  * filereader_0 *1--->1* interleave_0 *0--->0* formatadapter_0 *0--->0* soundtouch_0 *0--->0* paplayback_0 *
  *              *      *              *      *                 *      *              *      *              *
  ****************      ****************      *******************      ****************      ****************
```

To run example5, the SoundTouch library and the PortAudio library are needed first.

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

## Debug with `lkdb` in Realtime

When ***lark*** is running in a process, the debug utility `lkdb` can communicate with it to get info from ***lark*** and send command to ***lark***.

```
Usage:
  lkdb status
    - Print the lark running status
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
    - Set the logging LEVEL: 0=off 1=error 2=warning 3=info(default) 4=debug 5=verbose
  lkdb setdump DIRECTORY
    - Set dump directory path. Disable the dump when DIRECTORY=off.
```

For example, when running example2, in the other shell, you can tune example2's volume gains via `lkdb`.

```bash
$ lkdb status                               # Shows lark status

$ lkdb setparam RouteA blkgain_0 1 0 0.5    # Output volume of kanr-48000_16_2.pcm left channel should be lower
$ lkdb setparam RouteA blkgain_0 1 1 0.5    # Output volume of kanr-48000_16_2.pcm right channel should be lower
$ lkdb setparam RouteA blkgain_0 1 0 0.0    # Output volume of kanr-48000_16_2.pcm left channel should be muted
$ lkdb setparam RouteA blkgain_0 1 1 0.0    # Output volume of kanr-48000_16_2.pcm right channel should be muted

$ lkdb setparam RouteA blkgain_1 1 0 0.5    # Output volume of pacificrim-48000_16_2.pcm left channel should be lower
$ lkdb setparam RouteA blkgain_1 1 1 0.5    # Output volume of pacificrim-48000_16_2.pcm right channel should be lower
$ lkdb setparam RouteA blkgain_1 1 0 0.0    # Output volume of pacificrim-48000_16_2.pcm left channel should be muted
$ lkdb setparam RouteA blkgain_1 1 1 0.0    # Output volume of pacificrim-48000_16_2.pcm right channel should be muted

$ lkdb setparam RouteA blkgain_0 1 0 1.0    # Output volume of kanr-48000_16_2.pcm left channel should be recovered back
$ lkdb setparam RouteA blkgain_0 1 1 1.0    # Output volume of kanr-48000_16_2.pcm right channel should be recovered back

$ lkdb setparam RouteA blkgain_1 1 0 1.0    # Output volume of pacificrim-48000_16_2.pcm left channel should be recovered back
$ lkdb setparam RouteA blkgain_1 1 1 1.0    # Output volume of pacificrim-48000_16_2.pcm right channel should be recovered back
```

## Make Your Own Audio Route(s)

### Step 1

Design the route(s) in your mind. The prebuilt blocks(like gain, mixer, etc.) can be used directly. If you need custom block(s), you can compile the so library file(s) by yourself. [BlkPassthrough.cpp](https://gitee.com/wksuper/lark-release/blob/master/examples/BlkPassthrough.cpp) is an example of block source code. See [MANUAL.md - 4 Build Your Own Block](https://gitee.com/wksuper/lark-release/blob/master/MANUAL.md#4-build-your-own-block) for more detail.

### Step 2

For test(try run) only purpose, `lkdb` is qualified to do that.

For example, to run the same route as example1's, you can also use `lkdb` to create route, start/stop route, delete route.

In one shell,

```bash
$ x86_64-linux-gnu/bin/larkexample0    # Run lark
```

In the other shell,

```bash
$ lkdb newroute RouteA
Success
$ lkdb newblock RouteA libblkfilereader.so 1 0 examples/kanr-48000_16_2.pcm 48000 1 2
Created blkfilereader_0 from libblkfilereader.so on RouteA
$ lkdb newblock RouteA libblkinterleave.so 0 0
Created blkinterleave_0 from libblkinterleave.so on RouteA
$ lkdb newblock RouteA libblkalsaplayback.so 0 1
Created blkalsaplayback_0 from libblkalsaplayback.so on RouteA
$ lkdb newlink RouteA 48000 1 1 960 blkfilereader_0 0 blkinterleave_0 0
Created lnk_0 on RouteA
$ lkdb newlink RouteA 48000 1 1 960 blkfilereader_0 1 blkinterleave_0 1
Created lnk_1 on RouteA
$ lkdb newlink RouteA 48000 1 2 960 blkinterleave_0 0 blkalsaplayback_0 0
Created lnk_2 on RouteA
$ lkdb start RouteA      # The music playback should be started
Started RouteA
$ lkdb stop RouteA
Stopped RouteA
$ lkdb delete RouteA !
Deleted RouteA
```

The source code of this example is shown in [larkexample0.cpp](https://gitee.com/wksuper/lark-release/blob/master/examples/larkexample0.cpp).

### Step 3

For applying on real product, you need to call ***lark*** APIs to make your own audio route(s) in your process. [Examples](https://gitee.com/wksuper/lark-release/tree/master/examples) have been listed. Refer to [User Manual](https://gitee.com/wksuper/lark-release/blob/master/MANUAL.md) for detail.

## FAQs

**Q1**: What is "block"? What is "endpoint"? What is "link"? What is "route"?

**A**: A "block" is a software DSP processor unit. It is created from a dynamic load so library and managed by ***lark*** route. It accepts up-to 32 inputs(called input endpoints) data, processes them, and fills the processed data into the output endpoints (up-to 32 outputs). Usually, one endpoint represents one audio channel. However, one endpoint can also carry multi-channels. e.g. The output endpoint of "interleave" block would carry multi-channels. A "link" links an output endpoint of a block as the source to an input endpoint of the other block as the sink. A "route" is a blocks-linked chain(or graph).

**Q2**: On one machine, how many ***lark***s can I run? Within one ***lark***, how many routes can I create? Within one route, how many blocks can I create? How many first blocks and last blocks can I create?

**A**: On one machine, only one process is allowed to create the ***lark***. i.e. One machine runs one ***lark***. `lkdb` will communicate with the unique ***lark***. Within one ***lark***, 26 routes can be created at maximum, with the name "RouteA", "RouteB", ..., "RouteZ" specified manually. For each route, name is required and should be fixed to "Route#"(case sensitive) where "#" stands for [A-Z]. Within one route, no amount limitation to create the blocks. All the blocks/links created from a route belong to this route, which means their resources are managed by this route. Also, no amount limitation to create first blocks and last blocks.

**Q3**: What is "first block"? What is "last block"?

**A**: In one route, at least one "first block" and one "last block" have to be created with the "first" flag and the "last" flag specified separately. So that, the route can know from which block(s) to begin processing and from which block(s) to end processing. From the first block(s) to the last block(s), blocks should be linked without cut-off. The route can only start to run when the links are complete. If there is cut-off in the route, it is allowed, but the route won't be able to run. If you insist to run, the route will stay in BROKEN_RUNNING state until the cut-off is linked up.

**Q4**: What's the difference between "multi-routes" and "multi-first-blocks in one route"? If I have multiple inputs as the first blocks, how to select between "multi-routes" and "multi-first-blocks in one route"?

**A**: One route has one thread to process data. Normally "multi-first-blocks in one route" can work well. In this case, the multiple inputs are able to provide frames at the same pace, and they shouldn't be blocked by each other. For example, one input is alsacapture, one input is filereader. The scenario that needs multi-routes is, if the multiple inputs running in one route have chance to block each other, then they need to be separated into multi-routes. For example, one input is alsacapture, one input is echo-reference.

## Change Log

### 0.2

- Supported MacOS (x86_64)
  - `x86_64-apple-darwin/bin/larkexample4` and `x86_64-apple-darwin/bin/larkexample5` are able to run
- Splited `README.md` into [README.md](https://gitee.com/wksuper/lark-release/blob/master/README.md)(English) and [README-cn.md](https://gitee.com/wksuper/lark-release/blob/master/README-cn.md)(simplified Chinese)

### 0.1

- Support realtime manipulating audio routes
  - Load/Unload blocks in real time
  - Change routes in real time
  - Tune block parameters in real time
  - Set log level in real time
  - Dump data of each block in real time
- Support prebuilt I/O blocks
  - file-reader, file-writer, stream-in, stream-out, alsa-capture, alsa-playback, portaudio-playback
- Support prebuilt algorithm blocks
  - gain, mixer(duplicator), interleave, de-interleave, format-adapter, delay, align, buffer, speex-resampler, speex-preprocessor, soundtouch
- Support customizing blocks extensively
- Support up-to 32 input endpoints and 32 output endpoints for each block
- Support timestamp for each frame / each sample
- Support realtime debug tool
- Support for multiple operating systems
  - Linux (x86_64) currently
