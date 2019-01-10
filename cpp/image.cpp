#include <cstdlib>
#include <vector>
#include <iostream>

#include "image.hpp"


using namespace v8;
using namespace node;
using namespace std;


// ------ Aux macros

#define THIS_IMAGE                                                            \
	Image *image = ObjectWrap::Unwrap<Image>(info.This());
	
#define THIS_CHECK                                                            \
	if (image->_isDestroyed) return;

#define USE_UINT32_ARG(I, VAR, DEF)                                           \
	CHECK_LET_ARG(I, IsUint32(), "uint32");                                   \
	unsigned int VAR = IS_ARG_EMPTY(I) ? (DEF) : info[I]->Uint32Value();


// ------ Constructor and Destructor

Image::~Image() {
	
	_destroy();
	
}


void Image::_destroy() { DES_CHECK;
	
	if (_bitmap) {
		FreeImage_Unload(_bitmap);
		_bitmap = nullptr;
	}
	
	_isDestroyed = true;
	
	EventEmitter::_destroy();
	
}


// ------ Methods and props

NAN_GETTER(Image::widthGetter) { THIS_IMAGE; THIS_CHECK;
	
	RET_VALUE(JS_INT(image->_bitmap ? FreeImage_GetWidth(image->_bitmap) : 0));
	
}


NAN_GETTER(Image::heightGetter) { THIS_IMAGE; THIS_CHECK;
	
	RET_VALUE(JS_INT(image->_bitmap ? FreeImage_GetHeight(image->_bitmap) : 0));
	
}


NAN_METHOD(Image::load) { THIS_IMAGE; THIS_CHECK;
	
	REQ_OBJ_ARG(0, file);
	
	BYTE *bufferData = reinterpret_cast<BYTE*>(Buffer::Data(file));
	size_t bufferLength = Buffer::Length(file);
	
	FIMEMORY *memStream = FreeImage_OpenMemory(bufferData, bufferLength);
	
	FREE_IMAGE_FORMAT format = FreeImage_GetFileTypeFromMemory(memStream, bufferLength);
	FIBITMAP *tmpBitmap = FreeImage_LoadFromMemory(format, memStream);
	
	if (image->_bitmap) {
		FreeImage_Unload(image->_bitmap);
	}
	
	image->_bitmap = FreeImage_ConvertTo32Bits(tmpBitmap);
	
	FreeImage_Unload(tmpBitmap);
	
	FreeImage_CloseMemory(memStream);
	
	// adjust internal fields
	size_t num_pixels = FreeImage_GetWidth(image->_bitmap) * FreeImage_GetHeight(image->_bitmap);
	BYTE *pixels = FreeImage_GetBits(image->_bitmap);
	int num_bytes = static_cast<int>(num_pixels * 4);
	
	// FreeImage stores data in BGR. Convert from BGR to RGB.
	for (size_t i = 0; i < num_pixels; i++) {
		size_t i4 = i << 2;
		BYTE temp = pixels[i4 + 0];
		pixels[i4 + 0] = pixels[i4 + 2];
		pixels[i4 + 2] = temp;
	}
	
	V8_VAR_OBJ buffer = Nan::NewBuffer(num_bytes).ToLocalChecked();
	
	memcpy(Buffer::Data(buffer), pixels, num_bytes);
	
	Nan::Set(info.This(), JS_STR("_data"), buffer);
	
	image->emit("load");
	
}


NAN_METHOD(Image::unload) { THIS_IMAGE; THIS_CHECK;
	
	if (image->_bitmap) {
		FreeImage_Unload(image->_bitmap);
		image->_bitmap = nullptr;
	}
	
	Nan::Set(info.This(), JS_STR("_data"), Nan::Null());
	
	image->emit("load");
	
}


NAN_METHOD(Image::save) { THIS_IMAGE; THIS_CHECK;
	
	REQ_UTF8_ARG(0, dest)
	
	FREE_IMAGE_FORMAT format = FreeImage_GetFIFFromFilename(*dest);
	
	if (! image->_bitmap) {
		return;
	}
	
	FIBITMAP *output = image->_bitmap;
	unsigned bpp = FreeImage_GetBPP(output);
	
	if (format == FIF_JPEG && bpp != 24) {
		output = FreeImage_ConvertTo24Bits(output);
	}
	
	bool ret = FreeImage_Save(format, output, *dest) == 1;
	
	if (format == FIF_JPEG && bpp != 24) {
		FreeImage_Unload(output);
	}
	
	RET_VALUE(Nan::New<Boolean>(ret));
	
}


