# lark

***lark*** is a lite but powerful software audio DSP owned by Wang Kui. It provides a flexible and scalable way to design audio route(s) with high performance, low MCPS and low latency.
Main features (as of v0.1):

- Support realtime manipulating audio routes
  - Load/Unload blocks in real time
  - Change routes in real time
  - Tune block parameters in real time
  - Set log level in real time
- Support prebuilt I/O blocks
  - file-reader, file-writer, stream-in, stream-out, alsa-capture, alsa-playback, pa-playback
- Support prebuilt algorithm blocks
  - gain, mixer(duplicator), interleave, de-interleave, format-adapter, delay, align, buffer, speex-resampler, speex-preprocessor, soundtouch
- Support customizing blocks extensively
- Support up-to 32 input endpoints and 32 output endpoints for each block
- Support timestamp for each frame / each sample
- Support realtime debug tool
- Support for multiple operating systems
  - Linux (x86_64) currently

## Get Started

***lark*** is composed by a basic shared library `liblark.so`, various optional blocks(dynamic load libraries) e.g. `libblkmixer.so`, and an optional executable debug utility `lkdb`. Based on ***lark***, you can easily create either simple or complex audio route(s) as you want, start/stop any route in realtime, or set/get parameters to/from any block in realtime.

### Step 1

Since ***lark*** depends on the klogging library, you should install klogging at first. Go to <https://github.com/wksuper/klogging> to get klogging installed. Alternatively the prebuilt `libklogging.so` is also available in this repo. It will be installed along with lark installation.

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

The demo audio files in this repo are stored by git LFS. If the files' size is very small and they're unable to playback, you need to install git-lfs and checkout the LFS files.

```bash
$ sudo apt install git-lfs
$ git lfs install
$ cd lark-release
$ git lfs pull
```

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

