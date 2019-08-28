'use strict';

const util  = require('util');
const fs    = require('fs');

const EventEmitter = require('events');

const download = require('addon-tools-raub/download');

const { Image } = require('../core');

// Image.prototype.__proto__ = EventEmitter.prototype;
// function EventEmitter() { console.log('I am super! ---------------'); }

// Image.prototype = EventEmitter;
Object.setPrototypeOf(Image.prototype, EventEmitter.prototype);
Object.setPrototypeOf(Image, EventEmitter);
Image._super = EventEmitter;

class A extends Image {
	constructor() {
		super();
		console.log('RTEST2', Image.prototype.on, A.prototype.on, Object.getPrototypeOf(this));
	}
	x(){}
}


console.log('CALA1');
new A();
console.log('CALA2');
console.log('BBBB');
function JsImage(src) {
	console.log('RTEST11', Object.getPrototypeOf(this));
	Image.call(this);
	console.log('RTEST1', this.on);
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

JsImage.prototype = Object.create(Image.prototype);
JsImage.prototype.constructor = JsImage;
console.log('CCC');
Object.assign(JsImage.prototype, {
	
	addEventListener(type, cb) {
		this.on(type, cb);
	},
	
	
	removeEventListener(type, cb) {
		this.removeListener(type, cb);
	},
	
	
	dispatchEvent(e) {
		this.emit(e.type || 'unknown', e);
	},
	
	
	on(name, cb) {
		// console.log('ONONO', Image.prototype.on);
		Image.prototype.on.call(this, name, cb);
		// super.on(name, cb);
		
		if (name === 'load' && this._data) {
			cb.call(this);
		}
		
	},
	
	
	once(name, cb) {
		
		if (name === 'load' && this._data) {
			cb.call(this);
		} else {
			Image.prototype.once.call(this, name, cb);
			// super.once(name, cb);
		}
		
	},
	
	
	load(data) { this._load(data); },
	
	
	
	[util.inspect.custom]() { return this.toString(); },
	
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
		
	},
	
});

// Object.defineProperties({
	
// 	get complete() { return this._data !== null; },
	
// 	get data() { return this._data; },
	
	
// 	get naturalWidth() { return this.width; },
// 	get naturalHeight() { return this.height; },
	
// 	get wh() { return [this.width, this.height]; },
// 	get size() { const [width, height] = this.wh; return { width, height }; },
	
	
// 	get src() { return this._src; },
	
// 	set src(v) {
		
// 		// The same - do nothing
// 		if (v === this._src) {
// 			return;
// 		}
		
		
// 		this._error = null;
// 		this._data = null;
// 		this._src = v;
// 		this._isDataUri = false;
		
		
// 		// Empty - do nothing
// 		if ( ! this._src ) {
// 			return this._unload();
// 		}
		
		
// 		// Data URI
// 		if (/^data:/.test(this._src)) {
			
// 			this._isDataUri = true;
// 			const [head, body] = this._src.split(',');
// 			const isBase64 = head.indexOf('base64') > -1;
// 			const data = isBase64 ? Buffer.from(body, 'base64') : Buffer.from(unescape(body));
// 			console.log('image.js', 'this', this, this._load, this.save, this.width);
// 			this._load(data);
// 			return;
			
// 		}
		
		
// 		// Remote image
// 		if (/^https?:\/\//i.test(this._src)) {
			
// 			download(this._src).then(
// 				data => this._load(data),
// 				err => this.emit('error', err)
// 			);
			
// 			return;
			
// 		}
		
		
// 		// Filesystem image
// 		fs.readFile(this._src, (err, data) => {
// 			if (err) {
// 				return this.emit('error', err);
// 			}
// 			this._load(data);
// 		});
		
// 	},
	
	
// 	get onerror() { console.log('HERE FUCK', this);	return this.listeners('error'); },
// 	set onerror(cb) { this.on('error', cb); },
	
// 	get onload() { return this.listeners('load'); },
// 	set onload(cb) { this.on('load', cb); },
	
// });


Image.fromPixels = (width, height, bpp, pixels) => {
	
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
	img._load(fakeBmp);
	
	return img;
	
};


module.exports = JsImage;
