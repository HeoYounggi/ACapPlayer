#ifndef _ACAPSENDER_H_
#define _ACAPSENDER_H_
//==============================================================================

class CACapQueue;
class CACapManagerReport;
//==============================================================================

class CACapSender
{
public:
	CACapSender(CACapQueue* pcBuffer, CACapQueue* pcPool, CACapManagerReport* pcManagerReport);
	virtual ~CACapSender();

	bool Start();
	bool Stop();

	void SetPcapHandel(pcap_t* pPcapHandle);

	bool SetPlaySpeed(int nPos);

	bool SetLoopDelayCount(int nLoops, int nDelay);

	static unsigned int WINAPI ThreadProcWrapper(LPVOID pParameter);
	unsigned int ThreadProc();
	
	int GetSnedPacketCount();
protected:
	HANDLE m_hThread;
	unsigned m_uiThreadId;

	bool m_bTerminated;

	CACapQueue* m_pcBuffer;
	CACapQueue*	m_pcPool;

	CACapManagerReport* m_pcManagerReport;

	unsigned int m_unSpeed;

	pcap_t* m_pPcapHandle;

	int m_PlaySpeed;

	int m_nLoopCount;
	int m_nDelayTime;

	unsigned int m_unSendCount;
};
//==============================================================================

#endif // _ACAPSENDER_H_
