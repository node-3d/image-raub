# Image for Node.js

This is a part of [Node3D](https://github.com/node-3d) project.


## Synopsis

**Node.js** addon implementing the
[HTMLImageElement](https://developer.mozilla.org/en-US/docs/Web/API/HTMLImageElement/Image).

The `src` property can be:
* A local file.
* A data URI.
* A http(s) URL.

Uses [FreeImage](http://freeimage.sourceforge.net/) as a backend decoder.


## Install

```
npm i -s image-raub
```

> Note: compilation tools must be in place on your system.
For Windows, use **ADMIN PRIVELEGED** command line:
\`npm i -g windows-build-tools\`.
Also **Windows** needs **vcredist 2013** to be installed.


## Usage

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

* `addEventListener|on(string type, function cb)` - listen for Image events.
* `once(string type, function cb)` - listen for one Image event.
* `save(string dest, number ?w, number ?h)` - save Image to file. Resize to `w, h` is optional.


### Events:

* `'load'` - the requested file was loaded.
* `'error'` - an error has occured.
