# Image for Node.js

Image crossplatform addon with minimized dependencies.


## Install

```
npm i -s image-raub
```

Note: as this is a compiled addon, compilation tools must be in place on your system.
Such as MSVS13 for Windows, where **ADMIN PRIVELEGED** `npm i -g windows-build-tools`
most probably helps.


## Usage

This module implements Image class suitable for loading FreeImage supported formats.

Example usage:

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

Here `Image` is used to load a texture. Constructed object receives src property,
then the file is read and `'load'` event emitted. After this `image.data` is
available as a Buffer, containing whole pixel data, and `image.width`/`image.height`
contain image dimensions.

Image class is supposed to provide the basic interface, which other libs, such as
three.js are looking for. However, some additional features were added, and some removed.


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

* `on(string type, function cb)` - listen for Image events.
* `save(string dest, number ?w, number ?h)` - save Image to file. Resize to `w, h` is optional.


### Events:

* `'load'` - the requested file was loaded.
* `'error'` - an error has occured.
