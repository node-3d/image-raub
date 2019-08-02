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


#define RET_VALUE(VAL) return VAL;
#define RET_UNDEFINED RET_VALUE(env.Undefined())
#define RET_NULL RET_VALUE(env.Null())
#define RET_STR(val) RET_VALUE(JS_STR(val))
#define RET_NUM(val) RET_VALUE(JS_NUM(val))
#define RET_EXT(val) RET_VALUE(JS_EXT(val))
#define RET_BOOL(val) RET_VALUE(JS_BOOL(val))
#define RET_FUN(val) RET_VALUE(JS_FUN(val))
#define RET_OBJ(val) RET_VALUE(JS_OBJ(val))


#define JS_THROW(val)                                                         \
	Napi::Error::New(env, val).ThrowAsJavaScriptException();


#define REQ_ARGS(N)                                                           \
	if (info.Length() < (N)) {                                                \
		JS_THROW("Expected at least " #N " arguments");                       \
	}


#define IS_EMPTY(val) (val.IsNull() || val.IsUndefined())
#define IS_ARG_EMPTY(I) IS_EMPTY(info[I])


#define CHECK_REQ_ARG(I, C, T)                                                \
	if (info.Length() <= (I) || ! info[I].C) {                                \
		JS_THROW("Argument " #I " must be of type `" T "`");                  \
	}

#define CHECK_LET_ARG(I, C, T)                                                \
	if ( ! (IS_ARG_EMPTY(I) || info[I].C) ) {                                 \
		JS_THROW(                                                             \
			"Argument " #I                                                    \
			" must be of type `" T                                            \
			"` or be `null`/`undefined`"                                      \
		);                                                                    \
	}


#define REQ_STR_ARG(I, VAR)                                                   \
	CHECK_REQ_ARG(I, IsString(), "String");                                   \
	std::string VAR = info[I].ToString().Utf8Value();

#define USE_STR_ARG(I, VAR, DEF)                                              \
	CHECK_LET_ARG(I, IsString(), "String");                                   \
	std::string VAR = IS_ARG_EMPTY(I) ? (DEF) : info[I].ToString().Utf8Value();

#define LET_STR_ARG(I, VAR) USE_STR_ARG(I, VAR, "")


#define REQ_INT32_ARG(I, VAR)                                                 \
	CHECK_REQ_ARG(I, IsNumber(), "Int32");                                     \
	int VAR = info[I].ToNumber().Int32Value();

#define USE_INT32_ARG(I, VAR, DEF)                                            \
	CHECK_LET_ARG(I, IsNumber(), "Int32");                                    \
	int VAR = IS_ARG_EMPTY(I) ? (DEF) : info[I].Int32Value();

#define LET_INT32_ARG(I, VAR) USE_INT32_ARG(I, VAR, 0)

#define REQ_INT_ARG(I, VAR) REQ_INT32_ARG(I, VAR)
#define USE_INT_ARG(I, VAR, DEF) USE_INT32_ARG(I, VAR, DEF)
#define LET_INT_ARG(I, VAR) LET_INT32_ARG(I, VAR)


#define REQ_UINT32_ARG(I, VAR)                                                \
	CHECK_REQ_ARG(I, IsNumber(), "Uint32");                                   \
	unsigned int VAR = info[I].ToNumber().Uint32Value();

#define USE_UINT32_ARG(I, VAR, DEF)                                           \
	CHECK_LET_ARG(I, IsNumber(), "Uint32");                                   \
	unsigned int VAR = IS_ARG_EMPTY(I)                                        \
		? (DEF)                                                               \
		: info[I].ToNumber().Uint32Value();

#define LET_UINT32_ARG(I, VAR) USE_UINT32_ARG(I, VAR, 0)

#define REQ_UINT_ARG(I, VAR) REQ_UINT_ARG(I, VAR)
#define USE_UINT_ARG(I, VAR, DEF) USE_UINT32_ARG(I, VAR, DEF)
#define LET_UINT_ARG(I, VAR) LET_UINT32_ARG(I, VAR)


#define REQ_BOOL_ARG(I, VAR)                                                  \
	CHECK_REQ_ARG(I, IsBoolean(), "Bool");                                    \
	bool VAR = info[I].ToBoolean().Value();

#define USE_BOOL_ARG(I, VAR, DEF)                                             \
	CHECK_LET_ARG(I, IsBoolean(), "Bool");                                    \
	bool VAR = IS_ARG_EMPTY(I) ? (DEF) : info[I].ToBoolean().Value();

#define LET_BOOL_ARG(I, VAR) USE_BOOL_ARG(I, VAR, false)


#define REQ_OFFS_ARG(I, VAR)                                                  \
	CHECK_REQ_ARG(I, IsNumber(), "Number");                                   \
	size_t VAR = static_cast<size_t>(info[I].ToNumber().DoubleValue());

#define USE_OFFS_ARG(I, VAR, DEF)                                             \
	CHECK_LET_ARG(I, IsNumber(), "Number");                                   \
	size_t VAR = IS_ARG_EMPTY(I)                                              \
	? (DEF)                                                                   \
	: static_cast<size_t>(info[I].ToNumber().DoubleValue());

#define LET_OFFS_ARG(I, VAR) USE_OFFS_ARG(I, VAR, 0)


#define REQ_DOUBLE_ARG(I, VAR)                                                \
	CHECK_REQ_ARG(I, IsNumber(), "Number");                                   \
	double VAR = info[I].ToNumber().DoubleValue();

#define USE_DOUBLE_ARG(I, VAR, DEF)                                           \
	CHECK_LET_ARG(I, IsNumber(), "Number");                                   \
	double VAR = IS_ARG_EMPTY(I) ? (DEF) : info[I].ToNumber().DoubleValue();

#define LET_DOUBLE_ARG(I, VAR) USE_DOUBLE_ARG(I, VAR, 0.0)


#define REQ_FLOAT_ARG(I, VAR)                                                 \
	CHECK_REQ_ARG(I, IsNumber(), "Number");                                   \
	float VAR = info[I].ToNumber().FloatValue();

#define USE_FLOAT_ARG(I, VAR, DEF)                                            \
	CHECK_LET_ARG(I, IsNumber(), "Number");                                   \
	float VAR = IS_ARG_EMPTY(I) ? (DEF) : info[I].ToNumber().FloatValue();

#define LET_FLOAT_ARG(I, VAR) USE_FLOAT_ARG(I, VAR, 0.f)


#define REQ_EXT_ARG(I, VAR)                                                   \
	CHECK_REQ_ARG(I, IsExternal(), "Pointer");                                \
	Napi::External VAR = info[I].As<Napi::External>();

#define USE_EXT_ARG(I, VAR, DEF)                                              \
	CHECK_LET_ARG(I, IsExternal(), "Pointer");                                \
	Napi::External VAR = IS_ARG_EMPTY(I) ? (DEF) : info[I].As<Napi::External>();

#define LET_EXT_ARG(I, VAR) USE_EXT_ARG(I, VAR, JS_EXT(nullptr))


#define REQ_FUN_ARG(I, VAR)                                                   \
	CHECK_REQ_ARG(I, IsFunction(), "Function");                               \
	Napi::Function VAR = info[I].As<Napi::Function>();


#define REQ_OBJ_ARG(I, VAR)                                                   \
	CHECK_REQ_ARG(I, IsObject(), "Object");                                   \
	Napi::Object VAR = info[I].As<Napi::Object>();

#define USE_OBJ_ARG(I, VAR, DEF)                                              \
	CHECK_LET_ARG(I, IsObject(), "Object");                                   \
	Napi::Object VAR = IS_ARG_EMPTY(I) ? (DEF) : info[I].As<Napi::Object>();

#define LET_OBJ_ARG(I, VAR) USE_OBJ_ARG(I, VAR, info[I].As<Napi::Object>())


#define REQ_OBJ_ARG(I, VAR)                                                   \
	CHECK_REQ_ARG(I, IsObject(), "Object");                                   \
	Napi::Object VAR = info[I].As<Napi::Object>();

#define USE_OBJ_ARG(I, VAR, DEF)                                              \
	CHECK_LET_ARG(I, IsObject(), "Object");                                   \
	Napi::Object VAR = IS_ARG_EMPTY(I) ? (DEF) : info[I].As<Napi::Object>();

#define LET_OBJ_ARG(I, VAR) USE_OBJ_ARG(I, VAR, info[I].As<Napi::Object>())


#define REQ_ARRV_ARG(I, VAR)                                                  \
	CHECK_REQ_ARG(I, IsArrayBuffer(), "Object");                              \
	Napi::ArrayBuffer VAR = info[I].As<Napi::ArrayBuffer>();


#define REQ_BUF_ARG(I, VAR)                                                   \
	CHECK_REQ_ARG(I, IsBuffer(), "Buffer");                                   \
	Napi::Buffer<uint8_t> VAR = info[I].As< Napi::Buffer<uint8_t> >();


#define CTOR_CHECK(T)                                                         \
	if ( ! info.IsConstructCall() )                                           \
		JS_THROW(T " must be called with the 'new' keyword.");

#define DES_CHECK                                                             \
	if (_isDestroyed) return;

#define THIS_CHECK                                                            \
	NAPI_ENV;                                                                 \
	if (_isDestroyed) RET_UNDEFINED;

#define SETTER_CHECK(C, T)                                                    \
	if ( ! value.C )                                                          \
		JS_THROW("Value must be " T);


#define JS_METHOD(NAME) Napi::Value NAME(const Napi::CallbackInfo &info)
#define JS_GETTER(NAME) Napi::Value NAME(const Napi::CallbackInfo &info)
#define JS_SETTER(NAME)                                                       \
	void NAME(const Napi::CallbackInfo &info, const Napi::Value &value)

#define ACCESSOR_RW(CLASS, NAME)                                              \
	InstanceAccessor(#NAME, &CLASS::NAME ## Getter, &CLASS::NAME ## Setter)

#define ACCESSOR_R(CLASS, NAME)                                               \
	InstanceAccessor(#NAME, &CLASS::NAME ## Getter, nullptr)

#define ACCESSOR_M(CLASS, NAME)                                                \
	InstanceMethod(#NAME, &CLASS::NAME)


#define SETTER_STR_ARG                                                        \
	SETTER_CHECK(IsNumber(), "String");                                       \
	std::string v = value.ToString().Utf8Value();

#define SETTER_INT32_ARG                                                      \
	SETTER_CHECK(IsNumber(), "Int32");                                         \
	int v = value.ToNumber().Int32Value();

#define SETTER_INT_ARG SETTER_INT32_ARG

#define SETTER_BOOL_ARG                                                       \
	SETTER_CHECK(IsBoolean(), "Bool");                                        \
	bool v = value.ToBoolean().Value();

#define SETTER_UINT32_ARG                                                     \
	SETTER_CHECK(IsNumber(), "Uint32");                                       \
	unsigned int v = value.ToNumber().Uint32Value();

#define SETTER_UINT_ARG SETTER_UINT32_ARG

#define SETTER_OFFS_ARG                                                       \
	SETTER_CHECK(IsNumber(), "Number");                                       \
	size_t v = static_cast<size_t>(value.ToNumber().DoubleValue());

#define SETTER_DOUBLE_ARG                                                     \
	SETTER_CHECK(IsNumber(), "Number");                                       \
	double v = value.ToNumber().DoubleValue();

#define SETTER_FLOAT_ARG                                                      \
	SETTER_CHECK(IsNumber(), "Number");                                       \
	float v = value.ToNumber().FloatValue();

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


#define GET_AND_THROW_LAST_ERROR()                                            \
	do {                                                                      \
		const napi_extended_error_info *error_info;                           \
		napi_get_last_error_info((env), &error_info);                         \
		bool is_pending;                                                      \
		napi_is_exception_pending((env), &is_pending);                        \
		/* If an exception is already pending, don't rethrow it */            \
		if (!is_pending) {                                                    \
			const char* error_message = error_info->error_message != NULL     \
				? error_info->error_message                                   \
				: "empty error message";                                      \
			JS_THROW(error_message);                                          \
		}                                                                     \
	} while (0)

#define NAPI_CALL(the_call, ATE)                                              \
	do {                                                                      \
		if ((the_call) != napi_ok) {                                          \
			GET_AND_THROW_LAST_ERROR();                                       \
			ATE;                                                              \
		}                                                                     \
	} while (0)

#define JS_RUN_3(code, VAR, ATE)                                              \
	napi_value __RESULT_ ## VAR;                                              \
	NAPI_CALL(                                                                \
		napi_run_script(env, napi_value(JS_STR(code)), &__RESULT_ ## VAR),    \
		ATE                                                                   \
	);                                                                        \
	Napi::Value VAR(env, __RESULT_ ## VAR);

#define JS_RUN_2(code, VAR) JS_RUN_3(code, VAR, return)
#define JS_RUN JS_RUN_3


template<typename Type>
inline Type* getArrayData(Napi::Env env, Napi::Object obj, int *num = nullptr) {
	
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
	data = static_cast<Type *>(arr.Data());
	
	return data;
	
}

template<typename Type>
inline Type* getBufferData(Napi::Env env, Napi::Object obj, int *num = nullptr) {
	
	Type *data = nullptr;
	
	if (num) {
		*num = 0;
	}
	
	if ( ! obj.IsBuffer() ) {
		JS_THROW("Argument must be of type `Buffer`.");
		return data;
	}
	
	Napi::Buffer<uint8_t> arr = obj.As< Napi::Buffer<uint8_t> >();
	if (num) {
		*num = arr.Length() / sizeof(Type);
	}
	data = arr.Data();
	
	return data;
	
}


inline void *getData(Napi::Env env, Napi::Object obj) {
	
	void *pixels = nullptr;
	
	if (obj.IsArrayBuffer()) {
		pixels = getArrayData<unsigned char>(env, obj);
	} else if (obj.IsTypedArray()) {
		pixels = getArrayData<unsigned char>(
			env,
			obj.As<Napi::TypedArray>().ArrayBuffer()
		);
	} else if (obj.Has("data")) {
		Napi::Object data = obj.Get("data").As<Napi::Object>();
		if (data.IsArrayBuffer()) {
			pixels = getArrayData<unsigned char>(env, data);
		} else if (data.IsBuffer()) {
			pixels = getBufferData<unsigned char>(env, data);
		} else if (data.IsTypedArray()) {
			pixels = getArrayData<unsigned char>(
				env,
				data.As<Napi::TypedArray>().ArrayBuffer()
			);
		}
	}
	
	return pixels;
	
}


inline void consoleLog(Napi::Env env, int argc, Napi::Value *argv) {
	
	JS_RUN_2("((...args) => console.log(...args))", log);
	std::vector<napi_value> args;
	for (int i = 0; i < argc; i++) {
		args.push_back(napi_value(argv[i]));
	}
	
	log.As<Napi::Function>().MakeCallback(napi_value(env.Null()), args);
	
}


inline void consoleLog(Napi::Env env, const std::string &message) {
	
	Napi::Value arg = JS_STR(message);
	consoleLog(env, 1, &arg);
	
}


#endif // _ADDON_TOOLS_HPP_
