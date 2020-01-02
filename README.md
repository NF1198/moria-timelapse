# Moria Timelapse

Moria is a timelapse recording utility. This tool can also be used to capture static backgrounds from dynamic scenes.

## Theory of operation

Moria applies a temporal low-pass filter to pixel data. Images are captured from the camera at a relatively higher
frame rate then temporally low-pass filtered. This effectively allows the photographer to choose a virtual exposure
time of arbitrary length. Images are saved at a user-defined interval independent of the virtual exposure time.

For example, you can capture images from a camera at 15 fps, apply a filter period of 300 seconds (5 minutes) and save images every 60 seconds. The resulting images will have a virtual exposure time of 5 minutes.

### Tips

Filter results depend on floating point rounding errors. Excessively high frame rates can result in 
an unstable image due to high gain in the image filter. Run moria with the `--verbose` option to reveal
the gain used in the filter calculation. Gains of less than 20,000 should result in stable filter operation.

# Usage

```
$ moria --help
================================================
moria-timelapse
Timelapse recording utility
================================================

Allowed options:

Generic options:
  --version                   print version string
  --help                      show help message
  -c [ --config ] arg         name of configuration file.

Configuration:
  -d [ --device ] arg (=0)    default device ID (uses system default backend, 
                              e.g. v4l2 on *nix or vfw on win32)
  --gst arg                   gstreamer pipeline (will be used instead of 
                              deviceID, if specified); pipeline must end with 
                              "! appsink"
  -w [ --width ] arg (=320)   frame width (ignored if using gst pipeline)
  -h [ --height ] arg (=240)  frame height (ignored if using gst pipeline)
  --fps arg (=10)             capture frames per second (target; ignored if 
                              using gst pipeline)
  --save-interval arg (=10)   interval (seconds) at which frames are saved to 
                              disk
  --filter-period arg (=1)    virtual shutter speed (seconds)
  -O [ --output ] arg         output directory
  --utc arg (=0)              use UTC timestamps
  --timestamp arg (=1)        write timestamp in frame
  -v [ --verbose ]            verbose output
```

### Example using v4l2 backend (default on *nix)

```
$ moria -d 0 --width=640 --height=480 --filter-period=60 --save-interval=10 --output=/tmp/moria
```

### Example using gstreamer pipeline with H264 webcam

```
./moria --gst "v4l2src device=/dev/video2 ! queue !  video/x-h264,width=960,height=544,framerate=30/1 ! h264parse ! avdec_h264 ! videoconvert ! appsink" --filter-period=300 --save-interval=60 --output=/tmp/moria
```

### Example demonstrating how to make a video of recorded images (uses ffmpeg)

```
[/tmp/moria]$ cat *.jpg */*.jpg | ffmpeg -f image2pipe -r 30 -i - -vcodec libx264 -crf 25 -pix_fmt yuv420p timelapse.mp4
```

# Dependencies

* cmake
* opencv
* gstreamer

# Building

1) clone repository
2) run `./configure`
3) resolve dependency issues
4) run `cmake --build build`

# Credits 

Based on <a href="https://github.com/cginternals/cmake-init/"><img src="https://raw.githubusercontent.com/cginternals/cmake-init/master/cmake-init-logo.svg?sanitize=true" width="15%"></a>
