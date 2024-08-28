#pragma once
#include "CommonInclude.h"
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

	virtual bool IsInt32() const = 0;
	virtual bool IsUInt32() const = 0;
	virtual bool IsInt64() const = 0;
	virtual bool IsUInt64() const = 0;
	virtual bool IsFloat() const = 0;
	virtual bool IsDouble() const = 0;
	virtual bool IsBoolean() const = 0;
	virtual bool IsVector2() const = 0;
	virtual bool IsVector3() const = 0;
	virtual bool IsVector4() const = 0;

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
		return *this;
	}

	virtual bool IsInt32() const { return eastl::is_same_v<int32_t, Type>; }
	virtual bool IsUInt32() const { return eastl::is_same_v<uint32_t, Type>; }
	virtual bool IsInt64() const { return eastl::is_same_v<int64_t, Type>; }
	virtual bool IsUInt64() const { return eastl::is_same_v<uint64_t, Type>; }
	virtual bool IsFloat() const { return eastl::is_same_v<float, Type>; }
	virtual bool IsDouble() const { return eastl::is_same_v<double, Type>; }
	virtual bool IsBoolean() const { return eastl::is_same_v<bool, Type>; }
	virtual bool IsVector2() const { return eastl::is_same_v<Vector2, Type>; }
	virtual bool IsVector3() const { return eastl::is_same_v<Vector3, Type>; }
	virtual bool IsVector4() const { return eastl::is_same_v<Vector4, Type>; }

private:

	Type Data;
};

EA_RESTORE_VC_WARNING()
