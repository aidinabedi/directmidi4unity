
class MyCriticalSection
{
private:

	CRITICAL_SECTION CS;

public:

	MyCriticalSection() { ::InitializeCriticalSection(&CS); }
	~MyCriticalSection() { ::DeleteCriticalSection(&CS); }

	void enter() { ::EnterCriticalSection(&CS); }
	void leave() { ::LeaveCriticalSection(&CS); }
};

