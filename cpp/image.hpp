#ifndef _IMAGE_HPP_
#define _IMAGE_HPP_

#include <FreeImage.h>

#include <napi.h>

#include <at.hpp>


class Image : public Napi::ObjectWrap<Image> {
	
public:
	
	// Public Napi init
	static void init(Napi::Env env, Napi::Object exports);
	
	Image(const Napi::CallbackInfo& info);
	
	static bool isImage(Napi::Object obj);
	
	// Destroy an instance from C++ land
	void _destroy();
	
protected:
	
	JS_GETTER(_dataGetter);
	JS_GETTER(isDestroyedGetter);
	JS_GETTER(widthGetter);
	JS_GETTER(heightGetter);
	
	JS_METHOD(destroy);
	
	JS_METHOD(load);
	JS_METHOD(unload);
	JS_METHOD(save);
	
	JS_METHOD(drawImage);
	
	~Image();
	
 private:
	
	static Napi::FunctionReference _constructor;
	
	Napi::Value _data;
	
	bool _isDestroyed;
	
	FIBITMAP *_bitmap;
	
};


#endif // _IMAGE_HPP_
