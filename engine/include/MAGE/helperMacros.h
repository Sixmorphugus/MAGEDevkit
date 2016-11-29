#pragma once
#define CAT_I(a,b) a##b
#define CAT(a,b) CAT_I(a, b)

#define STRING(s) #s

#define ImposeShadowScope do {
#define CloseShadowScope } while (0)

// This is some crazy magic that helps produce __BASE__247
// Vanilla interpolation of __BASE__##__LINE__ would produce __BASE____LINE__
// I still can't figure out why it works, but it has to do with macro resolution ordering
// Obviously it's from stack overflow.
#define PP_CAT(a, b) PP_CAT_I(a, b)
#define PP_CAT_I(a, b) PP_CAT_II(~, a ## b)
#define PP_CAT_II(p, res) res

#define UNIQUE_NAME(base) PP_CAT(base, __COUNTER__)