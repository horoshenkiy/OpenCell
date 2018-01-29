#ifndef _PROPERTY_H
#define _PROPERTY_H

/* This simple defines for support properties. It's work only with MSCC, not with GCC or anythere.
* 
* ...
*	private:
*		int _field;
*
*	public:
*
*	PROPERTY(int, field);
*	GET(field) {
*		return _field;
*	}
*	SET(field) {
*		_field = value;
*	}
* ...
*/

#define PROPERTY(t,n)  __declspec( property \
(put = property__set_##n, get = property__get_##n)) t n; \
	typedef t property__tmp_type_##n

#define READONLY_PROPERTY(t,n) __declspec( property (get = property__get_##n) ) t n;\
	typedef t property__tmp_type_##n

#define WRITEONLY_PROPERTY(t,n) __declspec( property (put = property__set_##n) ) t n;\
	typedef t property__tmp_type_##n

#define GET(n) property__tmp_type_##n& property__get_##n()
#define SET(n) void property__set_##n(const property__tmp_type_##n& value)

#endif /* _PROPERTIES_H */ 