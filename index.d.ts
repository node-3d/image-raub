import { EventEmitter } from 'events';

declare module "image-raub" {
	// This nested namespace 'image' will merge with the enclosing 
	// declared namespace 'image'.
	// https://www.typescriptlang.org/docs/handbook/declaration-merging.html
	namespace image {
		type TWindowMode = 'windowed' | 'borderless' | 'fullscreen';

		type TSize = Readonly<{ width: number; height: number }>;

		type TPos = Readonly<{ x: number; y: number }>;

		type TRect = TSize & TPos & {
			left: number;
			top: number;
			right: number;
			bottom: number;
		};

		type TImage = TSize & Readonly<{
			data: Buffer;
			noflip?: boolean;
		}>;

		type TMonitorMode = TSize & Readonly<{
			rate: number; // refresh rate
		}>;

		type TMonitor = TMonitorMode & {
			is_primary: boolean; // is this screen primary
			name: string; // screen name
			pos_x: number; // global position x of the screen
			pos_y: number; // global position y of the screen
			width_mm: number; // screen width in mm
			height_mm: number; // screen height in mm
			modes: ReadonlyArray<TMonitorMode>;
		};

		type TEvent = {
			type: string;
			[key: string]: unknown;
		};

		type TMouseEvent = TEvent & {
			buttons: number;
			clientX: number;
			clientY: number;
			pageX: number;
			pageY: number;
			x: number;
			y: number;
			shiftKey: boolean;
			ctrlKey: boolean;
			altKey: boolean;
			metaKey: boolean;
		};

		type TMouseMoveEvent = TMouseEvent & {
			movementX: number;
			movementY: number;
		};

		type TMouseButtonEvent = TMouseEvent & {
			button: number;
			which: number;
		};

		type TMouseScrollEvent = TMouseEvent & {
			deltaX: number;
			deltaY: number;
			deltaZ: number;
			wheelDeltaX: number;
			wheelDeltaY: number;
			wheelDelta: number;
		};

		type TJoystickEvent = TEvent & {
			id: number;
			event: number;
		};

		type TKeyEvent = TEvent & {
			repeat: boolean;
			altKey: boolean;
			ctrlKey: boolean;
			metaKey: boolean;
			shiftKey: boolean;
			code: string | null;
			key: string | null;
			which: number;
			charCode: number;
		};

		type TDropEvent = TEvent & {
			dataTransfer: Readonly<{ [key: string]: never }>
			dropEffect: 'none';
			effectAllowed: 'all';
			files: ReadonlyArray<string>;
			items: ReadonlyArray<string>;
			types: ReadonlyArray<never>;
		};

		type TIconifyEvent = TEvent & {
			iconified: boolean;
		};

		type TPosEvent = TEvent & TPos;

		type TSizeEvent = TEvent & TSize;

		type TEventCb<T extends TEvent> = (event: T) => (void | boolean);

		type TWindowOpts = Readonly<{
			/** Major OpenGL version to be used. Default is 2. */
			major?: number;
			/** Minor OpenGL version to be used. Default is 1. */
			minor?: number;
			/** Window title, takes current directory as default. Default is $PWD. */
			width?: number;
			/** Window initial width. Default is 800. */
			height?: number;
			/** Window initial height. Default is 600. */
			display?: number;
			/** Display id to open window on a specific display. Default is undefined. */
			vsync?: boolean;
			/** If vsync should be used. Default is false. */
			autoIconify?: boolean;
			/** If the window is fullscreen, takes presedence over `mode`. Default is false. */
			fullscreen?: boolean;
			/** One of `'windowed', 'borderless', 'fullscreen'`. Default is 'windowed'. */
			mode?: TWindowMode;
			/** If fullscreen windows should iconify automatically on focus loss. Default is true. */
			decorated?: boolean;
			/** Multisample antialiasing level. Default is 2. */
			msaa?: number;
			/** Winodw icon. Default is null. */
			icon?: TImage;
			/** If window has borders (use `false` for borderless fullscreen). Default is true. */
			title?: string;
		}>;

		type TWindowPtr = number;
		
		/** GLFW Window API wrapper
		 * Window is a higher-level js-wrapper for GLFW API.
		 * It helps managing window instances. It also extends
		 * EventEmitter to provide event-handling.
		*/
		class Image extends EventEmitter {
			constructor(src?: string | null);
			
			/** Is image fully loaded? */
			readonly complete: boolean;
			
			/** Raw image data */
			readonly data: TImage;
			
			/** An Array, containing LOGICAL width and height. */
			readonly wh: [number, number];
			
			/** Alias for width. */
			readonly naturalWidth: number;
			
			/** Alias for height. */
			readonly naturalHeight: number;
			
			/** An Object, containing width and height. */
			readonly size: TSize;
			
			/** Image URI: local file / URL / data-uri */
			src: string;
			
			/** Alias for .on('error', cb).
			 * Setter adds a new callback.
			 * Getter returns an Array of currently existing callbacks.
			*/
			onerror: TEventCb<TKeyEvent> | ReadonlyArray<TEventCb<TKeyEvent>>;
			
			/** Alias for .on('load', cb).
			 * Setter adds a new callback.
			 * Getter returns an Array of currently existing callbacks.
			*/
			onload: TEventCb<TKeyEvent> | ReadonlyArray<TEventCb<TKeyEvent>>;
			
			/** Emit an event on behalf of this window. */
			emit(name: string, event: TEvent): boolean;
			
			/** Add event listener.
			 * 'error' - something went wrong.
			 * 'load' - image has been loaded.
			*/
			on(name: string, cb: (event: TEvent) => (void | boolean)): this;
			
			/** Alias for emit, type is expected inside the event object. */
			dispatchEvent(event: TEvent): void;
			
			/** Alias for on. */
			addEventListener(name: string): void;
			
			/** Alias for removeListener. */
			removeEventListener(name: string): void;
			
			/** Save the image to a file. */
			save(dest: string): void;
			
			/** Draws other image onto this one.
			 * @see https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/drawImage
			*/
			drawImage(
				image: TImage,
				sx: number,
				sy: number,
				sWidth: number,
				sHeight: number,
				dx: number,
				dy: number,
				dWidth: number,
				dHeight: number,
			): void;
		}
		
	}
	
	export = image;
}