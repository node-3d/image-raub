#ifndef _IMAGE_HPP_
#define _IMAGE_HPP_


#include <event-emitter.hpp>

#include <FreeImage.h>



#include <napi.h>

class MyObject : public Napi::ObjectWrap<MyObject> {
 public:
  static void init(Napi::Env env, Napi::Object exports);
  MyObject(const Napi::CallbackInfo& info);

 private:
  static Napi::FunctionReference constructor;

  Napi::Value GetValue(const Napi::CallbackInfo& info);
  Napi::Value PlusOne(const Napi::CallbackInfo& info);
  Napi::Value Multiply(const Napi::CallbackInfo& info);

  double value_;

	bool _isDestroyed;
	
	FIBITMAP *_bitmap;
};


class Image : public EventEmitter {
	
public:
	
	// Public V8 init/deinit
	static void init(V8_VAR_OBJ target);
	
	static bool isImage(V8_VAR_OBJ obj);
	
	// Destroy an instance from C++ land
	void _destroy();
	
protected:
	
	static NAN_METHOD(newCtor);
	
	static NAN_METHOD(destroy);
	static NAN_GETTER(isDestroyedGetter);
	
	static NAN_GETTER(widthGetter);
	static NAN_GETTER(heightGetter);
	
	static NAN_METHOD(load);
	static NAN_METHOD(unload);
	static NAN_METHOD(save);
	
	static NAN_METHOD(drawImage);
	
	~Image();
	
	
private:
	
	static V8_STORE_FT _protoImage; // for inheritance
	static V8_STORE_FUNC _ctorImage;
	
	bool _isDestroyed;
	
	FIBITMAP *_bitmap;
	
};

#endif // _IMAGE_HPP_
