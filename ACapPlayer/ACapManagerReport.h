#ifndef _ACAPMANAGER_REPORT_H_
#define _ACAPMANAGER_REPORT_H_
//==============================================================================

class CACapManagerReport
{
public:
	virtual void OnUpdateReportReader(TCHAR* m_szFilePath) = 0;
	virtual void OnUpdateReportTotalPacket(unsigned int m_unTotalPacket) = 0;
};
//==============================================================================

#endif // _ACAPMANAGER_REPORT_H_
