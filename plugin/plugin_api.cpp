#include <string>
#include "XPLMPlugin.h"
#include "XPLMDataAccess.h"
#include "XPLMProcessing.h"

#include "logger.hpp"
#include "zbo_cockpit_plugin.hpp"

#if IBM
#include <windows.h>
#endif

#if LIN
//#include <GL/gl.h>
#else
#if __GNUC__
//#include <OpenGL/gl.h>
#else
//#include <GL/gl.h>
#endif
#endif



#ifndef XPLM300
#error This is made to be compiled against the XPLM300 SDK
#endif

logger LOG("ZCockpitPlugin.log");

namespace zcockpit::plugin
{
	//
	//**** CRASH HANDLES **************
	//
	void register_crash_handler();
	void unregister_crash_handler();
	bool is_us_executing();
	void handle_crash(void *context);	// Function called when we detect a crash that was caused by us

	static XPLMPluginID s_my_plugin_id;
#if IBM
		static LPTOP_LEVEL_EXCEPTION_FILTER s_previous_windows_exception_handler;
		LONG WINAPI handle_windows_exception(EXCEPTION_POINTERS *ei);
		void write_mini_dump(PEXCEPTION_POINTERS exception_pointers);
#endif




	//
	//**** CRASH HANDLES **************
	//

	std::unique_ptr<ZboCockpitPlugin> zbo_cockpit_plugin{ nullptr };


	PLUGIN_API float FlightLoopCallback(
		float inElapsedSinceLastCall,
		float inElapsedTimeSinceLastFlightLoop,
		int inCounter,
		void* inRefcon) {
		try {
			zbo_cockpit_plugin->flightLoop(inElapsedSinceLastCall, inElapsedTimeSinceLastFlightLoop, inCounter, inRefcon);
		}
		catch(std::runtime_error& e) {
			LOG() << "Received Exception " << e.what();
			LOG() << "Tell Xplane to inactivate the callback";
			// Tell Xplane stop calling flightLoop
			return 0.0;
		}
		catch(...) {
			LOG() << "Unknown Exception";
			// Tell Xplane stop calling flightLoop
			return 0.0;
		}
		return 0.01f;
	}

	PLUGIN_API int XPluginStart(char* outName, char* outSig, char* outDesc) {
		// When this capability is set, the entire SDK runs with Unix paths and dir separators.
		XPLMEnableFeature("XPLM_USE_NATIVE_PATHS", 1);

		try {
			register_crash_handler();

			zbo_cockpit_plugin = std::make_unique<ZboCockpitPlugin>(std::make_unique<Server>(std::make_unique<Udp>()));
			XPLMRegisterFlightLoopCallback(FlightLoopCallback, 0.01f, nullptr);
			zbo_cockpit_plugin->pluginStart(outName, outSig, outDesc);
		}
		catch (std::runtime_error& e)
		{
			LOG(Severe) << "Failure to construct ZBO Cockpit Plugin: " << e.what();
		}
		return 1;
	}

	PLUGIN_API void	XPluginStop(void) {
		if (nullptr != zbo_cockpit_plugin) {
			LOG() << "ZBO Cockpit: Stopping plugin.";
			XPLMUnregisterFlightLoopCallback(FlightLoopCallback, nullptr);
			LOG() << "ZBO Cockpit: FlightLoop Callback Unregistered";
			zbo_cockpit_plugin->pluginStop();
		}
		LOG() << "ZBO Cockpit: Plugin stopped.";
		unregister_crash_handler();
	}

	PLUGIN_API void XPluginDisable(void) {
		LOG() << "zCockpit: Plugin disabled.";
	}

	PLUGIN_API int XPluginEnable(void) {
		LOG() << "zCockpit: Plugin enabled.";
		return 1;
	}

	PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFromWho, intptr_t inMessage, void* inParam) {
		if (nullptr != zbo_cockpit_plugin) {
			zbo_cockpit_plugin->receiveMessage(inFromWho, inMessage, inParam);
		}
	}


	//
	//**** CRASH HANDLES **************
	//

	// Registers the global crash handler. Should be called from XPluginStart
	void register_crash_handler()
	{
	#if SUPPORT_BACKGROUND_THREADS
		s_main_thread = std::this_thread::get_id();
	#endif
		s_my_plugin_id = XPLMGetMyID();

	#if APL || LIN
		struct sigaction sig_action = { .sa_sigaction = handle_posix_sig };

		sigemptyset(&sig_action.sa_mask);

	#if	LIN
		static uint8_t alternate_stack[SIGSTKSZ];
		stack_t ss = {
			.ss_sp = (void*)alternate_stack,
			.ss_size = SIGSTKSZ,
			.ss_flags = 0
		};

		sigaltstack(&ss, NULL);
		sig_action.sa_flags = SA_SIGINFO | SA_ONSTACK;
	#else
		sig_action.sa_flags = SA_SIGINFO;
	#endif

		sigaction(SIGSEGV, &sig_action, &s_prev_sigsegv);
		sigaction(SIGABRT, &sig_action, &s_prev_sigabrt);
		sigaction(SIGFPE, &sig_action, &s_prev_sigfpe);
		sigaction(SIGINT, &sig_action, &s_prev_sigint);
		sigaction(SIGILL, &sig_action, &s_prev_sigill);
		sigaction(SIGTERM, &sig_action, &s_prev_sigterm);
	#endif
		
	#if IBM
		// Load the debug helper library into the process already, this way we don't have to hit the dynamic loader
		// in an exception context where it's potentially unsafe to do so.
		HMODULE module = ::GetModuleHandleA("dbghelp.dll");
		if(!module)
			module = ::LoadLibraryA("dbghelp.dll");

		(void)module;
		s_previous_windows_exception_handler = SetUnhandledExceptionFilter(handle_windows_exception);
	#endif
	}

	// Unregisters the global crash handler. You need to call this in XPluginStop so we can clean up after ourselves
	void unregister_crash_handler()
	{
	#if APL || LIN
		sigaction(SIGSEGV, &s_prev_sigsegv, NULL);
		sigaction(SIGABRT, &s_prev_sigabrt, NULL);
		sigaction(SIGFPE, &s_prev_sigfpe, NULL);
		sigaction(SIGINT, &s_prev_sigint, NULL);
		sigaction(SIGILL, &s_prev_sigill, NULL);
		sigaction(SIGTERM, &s_prev_sigterm, NULL);
	#endif
		
	#if IBM
		SetUnhandledExceptionFilter(s_previous_windows_exception_handler);
	#endif
	}
	#if IBM
	LONG WINAPI handle_windows_exception(EXCEPTION_POINTERS *ei)
	{
		if(is_us_executing())
		{
			handle_crash(ei);
			return EXCEPTION_CONTINUE_SEARCH;
		}

		if(s_previous_windows_exception_handler)
			return s_previous_windows_exception_handler(ei);

		return EXCEPTION_CONTINUE_SEARCH;
	}
	#endif

	
	// Predicates that returns true if a thread is caused by us
	// The main idea is to check the plugin ID if we are on the main thread,
	// if not, we check if the current thread is known to be from us.
	// Returns false if the crash was caused by code that didn't come from our plugin
	bool is_us_executing()
	{
	#if SUPPORT_BACKGROUND_THREADS
		const std::thread::id thread_id = std::this_thread::get_id();

		if(thread_id == s_main_thread)
		{
			// Check if the plugin executing is our plugin.
			// XPLMGetMyID() will return the ID of the currently executing plugin. If this is us, then it will return the plugin ID that we have previously stashed away
			return (s_my_plugin_id == XPLMGetMyID());
		}

		if(s_thread_lock.test_and_set(std::memory_order_acquire))
		{
			// We couldn't acquire our lock. In this case it's better if we just say it's not us so we don't eat the exception
			return false;
		}

		const bool is_our_thread = (s_known_threads.find(thread_id) != s_known_threads.end());
		s_thread_lock.clear(std::memory_order_release);

		return is_our_thread;
	#else
		return (s_my_plugin_id == XPLMGetMyID());
	#endif
	}

	void handle_crash(void *context)
	{
	#if APL || LIN
		// NOTE: This is definitely NOT production code
		// backtrace and backtrace_symbols are NOT signal handler safe and are just put in here for demonstration purposes
		// A better alternative would be to use something like libunwind here
		
		void *frames[64];
		int frame_count = backtrace(frames, 64);
		char **names = backtrace_symbols(frames, frame_count);
		LOG() << "CRASH !!!!";
		const int fd = open("backtrace.txt", O_CREAT | O_RDWR | O_TRUNC | O_SYNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		if(fd >= 0)
		{
			for(int i = 0; i < frame_count; ++ i)
			{
				write(fd, names[i], strlen(names[i]));
				write(fd, "\n", 1);
				LOG() << names[i];
			}
			
			close(fd);
		}
		else {
			LOG() << "Can not open backtrace.txt";
			for(int i = 0; i < frame_count; ++ i)
			{
				LOG() << names[i];
			}
		}
		
	#endif
	#if IBM
		// Create a mini-dump file that can be later opened up in Visual Studio or WinDbg to do post mortem debugging
		write_mini_dump((PEXCEPTION_POINTERS)context);
	#endif
	}

	
#if IBM
	#include <DbgHelp.h>

	typedef BOOL(WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile,
		MINIDUMP_TYPE DumpType,
		CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
		CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
		CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam);

	void write_mini_dump(PEXCEPTION_POINTERS exception_pointers)
	{
		LOG() << "Mini Dump";

		HMODULE module = ::GetModuleHandleA("dbghelp.dll");
		if(!module)
			module = ::LoadLibraryA("dbghelp.dll");

		if(module)
		{
			const MINIDUMPWRITEDUMP pDump = MINIDUMPWRITEDUMP(::GetProcAddress(module, "MiniDumpWriteDump"));

			if(pDump)
			{
				// Create dump file
				const HANDLE handle = ::CreateFileA("crash_dump.dmp", GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

				if(handle != INVALID_HANDLE_VALUE)
				{
					MINIDUMP_EXCEPTION_INFORMATION exception_information = {};
					exception_information.ThreadId = ::GetCurrentThreadId();
					exception_information.ExceptionPointers = exception_pointers;
					exception_information.ClientPointers = false;

					pDump(GetCurrentProcess(), GetCurrentProcessId(), handle, MiniDumpNormal, &exception_information, nullptr, nullptr);
					::CloseHandle(handle);

					LOG() << "Mini Dump Done";
				}
			}
		}
	}
#endif
}