# Image for Node.js

This is a part of [Node3D](https://github.com/node-3d) project.

[![NPM](https://nodei.co/npm/image-raub.png?compact=true)](https://www.npmjs.com/package/image-raub)

[![Build Status](https://api.travis-ci.com/node-3d/image-raub.svg?branch=master)](https://travis-ci.com/node-3d/image-raub)
[![CodeFactor](https://www.codefactor.io/repository/github/node-3d/image-raub/badge)](https://www.codefactor.io/repository/github/node-3d/image-raub)

> npm i image-raub


## Synopsis

**Node.js** N-API addon implementing the
[HTMLImageElement](https://developer.mozilla.org/en-US/docs/Web/API/HTMLImageElement/Image).

> Note: this **addon uses N-API**, and therefore is ABI-compatible across different
Node.js versions. Addon binaries are precompiled and **there is no compilation**
step during the `npm i` command.

The `src` property can be:
* A local file.
* A data URI.
* A http(s) URL.


[FreeImage](http://freeimage.sourceforge.net/) is used as backend decoder.


Additional features include:
* Save the image to disk.
* Create an Image from raw RGBA pixel data.
* Some context 2d functionality, e.g. `drawImage`.


## Usage

### Load an OpenGL texture

```js
const Image = require('image-raub');
const image = new Image();
// Web-style onload
image.onload = () => {
	gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
	gl.bindTexture(gl.TEXTURE_2D, texture);
	gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, image);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR_MIPMAP_NEAREST);
	gl.generateMipmap(gl.TEXTURE_2D);
	gl.bindTexture(gl.TEXTURE_2D, null);
};

image.src = `${__dirname}/texture.jpg`;
```

Here `Image` is used to load a texture. The constructed object receives `src` property,
then the file is read and `'load'` event is emitted. After that, `image.data` is
available as a `Buffer`, containing the whole pixel data, and `image.width`/`image.height`
contain the dimensions.

### Make an OpenGL snapshot

```js
const memSize = screen.w * screen.h * 4; // estimated number of bytes
const storage = { data: Buffer.allocUnsafeSlow(memSize) };

gl.readPixels(
	0, 0,
	screen.w, screen.h,
	gl.RGBA,
	gl.UNSIGNED_BYTE,
	storage
);

const img = Image.fromPixels(screen.w, screen.h, 32, storage.data);

img.save(`${Date.now()}.jpg`);
```

Image can save its current content to the filesystem. It can also load from raw
pixel values using `static fromPixels()` method.


### Properties

* `get/set number width/naturalWidth` - image width.
* `get/set number height/naturalHeight` - image height.
* `get/set [width, height] wh` - image width and height.
* `get/set {width, height} size` - image width and height.
* `get/set string src` - image source file.
* `get/set function onload` - setter uses `.on('load', ...)` method, getter returns all listeners.
* `get/set function onerror` - setter uses `.on('error', ...)` method, getter returns all listeners.
* `get boolean complete` - if image has already been loaded.


### Methods:

* `save(string dest)` - save Image to file. Resize to `w, h` is optional.
* `drawImage(src, ...)` - refer to
[drawImage](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/drawImage),
but optional params `dx, dy` are ignored.
* `load(Buffer data)` - load from a file in memory.
* `static fromPixels(uint width, uint height, uint bpp, Buffer pixels)` - create
image from raw pixels. For example it helps when you want to store the result
of `glReadPixels()` as image.


### Events:

* `'load'` - the requested (`src`) file was loaded.
* `'error'` - an error has occured.
