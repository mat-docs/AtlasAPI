// *****************************************************************************
//
//             Copyright (c) 2005-2006 McLaren Electronic Systems Ltd                    
//
// *****************************************************************************
//
// 13/04/05	DSp		Feature 05041311164980 - Add api interface to ATLAS to allow decode DLL to add parameter definitions and values
// 28/04/05	RDP		Feature 05041311164980 - Add api interface to ATLAS to allow decode DLL to add parameter definitions and values
// 06/05/05	DSp		Feature 05050609375080 = Add fn to CoInitializeEx a data recorder thread
// 29/06/05 RDP		TicketID 05061310433263 - Implementation of MESL decoder
// 10/10/05 GP		Feature 05101114340742 - Add StopRecording to Atlasapi for use in Decode DLL to stop a session that can be restarted
// 19/06/06	LAL		Feature 06061614410174 - Add functions to support programmatic addition of maps and constants.
// 11/09/06 GP		Fault 06091108585242 - Atlas API: Add Refresh() to update new constants
// 20/10/21	SD		Support for Begin/EndConfig in the public part of Atlas API
//
#pragma once

#include "AtlasApiTypes.h"


// 28/03/06	RDP		TicketID 06031515563888 - ATLAS Session should optionally contain Lap parameters that can be displayed
//					in the Load Session Dialog and on ATLAS Displays


class CTAtlasApi : public CObject
{
// statics
public:
	static CCriticalSection m_sectionApi;
	static CTAtlasApi*	m_pAtlasApi;

	static void LoadApi();
	static void ReleaseApi( bool bForce=false );


	static DWORD ExceptionFilter(	LPEXCEPTION_POINTERS pExceptionInfo,
									HINSTANCE hInstance );

// Constructors and Destructors
public:
	CTAtlasApi();
	virtual ~CTAtlasApi();
	
// Operations
public:
	bool Init( LPCTSTR lpszDllFilePath );

	void SendMsgToServer (WORD* pwMsgBuffer, DWORD dwLength );
// 17/11/06	AM		06111411485599 - multiple decode DLL does not cope with messages to/from server
	void SendStreamMsgToServer (WORD* pwMsgBuffer, DWORD dwLength, WORD wStreamId = 15 );

	void SendMsgToClients (WORD* pwMsgBuffer, DWORD dwLength );
// 31/03/2010	100312008	JS	Add support to record from generic recorder into live data feed recorder
	void SendPriorityMsgToClients (WORD* pwMsgBuffer, DWORD dwLength );
// 17/11/06	AM		06111411485599 - multiple decode DLL does not cope with messages to/from server
	void SendStreamMsgToClients (WORD* pwMsgBuffer, DWORD dwLength, WORD wStreamId = 15 );
// 31/03/2010	100312008	JS	Add support to record from generic recorder into live data feed recorder
	void SendStreamPriorityMsgToClients (WORD* pwMsgBuffer, DWORD dwLength, WORD wStreamId = 15 );

// 20/10/21	SD		Support for Begin/EndConfig in the public part of Atlas API
	void BeginConfig PROTO_BEGIN_CONFIG;
	void EndConfig PROTO_END_CONFIG;
	void AddParameter PROTO_ADD_PARAMETER;
	void AddRationalConversion PROTO_ADD_RATIONAL_CONVERSION;
	void AddTextConversion PROTO_ADD_TEXT_CONVERSION;
	void AddTableConversion PROTO_ADD_TABLE_CONVERSION;
	void AddFormulaConversion PROTO_ADD_FORMULA_CONVERSION;
	void AddGroup PROTO_ADD_GROUP;
	bool Validate PROTO_VALIDATE;
	void AddEvent PROTO_ADD_EVENT;
	void AddEventInstance PROTO_ADD_EVENT_INSTANCE;
	// 02/04/07	AM		07033008490263 ActiveX: Add a new method: AddEventInstanceDebounce()
	void AddEventInstanceDebounce PROTO_ADD_EVENT_INSTANCE_DEBOUNCE;
	void Channels PROTO_CHANNELS;
	void AddChannelData PROTO_ADD_CHANNEL_DATA;
	void AddChannelsData PROTO_ADD_CHANNELS_DATA;
	void AddSynchroChannelData PROTO_ADD_SYNCHRO_CHANNEL_DATA;
	void SessionDetails PROTO_SESSION_DETAILS;
	void Lap PROTO_LAP;
	// 28/03/06	RDP		TicketID 06031515563888 - ATLAS Session should optionally contain Lap parameters that can be displayed
	//					in the Load Session Dialog and on ATLAS Displays
	void AddLapData PROTO_ADD_LAP_DATA;
	void AbortRecording PROTO_ABORT_RECORDING;
	bool IsServer PROTO_IS_SERVER;
	HRESULT COMCoInitializeEx PROTO_COINITIALIZEEX;
	void GetRecordingPath PROTO_GET_RECORDING_PATH;
	void StopRecording PROTO_STOP_RECORDING;

