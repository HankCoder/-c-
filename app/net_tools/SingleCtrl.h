#ifndef __SINGLECTRL_INCLUDE_H__
#define __SINGLECTRL_INCLUDE_H__

class  CSingleCtrl
{
public:
	CSingleCtrl(const char *pExeName);
	~CSingleCtrl() {};

	BOOL Check();
	void Register();
	void Remove();
protected:
	CSingleCtrl() {};
private:
	CString m_sExeName;
};

#endif