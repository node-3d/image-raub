#ifndef _EVENT_EMITTER_
#define _EVENT_EMITTER_


#include <addon-tools.hpp>

#include <string>
#include <map>
#include <deque>


#define THIS_EVENT_EMITTER                                                    \
	EventEmitter *eventEmitter = ObjectWrap::Unwrap<EventEmitter>(info.This());

#define EVENT_EMITTER_THIS_CHECK                                              \
	if (eventEmitter->_isDestroyed) return;


// This template class provides static-member initialization in-header
template <typename T>
class StaticHolder {
protected:
	static V8_STORE_FT _protoEventEmitter;
	static V8_STORE_FUNC _ctorEventEmitter;
};
template <typename T> V8_STORE_FT StaticHolder<T>::_protoEventEmitter;
template <typename T> V8_STORE_FUNC StaticHolder<T>::_ctorEventEmitter;


class EventEmitter : public StaticHolder<int>, public Nan::ObjectWrap {
	
	typedef Nan::CopyablePersistentTraits<v8::Function>::CopyablePersistent FN_TYPE;
	typedef std::deque<FN_TYPE> VEC_TYPE;
	typedef std::map<std::string, VEC_TYPE> MAP_TYPE;
	typedef std::map<int, FN_TYPE> FNMAP_TYPE;
	typedef VEC_TYPE::iterator IT_TYPE;
	typedef MAP_TYPE::iterator MAP_IT_TYPE;
	typedef FNMAP_TYPE::iterator FNMAP_IT_TYPE;
	
public:
	
	// Public V8 init
	static void init(V8_VAR_OBJ target) {
		
		V8_VAR_FT proto = Nan::New<v8::FunctionTemplate>(newCtor);
		
		proto->InstanceTemplate()->SetInternalFieldCount(1);
		proto->SetClassName(JS_STR("EventEmitter"));
		
		
		// Accessors
		V8_VAR_OT obj = proto->PrototypeTemplate();
		ACCESSOR_R(obj, isDestroyed);
		
		
		// -------- dynamic
		
		Nan::SetPrototypeMethod(proto, "listenerCount", jsListenerCount);
		Nan::SetPrototypeMethod(proto, "addEventListener", jsAddListener);
		Nan::SetPrototypeMethod(proto, "addListener", jsAddListener);
		Nan::SetPrototypeMethod(proto, "dispatchEvent", jsDispatchEvent);
		Nan::SetPrototypeMethod(proto, "emit", jsEmit);
		Nan::SetPrototypeMethod(proto, "eventNames", jsEventNames);
		Nan::SetPrototypeMethod(proto, "getMaxListeners", jsGetMaxListeners);
		Nan::SetPrototypeMethod(proto, "listeners", jsListeners);
		Nan::SetPrototypeMethod(proto, "off", jsRemoveListener);
		Nan::SetPrototypeMethod(proto, "on", jsAddListener);
		Nan::SetPrototypeMethod(proto, "once", jsAddListener);
		Nan::SetPrototypeMethod(proto, "prependListener", jsPrependListener);
		Nan::SetPrototypeMethod(proto, "prependOnceListener", jsPrependOnceListener);
		Nan::SetPrototypeMethod(proto, "removeAllListeners", jsRemoveAllListeners);
		Nan::SetPrototypeMethod(proto, "removeEventListener", jsRemoveListener);
		Nan::SetPrototypeMethod(proto, "removeListener", jsRemoveListener);
		Nan::SetPrototypeMethod(proto, "setMaxListeners", jsSetMaxListeners);
		Nan::SetPrototypeMethod(proto, "rawListeners", jsRawListeners);
		
		Nan::SetPrototypeMethod(proto, "destroy", jsDestroy);
		
		// -------- static
		
		V8_VAR_FUNC ctor = Nan::GetFunction(proto).ToLocalChecked();
		
		V8_VAR_OBJ ctorObj = V8_VAR_OBJ::Cast(ctor);
		
		Nan::SetMethod(ctorObj, "listenerCount", jsStaticListenerCount);
		
		
		_ctorEventEmitter.Reset(ctor);
		_protoEventEmitter.Reset(proto);
		
		Nan::Set(target, JS_STR("EventEmitter"), ctor);
		
	}
	
	
	// C++ side emit() method
	void emit(const std::string &name, int argc = 0, V8_VAR_VAL *argv = NULL) {
		
		// Important! As actual get map[key] produces a new (empty) map entry
		if ( _listeners.find(name) == _listeners.end() ) {
			return;
		}
		
		// A copy is intended, because handlers can call removeListener (and they DO)
		VEC_TYPE list = _listeners[name];
		
		if (list.empty()) {
			return;
		}
		
		for (IT_TYPE it = list.begin(); it != list.end(); ++it) {
			
			Nan::Callback callback(Nan::New(*it));
			
			if ( ! callback.IsEmpty() ) {
				Nan::AsyncResource async("EventEmitter::cpp_emit()");
				callback.Call(handle(), argc, argv, &async);
			}
			
		}
		
	}
	
	
	// C++ side on() method
	void on(const std::string &name, V8_VAR_FUNC cb) {
		
		V8_VAR_OBJ me = handle();
		
		V8_VAR_VAL onVal = Nan::Get(me, JS_STR("on")).ToLocalChecked();
		V8_VAR_FUNC onFunc = V8_VAR_FUNC::Cast(onVal);
		
		Nan::Callback onCb(onFunc);
		
		V8_VAR_VAL argv[] = { JS_STR(name.c_str()), cb };
		Nan::AsyncResource async("EventEmitter::cpp_on()");
		
		onCb.Call(me, 2, argv, &async);
		
	}
	
	
	void _destroy() { DES_CHECK;
		
		_isDestroyed = true;
		
		_listeners.clear();
		_raw.clear();
		_wrappedIds.clear();
		_rawIds.clear();
		
	}
	
	
	~EventEmitter () { _destroy(); }
	
	
protected:
	
