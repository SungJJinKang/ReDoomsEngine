#pragma once

class ID3D12ManagerInterface
{
public:
	virtual ~ID3D12ManagerInterface() = default;
	virtual void OnPreStartFrame() {}
	virtual void OnStartFrame() = 0;
	virtual void OnEndFrame() = 0;
	virtual void OnPostEndFrame() {}
};