	// 17/01/06	GP		Feature 06011217430642 - Add SetSessionFolder and SetSessionFileName functions to AtlasApi.
	//					All New funtions should return bool for Implemented.
	//
	bool GetAtlasVersion PROTO_GET_ATLAS_VERSION;
	bool SetSessionFolder PROTO_SET_SESSION_FOLDER;
	bool SetSessionFilename PROTO_SET_SESSION_FILENAME;

	//
	//	29/06/07	AM	07062815580899	The Atlas API does not allow the creation of a session
	//
	bool CreateAtlasSession PROTO_CREATE_ATLAS_SESSION;
	bool CloseAtlasSession PROTO_CLOSE_ATLAS_SESSION;

	//
    // 22/04/09	AM		090120018 - Facility to generate .ssv files via the Atlas API
    //
	bool CloseAtlasSessionEx PROTO_CLOSE_ATLAS_SESSION_EX;

	// 19/06/06	LAL		Feature 06061614410174 - Add functions to support programmatic addition of maps and constants.
	//
	bool AddConstantToSession( HANDLE hSession,
		                        LPCTSTR pszName,
								LPCTSTR pszDescription,
								LPCTSTR pszUnits,
								LPCTSTR pszFormat,
								double dfValue );
	bool AddLinearMapToSession( HANDLE hSession,
		                        LPCTSTR pszName, 
								LPCTSTR pszDescription, 
								LPCTSTR pszUnits, 
								LPCTSTR pszFormat, 
								DWORD dwNumPoints, 
								double adfBreakPoints[], 
								double adfValues[] );
	bool AddBiLinearMapToSession( HANDLE hSession,
		                            LPCTSTR pszName, 
									LPCTSTR pszDescription, 
									LPCTSTR pszUnits, 
									LPCTSTR pszFormat, 
									DWORD dwNumXPoints, 
									DWORD dwNumYPoints, 
									double adfBreakXPoints[],
									double adfBreakYPoints[], 
									double adfValues[] );

	// 11/09/06 GP		Fault 06091108585242 - Atlas API: Add Refresh() to update new constants
	bool Refresh PROTO_REFRESH;

	// 27/10/06	AM		Feature 06091820004288 - ATLAS API - Add multiple decode DLL capability 
	//											plus ability to set streamID, get licenced TAG
	//											channel and specify recording storage path

	bool SetStreamId( HANDLE hSession, WORD wStreamId);
	bool GetTagChannel( HANDLE hSession, BYTE& byChannelId );

	// 31/10/06	AM		06103009521099	ATLAS API to be modified to allow for the setting of
	//									the recording path.
	bool SetRecordPath( HANDLE hSession, CString& strRecordPath);

	//13/10/2008	KR		080902006 Add missing API function to ATLAS Write test
	// For logging
	//
	bool SetLoggingFile PROTO_SET_LOGGING_FILE;
	bool Log PROTO_LOG;

	//
	// For Data
	//
	bool FlushData PROTO_FLUSH_DATA;
	bool GetDataSourcePrefix PROTO_GET_DATA_SOURCE_PREFIX;

	//
	// For Laps
	//
	bool AddLapMarkerTime( HANDLE hSession,
											DWORD dwLapNumber,
											TAG_TIME sStartTime,
											BYTE byTriggerSource = 0,
											bool bLog = false,
											bool bUseMinLapTime = true,
											LPCSTR lpszLapName = _T("") );

	bool SetOutLapName PROTO_SET_OUT_LAP_NAME;
	bool SetDetailsForLap PROTO_SET_DETAILS_FOR_LAP;
	bool SetUseLapNames PROTO_SET_USE_LAP_NAMES;
	bool RefreshLaps PROTO_REFRESH_LAPS;

	//
	// For errors
	//
	bool AddErrorGroup PROTO_ADD_ERROR_GROUP;

	//
	// 14/12/07	AM			071211013 - ATLAS API: Add SetTeamID(n) method
	//
	bool SetTeamID( HANDLE hSession, int& swTeamId );

	bool GetTeamIDList( HANDLE hSession, std::vector<stTeamID> &vsTeamList);

// Implementation - Data
private:
	HINSTANCE				m_hInstance;
	int						m_nUsers;

	pfnSendMsgToServer_t			m_pfnSendMsgToServer;
	pfnSendMsgToClients_t			m_pfnSendMsgToClients;
// 31/03/2010	100312008	JS	Add support to record from generic recorder into live data feed recorder
	pfnSendPriorityMsgToClients_t			m_pfnSendPriorityMsgToClients;

//17/11/06	AM		06111411485599 Multiple decode DLL does not cope with messages to/from server
	pfnSendStreamMsgToServer_t		m_pfnSendStreamMsgToServer;
	pfnSendStreamMsgToClients_t		m_pfnSendStreamMsgToClients;
// 31/03/2010	100312008	JS	Add support to record from generic recorder into live data feed recorder
	pfnSendStreamPriorityMsgToClients_t		m_pfnSendStreamPriorityMsgToClients;

//	29/06/07	AM	07062815580899	The Atlas API does not allow the creation of a session
	pfnCreateAtlasSession_t			m_pfnCreateAtlasSession;
	pfnCloseAtlasSession_t			m_pfnCloseAtlasSession;