	EventEmitter () {
		_isDestroyed = false;
		_maxListeners = 0;
		_freeId = 0;
	}
	
	
private:
	
	static NAN_METHOD(newCtor) {
		
		CTOR_CHECK("EventEmitter");
		
		EventEmitter *eventEmitter = new EventEmitter();
		eventEmitter->Wrap(info.This());
		
		RET_VALUE(info.This());
		
	}
	
	
	static NAN_GETTER(isDestroyedGetter) { THIS_EVENT_EMITTER; EVENT_EMITTER_THIS_CHECK;
		
		RET_BOOL(eventEmitter->_isDestroyed);
		
	}
	
	
	// Deprecated static method
	static NAN_METHOD(jsStaticListenerCount) {
		
		REQ_OBJ_ARG(0, obj);
		EventEmitter *eventEmitter = ObjectWrap::Unwrap<EventEmitter>(obj);
		REQ_UTF8_ARG(1, name);
		
		const VEC_TYPE &list = eventEmitter->_listeners[*name];
		
		RET_INT(static_cast<int>(list.size()));
		
	}
	
	
	static NAN_METHOD(jsAddListener) {
		
		_wrapListener(info);
		
		RET_VALUE(info.This());
		
	}
	
	
	static NAN_METHOD(jsDispatchEvent) { THIS_EVENT_EMITTER; EVENT_EMITTER_THIS_CHECK;
		
		REQ_OBJ_ARG(0, event);
		
		if ( ! event->Has(JS_STR("type")) ) {
			return Nan::ThrowError("Event must have the `type` property.");
		}
		
		Nan::Utf8String name(event->Get(JS_STR("type")));
		
		V8_VAR_VAL args = event;
		
		eventEmitter->emit(*name, 1, &args);
		
		RET_BOOL(true);
		
	}
	
	
	static NAN_METHOD(jsEmit) { THIS_EVENT_EMITTER; EVENT_EMITTER_THIS_CHECK;
		
		REQ_UTF8_ARG(0, name);
		
		int length = info.Length();
		
		std::vector< V8_VAR_VAL > args;
		
		for (int i = 1; i < length; i++) {
			args.push_back(info[i]);
		}
		
		eventEmitter->emit(*name, length - 1, &args[0]);
		
		if ( eventEmitter->_listeners.find(*name) == eventEmitter->_listeners.end() ) {
			RET_BOOL(false);
		} else {
			RET_BOOL(true);
		}
		
	}
	
	
	static NAN_METHOD(jsEventNames) { THIS_EVENT_EMITTER; EVENT_EMITTER_THIS_CHECK;
		
		V8_VAR_ARR jsNames = Nan::New<v8::Array>(eventEmitter->_raw.size());
		
		if (eventEmitter->_raw.empty()) {
			RET_VALUE(jsNames);
			return;
		}
		
		int i = 0;
		for (MAP_IT_TYPE it = eventEmitter->_raw.begin(); it != eventEmitter->_raw.end(); ++it, i++) {
			
			jsNames->Set(JS_INT(i), JS_STR(it->first));
			
		}
		
		RET_VALUE(jsNames);
		
	}
	
	
	static NAN_METHOD(jsGetMaxListeners) { THIS_EVENT_EMITTER; EVENT_EMITTER_THIS_CHECK;
		
		RET_INT(eventEmitter->_maxListeners);
		
	}
	
	
	static NAN_METHOD(jsListenerCount) { THIS_EVENT_EMITTER; EVENT_EMITTER_THIS_CHECK;
		
		REQ_UTF8_ARG(0, name);
		
		const VEC_TYPE &list = eventEmitter->_listeners[*name];
		
		RET_INT(static_cast<int>(list.size()));
		
	}
	
	
	static NAN_METHOD(jsListeners) { THIS_EVENT_EMITTER; EVENT_EMITTER_THIS_CHECK;
		
		REQ_UTF8_ARG(0, name);
		
		VEC_TYPE &list = eventEmitter->_listeners[*name];
		
		V8_VAR_ARR jsListeners = Nan::New<v8::Array>(list.size());
		
		if (list.empty()) {
			RET_VALUE(jsListeners);
			return;
		}
		
		int i = 0;
		for (IT_TYPE it = list.begin(); it != list.end(); ++it, i++) {
			
			jsListeners->Set(JS_INT(i), Nan::New(*it));
			
		}
		
		RET_VALUE(jsListeners);
		
	}
	
	
	static inline void _reportListeners(
		const std::string &name,
		int numListeners,
		int maxListeners
	) {
		
		std::string msg = "EventEmitter Warning: too many listeners (";
		msg += std::to_string(numListeners);
		msg += " > ";
		msg += std::to_string(maxListeners);
		msg += ") on '";
		msg += name;
		msg += "' event, possible memory leak.\n";
		
		// Some JS magic to retrieve the call stack
		V8_VAR_STR code = JS_STR(
			"(new Error()).stack.split('\\n').slice(2).join('\\n')"
		);
		
		v8::Local<v8::Value> stack = v8::Script::Compile(
			Nan::GetCurrentContext(), code
		).ToLocalChecked()->Run(
			Nan::GetCurrentContext()
		).ToLocalChecked();
		Nan::Utf8String stackStr(stack);
		
		msg += *stackStr;
		
		consoleLog(msg);
		
	}
	
	
	static inline void _addListener(
		const Nan::FunctionCallbackInfo<v8::Value> &info,
		const std::string &name,
		V8_STORE_FUNC *cb,
		bool isFront
	) { THIS_EVENT_EMITTER; EVENT_EMITTER_THIS_CHECK;
		
		V8_VAR_VAL args[] = { info[0], info[1] };
		eventEmitter->emit("newListener", 2, args);
		
		if (isFront) {
			eventEmitter->_listeners[name].push_front(*cb);
			eventEmitter->_raw[name].push_front(*cb);
		} else {
			eventEmitter->_listeners[name].push_back(*cb);
			eventEmitter->_raw[name].push_back(*cb);
		}
		
		int count = eventEmitter->_raw[name].size();
		
		if (eventEmitter->_maxListeners > 0 && count > eventEmitter->_maxListeners) {
			
			_reportListeners(name, count, eventEmitter->_maxListeners);
			
		}
		
	}
	
	
	static inline void _wrapListener(
		const Nan::FunctionCallbackInfo<v8::Value> &info,
		bool isFront = false
	) {
		
		REQ_UTF8_ARG(0, name);
		REQ_FUN_ARG(1, cb);
		
		V8_STORE_FUNC persistentCb;
		persistentCb.Reset(cb);
		
		_addListener(info, *name, &persistentCb, isFront);
		
	}
	
	
	static inline void _addOnceListener(
		const Nan::FunctionCallbackInfo<v8::Value> &info,
		const std::string &name,
		V8_STORE_FUNC *raw,
		V8_STORE_FUNC *cb,
		bool isFront
	) { THIS_EVENT_EMITTER; EVENT_EMITTER_THIS_CHECK;
		
		V8_VAR_VAL args[] = { info[0], info[1] };
		eventEmitter->emit("newListener", 2, args);
		
		if (isFront) {
			eventEmitter->_listeners[name].push_front(*cb);
			eventEmitter->_raw[name].push_front(*raw);
		} else {
			eventEmitter->_listeners[name].push_back(*cb);
			eventEmitter->_raw[name].push_back(*raw);
		}
		
		int nextId = eventEmitter->_freeId++;
		eventEmitter->_wrappedIds[nextId] = *cb;
		eventEmitter->_rawIds[nextId] = *raw;
		
		int count = eventEmitter->_raw[name].size();
		
		if (eventEmitter->_maxListeners > 0 && count > eventEmitter->_maxListeners) {
			
			_reportListeners(name, count, eventEmitter->_maxListeners);
			
		}
		
	}
	
	
	static inline void _wrapOnceListener(
		const Nan::FunctionCallbackInfo<v8::Value> &info,
		bool isFront = false
	) {
		
		REQ_UTF8_ARG(0, name);
		REQ_FUN_ARG(1, raw);
		
		V8_VAR_STR code = JS_STR(R"(
			((emitter, name, cb) => (...args) => {
				cb(...args);
				emitter.removeListener(name, cb);
			})
		)");
		
		v8::Local<v8::Value> decor = v8::Script::Compile(
			Nan::GetCurrentContext(), code
		).ToLocalChecked()->Run(
			Nan::GetCurrentContext()
		).ToLocalChecked();
		Nan::Callback decorCb(Nan::To<v8::Function>(decor).ToLocalChecked());
		
		V8_VAR_VAL argv[] = { info.This(), info[0], raw };
		Nan::AsyncResource async("EventEmitter::js_once()");
		V8_VAR_VAL wrapValue = decorCb.Call(3, argv, &async).ToLocalChecked();
		V8_VAR_FUNC wrap = V8_VAR_FUNC::Cast(wrapValue);
		
		V8_STORE_FUNC persistentWrap;
		persistentWrap.Reset(wrap);
		
		V8_STORE_FUNC persistentRaw;
		persistentRaw.Reset(raw);
		
		_addOnceListener(info, *name, &persistentRaw, &persistentWrap, isFront);
		
	}
	
	
	static NAN_METHOD(jsOnce) {
		
		_wrapOnceListener(info);
		
		RET_VALUE(info.This());
		
	}
	