The source code of this example is shown in [larkexample1.cpp](https://github.com/wksuper/lark-release/blob/master/examples/larkexample1.cpp).

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

The source code of this example is shown in [larkexample2.cpp](https://github.com/wksuper/lark-release/blob/master/examples/larkexample2.cpp).

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

The source code of this example is shown in [larkexample3.cpp](https://github.com/wksuper/lark-release/blob/master/examples/larkexample3.cpp).

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

To run example5, the SoundTouch libaray is needed first.

```bash
$ sudo apt install libsoundtouch-dev
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

The source code of this example is shown in [larkexample5.cpp](https://github.com/wksuper/lark-release/blob/master/examples/larkexample5.cpp).

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

Design the route(s) in your mind. The prebuilt blocks(like gain, mixer, etc.) can be used directly. If you need custom block(s), you can compile the so library file(s) by yourself. [BlkPassthrough.cpp](https://github.com/wksuper/lark-release/blob/master/examples/BlkPassthrough.cpp) is an example of block source code. See [MANUAL.md - 2 Build Your Own Block](https://github.com/wksuper/lark-release/blob/master/MANUAL.md#2-build-your-own-block) for more detail.

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

The source code of this example is shown in [larkexample0.cpp](https://github.com/wksuper/lark-release/blob/master/examples/larkexample0.cpp).

### Step 3

For applying on real product, you need to call ***lark*** APIs to make your own audio route(s) in your process. [Examples](https://github.com/wksuper/lark-release/tree/master/examples) have been listed. Refer to [User Manual](https://github.com/wksuper/lark-release/blob/master/MANUAL.md) for detail.

## FAQs

**Q1-1**: What is "block"? What is "endpoint"? What is "link"? What is "route"?

**A**: A "block" is a software DSP processor unit. It is created from a dynamic load so library and managed by ***lark*** route. It accepts up-to 32 inputs(called input endpoints) data, processes them, and fills the processed data into the output endpoints (up-to 32 outputs). Usually, one endpoint represents one audio channel. However, one endpoint can also carry multi-channels. e.g. The output endpoint of "interleave" block would carry multi-channels. A "link" links an output endpoint of a block as the source to an input endpoint of the other block as the sink. A "route" is a blocks-linked chain(or graph).

**Q1-2**: On one machine, how many ***lark***s can I run? Within one ***lark***, how many routes can I create? Within one route, how many blocks can I create? How many first blocks and last blocks can I create?

**A**: On one machine, only one process is allowed to create the ***lark***. i.e. One machine runs one ***lark***. `lkdb` will communicate with the unique ***lark***. Within one ***lark***, 26 routes can be created at maximum, with the name "RouteA", "RouteB", ..., "RouteZ" specified manually. For each route, name is required and should be fixed to "Route#"(case sensitive) where "#" stands for [A-Z]. Within one route, no amount limitation to create the blocks. All the blocks/links created from a route belong to this route, which means their resources are managed by this route. Also, no amount limitation to create first blocks and last blocks.

**Q1-3**: What is "first block"? What is "last block"?

**A**: In one route, at least one "first block" and one "last block" have to be created with the "first" flag and the "last" flag specified separately. So that, the route can know from which block(s) to begin processing and from which block(s) to end processing. From the first block(s) to the last block(s), blocks should be linked without cut-off. The route can only start to run when the links are complete. If there is cut-off in the route, it is allowed, but the route won't be able to run. If you insist to run, the route will stay in BROKEN_RUNNING state until the cut-off is linked up.

**Q1-4**: What's the difference between "multi-routes" and "multi-first-blocks in one route"? If I have multiple inputs as the first blocks, how to select between "multi-routes" and "multi-first-blocks in one route"?

**A**: One route has one thread to process data. Normally "multi-first-blocks in one route" can work well. In this case, the multiple inputs are able to provide frames at the same pace, and they shouldn't be blocked by each other. For example, one input is alsacapture, one input is filereader. The scenario that needs multi-routes is, if the multiple inputs running in one route have chance to block each other, then they need to be separated into multi-routes. For example, one input is alsacapture, one input is echo-reference.

**Q2-1**: Is ***lark*** free to use?

**A**: Yes, ***lark*** is totally free for either personal use or commercial use.

**Q2-2**: I found bug(s), how to do?

**A**: Report to Wang Kui (wangkuisuper@hotmail.com) with clearly stated issue description. Information should be included but not limited to:

- What's the issue?
- On which platform?
- How to reproduce and is it recoverable?  (the steps)
- Reproduce rate?
- Version number and logs dump (or data dump if needed) <--- IMPORTANT
  - `lkdb loglevel` for logs dump
  - `lkdb setdump` for data dump

**Q2-2**: I have ideas to improve or add features to ***lark***, how to do?

**A**: Contact Wang Kui (wangkuisuper@hotmail.com) with the background and reason.

# lark

***lark***是一个轻量级但功能强大的软件音频DSP，作者是王揆。它提供了一种灵活可扩展的方法来设计高性能、低MCPS、低延时的音频路由。
主要特性（至v0.1版本）：

- 支持实时操作音频路由
  - 实时加载/卸载块
  - 实时改变路径
  - 实时调节块参数
  - 实时设定日志等级
- 支持预编译的输入/输出块
  - file-reader, file-writer, stream-in, stream-out, alsa-capture, alsa-playback, pa-playback
- 支持预编译的算法块
  - gain, mixer(duplicator), interleave, de-interleave, format-adapter, delay, align, buffer, speex-resampler, speex-preprocessor, soundtouch
- 支持无限客制化块
- 支持每块最多32个输入端点，32个输出端点
- 支持基于帧的时间戳和基于采样点的时间戳
- 支持实时调试工具
- 支持多操作系统
  - 当前仅Linux (x86_64)

## 开始

***lark***是由一个基础的动态库`liblark.so`，各种可选的块（动态加载库）例如`libblkmixer.so`，和一个可选的可执行调试工具`lkdb`组成。基于 ***lark***，您可以容易地创建既您想实现的既可以简单又可以复杂的音频路由，实时启动/停止任意一个路由，或者实时设定/获取任意块的参数。

### 第1步

由于 ***lark*** 依赖于klogging库，您首先需要安装klogging。访问 <https://gitee.com/wksuper/klogging> 以安装之。另一种方式是，此仓库已经包含了预编译好的`libklogging.so`，它会随着lark的安装而一起安装。

### 第2步

安装 ***lark*** 到您的Linux机器。

```bash
$ cd lark-release
$ sudo ./install.sh
$ sudo ldconfig
```

当您不需要 ***lark*** 时，您可以使用`sudo ./uninstall.sh`来移除它。

### 第3步

运行音频路由。

#### 例1：运行一个单路由，做立体声音乐文件播放

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

如果没错误的话，音乐文件应该开始播放了。

这个例子的源代码在此：[larkexample1.cpp](https://gitee.com/wksuper/lark-release/blob/master/examples/larkexample1.cpp).

#### 例1：运行一个单路由，把两个音乐文件输入做带各自增益的混合播放

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

如果没错误的话，两个音乐文件应该开始混合播放输出了。

这个例子的源代码在此：[larkexample2.cpp](https://gitee.com/wksuper/lark-release/blob/master/examples/larkexample2.cpp).

#### 例3：运行双路由，一个做音乐播放，一个采集麦克风以做关键词侦测

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

运行例3之前，需要先安装speex库。

```bash
$ sudo apt install libspeexdsp-dev
```

运行例3：

```bash
$ x86_64-linux-gnu/bin/larkexample3
```

如果没错误的话，播放和录音应该同时开始了。
录到的声音文件存放在`./cap-16000_16_1.pcm`。

这个例子的源代码在此：[larkexample3.cpp](https://gitee.com/wksuper/lark-release/blob/master/examples/larkexample3.cpp).

#### 例5：运行一个单播放路由，嵌入了SoundTouch以做音调、节拍、播放速率的调节

```
RouteA

 libblkfilereader.so   libblkinterleave.so   libblkformatdapter.so    libblksoundtouch.so   libblkpaplayback.so
  ****************      ****************      *******************      ****************      ****************
  *              *0--->0*              *      *                 *      *              *      *              *
  * filereader_0 *1--->1* interleave_0 *0--->0* formatadapter_0 *0--->0* soundtouch_0 *0--->0* paplayback_0 *
  *              *      *              *      *                 *      *              *      *              *
  ****************      ****************      *******************      ****************      ****************
```

运行例5之前需要先安装SoundTouch库。

```bash
$ sudo apt install libsoundtouch-dev
```

运行例5：

```bash
$ x86_64-linux-gnu/bin/larkexample5
```

如果没错误的话，播放应该开始了。

在另一个shell里，

```bash
$ lkdb status                                   # 显示lark状态

$ lkdb setparam RouteA blksoundtouch_0 1 0.6    # 音调变低
$ lkdb setparam RouteA blksoundtouch_0 1 1.8    # 音调变高
$ lkdb setparam RouteA blksoundtouch_0 1 1.0    # 音调变正常
$ lkdb setparam RouteA blksoundtouch_0 2 0.5    # 节拍变慢
$ lkdb setparam RouteA blksoundtouch_0 2 2.0    # 节拍变块
$ lkdb setparam RouteA blksoundtouch_0 2 1.0    # 节拍变正常
$ lkdb setparam RouteA blksoundtouch_0 3 2.2    # 播放速率变快
$ lkdb setparam RouteA blksoundtouch_0 3 0.4    # 播放速率变慢
$ lkdb setparam RouteA blksoundtouch_0 3 1.0    # 播放速率变正常
```

这个例子的源代码在此：[larkexample5.cpp](https://gitee.com/wksuper/lark-release/blob/master/examples/larkexample5.cpp).

## 用`lkdb`实时调试

当 ***lark*** 在一个进程里运行时，调试工具`lkdb`可以跟它通信，从 ***lark*** 获取信息，向 ***lark*** 发生命令。

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

举个例子，当例2运行时，在另外一个shell里，您可以通过`lkdb`调节例2的音量。

```bash
$ lkdb status                               # 显示lark状态

$ lkdb setparam RouteA blkgain_0 1 0 0.5    # kanr-48000_16_2.pcm左声道的输出音量变低
$ lkdb setparam RouteA blkgain_0 1 1 0.5    # kanr-48000_16_2.pcm右声道的输出音量变低
$ lkdb setparam RouteA blkgain_0 1 0 0.0    # kanr-48000_16_2.pcm左声道的输出静音
$ lkdb setparam RouteA blkgain_0 1 1 0.0    # kanr-48000_16_2.pcm右声道的输出静音

$ lkdb setparam RouteA blkgain_1 1 0 0.5    # pacificrim-48000_16_2.pcm左声道的输出音量变低
$ lkdb setparam RouteA blkgain_1 1 1 0.5    # pacificrim-48000_16_2.pcm右声道的输出音量变低
$ lkdb setparam RouteA blkgain_1 1 0 0.0    # pacificrim-48000_16_2.pcm左声道的输出静音
$ lkdb setparam RouteA blkgain_1 1 1 0.0    # pacificrim-48000_16_2.pcm右声道的输出静音

$ lkdb setparam RouteA blkgain_0 1 0 1.0    # kanr-48000_16_2.pcm左声道的输出音量恢复
$ lkdb setparam RouteA blkgain_0 1 1 1.0    # kanr-48000_16_2.pcm右声道的输出音量恢复

$ lkdb setparam RouteA blkgain_1 1 0 1.0    # pacificrim-48000_16_2.pcm左声道的输出音量恢复
$ lkdb setparam RouteA blkgain_1 1 1 1.0    # pacificrim-48000_16_2.pcm右声道的输出音量恢复
```

## 创建您自己的音频路由

### 第1步

在您的脑海中设计好路由。那些预编译好的块（像gain, mixer等）可以直接使用。如果您想要客制化的块，您可以自行编译动态库。[BlkPassthrough.cpp](https://gitee.com/wksuper/lark-release/blob/master/examples/BlkPassthrough.cpp)是一个块源码的例子。参考[MANUAL.md - 2 Build Your Own Block](https://gitee.com/wksuper/lark-release/blob/master/MANUAL.md#2-build-your-own-block)以获取更多信息。

### 第2步

如果只是尝试为目的，`lkdb`就可以做了。

例如，想要运行跟例1一样的路由，您也可以使用`lkdb`来创建路由，启动/停止路由，删除路由。

在一个shell里，

```bash
$ x86_64-linux-gnu/bin/larkexample0    # 运行lark
```

在另一个shell里，

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
$ lkdb start RouteA      # 音乐播放应该开始了
Started RouteA
$ lkdb stop RouteA
Stopped RouteA
$ lkdb delete RouteA !
Deleted RouteA
```

这个例子的源代码在此：[larkexample0.cpp](https://gitee.com/wksuper/lark-release/blob/master/examples/larkexample0.cpp).

### 第3步

要应用在真实产品中，您需要在您的进程里调用 ***lark*** 的API来创建您自己的音频路由。[样例](https://gitee.com/wksuper/lark-release/tree/master/examples)已经列出。参考[用户手册](https://gitee.com/wksuper/lark-release/blob/master/MANUAL.md)以获取更多信息。

## 常见问题与回答

**Q1-1**: “块”是什么？“端点”是什么？“链接”是什么？“路由”是什么？

**A**: 一个“块”就是一个软件DSP处理单元。它从一个动态加载库里被创建，并被 ***lark*** 路由所管理。它接受最多32路输入（即输入端点）数据，处理它们，把处理后的数据填入输出端点（最多32路输出）。
通常情况，一个端点代表一个声道音频，然而一个端点也可以运载多声道。例如“interleave”块的一个输出端点就可以运载多声道。
一个“链接”连接一个块的一个输出端点（作为源）至另一个块的一个输入端点（作为汇）。 一个“路由”是一个由多个块连接成的链条（或者图）。

**Q1-2**: 在一个机器上，我可以同时运行几个 ***lark*** 实例？在一个 ***lark*** 中，我可以创建几个路由？在一个路由中，我可以创建多少个块？多少个first块？多少个last块？

**A**: 在一个机器上，只有一个进程被容许创建一个 ***lark*** 实例。也就是说，一个机器运行一个 ***lark*** 实例。`lkdb`会跟这个唯一的 ***lark*** 实例通信。在一个 ***lark*** 中，做多可以创建26个路由，它们的名字需要被手动指定为"RouteA", "RouteB", ..., "RouteZ"。对于每个路由，名字是需要的，并且应该固定为"Route#"（大小写敏感），这里“#”代表[A-Z]。
在一个路由里，创建块没有数量限制。所有从一个路由里创建出来的块/链接都属于这个路由，也就是意味着它们的资源是被这个路由统一管理。
同样，创建first块和last块也没有数量限制。

**Q1-3**: 什么是“first块”？什么是“last块”？

**A**: 在一个路由里，至少需要创建一个“first块”和一个“last块”,这些块在创建的时候，都有“first”标志和“last”标志被各自指定。
这样的话，路由就可以知道从哪些块开始处理，到哪些块解除处理。从first块到last块之间，所有的块应该要被连接起来，不要有切断。只有所有连接都完整了，路由才可以启动。如果路由中间有切断，这是被容许的，但是路由就无法运行。如果您强制运行，路由将会进入BROKEN_RUNNING状态，直到切断被再接起来。

**Q1-4**: “多路由”跟“单路由多first块”有何不同？如果我有多路输入作为first块，我要如何在“多路由”和“单路由多first块”之间做选择？

**A**: 一个路由由一个线程处理数据。通常情况，“单路由多first块”可以工作地很好。在这种情况下，多输入以同样的步伐提供数据帧，并且它们不应该被相互阻塞。例如，一个输入是alsacapture，一个输入是filereader。
需要用到“多路由”的场景是，当多个输入放在一个路由里运行会有机会相互阻塞时，那么它们就应该被分离到多个路由。例如，一个输入alsacapture，一个输入是echo-reference。

**Q2-1**: ***lark***是免费的吗？

**A**: 是的，***lark***完全免费，即对于个人也对于商用。

**Q2-2**: 我发现了bug，怎么办？

**A**: 带着清晰的问题陈述，报告给王揆(wangkuisuper@hotmail.com)。信息包括但不限于：

- 问题是什么？
- 在哪个平台上？
- 如何复现，能恢复吗？  (具体步骤)
- 复现概率？
- 版本号，日志（或者数据如果需要的话）dump <--- 重要
  - `lkdb loglevel` 日志dump
  - `lkdb setdump` 数据dump

**Q2-2**: 我有想法改进或增加功能，怎么做？

**A**: 带着背景和原因联系王揆 (wangkuisuper@hotmail.com)。
