# ![logo](./examples/logo.png)

[English](https://gitee.com/wksuper/lark-release/blob/master/README.md) | [简体中文](https://gitee.com/wksuper/lark-release/blob/master/README-cn.md)

## 介绍

***百灵鸟*** (***lark***) 是一个免费、轻量、功能强大的软件音频DSP。它提供了一种灵活可扩展的方法来设计效能高、空间占用小、延时低的音频路由，让您可以像搭积木一样构建音频系统。

### 主要特性

- 支持实时操作音频路由
  - 实时加载/卸载块
  - 实时改变路径
  - 实时调节块参数
- 支持预编译的输入/输出块
  - 文件读取(file-reader), 文件写入(file-writer), 流输入(stream-in), 流输出(stream-out), alsa录音, alsa播放, tinyalsa录音, tinyalsa播放, portaudio录音, portaudio播放
- 支持预编译的算法块
  - 增益(gain), 混音器(mixer), 复制器(duplicator), 多路复用器(multiplexer), 声道交织(interleave), 声道分离(de-interleave), 格式适配器(format-adapter), 延时器(delay), 对齐(align), 缓存(buffer), 淡入(fade-in), 淡出(fade-out)
  - speexdsp算法: 重采样(speex-resampler), 前处理(speex-preprocessor)
  - soundtouch算法: 音调调节器(pitch), 节拍调节器(tempo), 速度调节器(rate)
  - sox效果器: 音量(vol), 镶边(flanger), 颤音(tremolo), 回音(echos), 合唱(chorus), 卡拉OK(oops), 均衡器(equalizer), 高通(highpass), 低通(lowpass), 等等
- 支持无限客制化块
- 支持每块最多32个输入端点，32个输出端点
- 支持基于帧的时间戳和基于采样点的时间戳
- 支持实时调试工具
  - 打印状态
  - 打印路由快照到文件
  - 实时设定日志等级
  - 转储日志到文件
  - 转储各个块的音频数据到文件
- 跨平台
  - Linux (x86_64), MacOS (x86_64), Android (aarch64)

## 开始吧

***百灵鸟*** 是由一个基础的动态库`liblark.so`，各种可选的块（动态加载库）例如`libblkmixer.so`，和一个可选的可执行调试工具`lkdb`组成。基于 ***百灵鸟***，您可以很容易地创建您想实现的既可以简单又可以复杂的音频路由，实时启动/停止任意一个路由，或者实时设定/获取任意块的参数。

### 第1步

由于 ***百灵鸟*** 依赖于klogging库，您首先需要安装klogging。访问 <https://gitee.com/wksuper/klogging> 以安装之。另一种方式是，此仓库已经包含了预编译好的`libklogging.so`，它会随着 ***百灵鸟*** 的安装而一起安装。

### 第2步

安装 ***百灵鸟*** 到您的Linux机器。

```bash
$ cd lark-release
$ sudo ./install.sh
$ sudo ldconfig
```

当您不需要 ***百灵鸟*** 时，您可以使用`sudo ./uninstall.sh`来移除它。

### 第3步

运行音频路由。

#### 例1：运行一个单路由，做立体声音乐文件播放

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

如果没错误的话，音乐文件应该开始播放了。

这个例子的源代码在此：[larkexample1.cpp](https://gitee.com/wksuper/lark-release/blob/master/examples/larkexample1.cpp)。

#### 例2：运行一个单路由，把两个音乐文件输入做带各自增益的混合播放

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

运行例2之前，需要先安装 ***PortAudio*** 库。

```bash
$ sudo apt install libportaudio2
```

运行例2：

```bash
$ x86_64-linux-gnu/bin/larkexample2
```

如果没错误的话，两个音乐文件应该开始混合播放输出了。

开启另一个shell，在里面您可以通过`lkdb`调节他们的音量增益。

```bash
$ lkdb status     # 显示百灵鸟状态
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
$ lkdb setparam RouteA blkgain_0 1 0 0.5    # kanr-48000_16_2.pcm左声道的输出音量变低
$ lkdb setparam RouteA blkgain_0 1 1 0.5    # kanr-48000_16_2.pcm右声道的输出音量变低
$ lkdb setparam RouteA blkgain_0 1 0 0.0    # kanr-48000_16_2.pcm左声道的输出静音
$ lkdb setparam RouteA blkgain_0 1 1 0.0    # kanr-48000_16_2.pcm右声道的输出静音

$ lkdb setparam RouteA blkgain_0 1 2 0.5    # pacificrim-48000_16_2.pcm左声道的输出音量变低
$ lkdb setparam RouteA blkgain_0 1 3 0.5    # pacificrim-48000_16_2.pcm右声道的输出音量变低
$ lkdb setparam RouteA blkgain_0 1 2 0.0    # pacificrim-48000_16_2.pcm左声道的输出静音
$ lkdb setparam RouteA blkgain_0 1 3 0.0    # pacificrim-48000_16_2.pcm右声道的输出静音

$ lkdb setparam RouteA blkgain_0 1 0 1.0    # kanr-48000_16_2.pcm左声道的输出音量恢复
$ lkdb setparam RouteA blkgain_0 1 1 1.0    # kanr-48000_16_2.pcm右声道的输出音量恢复

$ lkdb setparam RouteA blkgain_0 1 2 1.0    # pacificrim-48000_16_2.pcm左声道的输出音量恢复
$ lkdb setparam RouteA blkgain_0 1 3 1.0    # pacificrim-48000_16_2.pcm右声道的输出音量恢复
```

这个例子的源代码在此：[larkexample2.cpp](https://gitee.com/wksuper/lark-release/blob/master/examples/larkexample2.cpp)。

#### 例3：运行双路由，一个做音乐播放，一个采集麦克风以做关键词侦测

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

运行例3之前，需要先安装speex库。

```bash
$ sudo apt install libspeexdsp-dev
```

运行例3：

```bash
# 两个参数指的是alsa playback pcm name和capture pcm name。
# 它们随着机器的不同而不同。
$ x86_64-linux-gnu/bin/larkexample3 plughw:0,0 plughw:0,0
```

如果没错误的话，播放和录音应该同时开始了。
录到的声音文件存放在`./cap-16000_16_1.pcm`。

这个例子的源代码在此：[larkexample3.cpp](https://gitee.com/wksuper/lark-release/blob/master/examples/larkexample3.cpp)。

#### 例5：运行一个单播放路由，嵌入了 ***SoundTouch*** 以做音调、节拍、播放速率的调节

```
RouteA

 libblkfilereader.so   libblkformatdapter.so    libblksoundtouch.so   libblkpaplayback.so
  ****************      *******************      ****************      ****************
  *              *      *                 *      *              *      *              *
  * filereader_0 *0--->0* formatadapter_0 *0--->0* soundtouch_0 *0--->0* paplayback_0 *
  *              *      *                 *      *              *      *              *
  ****************      *******************      ****************      ****************
```

运行例5之前需要先安装 ***SoundTouch*** 库和 ***PortAudio*** 库。

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
$ lkdb status                                   # 显示百灵鸟状态
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

这个例子的源代码在此：[larkexample5.cpp](https://gitee.com/wksuper/lark-release/blob/master/examples/larkexample5.cpp)。

#### 例7：运行一个带SoX效果器的单路由，输出到ffplay

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

运行例7之前需要先安装 ***SoX*** 库和 ***ffmpeg*** 。

```bash
$ brew install sox
$ brew install ffmpeg
```

运行例7：

```bash
$ x86_64-apple-darwin/bin/larkexample7 | ffplay -i pipe:0 -f s32le -ar 48000 -ac 2 -autoexit
```

如果没错误的话，播放应该开始了。同时在ffplay的播放画面上，音频频谱正在被实时绘制如下：

![larkexample7-1](./examples/larkexample7-1.png)

在另一个shell里，

```bash
$ lkdb status    # 显示百灵鸟状态，找到高通/低通滤波器的块名称分别为'blksoxeffect_highpass_0'/'blksoxeffect_lowpass_0'
```

```bash
$ lkdb setparam RouteA blksoxeffect_highpass_0 0 800    # 只有高于800Hz的音乐信号输出到左喇叭
```

音频频谱实时绘制如下：

![larkexample7-2](./examples/larkexample7-2.png)

> 贴士：在ffplay的音频频谱中，红色代表左声道，绿色代表右声道。如果在某个时刻点某个频率显示为灰色，则表示左右声道在那个时刻的那个频率上能量均等。

```bash
$ lkdb setparam RouteA blksoxeffect_lowpass_0 0 800    # 只有低于800Hz的音乐信号输出到右喇叭
```

音频频谱实时绘制如下：

![larkexample7-3](./examples/larkexample7-3.png)

ffplay频谱颜色的实时变化验证了集成在 ***百灵鸟*** 路由里的sox效果器在实时生效。

这个例子的源代码在此：[larkexample7.cpp](https://gitee.com/wksuper/lark-release/blob/master/examples/larkexample7.cpp)。

### 更多的例子

[作者的博客文章：<我的工程师爸爸-音频应用DIY系列>](https://blog.csdn.net/weixin_44278307?type=blog)通过一系列小故事展示了利用 ***百灵鸟*** 来编程解决生活中遇到的音频问题。

## 用`lkdb`实时调试

当 ***百灵鸟*** 在一个进程里运行时，调试工具`lkdb`可以跟它通信，从 ***百灵鸟*** 获取信息，向 ***百灵鸟*** 发送命令。

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

除了以上例子中显示的实时打印状态、设定参数功能之外，`lkdb`还可以操作音频路由、抓取路由快照、改变日志等级、设定音频数据转储等。

### 不用写代码编译，通过`lkdb`跑一个跟例1一样的路由

在一个shell里，

```bash
$ x86_64-linux-gnu/bin/larkexample0    # 运行百灵鸟
```

在另一个shell里，

```bash
$ lkdb newroute RouteA
Created RouteA
$ lkdb newblock RouteA libblkfilereader.so true false examples/kanr-48000_16_2.pcm
Created blkfilereader_0 from libblkfilereader.so on RouteA
$ lkdb newblock RouteA libblkalsaplayback.so false true
Created blkalsaplayback_0 from libblkalsaplayback.so on RouteA
$ lkdb newlink RouteA 48000 S16_LE 2 960 blkfilereader_0 0 blkalsaplayback_0 0
Created lnk_0 on RouteA
$ lkdb start RouteA      # 音乐播放应该开始了
Started RouteA
$ lkdb stop RouteA
Stopped RouteA
$ lkdb delete RouteA !
Deleted RouteA
```

这个例子的源代码在此：[larkexample0.cpp](https://gitee.com/wksuper/lark-release/blob/master/examples/larkexample0.cpp)。

### 通过`lkdb`抓取例3路由的快照

当例3运行时，

```bash
$ lkdb status --dot | dot -Tpng -o larkexample3.png
```

路由快照会被存成文件`larkexample3.png`。

![larkexample3.png](./examples/larkexample3.png)

注意：这个功能需要graphviz(dot)先安装到您的机器。

```bash
$ sudo apt install graphviz
```

## 创建您自己的音频路由

### 第1步

在您的脑海中设计好路由。那些预编译好的块（像gain, mixer等）可以直接使用。如果您想要客制化的块，您可以自行编译动态库。
[BlkPassthrough.cpp](https://gitee.com/wksuper/lark-release/blob/master/examples/BlkPassthrough.cpp)是一个块源码的例子。
参考[MANUAL.md - 4 Build Your Own Block](https://gitee.com/wksuper/lark-release/blob/master/MANUAL.md#4-build-your-own-block)以获取更多信息。

### 第2步

在您的进程里调用 ***百灵鸟*** 的API来实现您的路由。[样例](https://gitee.com/wksuper/lark-release/tree/master/examples)已经列出。参考[用户手册](https://gitee.com/wksuper/lark-release/blob/master/MANUAL.md)以获取更多信息。

## 常见问题与回答

**Q1**: “块”是什么？“端点”是什么？“链接”是什么？“路由”是什么？

**A**: 一个“块”就是一个软件DSP处理单元。它从一个动态加载库里被创建，并被 ***百灵鸟*** 路由所管理。它接受最多32路输入（即输入端点）数据，处理它们，把处理后的数据填入输出端点（最多32路输出）。
通常情况，一个端点代表一个声道音频，然而一个端点也可以运载多声道。例如“interleave”块的一个输出端点就可以运载多声道。
一个“链接”连接一个块的一个输出端点（作为源）至另一个块的一个输入端点（作为汇）。 一个“路由”是一个由多个块连接成的链条（或者图）。

**Q2**: 在一个机器上，我可以同时运行几个 ***百灵鸟*** 实例？在一个 ***百灵鸟*** 中，我可以创建几个路由？在一个路由中，我可以创建多少个块？多少个first块？多少个last块？

**A**: 在一个机器上，只有一个进程被容许创建一个 ***百灵鸟*** 实例。也就是说，一个机器运行一个 ***百灵鸟*** 实例。`lkdb`会跟这个唯一的 ***百灵鸟*** 实例通信。在一个 ***百灵鸟*** 中，最多可以创建26个路由，它们的名字需要被手动指定为"RouteA", "RouteB", ..., "RouteZ"。对于每个路由，名字是需要的，并且应该固定为"Route#"（大小写敏感），这里“#”代表[A-Z]。
在一个路由里，创建块没有数量限制。所有从一个路由里创建出来的块/链接都属于这个路由，也就是意味着它们的资源是被这个路由统一管理。
同样，创建first块和last块也没有数量限制。

**Q3**: 什么是“first块”？什么是“last块”？

**A**: 在一个路由里，至少需要创建一个“first块”和一个“last块”，这些块在创建的时候，都有“first”标志和“last”标志被各自指定。
这样的话，路由就可以知道从哪些块开始处理，到哪些块结束处理。从first块到last块之间，所有的块应该要被连接起来，不要有切断。只有所有连接都完整了，路由才可以启动。如果路由中间有切断，这是被容许的，但是路由就无法运行。如果您强制运行，路由将会进入BROKEN_RUNNING状态，直到切断被再接起来。

**Q4**: “多路由”跟“单路由多first块”有何不同？如果我有多路输入作为first块，我要如何在“多路由”和“单路由多first块”之间做选择？

**A**: 一个路由用一个线程处理数据。通常情况，“单路由多first块”可以工作地很好。在这种情况下，多输入以同样的步伐提供数据帧，并且它们不应该被相互阻塞。例如，一个输入是alsacapture，一个输入是filereader。
需要用到“多路由”的场景是，当多个输入放在一个路由里运行会有机会相互阻塞时，那么它们就应该被分离到多个路由。例如，一个输入alsacapture，一个输入是echo-reference。

## 基于百灵鸟的开源应用

- [kplay](https://gitee.com/wksuper/kplay) - 一个可以实时调音的WAV文件播放器
