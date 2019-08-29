#ifndef _IMAGE_HPP_
#define _IMAGE_HPP_

#include <FreeImage.h>
#define NODE_ADDON_API_DISABLE_DEPRECATED
#include <addon-tools.hpp>


inline void inheritEs5(napi_env env, napi_value ctor, napi_value superCtor) {
	napi_value global, globalObject, setProto, ctorProtoProp, superCtorProtoProp;
	napi_value argv[2];
	
	napi_get_global(env, &global);
	napi_get_named_property(env, global, "Object", &globalObject);
	napi_get_named_property(env, globalObject, "setPrototypeOf", &setProto);
	napi_get_named_property(env, ctor, "prototype", &ctorProtoProp);
	napi_get_named_property(env, superCtor, "prototype", &superCtorProtoProp);
	
	argv[0] = ctorProtoProp;
	argv[1] = superCtorProtoProp;
	napi_call_function(env, global, setProto, 2, argv, nullptr);
	
	argv[0] = ctor;
	argv[1] = superCtor;
	napi_call_function(env, global, setProto, 2, argv, nullptr);
}

typedef Napi::Value (*StaticMethodCallback)(const Napi::CallbackInfo& info);
typedef Napi::Value (*StaticGetterCallback)(const Napi::CallbackInfo& info);
typedef void (*StaticSetterCallback)(const Napi::CallbackInfo& info);

#define DECLARE_ES5_CLASS(CLASS, NAME) \
	static Napi::FunctionReference _constructor; \
	static void CLASS::_finalizeEs5(napi_env e, void *dest, void* hint); \
	static napi_value CLASS::_createEs5(napi_env e, napi_callback_info i); \
	inline void super( \
		const Napi::CallbackInfo& info, \
		int argc = 0, \
		const Napi::Value *argv = nullptr \
	) { \
		Napi::Function ctor = _constructor.Value(); \
		if (ctor.Has("_super")) { \
			Napi::Function _super = ctor.Get("_super").As<Napi::Function>(); \
			std::vector<napi_value> args; \
			for (int i = 0; i < argc; i++) { \
				args.push_back(argv[i]); \
			} \
			_super.Call(info.This(), args); \
		} \
	}; \
	inline static Napi::Function wrap(Napi::Env env) { \
		napi_value __initResult; \
		napi_create_function(env, #NAME, 0, CLASS::_createEs5, nullptr, &__initResult); \
		Napi::Function ctor = Napi::Function(env, __initResult); \
		_constructor = Napi::Persistent(ctor); \
		_constructor.SuppressDestruct(); \
		return ctor; \
	}; \
	inline static Napi::Function wrap( \
		Napi::Env env, \
		Napi::Function superCtor \
	) { \
		Napi::Function ctor = wrap(env); \
		inheritEs5(env, ctor, superCtor); \
		ctor.Set("_super", superCtor); \
		return ctor; \
	}; \
	inline static void method( \
		const char *name, \
		StaticMethodCallback cb \
	) { \
		Napi::Function proto = _constructor.Value().Get("prototype").As<Napi::Function>(); \
		proto.DefineProperty(                                                   \
			Napi::PropertyDescriptor::Function(proto.Env(), proto, name, cb)   \
		); \
	}; \
	inline static void accessorR( \
		const char *name, \
		StaticGetterCallback getter \
	) { \
		Napi::Function proto = _constructor.Value().Get("prototype").As<Napi::Function>(); \
		proto.DefineProperty(                                                   \
			Napi::PropertyDescriptor::Accessor(proto.Env(), proto, name, getter)   \
		); \
	}; \
	inline static void accessorRw( \
		const char *name, \
		StaticGetterCallback getter, \
		StaticSetterCallback setter \
	) { \
		Napi::Function proto = _constructor.Value().Get("prototype").As<Napi::Function>(); \
		proto.DefineProperty(                                                   \
			Napi::PropertyDescriptor::Accessor( \
				proto.Env(), \
				proto, \
				name, \
				getter, \
				setter \
			)   \
		); \
	};

#define JS_DECLARE_METHOD(CLASS, NAME) \
	inline static Napi::Value __st_##NAME(const Napi::CallbackInfo &info) { \
		CLASS *that; \
		napi_unwrap(info.Env(), info.This(), reinterpret_cast<void**>(&that)); \
		return that->__i_##NAME(info); \
	}; \
	Napi::Value __i_##NAME(const Napi::CallbackInfo &info);

#define JS_DECLARE_GETTER(CLASS, NAME) JS_DECLARE_METHOD(CLASS, NAME##Getter)

#define JS_DECLARE_SETTER(CLASS, NAME)                                                       \
	inline static void __st_##NAME##Setter( \
		const Napi::CallbackInfo &info \
	) { \
		CLASS *that; \
		napi_unwrap(info.Env(), info.This(), reinterpret_cast<void**>(&that)); \
		that->__i_##NAME(info, info[0]); \
	}; \
	void __i_##NAME##Setter(const Napi::CallbackInfo &info, const Napi::Value &value);

#define JS_IMPLEMENT_METHOD(CLASS, NAME) \
	Napi::Value CLASS::__i_##NAME(const Napi::CallbackInfo &info)

#define JS_IMPLEMENT_GETTER(CLASS, NAME) JS_IMPLEMENT_METHOD(CLASS, NAME##Getter)

#define JS_IMPLEMENT_SETTER(CLASS, NAME) \
	void CLASS::__i_##NAME##Setter( \
		const Napi::CallbackInfo &info, \
		const Napi::Value &value \
	)

#define JS_ASSIGN_METHOD(NAME) method(#NAME, __st_##NAME)
#define JS_ASSIGN_GETTER(NAME) accessorR(#NAME, __st_##NAME##Getter)
#define JS_ASSIGN_SETTER(NAME) accessorRw(#NAME, __st_##NAME##Getter, __st_##NAME##Setter)

#define IMPLEMENT_ES5_CLASS(CLASS) \
	Napi::FunctionReference CLASS::_constructor; \
	void CLASS::_finalizeEs5(napi_env e, void *dest, void* hint) { \
		CLASS *instance = reinterpret_cast<CLASS*>(dest); \
		delete instance; \
	} \
	napi_value CLASS::_createEs5(napi_env env, napi_callback_info i) { \
		Napi::CallbackInfo info(env, i); \
		CLASS *instance = new CLASS(info); \
		napi_wrap(env, info.This(), instance, CLASS::_finalizeEs5, nullptr, nullptr); \
		return info.Env().Undefined(); \
	}


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
