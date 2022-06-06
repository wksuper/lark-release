# User Manual

## 1 class Lark

- Header file: <lark/lark.h>
- Library: `liblark.so` or `liblark.dylib` or `liblark.dll`
- Public APIs

### 1.1 `static Lark &Instance();`

Return the unique ***lark*** instance.

### 1.2 `virtual Route *NewRoute(const char *name, Route::Callbacks *cbs = nullptr) = 0;`

Create a route named `name` which be one of the 26 routes "RouteA", "RouteB", ..., "RouteZ".
The `cbs` is an optional argument. It is used when something need to be done on route started or on route stopped or on route exits.
Note: DO NOT call route APIs(like route->SetParameter() etc.) in these callbacks, otherwise deadlock will be caused.

### 1.3 `virtual void DeleteRoute(Route *route) = 0;`

Manually delete a route.
This is not a mandatory action when the route is not used since all the routes' resources are managered by the ***lark*** instance,
and all of them will be deleted automatically when the ***lark*** instance is destroying.

### 1.4 `virtual FIFO *NewFIFO(unsigned int rate, size_t sampleSizeInBytes, samples_t bufSizeInSamples, samples_t delayInSamples = 0) = 0;`

Create a FIFO which can be used by two threads, one for writing data, one for reading data.
As an example, please refer to [larkexample3.cpp](https://gitee.com/wksuper/lark-release/blob/master/examples/larkexample3.cpp).

### 1.5 `virtual void DeleteFIFO(FIFO *fifo) = 0;`

Manually delete a fifo.
This is not a mandatory action when the fifo is not used since all the fifos' resources are managered by the ***lark*** instance,
and all of them will be deleted automatically when the ***lark*** instance is destroying.

## 2 class Route

- Header file: <lark/Route.h>
- Library: `liblark.so` or `liblark.dylib` or `liblark.dll`
- Public APIs

[TODO]

## 3 Prebuilt Blocks

### 3.1 BlkGain

The BlkGain block is able to add gain on its input endpoints' data, and the gain takes effect immediately.
Internally, the data from the input endpoint index N will go to the output endpoint with the same index N. i.e. I00 goes to O00, I01 goes to O01, and so on.

- SO Name
  - "libblkgain.so"
- Creation Arguments
  - None
- SetParameter Arguments
  - PARAMID 1: set gain
  - PARAMS: INEPIDX1 FLOATGAIN1 [INEPIDX2 FLOATGAIN2 ...]
- e.g. To set 0.707 to input endpoint 3 and 0.5 to input endpoint 8

c++:

```c++
lark::Parameters args{ "3", "0.707", "8", "0.5" };
route->SetPrameter(blk, 1, args);
```

shell:

```bash
$ lkdb setparam RouteA blkgain_0 1 3 0.707 8 0.5
```

- SetParameter Arguments
  - PARAMID 2: [TODO]
- GetParameter Arguments
  - None
- Pending by No Input?
  - Yes

### 3.2 BlkMixer

The BlkMixer block is able to mix multiple input endpoints(mono per endpoint) into one mono output endpoint.
The block also has the functionality of duplicating output.
When the first output endpoint is linked, the mixed stream will go to this output endpoint.
When the second output endpoint is linked, the mixed stream will be duplicated and also go to the second output endpoint.
So does the third, fourth, etc. output endpoint.

- SO Name
  - "libblkmixer.so"
- Creation Arguments
  - PARAMID 1: mixing coefficient
  - PARAMS: [INEPIDX1 COEF1] [INEPIDX2 COEF2] ...
- SetParameter Arguments
  - None
- GetParameter Arguments
  - None
- Pending by No Input?
  - Yes

### 3.3 BlkInterleave

The BlkInterleave block is able to merge multiple input endpoints(mono per endpoint) into one interleaved output endpoint.

- SO Name
  - "libblkinterleave.so"
- Creation Arguments
  - None
- SetParameter Arguments
  - None
- GetParameter Arguments
  - None
- Pending by No Input?
  - Yes

### 3.4 BlkDeinterleave

The BlkDeinterleave block is able to split one interleaved input endpoint into multiple mono output endpoints.

- SO Name
  - "libblkdeinterleave.so"
- Creation Arguments
  - None
- SetParameter Arguments
  - None
- GetParameter Arguments
  - None
- Pending by No Input?
  - Yes

### 3.5 BlkDelay

The BlkDelay block is able to add delay between the input endpoints and the output endpoints.

- SO Name
  - "libblkdelay.so"
- Creation Arguments
  - None
- SetParameter Arguments
  - PARAMID 1: set delay
  - PARAMS: DELAYTIME (in micro-second)
- GetParameter Arguments
  - None
- Pending by No Input?
  - Yes

### 3.6 BlkBuffer

The BlkBuffer block is able to buffer amount of data coming from the unique input endpoint.
When the buffer is full, the oldest sample will be overwritten.
To read the buffered data, use the `ioctl` API with `id_t id` to be 0, `void *data` to be a pointer to

```c++
struct BlkBufferPullData {
    // [out] *buf is the output data
    void *buf;

    // [in] buf size in bytes
    size_t size;

    // [out] the output data timestamp
    int64_t timestamp;
};
```

- SO Name
  - "libblkbuffer.so"
- Creation Arguments
  - RATE FORMAT CHNUM BUFFERTIME
- SetParameter Arguments
  - None
- GetParameter Arguments
  - None
- Pending by No Input?
  - Yes

### 3.7 BlkStreamIn

The BlkStreamIn block is able to pull data from route outside via DataProducer passed to it when creation.
It must have to be a 'first' block in a route.

- SO Name
  - "libblkstreamin.so"
- Creation Arguments
  - RATE FORMAT CHNUM DATAPRODUCER
- SetParameter Arguments
  - None
- GetParameter Arguments
  - None
- Pending by No Input?
  - No

### 3.8 BlkStreamOut

The BlkStreamOut block is able to push data to route outside via DataConsumer passed to it when creation.
It must have to be a 'last' block in a route.

- SO Name
  - "libblkstreamout.so"
- Creation Arguments
  - RATE FORMAT CHNUM DATACONSUMER
- SetParameter Arguments
  - None
- GetParameter Arguments
  - None
- Pending by No Input?
  - Yes

### 3.9 BlkAlsaPlayback

The BlkAlsaPlayback block is able to write interleaved data from the input endpoint to ALSA lib.
The RATE, FORMAT, CHNUM are decided by the input endpoint when it is linked.
BlkAlsaPlayback can accept output endpoints to duplicate the data(with timestamp) to that it already wrote to ALSA lib.

- SO Name
  - "libblkalsaplayback.so"
- Creation Arguments
  - PCMNAME  (e.g. "default", "hw:0,0", ...)
  - No argument means pcm name is "default"
- SetParameter Arguments
  - None
- GetParameter Arguments
  - None
- Pending by No Input?
  - Yes

### 3.10 BlkAlsaCapture

The BlkAlsaCapture block is able to read interleaved data from ALSA lib to its output endpoint.
It should be a 'first' block in a route.
The RATE, FORMAT, CHNUM are decided by the output endpoint when it is linked.
This block depends on `libasound.so`.

- SO Name
  - "libblkalsacapture.so"
- Creation Arguments
  - PCMNAME  (e.g. "default", "hw:0,0", ...)
  - No argument means pcm name is "default"
- SetParameter Arguments
  - None
- GetParameter Arguments
  - None
- Pending by No Input?
  - N/A

### 3.11 BlkFileReader

The BlkFileReader block is able to read data from the PCM audio file and output to the unique output endpoint(mono or multi-interleaved-channels).
It should be a 'first' block in a route.

- SO Name
  - "libblkfilereader.so"
- Creation Arguments
  - FILEPATH
- SetParameter Arguments
  - None
- GetParameter Arguments
  - None
- Pending by No Input?
  - N/A

### 3.12 BlkFileWriter

The BlkFileWriter block is able to write the mono or multi-interleaved-channels data from its unique input endpoint into a PCM audio file.
It should be a 'last' block in a route.

- SO Name
  - "libblkfilewriter.so"
- Creation Arguments
  - FILEPATH
- SetParameter Arguments
  - None
- GetParameter Arguments
  - None
- Pending by No Input?
  - Yes

### 3.13 BlkPassthrough

The BlkPassthrough block bypass its input endpoints to its output endpoints without any change.
The data from the input endpoint index N will go to the output endpoint with the same index N. i.e. I00 goes to O00, I01 goes to O01, and so on.

- SO Name
  - "libblkpassthrough.so"
- Creation Arguments
  - None
- SetParameter Arguments
  - None
- GetParameter Arguments
  - None
- Pending by No Input?
  - Yes

### 3.14 BlkSpeexResampler

The BlkSpeexResampler block resample its input endpoints(one channel per endpoint) to its output endpoints(one channel per endpoint).
All the input endpoints should be at the same sampling rate. The input rate is decided by its link linked.
All the output endpoints should be at the same sampling rate. The output rate is decided by its link linked.
This block depends on `libspeexdsp.so`.

- SO Name
  - "libblkspeexresampler.so"
- Creation Arguments
  - None
- SetParameter Arguments
  - None
- GetParameter Arguments
  - None
- Pending by No Input?
  - Yes

### 3.15 BlkSpeexPreprocessor

The BlkSpeexPreprocessor block performs echo cancelation and denoise on its microphone input endpoints.
Input endpoint index 0 to 15 represent the microphone channels(one channel per endpoint).
Input endpoint index 16 to 31 represent the echo-reference channels(one channel per endpoint).
The clean microphone signals are output to the output endpoints(one channel per endpoint).
This block depends on `libspeexdsp.so`.

- SO Name
  - "libblkspeexpreprocessor.so"
- Creation Arguments
  - None
- SetParameter Arguments
  - None
- GetParameter Arguments
  - None
- Pending by No Input?
  - Yes

### 3.16 BlkAlign

The BlkAlign block is able to align the microphone input endpoint(mono or interleaved multi-channel) with the echo-reference input endpoint(mono or interleaved multi-channel) via their timestamps.
The microphone input endpoint must be fixed to index 0. The echo-reference input endpoint must be fixed to index 1.
The microphone output endpoint must be fixed to index 0. The echo-reference output endpoint must be fixed to index 1.
Most of AEC algorithms require the echo-reference signal arrives ahead of the microphone signal with a constant delay. This block can achieve this by setting the constant delay parameter when creation.

- SO Name
  - "libblkalign.so"
- Creation Arguments
  - CONSTANTDELAY (in micro-second)
- SetParameter Arguments
  - None
- GetParameter Arguments
  - None
- Pending by No Input?
  - Yes

### 3.17 BlkFormatAdapter

The BlkFormatAdapter block is able to convert the input endpoint data from a format to another format sent to the output endpoint.
Internally, the data from the input endpoint index N will go to the output endpoint with the same index N. i.e. I00 goes to O00, I01 goes to O01, and so on.

- SO Name
  - "libblkformatadapter.so"
- Creation Arguments
  - None
- SetParameter Arguments
  - None
- GetParameter Arguments
  - None
- Pending by No Input?
  - Yes

### 3.18 BlkSoundTouch

The BlkSoundTouch block is able to increase/decrease sound pitch, speed-up/slow-down sound tempo, and speed-up/slow-down sound rate.
It only accept 1 input endpoint with mono or interleaved float channel and 1 output endpoint with mono or interleaved float channel.
This block depends on `libSoundTouch.so`.

- SO Name
  - "libblksoundtouch.so"
- Creation Arguments
  - None
- SetParameter Arguments
  - PARAMID 1: set pitch
  - PARAMS: PITCH
    - Original PITCH=1.0, smaller values represent lower pitches, larger values higher pitch
  - PARAMID 2: set tempo
  - PARAMS: TEMPO
    - Normal TEMPO=1.0, smaller values represent slower tempo, larger faster tempo
  - PARAMID 3: set rate
  - PARAMS: RATE
    - Normal RATE=1.0, smaller values represent slower rate, larger faster rates
- GetParameter Arguments
  - None
- Pending by No Input?
  - Yes

## 4 Build Your Own Block

### 4.1 Write Your Block Source Code

Customizable blocks are very easy to be created by following below five steps.

In your MyBlock.cpp,

Step 1 - #include <lark/lark.h>

Step 2 - Define your block class publicly derived from the pure virtual base class lark::Block.

Step 3 - Your block class should contain a constructor to explicitly construct the base class lark::Block **with the 'first' flag and the 'last' flag**.

Step 4 - Override the `virtual lark::samples_t ProcessFrame()` of base class lark::Block.

```c++
#include <lark/lark.h>

class MyBlock : public lark::Block {
public:
    MyBlock(bool first, bool last) : Block(first, last) { ... }

private:
    virtual lark::samples_t ProcessFrame() override final { ... }
};
```

Step 5 - Define a non-static C function named 'CreateBlock' in which your block instance on the heap is returned.

```c++
extern "C" lark::Block *CreateBlock(bool first, bool last, const lark::Parameters &args)
{
    return new MyBlock(first, last);
}
```

### 4.2 Compile It into SO Library

When linking, don't forget to link liblark.so.  e.g.

```bash
$ g++ MyBlock.cpp -fPIC -shared -llark -o libmyblock.so
```

### 4.3 Install Your Library

```bash
$ sudo install libmyblock.so /usr/local/lib/
```

### 4.4 Create Your Block in The Route

c++:

```c++
lark::Block *blk = route->NewBlock("libmyblock.so", ...);
```

shell:

```bash
$ lkdb newblock RouteX libmyblock.so ...
```

## 5 Members of The Base Class lark::Block

When deriving from the base class lark::Block, its public and protected members are able to be accessed. And all the virtual functions can be overridden.

### 5.1 `bool ActiveOutEpsDataIsEmpty() const;`

As said by the function name, this function returns true if any one of the active output endpoints' data is empty, and false on otherwise. It often helps to check whether the output data has been fetched by the next block(s) or not when `ProcessFrame()` is called.

### 5.2 `bool ActiveInEpsDataIsReady() const;`

As said by the function name, this function returns true if all the active input endpoints' data is ready, and false on otherwise. It often helps to check if the input data is ready or not when `ProcessFrame()` is called.

### 5.3 `void ClearActiveInEpsData();`

As said by the function name, this function makes all the input endpoints' data empty.

### 5.4 `InputEndpoint &InEp(size_t idx);`

Returns the InputEndpoint instance with index 'idx'.

### 5.5 `OutputEndpoint &OutEp(size_t idx);`

Returns the OutputEndpoint instance with index 'idx'.

### 5.6 `const std::map<size_t, InputEndpoint*> &ActiveInEps() const;`

Returns the active input endpoints.
'map.first' is the input endpoint index, 'map.second' is the input endpoint pointer.

### 5.7 `const std::map<size_t, OutputEndpoint*> &ActiveOutEps() const;`

Returns the active output endpoints.
'map.first' is the output endpoint index, 'map.second' is the output endpoint pointer.

### 5.8 `virtual int OnInEpLinked(size_t epIdx);`

When the input endpoint 'epIdx' is linked, this function will be called, and returns lark::ErrorType.
It can be overridden.

### 5.9 `virtual void OnInEpUnlinked(size_t epIdx);`

When the input endpoint 'epIdx' is unlinked, this function will be called.
It can be overridden.

### 5.10 `virtual int OnOutEpLinked(size_t epIdx);`

When the output endpoint 'epIdx' is linked, this function will be called, and returns lark::ErrorType.
It can be overridden.

### 5.11 `virtual void OnOutEpUnlinked(size_t epIdx);`

When the output endpoint 'epIdx' is unlinked, this function will be called.
It can be overridden.

### 5.12 `virtual int Start();`

When the route this block belongs to is starting, this function will be called, and returns lark::ErrorType.
It can be overridden.

### 5.13 `virtual samples_t ProcessFrame() = 0;`

After the route this block belongs to started, this ProcessFrame() function could be called at any time.
It must be overridden.
In this function, ActiveInEps() is the input data and ActiveOutEps() is the output data.
Important: After processing, don't forget to update FB().availSampleNum in ActiveInEps()'s InputEndpoint and ActiveOutEps()'s OutputEndpoint respectively.
Return the actual processed sample number on success or a negative lark::ErrorType value on failure.

### 5.14 `virtual int Stop();`

When the route this block belongs to is stopping, this function will be called, and returns lark::ErrorType.
It can be overridden.

### 5.15 `virtual int SetParameter(id_t paramId, const Parameters &params);`

This function can pass parameters from outside at any time, and returns lark::ErrorType.
paramId: the parameter ID defined by this block
params: the parameters associated with paramId defined by this block
It can be overridden.

### 5.16 `virtual int GetParameter(id_t paramId, Parameters &params);`

This function can pass parameters to outside at any time, and returns lark::ErrorType.
paramId: the parameter ID defined by this block
params: the parameters associated with paramId defined by this block
It can be overridden.

### 5.17 `virtual void Reset();`

This function resets the block internal state.
It could be called when the route encounters an error frame.
It can be overridden.

### 5.18 `virtual size_t Delay() const;`

This function returns the delay(in micro-secondond) from the input endpoint(s) to the output endpoint(s).
It could be called at any time.
It can be overridden.

## 6 Configuration File

***lark*** supports some parameters to be configured before being launched.

If `/etc/lark.conf` exists, ***lark*** will read configurations from it, otherwise default configurations will be applied.

A typical `/etc/lark.conf` looks like:

```conf
[config]
loglevel=6
dumppath=/your/lark/dump/path
```

where `loglevel`'s value is interpreted as

- 0: off
- 1: fatal
- 2: error
- 3: warning
- 4: info
- 5: debug
- 6: verbose

`dumppath`'s value is interpreted as the directory in which the data dump and log dump will be saved.

If none of them is assigned, then lark will use loglevel=4(info) as default and no log/data dump as default.

Note: `lkdb loglevel` and `lkdb setdump` only change the log level and dump path in real time, and do not take effect on this configuration file.
