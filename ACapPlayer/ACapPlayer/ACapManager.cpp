#include "pch.h"
//==============================================================================

#include "ACapManager.h"

#include "ACapQueue.h"
#include "ACapReader.h"
#include "ACapSender.h"
//==============================================================================

#define ACAP_MAX_SIZE	(1000)
//==============================================================================

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//==============================================================================

CACapManager::CACapManager(HWND hOwner)
{
	m_hOwner = hOwner;
	m_ptQueue = new TACAPQueueItem[ACAP_MAX_SIZE];
	m_pcBuffer = new CACapQueue(ACAP_MAX_SIZE);
	m_pcPool = new CACapQueue(ACAP_MAX_SIZE);

	pDeviceList = NULL;
	
	memset(m_szDeviceList, 0, sizeof(m_szDeviceList));
	pDevice = NULL;
	m_unDeviceCout = 0;

	for (unsigned int i = 0; i < ACAP_MAX_SIZE; i++)
	{
		m_pcPool->Push(m_ptQueue + i);

	}

	m_pcReader = new CACapReader(m_pcBuffer, m_pcPool, this);
	m_pcSender = new CACapSender(m_pcBuffer, m_pcPool, this);

	if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &pDeviceList, cErrBuf) == -1)
	{
		return;
	}
	pDevice = pDeviceList;

	while (pDevice != NULL)
	{
		strcpy_s(m_szDeviceList[m_unDeviceCout], MAX_PATH, pDevice->name);
		m_unDeviceCout++;

		pDevice = pDevice->next;
	}

	pcap_freealldevs(pDeviceList);
}
//==============================================================================

CACapManager::~CACapManager()
{
	delete m_pcReader;
	m_pcReader = NULL;
	delete m_pcSender;
	m_pcSender = NULL;

	delete m_pcPool;
	m_pcPool = NULL;
	delete m_pcBuffer;
	m_pcBuffer = NULL;
	delete m_ptQueue;
	m_ptQueue = NULL;
}
//==============================================================================

bool CACapManager::SetFilePath(TCHAR* pszFilePath)
{
	m_pcReader->SetFilePath(pszFilePath);

	return true;
}
//==============================================================================

bool CACapManager::GetPlaySpeed(int nPos)
{
	m_pcSender->SetPlaySpeed(nPos);

	return true;
}
//==============================================================================

bool CACapManager::GetLoopDelayCount(int nLoops, int nDelay)
{
	m_pcReader->SetLoopDelayCount(nLoops, nDelay);
	m_pcSender->SetLoopDelayCount(nLoops,nDelay);

	return true;
}
//==============================================================================

bool CACapManager::GetNICList(std::vector<std::string>& nicList)
{
	if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &pDeviceList, cErrBuf) == -1)
	{
		return false;
	}

	pDevice = pDeviceList;

	while (pDevice != NULL)
	{
		nicList.push_back(pDevice->description);
		pDevice = pDevice->next;
	}

	pcap_freealldevs(pDeviceList);
	return true;
}
//==============================================================================

bool CACapManager::SetNIC(int SelectedIndex)
{

	pcap_t* pNetDevice = pcap_open_live(m_szDeviceList[SelectedIndex], 65536, 1, 1000, cErrBuf);
	if (pNetDevice == nullptr)
	{
		AfxMessageBox(TEXT("Error opening network device."));
		return 0;
	}
	m_pcSender->SetPcapHandel(pNetDevice);

#if 0
	TRACE("%d\n", SelectedIndex);
	if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &pDeviceList, cErrBuf) == -1)
	{
		return false;
	}

	pcap_if_t* pDevice = pDeviceList;
	for (int i = 0; i < SelectedIndex; i++)
	{
		pDevice = pDevice->next;
	}

	pcap_t* pNetDivice = pcap_open_live(pDevice->name, 65536, 1, 1000, cErrBuf);



	if (pNetDivice == nullptr)
	{
		AfxMessageBox(TEXT("Error opening network device."));
		return 0;
	}
	m_pcSender->SetPcapHandel(pNetDivice);
	pcap_freealldevs(pDeviceList);

#endif
	return true;
}
//==============================================================================

bool CACapManager::Start()
{
	PTACAPQueueItem ptItem;

	while (m_pcBuffer->GetSize() > 0)
	{
		m_pcBuffer->Pop(&ptItem);
		m_pcPool->Push(ptItem);
	}
	m_pcBuffer->Clear();

	m_pcSender->Start();
	m_pcReader->Start();

	return true;
}
//==============================================================================

bool CACapManager::Stop()
{
	PTACAPQueueItem ptItem;

	m_pcReader->Stop();
	m_pcSender->Stop();

	while (m_pcBuffer->GetSize() > 0)
	{
		m_pcBuffer->Pop(&ptItem);
		m_pcPool->Push(ptItem);
	}
	m_pcBuffer->Clear();

	return true;
}
//==============================================================================

void CACapManager::OnUpdateReportReader(TCHAR* m_szFilePath)
{
	WPARAM wParam = NULL;
	if (m_szFilePath != NULL)
	{
		LPARAM lParam = (LPARAM)m_szFilePath;

		::PostMessage(m_hOwner, WM_ACAP_SEND_FILE_NAME, wParam, lParam);
	}

	return;
}
//==============================================================================

void CACapManager::OnUpdateReportTotalPacketCount(unsigned int m_unTotalPacket)
{
	WPARAM wParam = NULL;
	if (m_unTotalPacket != 0)
	{
		LPARAM lParam = (LPARAM)m_unTotalPacket;

		::PostMessage(m_hOwner, WN_ACAP_TOTAL_PACKET_COUNT, wParam, lParam);
	}

	return;
}
//==============================================================================

unsigned int CACapManager::GetTotalPacket()
{
	WPARAM wParam = NULL;
	LPARAM lParam = NULL;
	//unsigned int m_unTotalCount = m_pcReader->TotalPacketCount();
	//::PostMessage(m_hOwner, WN_TOTAL_PACKET_COUNT, wParam, lParam);
	return 0;
}
//==============================================================================

int CACapManager::GetPacketSendCount()
{
	unsigned int m_unSendCount = m_pcSender->GetSnedPacketCount();

	return m_unSendCount;
}
//==============================================================================
