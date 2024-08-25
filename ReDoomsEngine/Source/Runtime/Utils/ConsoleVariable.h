#pragma once
#include "EASTL/hash_map.h"
#include "EAStdC/EASingleton.h"

#include <EASTL/type_traits.h>

class FConsoleVariable;
class FConsoleVariableManager
{
public:

	void AddConsoleVariable(FConsoleVariable* const InConsoleVariable);
	FConsoleVariable* GetConsoleVariable(const char* const InName);
	const FConsoleVariable* GetConsoleVariable(const char* const InName) const;
	const eastl::hash_map<const char*, FConsoleVariable*>& GetConsoleVariables() const;

private:

	eastl::hash_map<const char*, FConsoleVariable*> ConsoleVariables;

};
class FConsoleVariableManagerSingleton : public EA::StdC::SingletonAdapter<FConsoleVariableManager, true> { };

enum EConsoleVariableFlag
{
	DefaultConsoleVariableFlag = 0,
	HideInEditor = 1 << 0
};

class FConsoleVariable
{
public:

	FConsoleVariable(const char* const InName, const EConsoleVariableFlag InConsoleVariableFlag = EConsoleVariableFlag::DefaultConsoleVariableFlag);

	inline const char* GetName() const
	{
		return Name;
	}

	virtual int32_t GetInt() const = 0;
	virtual uint32_t GetUInt() const = 0;
	virtual int64_t GetInt64() const = 0;
	virtual uint64_t GetUInt64() const = 0;
	virtual float GetFloat() const = 0;
	virtual double GetDouble() const = 0;
	virtual bool GetBool() const = 0;
	virtual void SetInt(const int32_t InData) = 0;
	virtual void SetUInt(const uint32_t InData) = 0;
	virtual void SetInt64(const int64_t InData) = 0;
	virtual void SetUInt64(const uint64_t InData) = 0;
	virtual void SetFloat(const float InData) = 0;
	virtual void SetDouble(const double InData) = 0;
	virtual void SetBool(const bool InData) = 0;

	virtual bool IsInt32() const = 0;
	virtual bool IsUInt32() const = 0;
	virtual bool IsInt64() const = 0;
	virtual bool IsUInt64() const = 0;
	virtual bool IsFloat() const = 0;
	virtual bool IsDouble() const = 0;
	virtual bool IsBoolean() const = 0;

	inline EConsoleVariableFlag GetConsoleVariableFlag()
	{
		return ConsoleVariableFlag;
	}

private:

	const char* Name;
	EConsoleVariableFlag ConsoleVariableFlag;
};

EA_DISABLE_VC_WARNING(4244)

template <typename Type>
class TConsoleVariable : public FConsoleVariable
{
public:

	template<typename InitialDataType>
	TConsoleVariable(const char* const InName, const InitialDataType InInitialData, const EConsoleVariableFlag InConsoleVariableFlag = EConsoleVariableFlag::DefaultConsoleVariableFlag)
		: FConsoleVariable(InName, InConsoleVariableFlag), Data(static_cast<Type>(InInitialData))
	{
		// @todo : read from *.ini

	}

	inline operator Type() const
	{
		return Data;
	}
	TConsoleVariable<Type>& operator=(const Type InData)
	{
		Data = InData;
	}

	virtual int32_t GetInt() const 
	{
		EA_ASSERT(IsInt32() || IsInt64());
		return static_cast<int32_t>(Data);
	}
	virtual uint32_t GetUInt() const
	{
		EA_ASSERT(IsUInt32() || GetUInt64());
		return static_cast<uint32_t>(Data);
	}
	virtual int64_t GetInt64() const
	{
		EA_ASSERT(IsInt64());
		return Data;
	}
	virtual uint64_t GetUInt64() const
	{
		EA_ASSERT(GetUInt64());
		return Data;
	}
	virtual float GetFloat() const
	{
		EA_ASSERT(IsFloat());
		return Data;
	}
	virtual double GetDouble() const
	{
		EA_ASSERT(IsDouble());
		return Data;
	}
	virtual bool GetBool() const
	{
		EA_ASSERT(IsBoolean());
		return Data;
	}

	virtual void SetInt(const int32_t InData)
	{
		EA_ASSERT(IsInt32());
		Data = InData;
	}
	virtual void SetUInt(const uint32_t InData)
	{
		EA_ASSERT(IsUInt32());
		Data = InData;
	}
	virtual void SetInt64(const int64_t InData)
	{
		EA_ASSERT(IsInt64());
		Data = InData;
	}
	virtual void SetUInt64(const uint64_t InData)
	{
		EA_ASSERT(IsUInt64());
		Data = InData;
	}
	virtual void SetFloat(const float InData)
	{
		EA_ASSERT(IsFloat());
		Data = InData;
	}
	virtual void SetDouble(const double InData)
	{
		EA_ASSERT(IsDouble());
		Data = InData;
	}
	virtual void SetBool(const bool InData)
	{
		EA_ASSERT(IsBoolean());
		Data = InData;
	}

	virtual bool IsInt32() const { return eastl::is_same_v<int32_t, Type>; }
	virtual bool IsUInt32() const { return eastl::is_same_v<uint32_t, Type>; }
	virtual bool IsInt64() const { return eastl::is_same_v<int64_t, Type>; }
	virtual bool IsUInt64() const { return eastl::is_same_v<uint64_t, Type>; }
	virtual bool IsFloat() const { return eastl::is_same_v<float, Type>; }
	virtual bool IsDouble() const { return eastl::is_same_v<double, Type>; }
	virtual bool IsBoolean() const { return eastl::is_same_v<bool, Type>; }

private:

	Type Data;
};

EA_RESTORE_VC_WARNING()
