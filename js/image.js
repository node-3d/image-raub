'use strict';

const util = require('util');
const EventEmitter = require('events');

const Image = require('../core');


class JsImage extends EventEmitter {
	
	constructor() {
		
		super();
		
		this.emit = this.emit.bind(this);
		
		this._image = new Image(this);
		
		this._complete = false;
		
		this.on('load', src => {
			this._complete = true;
			if (this._loadCb) {
				this._loadCb(src);
			}
		});
		
	}
	
	
	get complete() { return this._complete; }
	
	get src() { return this._image.src; }
	set src(v) {
		if (v === this._image.src) {
			this.emit('load', v);
			return;
		}
		this._complete = false;
		this._image.src = v;
	}
	
	
	[util.inspect.custom]() { return this.toString(); }
	
	toString() {
		return `Image { src: [${this.src}] }`
	}
	
}


module.exports = JsScene;
