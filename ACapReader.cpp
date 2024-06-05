#include "pch.h"
//==============================================================================

#include "ACapReader.h"

#include "ACapQueue.h"
#include "ACapManagerReport.h"
//==============================================================================

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//==============================================================================

CACapReader::CACapReader(CACapQueue* pcBuffer, CACapQueue* pcPool, CACapManagerReport* pcReport)
{
	m_pcBuffer = pcBuffer;
	m_pcPool = pcPool;
	m_pcReport = pcReport;

	m_bTerminated = false;
	m_hThread = NULL;
	m_pszFilePath = NULL;

	m_uiThreadId = 0;
	m_unTotalPacket = 0;
	m_nLoopCount = 0;
	m_nDelayTime = 0;

	m_pPcapHandle = NULL;

	memset(m_szFilePath, 0, sizeof(m_szFilePath));
}
//==============================================================================

CACapReader::~CACapReader()
{
	m_pcBuffer = NULL;
	m_pcPool = NULL;
}
//==============================================================================

bool CACapReader::SetFilePath(TCHAR* pszFilePath)
{
	char m_errbuf[PCAP_ERRBUF_SIZE];

	_tcscpy_s(m_szFilePath, MAX_PATH, pszFilePath);

	m_pPcapHandle = pcap_open_offline((CW2A)pszFilePath, m_errbuf);

	return true;
}
//==============================================================================

bool CACapReader::SetLoopDelayCount(int nLoops, int nDelay)
{
	m_nLoopCount = nLoops;
	m_nDelayTime = nDelay;
	return true;
}
//==============================================================================

bool CACapReader::Start()
{
	m_bTerminated = false;

	m_hThread = (HANDLE)::_beginthreadex(NULL, 0, ThreadProcWrapper, this, 0, &m_uiThreadId);

	return true;
}
//==============================================================================

bool CACapReader::Stop()
{
	m_bTerminated = true;
	Sleep(1);
	//WaitForSingleObject(m_hThread, INFINITE);
	WaitForSingleObject(m_hThread, 2000);

	return true;
}
//==============================================================================

unsigned int WINAPI CACapReader::ThreadProcWrapper(LPVOID pParameter)
{
	CACapReader* pcReader = reinterpret_cast<CACapReader*>(pParameter);
	return pcReader->ThreadProc();
}
//==============================================================================

unsigned int CACapReader::ThreadProc()
{
	// File Open
	FILE* pFile = NULL;
	errno_t eError;

	pcap_file_header tGlobalHeader;
	PTACAPQueueItem ptPacketItem;

	size_t unReadSize;
	bool bResult;
	m_unTotalPacket = 0;

	eError = _tfopen_s(&pFile, m_szFilePath, TEXT("rb"));
	if (eError != 0 || pFile == NULL)
	{
		if (pFile)
		{
			fclose(pFile);
		}
		return -1;
	}

	// Read Global Header
	unReadSize = fread(&tGlobalHeader, 1, sizeof(pcap_file_header), pFile);  // 24 Bytes
	if (unReadSize != sizeof(pcap_file_header))
	{
		fclose(pFile);
		pFile = NULL;

		return -1;
	}
	if (m_pPcapHandle == NULL)
	{
		fclose(pFile);
		pFile = NULL;

		return -1;
	}

	pcap_dispatch(m_pPcapHandle, 0, [](u_char* user, const struct pcap_pkthdr* pkthdr, const u_char* packet) {
		int* count = (int*)user;
		(*count)++;
		}, (u_char*)&m_unTotalPacket);
	m_pcReport->OnUpdateReportTotalPacketCount(m_unTotalPacket);

	m_pszFilePath = m_szFilePath;
	m_pcReport->OnUpdateReportReader(m_pszFilePath);

	for (int i = 0; i < m_nLoopCount; i++)
	{
		//파일 포인터를 글로벌 헤더 다음으로 이동
		fseek(pFile, sizeof(pcap_file_header), SEEK_SET);
		
		m_unTotalPacket=0;
		while (m_bTerminated == false)
		{
			m_unTotalPacket++;
			// 1. Get Item from Pool
			bResult = m_pcPool->Pop(&ptPacketItem);
			if (bResult == false)
			{
				Sleep(10);
				continue;
			}

			// 2. Read One Packet
			// 2.1 Packet Header
			unReadSize = fread(&ptPacketItem->tPacketHeader, 1, sizeof(pcap_pkthdr), pFile); // 16 Bytes
			if (unReadSize != sizeof(pcap_pkthdr))
			{
				break;
			}

			// 2.2 Packet Data
			unReadSize = fread(ptPacketItem->zbPacketData, 1, ptPacketItem->tPacketHeader.caplen, pFile); // 16 Bytes
			if (unReadSize != ptPacketItem->tPacketHeader.caplen)
			{
				break;
			}

			// 3. Push Item to Buffer
			bResult = m_pcBuffer->Push(ptPacketItem);
			while (bResult == false)
			{
				Sleep(1);
				bResult = m_pcBuffer->Push(ptPacketItem);
			}
		}
		Sleep(m_nDelayTime);
	}
	fclose(pFile);
	pFile = NULL;

	return 0;
}
//==============================================================================
