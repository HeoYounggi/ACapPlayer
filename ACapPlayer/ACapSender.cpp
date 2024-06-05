#include "pch.h"
//==============================================================================

#include "ACapSender.h"

#include "ACapQueue.h"
#include "ACapManagerReport.h"
//==============================================================================

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif 
//==============================================================================

CACapSender::CACapSender(CACapQueue* pcBuffer, CACapQueue* pcPool, CACapManagerReport* pcManagerReport)
{
	m_pcBuffer = pcBuffer;
	m_pcPool = pcPool;
	m_pcManagerReport = pcManagerReport;
	
	m_bTerminated = false;
	m_hThread = NULL;
	m_pPcapHandle = NULL;

	m_unSpeed = 80;
	m_PlaySpeed = 20;
	m_uiThreadId = 0;

	m_unSendCount = 0;
}
//==============================================================================

CACapSender::~CACapSender()
{
	m_pcBuffer = NULL;
	m_pcPool = NULL;
}
//==============================================================================

bool CACapSender::Start()
{
	m_bTerminated = false;

	m_hThread = (HANDLE)::_beginthreadex(NULL, 0, ThreadProcWrapper, this, 0, &m_uiThreadId);

	return true;
}
//==============================================================================

bool CACapSender::Stop()
{
	m_bTerminated = true;
	Sleep(1);
	//WaitForSingleObject(m_hThread, INFINITE);
	WaitForSingleObject(m_hThread, 2000);

	return true;
}
//==============================================================================

void CACapSender::SetPcapHandel(pcap_t* pPcapHandle)
{
	m_pPcapHandle = pPcapHandle;
}
//==============================================================================

bool CACapSender::SetPlaySpeed(int nPos)
{
	switch (nPos)
	{
	case 0:
		m_PlaySpeed = m_PlaySpeed * 32;
		break;
	case 1:
		m_PlaySpeed = m_PlaySpeed * 16;
		break;
	case 2:
		m_PlaySpeed = m_PlaySpeed * 8;
		break;
	case 3:
		m_PlaySpeed = m_PlaySpeed * 4;
		break;
	case 4:
		m_PlaySpeed = m_PlaySpeed * 2;
		break;
	case 5:
		m_PlaySpeed = m_PlaySpeed * 1;
		break;
	case 6:
		//m_PlaySpeed = 0.000001;
		break;
	}

	return true;
}
//==============================================================================

unsigned int WINAPI CACapSender::ThreadProcWrapper(LPVOID pParameter)
{
	CACapSender* pcSender= reinterpret_cast<CACapSender*>(pParameter);
	return pcSender->ThreadProc();
}
//==============================================================================

unsigned int CACapSender::ThreadProc()
{
	PTACAPQueueItem ptPacketItem;

	bool bResult;

	m_unSendCount = 0;

	unsigned long long unStartPacketTimeUS = 0; // us
	unsigned long long unCurrentPacketTimeUS = 0; // us

	unsigned long long unStartTickUS = GetTickCount64() * 1000; // us
	unsigned long long unCurrentTickUS;

	unsigned long long unDiffPacketTimeUS;
	unsigned long long unDiffTickTimeUS;

	while (m_bTerminated == false)
	{
		if (m_pcBuffer->IsEmpty() == true)
		{
			Sleep(10);
			continue;
		}

		// 1. Get Item from Buffer
		bResult = m_pcBuffer->Pop(&ptPacketItem);
		if (bResult == false)
		{
			Sleep(10);
			continue;
		}

		// 2. Calculate Time
		if (unStartPacketTimeUS == 0)
		{
			unStartPacketTimeUS = ptPacketItem->tPacketHeader.ts.tv_sec * 1000000;
			unStartPacketTimeUS += ptPacketItem->tPacketHeader.ts.tv_usec;
		}

		unCurrentPacketTimeUS = ptPacketItem->tPacketHeader.ts.tv_sec * 1000000;
		unCurrentPacketTimeUS += ptPacketItem->tPacketHeader.ts.tv_usec;

		unCurrentTickUS = GetTickCount64() * 1000;

		unDiffPacketTimeUS = unCurrentPacketTimeUS - unStartPacketTimeUS;
		unDiffTickTimeUS = unCurrentTickUS - unStartTickUS;

		unDiffTickTimeUS = unDiffTickTimeUS * m_unSpeed / m_PlaySpeed ;
		//unDiffTickTimeUS = unDiffTickTimeUS * m_unSpeed / 10;

		while (unDiffPacketTimeUS > unDiffTickTimeUS)
		{
			Sleep(1);
			unCurrentTickUS = GetTickCount64() * 1000;

			unDiffPacketTimeUS = unCurrentPacketTimeUS - unStartPacketTimeUS;
			unDiffTickTimeUS = unCurrentTickUS - unStartTickUS;

			unDiffTickTimeUS = unDiffTickTimeUS * m_unSpeed / m_PlaySpeed;
		}

		// 3. Send Packet
		pcap_sendpacket(m_pPcapHandle, ptPacketItem->zbPacketData, ptPacketItem->tPacketHeader.len);
		m_unSendCount++;
		// 4. Push Item to Pool
		bResult = m_pcPool->Push(ptPacketItem);
		while (bResult == false)
		{
			//Sleep(1);
			SwitchToThread();
			bResult = m_pcPool->Push(ptPacketItem);
		}

	}

	pcap_close(m_pPcapHandle);
	return 0;
}
//==============================================================================

int CACapSender::GetSnedPacketCount()
{
	return m_unSendCount;
}
//==============================================================================