// https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/drawImage
NAN_METHOD(Image::drawImage) { THIS_IMAGE; THIS_CHECK;
	
	REQ_OBJ_ARG(0, _src);
	Image *src = ObjectWrap::Unwrap<Image>(_src);
	
	if (! src->_bitmap) {
		return;
	}
	
	LET_UINT32_ARG(1, arg1);
	LET_UINT32_ARG(2, arg2);
	LET_UINT32_ARG(3, arg3);
	LET_UINT32_ARG(4, arg4);
	// LET_UINT32_ARG(5, arg5);
	// LET_UINT32_ARG(6, arg6);
	LET_UINT32_ARG(7, arg7);
	LET_UINT32_ARG(8, arg8);
	
	unsigned sx = 0;
	unsigned sy = 0;
	// unsigned dx = 0;
	// unsigned dy = 0;
	unsigned sWidth = FreeImage_GetWidth(src->_bitmap);
	unsigned sHeight = FreeImage_GetHeight(src->_bitmap);
	
	unsigned dWidth;
	unsigned dHeight;
	
	if (info.Length() == 3) {
		// drawImage(image, dx, dy);
		dWidth = sWidth;
		dHeight = sHeight;
	} else if (info.Length() == 5) {
		// drawImage(image, dx, dy, dWidth, dHeight);
		dWidth = arg3;
		dHeight = arg4;
	} else if (info.Length() == 9) {
		// drawImage(image, sx, sy, sWidth, sHeight, dx, dy, dWidth, dHeight);
		sx = arg1;
		sy = arg2;
		sWidth = arg3;
		sHeight = arg4;
		dWidth = arg7;
		dHeight = arg8;
	} else {
		return;
	}
	
	FIBITMAP *result = FreeImage_RescaleRect(
		src->_bitmap, dWidth, dHeight, sx, sy, sx + sWidth, sy + sHeight
	);
	
	if (image->_bitmap) {
		FreeImage_Unload(image->_bitmap);
	}
	
	image->_bitmap = result;
	
	// ---------- TODO: DRY
	
	// adjust internal fields
	size_t num_pixels = FreeImage_GetWidth(image->_bitmap) * FreeImage_GetHeight(image->_bitmap);
	BYTE *pixels = FreeImage_GetBits(image->_bitmap);
	int num_bytes = static_cast<int>(num_pixels * 4);
	
	// ---------- TODO: UNSURE what happens to BGR above
	
	// // FreeImage stores data in BGR. Convert from BGR to RGB.
	// for (size_t i = 0; i < num_pixels; i++) {
	// 	size_t i4 = i << 2;
	// 	BYTE temp = pixels[i4 + 0];
	// 	pixels[i4 + 0] = pixels[i4 + 2];
	// 	pixels[i4 + 2] = temp;
	// }
	
	V8_VAR_OBJ buffer = Nan::NewBuffer(num_bytes).ToLocalChecked();
	
	memcpy(Buffer::Data(buffer), pixels, num_bytes);
	
	Nan::Set(info.This(), JS_STR("_data"), buffer);
	
}


// ------ System methods and props for ObjectWrap

V8_STORE_FT Image::_protoImage;
V8_STORE_FUNC Image::_ctorImage;


void Image::init(V8_VAR_OBJ target) {
	
	V8_VAR_FT proto = Nan::New<FunctionTemplate>(newCtor);
	
	// class Image inherits EventEmitter
	V8_VAR_FT parent = Nan::New(EventEmitter::_protoEventEmitter);
	proto->Inherit(parent);
	
	proto->InstanceTemplate()->SetInternalFieldCount(1);
	proto->SetClassName(JS_STR("Image"));
	
	
	// Accessors
	V8_VAR_OT obj = proto->PrototypeTemplate();
	ACCESSOR_R(obj, isDestroyed);
	
	ACCESSOR_R(obj, width);
	ACCESSOR_R(obj, height);
	
	// -------- dynamic
	
	Nan::SetPrototypeMethod(proto, "destroy", destroy);
	
	Nan::SetPrototypeMethod(proto, "save", save);
	Nan::SetPrototypeMethod(proto, "_load", load);
	Nan::SetPrototypeMethod(proto, "_unload", unload);
	Nan::SetPrototypeMethod(proto, "drawImage", drawImage);
	
	// -------- static
	
	V8_VAR_FUNC ctor = Nan::GetFunction(proto).ToLocalChecked();
	
	_protoImage.Reset(proto);
	_ctorImage.Reset(ctor);
	
	Nan::Set(target, JS_STR("Image"), ctor);
	
	
}


bool Image::isImage(V8_VAR_OBJ obj) {
	return Nan::New(_protoImage)->HasInstance(obj);
}


NAN_METHOD(Image::newCtor) {
	
	CTOR_CHECK("Image");
	
	Image *image = new Image();
	image->Wrap(info.This());
	
	RET_VALUE(info.This());
	
}


NAN_METHOD(Image::destroy) { THIS_IMAGE; THIS_CHECK;
	
	image->emit("destroy");
	
	image->_destroy();
	
}


NAN_GETTER(Image::isDestroyedGetter) { THIS_IMAGE;
	
	RET_VALUE(JS_BOOL(image->_isDestroyed));
	
}

