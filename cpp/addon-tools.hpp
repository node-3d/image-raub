#ifndef _ADDON_TOOLS_HPP_
#define _ADDON_TOOLS_HPP_


#include <napi.h>


#define NAPI_ENV Napi::Env env = info.Env();
#define NAPI_HS Napi::HandleScope scope(env);


#define JS_STR(val) Napi::String::New(env, val)
#define JS_NUM(val) Napi::Number::New(env, static_cast<double>(val))
#define JS_EXT(val) Napi::External::New(env, reinterpret_cast<void*>(val))
#define JS_BOOL(val) Napi::Boolean::New(env, val)
#define JS_FUN(val) Napi::Function::Function(env, val)
#define JS_OBJ(val) Napi::Object::Object(env, val)


#define RET_VALUE(VAL) return VAL
#define RET_UNDEFINED RET_VALUE(Napi::Env::Undefined())
#define RET_NULL RET_VALUE(Napi::Env::Null())
#define RET_STR(val) RET_VALUE(JS_STR(val))
#define RET_NUM(val) RET_VALUE(JS_NUM(val))
#define RET_EXT(val) RET_VALUE(JS_EXT(val))
#define RET_BOOL(val) RET_VALUE(JS_BOOL(val))
#define RET_FUN(val) RET_VALUE(JS_FUN(val))
#define RET_OBJ(val) RET_VALUE(JS_OBJ(val))


#define JS_THROW(val)                                                         \
	Napi::Error::New(env, val).ThrowAsJavaScriptException();                  \
	return;


