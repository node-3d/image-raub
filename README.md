# Image for Node.js

This is a part of [Node3D](https://github.com/node-3d) project.

[![NPM](https://badge.fury.io/js/image-raub.svg)](https://badge.fury.io/js/image-raub)
[![ESLint](https://github.com/node-3d/image-raub/actions/workflows/eslint.yml/badge.svg)](https://github.com/node-3d/image-raub/actions/workflows/eslint.yml)
[![Test](https://github.com/node-3d/image-raub/actions/workflows/test.yml/badge.svg)](https://github.com/node-3d/image-raub/actions/workflows/test.yml)
[![Cpplint](https://github.com/node-3d/image-raub/actions/workflows/cpplint.yml/badge.svg)](https://github.com/node-3d/image-raub/actions/workflows/cpplint.yml)

```console
npm i -s image-raub
```

**Node.js** N-API addon implementing the
[HTMLImageElement](https://developer.mozilla.org/en-US/docs/Web/API/HTMLImageElement/Image).
It is useful for other Node.js addons that operate on decoded images.

> Note: this **addon uses N-API**, and therefore is ABI-compatible across different
Node.js versions. Addon binaries are precompiled and **there is no compilation**
step during the `npm i` command.

Load images from:
* Local file.
* Data URI.
* Http(s) URL.
* Node.js Blob URL.
* Raw RGBA pixel data


[FreeImage](http://freeimage.sourceforge.net/) is used as back-end.

Additional features:
* `save` - saves the image to a local file.
* `drawImage` - is similar to
	[drawImage](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/drawImage).


## Image

```js
const Image = require('image-raub');
```

See [TypeScript defenitions](/index.d.ts) for more details.


### Set window icon

Compatible with [glfw-raub](https://github.com/node-3d/glfw-raub) `window.icon` property.

```js
const icon = new Image();
icon.src = __dirname + '/icons/logo.png';
icon.on('load', () => { window.icon = icon; });
```


### Load an OpenGL texture

Here `Image` is used to load a texture. The constructed object receives `src` property,
then the file is read and `'load'` event is emitted. After that, `image.data` is
available as a `Buffer`, containing the whole pixel data, and `image.width`/`image.height`
contain the dimensions.

```js
const Image = require('image-raub');
const image = new Image();
// Web-style onload
image.onload = () => {
	gl.bindTexture(gl.TEXTURE_2D, texture);
	gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, image);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR_MIPMAP_NEAREST);
	gl.generateMipmap(gl.TEXTURE_2D);
	gl.bindTexture(gl.TEXTURE_2D, null);
};

image.src = `${__dirname}/texture.jpg`;
```


### Make an OpenGL snapshot

Image can save its current content to the filesystem. It can also load from raw
pixel values using `static fromPixels()` method.

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

