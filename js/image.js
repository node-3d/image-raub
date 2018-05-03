'use strict';

const util         = require('util');
const fs           = require('fs');
const EventEmitter = require('events');

const { Image } = require('../core');


class JsImage extends Image {
	
	constructor(src) {
		
		super();
		
		this._complete = false;
		this._src = '';
		
		this.on('load', err => {
			this._complete = true;
		});
		
		this.on('error', err => {
			this._complete = false;
			console.error('Image Error:', err);
		});
		
		if (typeof src === 'string') {
			this.src = src;
		}
		
	}
	
	
	addEventListener(type, cb) {
		this.on(type, cb.bind(this));
	}
	
	on(name, cb) {
		super.on(name, cb);
		if (name === 'load' && this._complete) {
			cb.call(this);
		}
	}
	
	once(name, cb) {
		super.once(name, cb);
		if (name === 'load' && this._complete) {
			cb.call(this);
		}
	}
	
	
	get complete() { return this._complete; }
	
	get data() { return this._complete ? this.data : null; }
	
	
	get naturalWidth() { return this.width; }
	get naturalHeight() { return this.height; }
	
	get wh() { return [this.width, this.height]; }
	get size() { const [width, height] = this.wh; return { width, height }; }
	
	
	get src() { return this._src; }
	
	set src(v) {
		
		if (v === this._src) {
			this.emit('load');
			return;
		}
		
		this._complete = false;
		this._src = v;
		
		fs.readFile(this._src, (err, data) => {
			if (err) {
				return this.emit('error', err);
			}
			this.load(data);
		});
		
	}
	
	
	get onerror() { return this.listeners('error'); }
	set onerror(cb) { this.on('error', cb); }
	
	get onload() { return this.listeners('load'); }
	set onload(cb) { this.on('load', cb); }
	
	
	[util.inspect.custom]() { return this.toString(); }
	toString() { return `Image { ${this.width}x${this.height} ${this.src} }`; }
	
}


module.exports = JsImage;
