#include <chrono>
#include <thread>
#include <napi.h>

using namespace Napi;

std::thread nativeThread;
ThreadSafeFunction tsfn;

Value Adapter(const CallbackInfo &info) {
	Napi::Env env = info.Env();

	if (info.Length() < 1) {
		throw TypeError::New(env, "Expected one argument");
	} else if (!info[0].IsFunction()) {
		throw TypeError::New(env, "Expected first arg to be function");
	}

	// Create a ThreadSafeFunction
	tsfn = ThreadSafeFunction::New(env, info[0].As<Function>(), // JavaScript function called asynchronously
			"Resource Name",         // Name
			0,                       // Unlimited queue
			1,                       // Only one thread will use this initially
			[](Napi::Env) {        // Finalizer used to clean threads up
				nativeThread.join();
			});

	// Create a native thread
	nativeThread = std::thread([] {
		auto callback = []( Napi::Env env, Function jsCallback, int* value ) {
			// Transform native data into JS data, passing it to the provided
			// `jsCallback` -- the TSFN's JavaScript function.
			jsCallback.Call( {Number::New( env, *value )});

			// We're finished with the data.
			delete value;
		};

		while (true)
		{
			// Create new data
			int* value = new int( clock() );

			// Perform a blocking call
			napi_status status = tsfn.BlockingCall( value, callback );
			if ( status != napi_ok )
			{
				// Handle error
				break;
			}

			std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
		}

		// Release the thread-safe function
		tsfn.Release();
	});

	return Boolean::New(env, true);
}

Napi::Object Init(Napi::Env env, Object exports) {
	exports.Set("adapter", Function::New(env, Adapter));
	return exports;
}

NODE_API_MODULE( adapter, Init )
