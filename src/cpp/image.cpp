#include <locale>
#include <codecvt>

#include "image.hpp"


IMPLEMENT_ES5_CLASS(Image);

void Image::init(Napi::Env env, Napi::Object exports) {
	
	Napi::Function ctor = wrap(env);
	
	JS_ASSIGN_GETTER(isDestroyed);
	JS_ASSIGN_GETTER(width);
	JS_ASSIGN_GETTER(height);
	
	JS_ASSIGN_METHOD(destroy);
	JS_ASSIGN_METHOD(save);
	JS_ASSIGN_METHOD(_load);
	JS_ASSIGN_METHOD(_unload);
	JS_ASSIGN_METHOD(drawImage);
	
	exports.Set("Image", ctor);
	
}


Image::Image(const Napi::CallbackInfo &info) {
	super(info);
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


JS_IMPLEMENT_GETTER(Image, width) { THIS_CHECK;
	RET_NUM(_bitmap ? FreeImage_GetWidth(_bitmap) : 0);
}


JS_IMPLEMENT_GETTER(Image, height) { THIS_CHECK;
	RET_NUM(_bitmap ? FreeImage_GetHeight(_bitmap) : 0);
}


inline Napi::Buffer<uint8_t> createBuffer(Napi::Env env, FIBITMAP *bmp) {
	
	size_t pixelCount = (
		FreeImage_GetWidth(bmp) * FreeImage_GetHeight(bmp)
	);
	size_t byteCount = pixelCount * 4;
	const BYTE *src = FreeImage_GetBits(bmp);
	
	Napi::Buffer<uint8_t> buffer = Napi::Buffer<uint8_t>::New(env, byteCount);
	uint8_t *dest = buffer.Data();
	
	// FreeImage stores data in BGR. Convert from BGR to RGB.
	for (size_t i = 0; i < pixelCount; i++) {
		size_t i4 = i << 2;
		dest[i4 + 0] = src[i4 + 2];
		dest[i4 + 1] = src[i4 + 1];
		dest[i4 + 2] = src[i4 + 0];
		dest[i4 + 3] = src[i4 + 3];
	}
	
	return buffer;
	
}


JS_IMPLEMENT_METHOD(Image, _load) { THIS_CHECK;
	
	REQ_BUF_ARG(0, file);
	LET_BOOL_ARG(1, swapBytes);
	
	BYTE *bufferData = reinterpret_cast<BYTE*>(file.Data());
	size_t bufferLength = file.Length();
	
	FIMEMORY *memStream = FreeImage_OpenMemory(bufferData, bufferLength);
	
	FREE_IMAGE_FORMAT format = FreeImage_GetFileTypeFromMemory(memStream, bufferLength);
	FIBITMAP *tmpBitmap = FreeImage_LoadFromMemory(format, memStream);
	
	if (_bitmap) {
		FreeImage_Unload(_bitmap);
	}
	
	_bitmap = FreeImage_ConvertTo32Bits(tmpBitmap);
	
	FreeImage_Unload(tmpBitmap);
	
	FreeImage_CloseMemory(memStream);
	
	// Swap R-B bytes?
	if (swapBytes) {
		size_t pixelCount = (
			FreeImage_GetWidth(_bitmap) * FreeImage_GetHeight(_bitmap)
		);
		BYTE *pixels = FreeImage_GetBits(_bitmap);
		
		for (size_t i = 0; i < pixelCount; i++) {
			size_t i4 = i << 2;
			BYTE temp = pixels[i4 + 0];
			pixels[i4 + 0] = pixels[i4 + 2];
			pixels[i4 + 2] = temp;
		}
	}
	
	Napi::Object that = info.This().As<Napi::Object>();
	that.Set("_data", createBuffer(env, _bitmap));
	
	emit(info, "load");
	
	RET_UNDEFINED;
	
}


JS_IMPLEMENT_METHOD(Image, _unload) { THIS_CHECK;
	
	if (_bitmap) {
		FreeImage_Unload(_bitmap);
		_bitmap = nullptr;
	}
	
	Napi::Object that = info.This().As<Napi::Object>();
	that.Set("_data", env.Null());
	
	emit(info, "load");
	
	RET_UNDEFINED;
	
}


JS_IMPLEMENT_METHOD(Image, save) { THIS_CHECK;
	
	REQ_STR_ARG(0, dest);
	
	FREE_IMAGE_FORMAT format = FreeImage_GetFIFFromFilename(dest.c_str());
	
	if ( ! _bitmap ) {
		RET_BOOL(false);
	}
	
	FIBITMAP *output = FreeImage_Clone(_bitmap);
	
	unsigned bpp = FreeImage_GetBPP(output);
	
	if (format == FIF_JPEG && bpp != 24) {
		FIBITMAP *old = output;
		output = FreeImage_ConvertTo24Bits(output);
		FreeImage_Unload(old);
	}
	
#ifdef _WIN32
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring ws = converter.from_bytes(dest);
	bool ret = FreeImage_SaveU(format, output, ws.c_str()) == 1;
#else
	bool ret = FreeImage_Save(format, output, dest.c_str()) == 1;
#endif
	
	FreeImage_Unload(output);
	
	RET_BOOL(ret);
	
}


// https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/drawImage
JS_IMPLEMENT_METHOD(Image, drawImage) { THIS_CHECK;
	
	REQ_OBJ_ARG(0, _src);
	Image *src = unwrap(_src);
	
	if ( ! src->_bitmap ) {
		RET_UNDEFINED;
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
		RET_UNDEFINED;
	}
	
	FIBITMAP *result = FreeImage_RescaleRect(
		src->_bitmap, dWidth, dHeight, sx, sy, sx + sWidth, sy + sHeight
	);
	
	if (_bitmap) {
		FreeImage_Unload(_bitmap);
	}
	
	_bitmap = result;
	
	Napi::Object that = info.This().As<Napi::Object>();
	that.Set("_data", createBuffer(env, _bitmap));
	
	RET_UNDEFINED;
	
}


JS_IMPLEMENT_METHOD(Image, destroy) { THIS_CHECK;
	emit(info, "destroy");
	_destroy();
	RET_UNDEFINED;
}


JS_IMPLEMENT_GETTER(Image, isDestroyed) { NAPI_ENV;
	RET_BOOL(_isDestroyed);
}


void Image::emit(const Napi::CallbackInfo& info, const char* name) {
	eventEmit(info.This().As<Napi::Object>(), name);
}