    //
    // 22/04/09	AM		090120018 - Facility to generate .ssv files via the Atlas API
    //
	pfnCloseAtlasSessionEx_t		m_pfnCloseAtlasSessionEx;

// 20/10/21	SD		Support for Begin/EndConfig in the public part of Atlas API
	pfnBeginConfig_t				m_pfnBeginConfig;
	pfnEndConfig_t					m_pfnEndConfig;
	pfnAddParameter_t				m_pfnAddParameter;
	pfnAddRationalConversion_t		m_pfnAddRationalConversion;
	pfnAddTextConversion_t			m_pfnAddTextConversion;
	pfnAddTableConversion_t			m_pfnAddTableConversion;
	pfnAddFormulaConversion_t		m_pfnAddFormulaConversion;
	pfnAddGroup_t					m_pfnAddGroup;
	pfnValidate_t					m_pfnValidate;
	pfnAddEvent_t					m_pfnAddEvent;
	pfnAddEventInstance_t			m_pfnAddEventInstance;
// 02/04/07	AM		07033008490263 ActiveX: Add a new method: AddEventInstanceDebounce()
	pfnAddEventInstanceDebounce_t	m_pfnAddEventInstanceDebounce;
	pfnChannels_t					m_pfnChannels;
	pfnAddChannelData_t				m_pfnAddChannelData;
	pfnAddChannelsData_t			m_pfnAddChannelsData;
	pfnAddSynchroChannelData_t		m_pfnAddSynchroChannelData;
	pfnSessionDetails_t				m_pfnSessionDetails;
	pfnLap_t						m_pfnLap;
	// 16/03/06	RDP		TicketID 06031515563888 - Lap parameters
	pfnAddLapData_t					m_pfnAddLapData;
	pfnAbortRecording_t				m_pfnAbortRecording;
	pfnIsServer_t					m_pfnIsServer;
	pfnCoInitializeEx_t				m_pfnCOMCoInitialize;
	pfnGetRecordingPath_t			m_pfnGetRecordingPath;
	pfnStopRecording_t				m_pfnStopRecording;

	// 17/01/06	GP		Feature 06011217430642 - Add SetSessionFolder and SetSessionFileName functions to AtlasApi.
	//					All New funtions should return bool for Implemented.
	//
	pfnGetAtlasVersion_t			m_pfnGetAtlasVersion;
	pfnSetSessionFolder_t			m_pfnSetSessionFolder;
	pfnSetSessionFilename_t			m_pfnSetSessionFilename;

	// 19/06/06	LAL		Feature 06061614410174 - Add functions to support programmatic addition of maps and constants.
	//
	pfnAddConstantToSession_t		m_pfnAddConstantToSession;
	pfnAddLinearMapToSession_t		m_pfnAddLinearMapToSession;
	pfnAddBiLinearMapToSession_t	m_pfnAddBiLinearMapToSession;

	// 11/09/06 GP		Fault 06091108585242 - Atlas API: Add Refresh() to update new constants
	pfnRefresh_t					m_pfnRefresh;

	// 27/10/06	AM		Feature 06091820004288 - Add multiple decode DLLs
	pfnSetStreamId_t				m_pfnSetStreamId;
	pfnGetTagChannel_t				m_pfnGetTagChannel;
	// 31/10/06	AM		06103009521099	ATLAS API to be modified to allow for the setting of
	//									the recording path.
	pfnSetRecordPath_t				m_pfnSetRecordPath;

	//13/10/2008	KR		080902006 Add missing API function to ATLAS Write test
	// For logging
	//
	pfnSetLoggingFile_t				m_pfnSetLoggingFile;
	pfnLog_t						m_pfnLog;

	//
	// For data
	//
	pfnFlushData_t					m_pfnFlushData;
	pfnGetDataSourcePrefix_t		m_pfnGetDataSourcePrefix;

	//
	// For laps
	//
	pfnAddLapMarkerTime_t			m_pfnAddLapMarkerTime;
	pfnSetOutLapName_t				m_pfnSetOutLapName;
	pfnSetDetailsForLap_t			m_pfnSetDetailsForLap;
	pfnSetUseLapNames_t				m_pfnSetUseLapNames;
	pfnRefreshLaps_t				m_pfnRefreshLaps;

	//
	// For errors
	//
	pfnAddErrorDescription_t		m_pfnAddErrorDescription;
	pfnAddErrorGroup_t				m_pfnAddErrorGroup;

	//
	// 14/12/07	AM			071211013 - ATLAS API: Add SetTeamID(n) method
	//
	pfnSetTeamID_t					m_pfnSetTeamID;
	pfnGetTeamIDList_t				m_pfnGetTeamIDList;

// Implementation - functions
private:
	bool GetApiFunctions();


// Implementation - Diagnostics
#ifdef _DEBUG
public:          
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& rDC) const;
#endif
};

