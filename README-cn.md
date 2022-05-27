# lark

[English](https://gitee.com/wksuper/lark-release/blob/master/README.md) | [简体中文](https://gitee.com/wksuper/lark-release/blob/master/README-cn.md)

***lark***是一个轻量级但功能强大的软件音频DSP，作者是王揆。它提供了一种灵活可扩展的方法来设计高性能、低MCPS、低延时的音频路由。
主要特性（至v0.2版本）：

- 支持实时操作音频路由
  - 实时加载/卸载块
  - 实时改变路径
  - 实时调节块参数
  - 实时设定日志等级
  - 实时转储每个块的数据
- 支持预编译的输入/输出块
  - file-reader, file-writer, stream-in, stream-out, alsa-capture, alsa-playback, portaudio-playback
- 支持预编译的算法块
  - gain, mixer(duplicator), interleave, de-interleave, format-adapter, delay, align, buffer, speex-resampler, speex-preprocessor, soundtouch
- 支持无限客制化块
- 支持每块最多32个输入端点，32个输出端点
- 支持基于帧的时间戳和基于采样点的时间戳
- 支持实时调试工具
- 支持多操作系统
  - Linux (x86_64), MacOS (x86_64)

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

#### 例2：运行一个单路由，把两个音乐文件输入做带各自增益的混合播放

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

运行例5之前需要先安装SoundTouch库和PortAudio库。

```bash
$ sudo apt install libsoundtouch-dev
$ sudo apt install libportaudio2
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

在您的脑海中设计好路由。那些预编译好的块（像gain, mixer等）可以直接使用。如果您想要客制化的块，您可以自行编译动态库。[BlkPassthrough.cpp](https://gitee.com/wksuper/lark-release/blob/master/examples/BlkPassthrough.cpp)是一个块源码的例子。参考[MANUAL.md - 4 Build Your Own Block](https://gitee.com/wksuper/lark-release/blob/master/MANUAL.md#4-build-your-own-block)以获取更多信息。

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

**Q1**: “块”是什么？“端点”是什么？“链接”是什么？“路由”是什么？

**A**: 一个“块”就是一个软件DSP处理单元。它从一个动态加载库里被创建，并被 ***lark*** 路由所管理。它接受最多32路输入（即输入端点）数据，处理它们，把处理后的数据填入输出端点（最多32路输出）。
通常情况，一个端点代表一个声道音频，然而一个端点也可以运载多声道。例如“interleave”块的一个输出端点就可以运载多声道。
一个“链接”连接一个块的一个输出端点（作为源）至另一个块的一个输入端点（作为汇）。 一个“路由”是一个由多个块连接成的链条（或者图）。

**Q2**: 在一个机器上，我可以同时运行几个 ***lark*** 实例？在一个 ***lark*** 中，我可以创建几个路由？在一个路由中，我可以创建多少个块？多少个first块？多少个last块？

**A**: 在一个机器上，只有一个进程被容许创建一个 ***lark*** 实例。也就是说，一个机器运行一个 ***lark*** 实例。`lkdb`会跟这个唯一的 ***lark*** 实例通信。在一个 ***lark*** 中，最多可以创建26个路由，它们的名字需要被手动指定为"RouteA", "RouteB", ..., "RouteZ"。对于每个路由，名字是需要的，并且应该固定为"Route#"（大小写敏感），这里“#”代表[A-Z]。
在一个路由里，创建块没有数量限制。所有从一个路由里创建出来的块/链接都属于这个路由，也就是意味着它们的资源是被这个路由统一管理。
同样，创建first块和last块也没有数量限制。

**Q3**: 什么是“first块”？什么是“last块”？

**A**: 在一个路由里，至少需要创建一个“first块”和一个“last块”，这些块在创建的时候，都有“first”标志和“last”标志被各自指定。
这样的话，路由就可以知道从哪些块开始处理，到哪些块结束处理。从first块到last块之间，所有的块应该要被连接起来，不要有切断。只有所有连接都完整了，路由才可以启动。如果路由中间有切断，这是被容许的，但是路由就无法运行。如果您强制运行，路由将会进入BROKEN_RUNNING状态，直到切断被再接起来。

**Q4**: “多路由”跟“单路由多first块”有何不同？如果我有多路输入作为first块，我要如何在“多路由”和“单路由多first块”之间做选择？

**A**: 一个路由用一个线程处理数据。通常情况，“单路由多first块”可以工作地很好。在这种情况下，多输入以同样的步伐提供数据帧，并且它们不应该被相互阻塞。例如，一个输入是alsacapture，一个输入是filereader。
需要用到“多路由”的场景是，当多个输入放在一个路由里运行会有机会相互阻塞时，那么它们就应该被分离到多个路由。例如，一个输入alsacapture，一个输入是echo-reference。

## 版本历史

### 0.2

- 支持 MacOS (x86_64)
  - `x86_64-apple-darwin/bin/larkexample4`和`x86_64-apple-darwin/bin/larkexample5`可以正常运行了
- 把`README.md`拆分成了[README.md](https://gitee.com/wksuper/lark-release/blob/master/README.md)(英语)和[README-cn.md](https://gitee.com/wksuper/lark-release/blob/master/README-cn.md)(简体中文)

### 0.1

- 支持实时操作音频路由
  - 实时加载/卸载块
  - 实时改变路径
  - 实时调节块参数
  - 实时设定日志等级
  - 实时转储每个块的数据
- 支持预编译的输入/输出块
  - file-reader, file-writer, stream-in, stream-out, alsa-capture, alsa-playback, portaudio-playback
- 支持预编译的算法块
  - gain, mixer(duplicator), interleave, de-interleave, format-adapter, delay, align, buffer, speex-resampler, speex-preprocessor, soundtouch
- 支持无限客制化块
- 支持每块最多32个输入端点，32个输出端点
- 支持基于帧的时间戳和基于采样点的时间戳
- 支持实时调试工具
- 支持多操作系统
  - 当前仅Linux (x86_64)