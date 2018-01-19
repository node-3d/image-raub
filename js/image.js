'use strict';

const util = require('util');
const EventEmitter = require('events');

const { Image } = require('../core');


class JsImage extends EventEmitter {
	
	constructor() {
		
		super();
		
		this.emit = this.emit.bind(this);
		
		this._image = new Image(this);
		
		this._complete = false;
		this._src = '';
		
		this.on('load', src => {
			this._complete = true;
			if (this._loadCb) {
				this._loadCb(src);
			}
		});
		
	}
	
	
	on(name, cb) {
		super.on(name, cb);
		if (name === 'load' && this._complete) {
			cb.call(this, this._src);
		}
	}
	
	
	get complete() { return this._complete; }
	
	
	get width() { return this._complete ? this._image.width : 0; }
	get height() { return this._complete ? this._image.height : 0; }
	get naturalWidth() { return this.width; }
	get naturalHeight() { return this.height; }
	
	get wh() { return [this.width, this.height]; }
	get size() { const [width, height] = this.wh; return { width, height }; }
	
	
	get src() { return this._src; }
	set src(v) {
		if (v === this._src) {
			this.emit('load', v);
			return;
		}
		this._complete = false;
		this._src = v;
		this._image.load(v);
	}
	
	get onload() { return this.listeners; }
	set onload(cb) { this.on('load', cb); }
	
	
	[util.inspect.custom]() { return this.toString(); }
	toString() { return `Image { ${this.width}x${this.height} ${this.src} }`; }
	
	
	save(dest, w, h) { return this._image.save(dest, w, h); }
	
}


module.exports = JsImage;
