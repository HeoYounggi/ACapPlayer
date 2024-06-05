#ifndef _ACAPREADER_H_
#define _ACAPREADER_H_
//==============================================================================

class CACapQueue;
class CACapManagerReport;
//==============================================================================

class CACapReader
{
public:
	CACapReader(CACapQueue* pcBuffer, CACapQueue* pcPool, CACapManagerReport* pcReport);//HWND hOwner
	virtual ~CACapReader();

	bool SetFilePath(TCHAR* pszFilePath);
	bool SetLoopDelayCount(int nLoops, int nDelay);

	bool Start();
	bool Stop();

	static unsigned int WINAPI ThreadProcWrapper(LPVOID pParameter);
	unsigned int ThreadProc();

protected:
	HANDLE m_hThread;
	unsigned m_uiThreadId;

	bool m_bTerminated;
	
	CACapQueue* m_pcBuffer;
	CACapQueue*	m_pcPool;

	CACapManagerReport* m_pcReport;

	//HWND m_hOwner;

	TCHAR m_szFilePath[MAX_PATH];
	TCHAR* m_pszFilePath;

	int m_nLoopCount;
	int m_nDelayTime;

	unsigned int m_unTotalPacket;

	pcap_t* m_pPcapHandle;
};
//==============================================================================

#endif // _ACAPREADER_H_