	static NAN_METHOD(jsPrependListener) {
		
		_wrapListener(info, true);
		
		RET_VALUE(info.This());
		
	}
	
	static NAN_METHOD(jsPrependOnceListener) {
		
		_wrapOnceListener(info, true);
		
		RET_VALUE(info.This());
		
	}
	
	
	static NAN_METHOD(jsRemoveAllListeners) { THIS_EVENT_EMITTER; EVENT_EMITTER_THIS_CHECK;
		
		RET_VALUE(info.This());
		
		if (info.Length() == 0) {
			
			MAP_TYPE tmpMap = eventEmitter->_raw;
			
			eventEmitter->_listeners.clear();
			eventEmitter->_raw.clear();
			eventEmitter->_wrappedIds.clear();
			eventEmitter->_rawIds.clear();
			
			for (MAP_IT_TYPE itMap = tmpMap.begin(); itMap != tmpMap.end(); ++itMap) {
				
				const std::string &current = itMap->first;
				VEC_TYPE &list = itMap->second;
				
				for (IT_TYPE it = list.begin(); it != list.end(); ++it) {
					
					V8_VAR_VAL args[] = { JS_STR(current.c_str()), Nan::New(*it) };
					eventEmitter->emit("removeListener", 2, args);
					
				}
				
			}
			
			return;
			
		}
		
		REQ_UTF8_ARG(0, n);
		
		std::string name = std::string(*n);
		VEC_TYPE &list = eventEmitter->_raw[name];
		
		if (list.empty()) {
			return;
		}
		
		if (eventEmitter->_rawIds.size()) {
			
			std::vector<int> removes;
			
			for (IT_TYPE it = list.begin(); it != list.end(); ++it) {
				
				FN_TYPE fn = *it;
				
				for (FNMAP_IT_TYPE itRaw = eventEmitter->_rawIds.begin(); itRaw != eventEmitter->_rawIds.end(); ++itRaw) {
					if (fn == itRaw->second) {
						removes.push_back(itRaw->first);
					}
				}
				
			}
			
			if (removes.size()) {
				for (std::vector<int>::const_iterator it = removes.begin(); it != removes.end(); ++it) {
					
					eventEmitter->_wrappedIds.erase(*it);
					eventEmitter->_rawIds.erase(*it);
					
				}
			}
			
		}
		
		
		VEC_TYPE tmpVec = eventEmitter->_raw[name];
		
		eventEmitter->_listeners[name].clear();
		eventEmitter->_raw[name].clear();
		
		for (IT_TYPE it = tmpVec.begin(); it != tmpVec.end(); ++it) {
			
			V8_VAR_VAL args[] = { JS_STR(name.c_str()), Nan::New(*it) };
			eventEmitter->emit("removeListener", 2, args);
			
		}
		
	}
	
	
	static NAN_METHOD(jsRemoveListener) { THIS_EVENT_EMITTER; EVENT_EMITTER_THIS_CHECK;
		
		RET_VALUE(info.This());
		
		REQ_UTF8_ARG(0, n);
		REQ_FUN_ARG(1, raw);
		
		V8_STORE_FUNC persistentRaw;
		persistentRaw.Reset(raw);
		
		std::string name = std::string(*n);
		
		VEC_TYPE &rawList = eventEmitter->_raw[name];
		
		if (rawList.empty()) {
			return;
		}
		
		V8_VAR_VAL args[] = { info[0], info[1] };
		
		for (IT_TYPE it = rawList.begin(); it != rawList.end(); ++it) {
			
			if (*it == persistentRaw) {
				rawList.erase(it);
				if (rawList.empty()) {
					eventEmitter->_raw.erase(name);
				}
				break;
			}
			
		}
		
		
		VEC_TYPE &wrapList = eventEmitter->_listeners[name];
		
		if (eventEmitter->_wrappedIds.size() == 0) {
			
			for (IT_TYPE it = wrapList.begin(); it != wrapList.end(); ++it) {
				
				if (*it == persistentRaw) {
					wrapList.erase(it);
					if (wrapList.empty()) {
						eventEmitter->_listeners.erase(name);
					}
					break;
				}
				
			}
			
			eventEmitter->emit("removeListener", 2, args);
			return;
			
		}
		
		
		for (FNMAP_IT_TYPE itRaw = eventEmitter->_rawIds.begin(); itRaw != eventEmitter->_rawIds.end(); ++itRaw) {
			
			if (persistentRaw == itRaw->second) {
				
				FN_TYPE fn = eventEmitter->_wrappedIds[itRaw->first];
				
				for (IT_TYPE it = wrapList.begin(); it != wrapList.end(); ++it) {
					
					if (*it == fn) {
						wrapList.erase(it);
						if (wrapList.empty()) {
							eventEmitter->_listeners.erase(name);
						}
						break;
					}
					
				}
				
				eventEmitter->_wrappedIds.erase(itRaw->first);
				eventEmitter->_rawIds.erase(itRaw->first);
				
				break;
				
			}
			
		}
		
		eventEmitter->emit("removeListener", 2, args);
		
	}
	
	
	static NAN_METHOD(jsSetMaxListeners) { THIS_EVENT_EMITTER; EVENT_EMITTER_THIS_CHECK;
		
		REQ_INT32_ARG(0, value);
		
		eventEmitter->_maxListeners = value;
		
		RET_VALUE(info.This());
		
	}
	
	
	static NAN_METHOD(jsRawListeners) { THIS_EVENT_EMITTER; EVENT_EMITTER_THIS_CHECK;
		
		REQ_UTF8_ARG(0, name);
		
		VEC_TYPE &list = eventEmitter->_raw[*name];
		
		V8_VAR_ARR jsListeners = Nan::New<v8::Array>(list.size());
		
		if (list.empty()) {
			RET_VALUE(jsListeners);
			return;
		}
		
		int i = 0;
		for (IT_TYPE it = list.begin(); it != list.end(); ++it, i++) {
			
			jsListeners->Set(JS_INT(i), Nan::New(*it));
			
		}
		
		RET_VALUE(jsListeners);
		
	}
	
	
	static NAN_METHOD(jsDestroy) { THIS_EVENT_EMITTER; EVENT_EMITTER_THIS_CHECK;
		
		eventEmitter->emit("destroy");
		
		eventEmitter->_destroy();
		
	}
	
	
private:
	
	bool _isDestroyed;
	
	int _maxListeners;
	
	MAP_TYPE _listeners;
	MAP_TYPE _raw;
	
	int _freeId;
	FNMAP_TYPE _wrappedIds;
	FNMAP_TYPE _rawIds;
	
};


#endif // _EVENT_EMITTER_
