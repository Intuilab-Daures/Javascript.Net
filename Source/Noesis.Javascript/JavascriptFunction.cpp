#include "JavascriptFunction.h"
#include "JavascriptInterop.h"
#include "JavascriptContext.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Noesis { namespace Javascript {

////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////

JavascriptFunction::JavascriptFunction( v8::Handle<v8::Object> iFunction, JavascriptContext^ context)
{
	if (!iFunction->IsFunction())
		throw gcnew System::ArgumentException("Trying to use non-function as function");
	
	if(!context)
		throw gcnew System::ArgumentException("Must provide a JavascriptContext");

    v8::Isolate *isolate = JavascriptContext::GetCurrentIsolate();
    mFuncHandle = new Persistent<Function>(isolate, Handle<Function>::Cast(iFunction));
	mContext = context;
}

JavascriptFunction::~JavascriptFunction()
{
    Destroy();
}

JavascriptFunction::!JavascriptFunction()
{
    Destroy();
}

System::Object^ JavascriptFunction::Call(... cli::array<System::Object^>^ args)
{	
	JavascriptScope scope(mContext);
    v8::Isolate *isolate = JavascriptContext::GetCurrentIsolate();
    HandleScope handleScope(isolate);

    Local<Function> pFunction = Local<Function>::New(isolate, *mFuncHandle);
    Handle<v8::Object> global = pFunction->CreationContext()->Global();

	int argc = args->Length;
	Handle<v8::Value> *argv = new Handle<v8::Value>[argc];
	for (int i = 0; i < argc; i++)
	{
		argv[i] = JavascriptInterop::ConvertToV8(args[i]);
	}

    Local<Value> retVal = pFunction->Call(global, argc, argv);

	delete [] argv;
	return JavascriptInterop::ConvertFromV8(retVal);
}

bool JavascriptFunction::operator==( JavascriptFunction^ func1, JavascriptFunction^ func2 )
{
	if(ReferenceEquals(func2, nullptr)) {
		return false;
	}
    v8::Isolate *isolate = JavascriptContext::GetCurrentIsolate();
    Handle<Function> jsFuncPtr1 = Local<Function>::New(isolate, *(func1->mFuncHandle));
    Handle<Function> jsFuncPtr2 = Local<Function>::New(isolate, *(func2->mFuncHandle));

	return jsFuncPtr1->Equals(jsFuncPtr2);
}

bool JavascriptFunction::Equals( JavascriptFunction^ other )
{
	return this == other;
}

bool JavascriptFunction::Equals(Object^ other )
{
	JavascriptFunction^ otherFunc = dynamic_cast<JavascriptFunction^>(other);
	return (otherFunc && this->Equals(otherFunc));
}

void JavascriptFunction::Destroy()
{
    if (mFuncHandle)
    {
        if (mContext->GetCurrent() != nullptr && mContext->GetCurrentIsolate() != nullptr) JavascriptScope scope(mContext);
        delete mFuncHandle;
        mFuncHandle = nullptr;
    }
}


} } // namespace Noesis::Javascript

////////////////////////////////////////////////////////////////////////////////////////////////////