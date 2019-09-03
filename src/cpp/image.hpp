#ifndef _IMAGE_HPP_
#define _IMAGE_HPP_

#include <FreeImage.h>

#include <addon-tools.hpp>

class Image {
DECLARE_ES5_CLASS(Image, Image);
	
public:
	static void init(Napi::Env env, Napi::Object exports);
	explicit Image(const Napi::CallbackInfo& info);
	~Image();
	
	void _destroy();
	
 private:
	JS_DECLARE_GETTER(Image, isDestroyed);
	JS_DECLARE_GETTER(Image, width);
	JS_DECLARE_GETTER(Image, height);
	
	JS_DECLARE_METHOD(Image, destroy);
	JS_DECLARE_METHOD(Image, _load);
	JS_DECLARE_METHOD(Image, _unload);
	JS_DECLARE_METHOD(Image, save);
	JS_DECLARE_METHOD(Image, drawImage);
	
	void emit(const Napi::CallbackInfo& info, const char* name);
	
	bool _isDestroyed;
	FIBITMAP *_bitmap;
	
};


#endif // _IMAGE_HPP_