#define REQ_ARGS(N)                                                           \
	if (info.Length() < (N))                                                  \
		JS_THROW("Expected at least " #N " arguments");


#define IS_EMPTY(val) (val->IsNull() || val->IsUndefined())
#define IS_ARG_EMPTY(I) IS_EMPTY(info[I])


#define CHECK_REQ_ARG(I, C, T)                                                \
	if (info.Length() <= (I) || ! info[I]->C)                                 \
		JS_THROW("Argument " #I " must be of type `" T "`");

#define CHECK_LET_ARG(I, C, T)                                                \
	if ( ! (IS_ARG_EMPTY(I) || info[I]->C) )                                  \
		JS_THROW("Argument " #I " must be of type `" T "` or be `null`/`undefined`");


#define REQ_STR_ARG(I, VAR)                                                   \
	CHECK_REQ_ARG(I, IsString(), "String");                                   \
	std::string VAR = info[I].ToString().Utf8Value();

#define LET_STR_ARG(I, VAR)                                                   \
	CHECK_LET_ARG(I, IsString(), "String");                                   \
	std::string VAR = IS_ARG_EMPTY(I) ? "" : info[I].ToString().Utf8Value();

#define REQ_INT32_ARG(I, VAR)                                                 \
	CHECK_REQ_ARG(I, IsInt32(), "Int32");                                     \
	int VAR = info[I].ToNumber()->Int32Value();

#define LET_INT32_ARG(I, VAR)                                                 \
	CHECK_LET_ARG(I, IsInt32(), "Int32");                                     \
	int VAR = IS_ARG_EMPTY(I) ? 0 : info[I].ToNumber()->Int32Value();
	
#define REQ_UINT32_ARG(I, VAR)                                                \
	CHECK_REQ_ARG(I, IsUint32(), "Uint32");                                   \
	unsigned int VAR = info[I].ToNumber()->Uint32Value();

#define LET_UINT32_ARG(I, VAR)                                                \
	CHECK_LET_ARG(I, IsUint32(), "Uint32");                                   \
	unsigned int VAR = IS_ARG_EMPTY(I) ? 0 : info[I].ToNumber()->Uint32Value();

#define REQ_INT_ARG(I, VAR) LET_INT32_ARG(I, VAR)
#define LET_INT_ARG(I, VAR) REQ_UINT32_ARG(I, VAR)

#define REQ_BOOL_ARG(I, VAR)                                                  \
	CHECK_REQ_ARG(I, IsBoolean(), "Bool");                                    \
	bool VAR = info[I].ToBoolean().Value();

#define LET_BOOL_ARG(I, VAR)                                                  \
	CHECK_LET_ARG(I, IsBoolean(), "Bool");                                    \
	bool VAR = IS_ARG_EMPTY(I) ? false : info[I].ToBoolean().Value();

#define REQ_OFFS_ARG(I, VAR)                                                  \
	CHECK_REQ_ARG(I, IsNumber(), "Number");                                   \
	size_t VAR = static_cast<size_t>(info[I].ToNumber()->DoubleValue());

#define LET_OFFS_ARG(I, VAR)                                                  \
	CHECK_LET_ARG(I, IsNumber(), "Number");                                   \
	size_t VAR = IS_ARG_EMPTY(I)                                              \
		? 0                                                                   \
		: static_cast<size_t>(info[I].ToNumber()->DoubleValue());


#define REQ_DOUBLE_ARG(I, VAR)                                                \
	CHECK_REQ_ARG(I, IsNumber(), "Number");                                   \
	double VAR = info[I].ToNumber()->DoubleValue();

#define LET_DOUBLE_ARG(I, VAR)                                                \
	CHECK_LET_ARG(I, IsNumber(), "Number");                                   \
	double VAR = IS_ARG_EMPTY(I) ? 0.0 : info[I].ToNumber()->DoubleValue();


#define REQ_FLOAT_ARG(I, VAR)                                                 \
	CHECK_REQ_ARG(I, IsNumber(), "Number");                                   \
	float VAR = info[I].ToNumber()->FloatValue();

#define LET_FLOAT_ARG(I, VAR)                                                 \
	CHECK_LET_ARG(I, IsNumber(), "Number");                                   \
	float VAR = IS_ARG_EMPTY(I) ? 0.f : info[I].ToNumber()->FloatValue();


#define REQ_EXT_ARG(I, VAR)                                                   \
	CHECK_REQ_ARG(I, IsExternal(), "Pointer");                                \
	Napi::External VAR = info[I].As<Napi::External>();

#define LET_EXT_ARG(I, VAR)                                                   \
	CHECK_LET_ARG(I, IsExternal(), "Number");                                 \
	Napi::External VAR = IS_ARG_EMPTY(I)
		? JS_EXT(nullptr)
		: info[I].As<Napi::External>();


#define REQ_FUN_ARG(I, VAR)                                                   \
	CHECK_REQ_ARG(I, IsFunction(), "Function");                               \
	Napi::Function VAR = info[I].As<Napi::Function>();


#define REQ_OBJ_ARG(I, VAR)                                                   \
	CHECK_REQ_ARG(I, IsObject(), "Object");                                   \
	Napi::Object VAR = info[I].As<Napi::Object>();

#define LET_OBJ_ARG(I, VAR)                                                   \
	CHECK_LET_ARG(I, IsObject(), "Object");                                   \
	Napi::Object VAR = IS_ARG_EMPTY(I)                                        \
		? Napi::Object::Object()                                              \
		: info[I].As<Napi::Object>();


#define REQ_ARRV_ARG(I, VAR)                                                  \
	CHECK_REQ_ARG(I, IsArrayBuffer(), "Object");                              \
	Napi::ArrayBuffer VAR = info[I].As<Napi::ArrayBuffer>();


#define SET_PROP(OBJ, KEY, VAL) OBJ->Set(JS_STR(KEY), VAL);
#define SET_I(ARR, I, VAL) ARR->Set(I, VAL);


#define CTOR_CHECK(T)                                                         \
	if ( ! info.IsConstructCall() )                                           \
		JS_THROW(T " must be called with the 'new' keyword.");

#define DES_CHECK                                                             \
	if (_isDestroyed) return;

#define SETTER_CHECK(C, T)                                                    \
	if ( ! value->C )                                                         \
		JS_THROW("Value must be " T);


#define ACCESSOR_RW(OBJ, NAME)                                                \
	Napi::InstanceAccessor(#NAME, NAME ## Getter, NAME ## Setter);

#define ACCESSOR_R(OBJ, NAME)                                                 \
	Napi::InstanceAccessor(#NAME, NAME ## Getter, nullptr);


#define SETTER_STR_ARG                                                        \
	SETTER_CHECK(IsString(), "String");                                       \
	std::string v = value.ToString().Utf8Value();

#define SETTER_INT32_ARG                                                      \
	SETTER_CHECK(IsInt32(), "Int32");                                         \
	int v = value.ToNumber()->Int32Value();

#define SETTER_INT_ARG SETTER_INT32_ARG

#define SETTER_BOOL_ARG                                                       \
	SETTER_CHECK(IsBoolean(), "Bool");                                        \
	bool v = value.ToBoolean().Value();

#define SETTER_UINT32_ARG                                                     \
	SETTER_CHECK(IsUint32(), "Uint32");                                       \
	unsigned int v = value.ToNumber()->Uint32Value();

#define SETTER_UINT_ARG SETTER_UINT32_ARG

#define SETTER_OFFS_ARG                                                       \
	SETTER_CHECK(IsNumber(), "Number");                                       \
	size_t v = static_cast<size_t>(value.ToNumber()->DoubleValue());

#define SETTER_DOUBLE_ARG                                                     \
	SETTER_CHECK(IsNumber(), "Number");                                       \
	double v = value.ToNumber()->DoubleValue();

#define SETTER_FLOAT_ARG                                                      \
	SETTER_CHECK(IsNumber(), "Number");                                       \
	float v = value.ToNumber()->FloatValue();

#define SETTER_EXT_ARG                                                        \
	SETTER_CHECK(IsExternal(), "Pointer");                                    \
	Napi::External v = value.As<Napi::External>();

#define SETTER_FUN_ARG                                                        \
	SETTER_CHECK(IsFunction(), "Function");                                   \
	Napi::Function v = value.As<Napi::Function>()

#define SETTER_OBJ_ARG                                                        \
	SETTER_CHECK(IsObject(), "Object");                                       \
	Napi::Object v = value.As<Napi::Object>()

#define SETTER_ARRV_ARG                                                       \
	SETTER_CHECK(IsArrayBuffer(), "TypedArray");                              \
	Napi::ArrayBuffer v = value.As<Napi::ArrayBuffer>();


template<typename Type>
inline Type* getArrayData(Napi::Object obj, int *num = nullptr) {
	
	Type *data = nullptr;
	
	if (num) {
		*num = 0;
	}
	
	if ( ! obj.IsArrayBuffer() ) {
		JS_THROW("Argument must be of type `TypedArray`.");
		return data;
	}
	
	Napi::ArrayBuffer arr = obj.As<Napi::ArrayBuffer>();
	if (num) {
		*num = arr.ByteLength() / sizeof(Type);
	}
	data = arr.Data<Type*>();
	
	return data;
	
}

template<typename Type>
inline Type* getBufferData(Napi::Object obj, int *num = nullptr) {
	
	Type *data = nullptr;
	
	if (num) {
		*num = 0;
	}
	
	if ( ! obj.IsBuffer() ) {
		JS_THROW("Argument must be of type `Buffer`.");
		return data;
	}
	
	Napi::Buffer arr = obj.As<Napi::Buffer>();
	if (num) {
		*num = arr.Length() / sizeof(Type);
	}
	data = arr.Data<Type*>();
	
	return data;
	
}


inline void *getData(Napi::Object obj) {
	
	void *pixels = nullptr;
	
	if (obj->IsArrayBuffer()) {
		pixels = getArrayData<unsigned char>(obj);
	} else if (obj->IsTypedArray()) {
		pixels = getArrayData<unsigned char>(obj.As<Napi::TypedArray>().ArrayBuffer());
	} else if (obj.Has("data")) {
		Napi::Value data = obj.Get("data");
		if (data->IsArrayBuffer()) {
			pixels = getArrayData<unsigned char>(data);
		} else if (data->IsBuffer()) {
			pixels = getBufferData<unsigned char>(data);
		} else if (obj->IsTypedArray()) {
			pixels = getArrayData<unsigned char>(data.As<Napi::TypedArray>().ArrayBuffer());
		}
	}
	
	return pixels;
	
}


inline void consoleLog(int argc, Napi::Value *argv) {
	// NAPI_EXTERN napi_status napi_run_script(napi_env env,
 //                                        napi_value script,
 //                                        napi_value* result);
	Napi::String code = JS_STR("((...args) => console.log(...args))");
	
	Napi::Value log = v8::Script::Compile(
		Nan::GetCurrentContext(), code
	)->Run(
		Nan::GetCurrentContext()
	);
	Nan::Callback logCb(Nan::To<v8::Function>(log));
	
	Nan::AsyncResource async("consoleLog()");
	
	logCb.Call(argc, argv, &async);
	
}


inline void consoleLog(const std::string &message) {
	
	Napi::Value arg = JS_STR(message);
	consoleLog(1, &arg);
	
}


#endif // _ADDON_TOOLS_HPP_
