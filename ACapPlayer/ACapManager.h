#ifndef _ACAPMANAGER_H_
#define _ACAPMANAGER_H_
//==============================================================================

#include "ACapManagerReport.h"
//==============================================================================

#define WM_ACAP_UPDATE_STATUS	(WM_USER + 1)
#define WM_SEND_FILE_NAME_MANAGER (WM_USER + 1)
//==============================================================================

struct TACAPQueueItem;
class CACapQueue;
class CACapReader;
class CACapSender;
//==============================================================================

class CACapManager : public CACapManagerReport
{
public:
	CACapManager(HWND hOwner);
	virtual ~CACapManager();

	//파일 가져오기
	bool SetFilePath(TCHAR* pszFilePath);

	//재생 속도 가져오기
	bool GetPlaySpeed(int nPos);

	//NIC 리스트 가져오기 및 선택한 디바이스 정보 가져오기
	bool GetNICList(std::vector<std::string>& nicList);
	bool SetNIC(int SelectedIndex);

	bool Start();
	bool Stop();

	virtual void OnUpdateReportReader(TCHAR* m_szFilePath);
	void OnUpdateReportTotalPacket(unsigned int m_unTotalPacket);

	int GetPacketSendCount();

protected:
	HWND m_hOwner;

	TACAPQueueItem* m_ptQueue;

	CACapQueue* m_pcBuffer;
	CACapQueue* m_pcPool;

	CACapReader* m_pcReader;
	CACapSender* m_pcSender;

	char m_szDeviceList[32][MAX_PATH];
	unsigned int m_unDeviceCout;

	pcap_if_t* pDeviceList;
	pcap_if_t* pDevice;
	CHAR cErrBuf[PCAP_ERRBUF_SIZE];
};
//==============================================================================


#endif // _ACAPMANAGER_H_
