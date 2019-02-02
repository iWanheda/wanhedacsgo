#pragma once
#include <Windows.h>
#include <unordered_map>
#define k_page_writeable (PAGE_READWRITE | PAGE_EXECUTE_READWRITE)
#define k_page_readable (k_page_writeable|PAGE_READONLY|PAGE_WRITECOPY|PAGE_EXECUTE_READ|PAGE_EXECUTE_WRITECOPY)
#define k_page_offlimits (PAGE_GUARD|PAGE_NOACCESS)


namespace VMT
{
	class VMTHookManager
	{
	public:
		VMTHookManager() {}

		VMTHookManager(void* instance);
		~VMTHookManager();
		void Init(void* instance);
		void Restore();

		template <class T>
		T HookFunction(int index, void* func)
		{
			/// index is out of range or null function pointer
			if (index < 0 || index >= m_size || !func)
				return reinterpret_cast<T>(nullptr);

			/// get the original (this assumes that you dont try to hook the function twice)
			auto original = reinterpret_cast<T>(m_vtable[index]);
			m_original_funcs[index] = original;

			unsigned long old_protection, tmp;
			VProtect(&m_vtable[index], sizeof(void*), 0x40, &old_protection); /// 0x40 = PAGE_EXECUTE_READWRITE
			m_vtable[index] = func;
			VProtect(&m_vtable[index], sizeof(void*), old_protection, &tmp);

			return original;
		}
		void UnHookFunction(int index);

		template <class T>
		T GetOriginalFunction(int index)
		{
			/// index is out of range
			if (index < 0 || index >= m_size)
				return reinterpret_cast<T>(nullptr);

			/// function isn't even hooked - just return the one in the vtable
			if (m_original_funcs.find(index) == m_original_funcs.end())
				return reinterpret_cast<T>(m_vtable[index]);

			return reinterpret_cast<T>(m_original_funcs[index]);
		}

		template <class T>
		static T GetFunction(void* instance, int index)
		{
			const auto vtable = *static_cast<void***>(instance);
			return reinterpret_cast<T>(vtable[index]);
		}

	private:
		std::unordered_map<int, void*> m_original_funcs;
		void* m_instance = nullptr;
		void** m_vtable = nullptr;
		int m_size = 0;

	private:
		void VProtect(void* address, unsigned int size, unsigned long new_protect, unsigned long* old_protect);
	};
	/*template<class Function> Function getvfunc(PVOID Base, DWORD Index)
	{

		PDWORD* VTablePointer = (PDWORD*)Base;
		PDWORD VTableFunctionBase = *VTablePointer;
		DWORD dwAddress = VTableFunctionBase[Index];
		return (Function)(dwAddress);
	}
	inline void* GetFunction(void* Instance, int Index)
	{
		DWORD VirtualFunction = (*(DWORD*)Instance) + sizeof(DWORD) * Index;
		return (void*)*((DWORD*)VirtualFunction);
	}

	class CVMTHookManager
	{
	public:
		CVMTHookManager();
		~CVMTHookManager();
		//CVMTHookManager(PDWORD* Interface);
		CVMTHookManager(void*);

		//bool Init(PDWORD * Interface);

	    bool Init(void* Interface);
		/*Hook/Unhook*/
		/*void HookTable(bool hooked);
		/*Hooks function*/
		/*void* HookFunction(int Index, void* hkFunction);

	private:
		int count;
		void* pCopyTable;
		DWORD* pObject;
		void* pOrgTable;

		void*	m_ClassBase;
		PDWORD	m_NewVT, m_OldVT;
		DWORD	m_VTSize;

		/*Returns if you can read the pointer*/
		/*bool CanReadPointer(void* table);
		/*Gets VMT count*/
		/*int GetCount();
		DWORD GetVTCount(PDWORD pdwVMT);
	};*/
}



namespace detail
{
	class protect_guard
	{
	public:
		protect_guard(void* base, size_t len, std::uint32_t flags)
		{
			_base = base;
			_length = len;
			if (!VirtualProtect(base, len, flags, (PDWORD)&_old))
				throw std::runtime_error("Failed to protect region.");
		}
		~protect_guard()
		{
			VirtualProtect(_base, _length, _old, (PDWORD)&_old);
		}

	private:
		void*         _base;
		size_t        _length;
		std::uint32_t _old;
	};
}

class vfunc_hook
{
public:
	uintptr_t * search_free_data_page(const char * module_name, const std::size_t vmt_size);
	vfunc_hook();
	vfunc_hook(void* base);
	~vfunc_hook();

	bool setup(void * base, const char * moduleName = nullptr);
	template<typename T>
	void hook_index(int index, T fun)
	{
		assert(index >= 0 && index <= (int)vftbl_len);
		new_vftb1[index + 1] = reinterpret_cast<std::uintptr_t>(fun);
	}
	void unhook_index(int index)
	{
		new_vftb1[index] = old_vftbl[index];
	}
	void unhook_all()
	{
		try {
			if (old_vftbl != nullptr) {
				auto guard = detail::protect_guard{ class_base, sizeof(std::uintptr_t), PAGE_READWRITE };
				*(std::uintptr_t**)class_base = old_vftbl;
				old_vftbl = nullptr;
			}
		}
		catch (...) {
		}
	}

	template<typename T>
	T get_original(int index)
	{
		return (T)old_vftbl[index];
	}

private:

	static inline std::size_t estimate_vftbl_length(std::uintptr_t* vftbl_start);

	void*           class_base;
	std::size_t     vftbl_len;
	std::uintptr_t* new_vftb1;
	std::uintptr_t* old_vftbl;
	LPCVOID         search_base = nullptr;
	bool wasAllocated = false;
};



