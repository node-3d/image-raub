'use strict';

const { inspect, inherits } = require('util');
const Emitter = require('events');

const download = require('addon-tools-raub/download');

const { Image } = require('../core');


inherits(Image, Emitter);

class JsImage extends Image {
	
	constructor(src) {
		
		super();
		
		this._src = '';
		this._isDataUri = false;
		this._data = null;
		
		this.on('error', err => {
			this._data = null;
			this._error = err;
			console.error('Image Error:', err);
		});
		
		if (typeof src === 'string') {
			this.src = src;
		}
		
	}
	
	
	addEventListener(type, cb) {
		this.on(type, cb);
	}
	
	
	removeEventListener(type, cb) {
		this.removeListener(type, cb);
	}
	
	
	dispatchEvent(e) {
		this.emit(e.type || 'unknown', e);
	}
	
	
	on(name, cb) {
		
		super.on(name, cb);
		
		if (name === 'load' && this._data) {
			cb.call(this);
		}
		
	}
	
	
	once(name, cb) {
		if (name === 'load' && this._data) {
			cb.call(this);
		} else {
			super.once(name, cb);
		}
	}
	
	
	load(data) { this._load(data); }
	
	
	get complete() { return this._data !== null; }
	
	get data() { return this._data; }
	
	
	get naturalWidth() { return this.width; }
	get naturalHeight() { return this.height; }
	
	get wh() { return [this.width, this.height]; }
	
	get size() { const [width, height] = this.wh; return { width, height }; }
	
	
	get src() { return this._src; }
	
	set src(v) {
		
		// The same - do nothing
		if (v === this._src) {
			return;
		}
		
		this._error = null;
		this._data = null;
		this._src = v;
		this._isDataUri = false;
		
		// Empty - do nothing
		if ( ! this._src ) {
			return this._unload();
		}
		
		// Data URI
		if (/^data:/.test(this._src)) {
			this._isDataUri = true;
			const [head, body] = this._src.split(',');
			const isBase64 = head.indexOf('base64') > -1;
			const data = isBase64 ? Buffer.from(body, 'base64') : Buffer.from(unescape(body));
			this._load(data);
			return;
		}
		
		// Remote image
		if (/^https?:\/\//i.test(this._src)) {
			download(this._src).then(
				data => this._load(data),
				err => this.emit('error', err)
			);
			return;
		}
		
		// Filesystem image
		require('fs').readFile(this._src, (err, data) => {
			if (err) {
				return this.emit('error', err);
			}
			this._load(data);
		});
		
	}
	
	
	get onerror() { return this.listeners('error'); }
	set onerror(cb) { this.on('error', cb); }
	
	get onload() { return this.listeners('load'); }
	set onload(cb) { this.on('load', cb); }
	
	
	[inspect.custom]() { return this.toString(); }
	
	toString() {
		
		if ( ! this.src ) {
			return 'Image { EMPTY }';
		}
		
		const src = this._isDataUri ? `${this.src.slice(0, 32)}...` : this.src;
		
		if (this._error ) {
			return `Image { ERROR, ${src} }`;
		}
		
		if ( ! this._data ) {
			return `Image { LOADING, ${src} }`;
		}
		
		return `Image { ${this.width}x${this.height} ${src} }`;
		
	}
	
	
	static fromPixels(width, height, bpp, pixels) {
		
		const memSize = width * height * Math.floor(bpp / 8);
		
		// ====== MIMIC BMP
		
		// see https://en.wikipedia.org/wiki/BMP_file_format
		const dibSize = 40;
		const headerSize = 14 + dibSize;
		const bmpSize = headerSize + memSize;
		const fakeBmp = Buffer.allocUnsafeSlow(bmpSize);
		let pos = 0;
		
		// ---------- BMP header
		
		fakeBmp.write('BM', pos, 2, 'ascii');
		pos += 2;
		
		fakeBmp.writeUInt32LE(bmpSize, pos);
		pos += 4;
		
		pos += 4; // skip unused
		
		fakeBmp.writeUInt32LE(headerSize, pos);
		pos += 4;
		
		// ---------- DIB header
		
		fakeBmp.writeUInt32LE(dibSize, pos);
		pos += 4;
		
		fakeBmp.writeInt32LE(width, pos);
		pos += 4;
		
		fakeBmp.writeInt32LE(height, pos);
		pos += 4;
		
		fakeBmp.writeUInt16LE(1, pos);
		pos += 2;
		
		fakeBmp.writeUInt16LE(bpp, pos);
		pos += 2;
		
		fakeBmp.writeUInt32LE(0, pos);
		pos += 4;
		
		fakeBmp.writeUInt32LE(memSize, pos);
		pos += 4;
		
		fakeBmp.writeUInt32LE(0x0ec4, pos);
		pos += 4;
		
		fakeBmp.writeUInt32LE(0x0ec4, pos);
		pos += 4;
		
		fakeBmp.writeUInt32LE(0, pos);
		pos += 4;
		
		fakeBmp.writeUInt32LE(0, pos);
		pos += 4;
		
		// ---------- PIXELS
		
		pixels.copy(fakeBmp, pos);
		
		// ====== STORE JPEG
		
		const img = new Image();
		img._load(fakeBmp, true);
		
		return img;
		
	}
	
}


module.exports = JsImage;
