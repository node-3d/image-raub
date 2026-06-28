import { resolveObjectURL } from 'node:buffer';
import EventEmitter from 'node:events';
import fs from 'node:fs';
import { inspect, inherits } from 'node:util';
import { download, getLogger } from '@node-3d/addon-tools';
import { createBmpFromPixels } from './bmp.ts';
import { native } from './native.ts';
import type { TNativeImage } from './native.ts';

export type TSize = Readonly<{ width: number; height: number }>;

export type TImageEvent = {
	type?: string;
	[key: string]: unknown;
};

export type TImageEventCallback = (this: Image, event?: unknown) => unknown;

type TNativeImageWithEvents = TNativeImage & EventEmitter;

const logger = getLogger('image');
const NativeImage = native.Image as unknown as new () => TNativeImageWithEvents;

inherits(NativeImage, EventEmitter);

/**
 * Image loader for local files, URLs, data URIs, Blob URLs, and raw RGBA pixels.
 *
 * The class is similar to `HTMLImageElement` and emits `load` and `error`
 * events. Loaded images expose RGBA pixel bytes through `data`.
 */
export class Image extends NativeImage {
	/**
	 * Create an image.
	 *
	 * Passing `src` starts loading immediately. Relative file paths resolve from
	 * the current working directory.
	 */
	public constructor(src?: string | null) {
		super();

		this._src = '';
		this._isDataUri = false;
		this._data = null;
		this._error = null;

		this.on('error', (error) => {
			this._data = null;
			this._error = error;
			logger.error('Image Error:', error);
		});

		if (typeof src === 'string') {
			this.src = src;
		}
	}

	/** Has the underlying native image object been destroyed? */
	public declare readonly isDestroyed: boolean;

	/** Image width in pixels, or `0` before an image is loaded. */
	public declare readonly width: number;

	/** Image height in pixels, or `0` before an image is loaded. */
	public declare readonly height: number;

	/** Is the image fully loaded? */
	public get complete(): boolean {
		return this._data !== null;
	}

	/** Raw RGBA image data, or `null` when the image is empty or still loading. */
	public get data(): Buffer | null {
		return this._data;
	}

	/** Alias for `width`. */
	public get naturalWidth(): number {
		return this.width;
	}

	/** Alias for `height`. */
	public get naturalHeight(): number {
		return this.height;
	}

	/** Width and height as a tuple. */
	public get wh(): [number, number] {
		return [this.width, this.height];
	}

	/** Width and height as an object. */
	public get size(): TSize {
		const [width, height] = this.wh;
		return { width, height };
	}

	/** Image URI: local file, HTTP(S) URL, data URI, or Node.js Blob URL. */
	public get src(): string {
		return this._src;
	}

	/**
	 * Start loading an image.
	 *
	 * Supports local file paths, HTTP(S) URLs, data URIs, and Node.js Blob URLs.
	 * Setting an empty value unloads the image.
	 */
	public set src(value: string | null | undefined) {
		const src = value ?? '';

		if (src === this._src) {
			return;
		}

		this._error = null;
		this._data = null;
		this._src = src;
		this._isDataUri = false;

		if (!this._src) {
			this._unload();
			return;
		}

		if (this._src.startsWith('blob:nodedata:')) {
			this.loadBlobUrl(this._src);
			return;
		}

		if (this._src.startsWith('data:')) {
			this._isDataUri = true;
			this.loadDataUri(this._src);
			return;
		}

		if (/^https?:\/\//iu.test(this._src)) {
			this.loadRemote(this._src);
			return;
		}

		this.loadFile(this._src);
	}

	/** Save the image to a file. Returns `false` if there is no image to save. */
	public declare save: (dest: string) => boolean;

	/**
	 * Draw another image onto this one.
	 *
	 * The overloads follow the browser canvas `drawImage()` argument forms.
	 */
	public declare drawImage: {
		(image: Image, dx: number, dy: number): void;
		(image: Image, dx: number, dy: number, dWidth: number, dHeight: number): void;
		(
			image: Image,
			sx: number,
			sy: number,
			sWidth: number,
			sHeight: number,
			dx: number,
			dy: number,
			dWidth: number,
			dHeight: number,
		): void;
	};

	/**
	 * Similar to `.on('error', cb)`.
	 *
	 * Setting `null` or `undefined` removes all current error listeners.
	 */
	public get onerror(): readonly TImageEventCallback[] {
		return this.listeners('error') as TImageEventCallback[];
	}

	public set onerror(cb: TImageEventCallback | null | undefined) {
		if (cb) {
			this.on('error', cb);
		} else {
			this.removeAllListeners('error');
		}
	}

