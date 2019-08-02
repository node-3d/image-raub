#include <cstdlib>
#include <vector>
#include <iostream>

#include "image.hpp"


using namespace Napi;
using namespace std;


// ------ Aux macros

#define THIS_IMAGE                                                            \
	NAPI_ENV;                                                                 \
	Image *image = this;

#define THIS_CHECK                                                            \
	if (image->_isDestroyed) return env.Undefined();


// ------ Methods and props

JS_GETTER(Image::_dataGetter) {
	THIS_IMAGE;
	THIS_CHECK;
	
	return _data;
	
}


JS_GETTER(Image::widthGetter) { THIS_IMAGE; THIS_CHECK;
	
	RET_NUM(image->_bitmap ? FreeImage_GetWidth(image->_bitmap) : 0);
	
}


JS_GETTER(Image::heightGetter) { THIS_IMAGE; THIS_CHECK;
	
	RET_NUM(image->_bitmap ? FreeImage_GetHeight(image->_bitmap) : 0);
	
}


JS_METHOD(Image::load) { THIS_IMAGE; THIS_CHECK;
	
	REQ_BUF_ARG(0, file);
	
	BYTE *bufferData = reinterpret_cast<BYTE*>(file.Data());
	size_t bufferLength = file.Length();
	
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
	size_t num_pixels = FreeImage_GetWidth(image->_bitmap) *
		FreeImage_GetHeight(image->_bitmap);
	BYTE *pixels = FreeImage_GetBits(image->_bitmap);
	int num_bytes = static_cast<int>(num_pixels * 4);
	
	// FreeImage stores data in BGR. Convert from BGR to RGB.
	for (size_t i = 0; i < num_pixels; i++) {
		size_t i4 = i << 2;
		BYTE temp = pixels[i4 + 0];
		pixels[i4 + 0] = pixels[i4 + 2];
		pixels[i4 + 2] = temp;
	}
	
	Napi::Object buffer = Napi::Buffer<BYTE>::New(env, num_bytes);
	memcpy(buffer.Data(), pixels, num_bytes);
	_data = buffer;
	
	image->emit("load");
	
	return env.Undefined();
	
}


JS_METHOD(Image::unload) { THIS_IMAGE; THIS_CHECK;
	
	if (image->_bitmap) {
		FreeImage_Unload(image->_bitmap);
		image->_bitmap = nullptr;
	}
	
	_data = env.Null();
	
	image->emit("load");
	
	return env.Undefined();
	
}


JS_METHOD(Image::save) { THIS_IMAGE; THIS_CHECK;
	
	REQ_STR_ARG(0, dest)
	
	FREE_IMAGE_FORMAT format = FreeImage_GetFIFFromFilename(dest.data());
	
	if ( ! image->_bitmap ) {
		return RET_BOOL(false);
	}
	
	FIBITMAP *output = image->_bitmap;
	unsigned bpp = FreeImage_GetBPP(output);
	
	if (format == FIF_JPEG && bpp != 24) {
		output = FreeImage_ConvertTo24Bits(output);
	}
	
	bool ret = FreeImage_Save(format, output, dest.data()) == 1;
	
	if (format == FIF_JPEG && bpp != 24) {
		FreeImage_Unload(output);
	}
	
	RET_BOOL(ret);
	
}


// https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/drawImage
JS_METHOD(Image::drawImage) { THIS_IMAGE; THIS_CHECK;
	
	REQ_OBJ_ARG(0, _src);
	Image *src = Napi::ObjectWrap<Image>::Unwrap(_src);
	
	if ( ! src->_bitmap ) {
		return env.Undefined();
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
		return env.Undefined();
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
	
	Napi::Object buffer = Napi::Buffer<BYTE>::New(env, num_bytes);
	memcpy(buffer.Data(), pixels, num_bytes);
	_data = buffer;
	
	return env.Undefined();
	
}


// ------ System methods and props for ObjectWrap

Napi::FunctionReference Image::_constructor;

void Image::init(Napi::Env env, Napi::Object exports) {
	
	Napi::Function func = DefineClass(env, "Image", {
		ACCESSOR_R(Image, isDestroyed),
		ACCESSOR_R(Image, width),
		ACCESSOR_R(Image, height),
		ACCESSOR_M("destroy", &Image::destroy),
		ACCESSOR_M("save", &Image::save),
		ACCESSOR_M("_load", &Image::load),
		ACCESSOR_M("_unload", &Image::unload),
		ACCESSOR_M("drawImage", &Image::drawImage),
	});
	
	_constructor = Napi::Persistent(func);
	_constructor.SuppressDestruct();
	
	exports.Set("Image", func);
	
}


bool Image::isImage(Napi::Object obj) {
	return obj.InstanceOf(_constructor.Value());
}

Image::Image(const Napi::CallbackInfo &info): Napi::ObjectWrap<Image>(info) {
	NAPI_ENV;
	_data = env.Null();
	_isDestroyed = false;
	_bitmap = nullptr;
}

Image::~Image() {
	_destroy();
}


void Image::_destroy() { DES_CHECK;
	
	if (_bitmap) {
		FreeImage_Unload(_bitmap);
		_bitmap = nullptr;
	}
	
	_isDestroyed = true;
	
}

JS_METHOD(Image::destroy) { THIS_IMAGE; THIS_CHECK;
	
	image->emit("destroy");
	
	image->_destroy();
	
}


JS_GETTER(Image::isDestroyedGetter) { THIS_IMAGE;
	
	RET_VALUE(JS_BOOL(image->_isDestroyed));
	
}