	/**
	 * Similar to `.on('load', cb)`.
	 *
	 * Setting `null` or `undefined` removes all current load listeners.
	 */
	public get onload(): readonly TImageEventCallback[] {
		return this.listeners('load') as TImageEventCallback[];
	}

	public set onload(cb: TImageEventCallback | null | undefined) {
		if (cb) {
			this.on('load', cb);
		} else {
			this.removeAllListeners('load');
		}
	}

	/** Alias for `.on(type, cb)`. */
	public addEventListener(type: string | symbol, cb: TImageEventCallback): void {
		this.on(type, cb);
	}

	/** Alias for `.removeListener(type, cb)`. */
	public removeEventListener(type: string | symbol, cb: TImageEventCallback): void {
		this.removeListener(type, cb);
	}

	/** Emit an event object on behalf of this image. */
	public dispatchEvent(event: TImageEvent): void {
		this.emit(event.type ?? 'unknown', event);
	}

	/**
	 * Add an event listener.
	 *
	 * If a `load` listener is added after the image has loaded, it is called
	 * immediately with this image as its `this` value.
	 */
	public override on(eventName: string | symbol, listener: TImageEventCallback): this {
		super.on(eventName, listener);

		if (eventName === 'load' && this._data) {
			listener.call(this);
		}

		return this;
	}

	/**
	 * Add a one-time event listener.
	 *
	 * If a `load` listener is added after the image has loaded, it is called
	 * immediately instead of being registered.
	 */
	public override once(eventName: string | symbol, listener: TImageEventCallback): this {
		if (eventName === 'load' && this._data) {
			listener.call(this);
			return this;
		}

		super.once(eventName, listener);
		return this;
	}

	/** Load encoded image data from a buffer and emit `load` or `error`. */
	public load(data: Buffer): void {
		this._load(data);
	}

	/** Node.js custom inspect hook. */
	public [inspect.custom](): string {
		return this.toString();
	}

	/** Return a short human-readable image state summary. */
	public override toString(): string {
		if (!this.src) {
			return 'Image { EMPTY }';
		}

		const src = this._isDataUri ? `${this.src.slice(0, 32)}...` : this.src;

		if (this._error) {
			return `Image { ERROR, ${src} }`;
		}

		if (!this._data) {
			return `Image { LOADING, ${src} }`;
		}

		return `Image { ${this.width}x${this.height} ${src} }`;
	}

	/** Load an image and resolve after its `load` event fires. */
	public static loadAsync(src: string): Promise<Image> {
		return new Promise((res, rej) => {
			const image = new Image();

			image.on('load', () => res(image));
			image.on('error', rej);

			image.src = src;
		});
	}

	/**
	 * Create an image from raw pixel data.
	 *
	 * `bpp` is bits per pixel. Use `32` for RGBA buffers.
	 */
	public static fromPixels(width: number, height: number, bpp: number, pixels: Buffer): Image {
		const buffer = createBmpFromPixels(width, height, bpp, pixels);
		const image = new Image();

		image.loadBuffer(buffer, true);

		return image;
	}

	// Last requested source string, kept even while an async load is pending.
	private _src: string;
	// Used only to shorten data URIs in inspect/toString output.
	private _isDataUri: boolean;
	// RGBA pixel buffer exposed by the native loader after successful loads.
	private _data: Buffer | null;
	// Last load failure, used to display the current state in toString().
	private _error: unknown;

	// Internal wrapper for native `_load`; keeps public code away from underscored APIs.
	private loadBuffer(data: Buffer, swapBytes = false): void {
		this._load(data, swapBytes);
	}

	// Resolve Node object URLs and load their bytes through the native decoder.
	private async loadBlobUrl(src: string): Promise<void> {
		try {
			const blob = resolveObjectURL(src);

			if (!blob) {
				this.emit('error', new Error(`Blob URL not found: ${src}`));
				return;
			}

			const arrayBuffer = await blob.arrayBuffer();
			this._load(Buffer.from(arrayBuffer));
		} catch (error) {
			this.emit('error', error);
		}
	}

	// Decode data URI payloads before handing bytes to the native decoder.
	private loadDataUri(src: string): void {
		const [head = '', body = ''] = src.split(',', 2);
		const data = head.includes('base64') ?
			Buffer.from(body, 'base64') :
			Buffer.from(decodeURIComponent(body));

		this._load(data);
	}

	// Download remote images and load them as encoded image bytes.
	private async loadRemote(src: string): Promise<void> {
		try {
			this._load(await download(src));
		} catch (error) {
			this.emit('error', error);
		}
	}

	// Read local files and load them as encoded image bytes.
	private loadFile(src: string): void {
		fs.readFile(src, (error, data) => {
			if (error) {
				this.emit('error', error);
				return;
			}

			this._load(data);
		});
	}
}
