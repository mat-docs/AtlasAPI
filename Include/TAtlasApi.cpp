// *****************************************************************************
//
//             Copyright (c) 2005-2009 McLaren Electronic Systems Ltd                    
//
// *****************************************************************************

#include "stdafx.h"
#include "TAtlasApi.h"

//
// *****************************************************************************
//                             STATIC VARIABLES
// *****************************************************************************
//
CCriticalSection CTAtlasApi::m_sectionApi;
CTAtlasApi* CTAtlasApi::m_pAtlasApi = NULL;

//
// *****************************************************************************
//                             STATIC OPERATIONS
// *****************************************************************************
//



// *****************************************************************************
//
// STATIC PUBLIC
//
// 13/04/05	DSp		Feature 05041311164980 - Add api interface to ATLAS to allow decode DLL to add parameter definitions and values
// 20/10/21	SD		Support for Begin/EndConfig in the public part of Atlas API
//
void CTAtlasApi::LoadApi()
{
	CSingleLock lock( &m_sectionApi );

	if (CTAtlasApi::m_pAtlasApi==NULL)
	{
		CTAtlasApi::m_pAtlasApi = new CTAtlasApi();
		ASSERT( CTAtlasApi::m_pAtlasApi );

		// AtlasApi.DLL will be in the same folder as the main AtlasDataServer or ATLAS exe, so no path needed
		CTAtlasApi::m_pAtlasApi->Init( _T("AtlasApi.DLL") );
	}

	CTAtlasApi::m_pAtlasApi->m_nUsers++;
}

// *****************************************************************************
//
// STATIC PUBLIC
//
// 13/04/05	DSp		Feature 05041311164980 - Add api interface to ATLAS to allow decode DLL to add parameter definitions and values
//
void CTAtlasApi::ReleaseApi( bool bForce )
{
	CSingleLock lock( &m_sectionApi );

	ASSERT( CTAtlasApi::m_pAtlasApi!=NULL );
	if (CTAtlasApi::m_pAtlasApi==NULL) return ;

	CTAtlasApi::m_pAtlasApi->m_nUsers--;

	if (bForce || (CTAtlasApi::m_pAtlasApi->m_nUsers==0))
	{
		delete CTAtlasApi::m_pAtlasApi;
		CTAtlasApi::m_pAtlasApi = NULL;
	}
}



// *****************************************************************************
//
// STATIC PUBLIC
//
// 13/04/05	DSp		Feature 05041311164980 - Add api interface to ATLAS to allow decode DLL to add parameter definitions and values
//
DWORD CTAtlasApi::ExceptionFilter(	LPEXCEPTION_POINTERS pExceptionInfo,
									HINSTANCE hInstance )
{
	TCHAR szModuleName[ _MAX_PATH + 1];
	::GetModuleFileName(hInstance, szModuleName, _MAX_PATH);

	DWORD dwCode = pExceptionInfo->ExceptionRecord->ExceptionCode;

	CString strException;
	if (dwCode==0xE06D7363)
	{
		// This magic number is the code for a C++ exception trapped by a C structured exception handler
		strException.Format(_T("C++ Exception occured in %s"),	szModuleName);
	}
	else
	{
		strException.Format(_T("Structured Exception %X occured in %s"), dwCode,	szModuleName);
	}

	TRACE( strException + _T("\r\n") );

	// Exception is recognised, continue execution
	return (EXCEPTION_EXECUTE_HANDLER);
}


// *****************************************************************************
//
// PUBLIC
//
// 13/04/05	DSp		Feature 05041311164980 - Add api interface to ATLAS to allow decode DLL to add parameter definitions and values
// 17/01/06	GP		Feature 06011217430642 - Add SetSessionFolder and SetSessionFileName functions to AtlasApi.
// 28/03/06	RDP		TicketID 06031515563888 - ATLAS Session should optionally contain Lap parameters that can be displayed
//					in the Load Session Dialog and on ATLAS Displays.
// 19/06/06	LAL		Feature 06061614410174 - Add functions to support programmatic addition of maps and constants.
// 11/09/06 GP		Fault 06091108585242 - Atlas API: Add Refresh() to update new constants.
//
CTAtlasApi::CTAtlasApi() :
	m_hInstance(NULL),
	m_nUsers( 0 ),
	m_pfnSendMsgToServer(NULL),
	m_pfnSendMsgToClients(NULL),
	m_pfnAddParameter(NULL),
	m_pfnAddRationalConversion(NULL),
	m_pfnAddTextConversion(NULL),
	m_pfnAddTableConversion(NULL),
	m_pfnAddFormulaConversion(NULL),
	m_pfnAddGroup(NULL),
	m_pfnValidate(NULL),
	m_pfnChannels(NULL),
	m_pfnAddEvent(NULL),
	m_pfnAddEventInstance(NULL),
//	02/04/07	AM		07033008490263 ActiveX: Add a new method: AddEventInstanceDebounce()
	m_pfnAddEventInstanceDebounce(NULL),
	m_pfnAddChannelData(NULL),
	m_pfnAddChannelsData(NULL),
	m_pfnAddSynchroChannelData(NULL),
	m_pfnSessionDetails(NULL),
	m_pfnLap(NULL),
	m_pfnAddLapData(NULL),
	m_pfnAbortRecording(NULL),
	m_pfnIsServer(NULL),
	m_pfnGetAtlasVersion(NULL),
	m_pfnSetSessionFolder(NULL),
	m_pfnSetSessionFilename(NULL),
	m_pfnAddConstantToSession(NULL),
	m_pfnAddLinearMapToSession(NULL),
	m_pfnAddBiLinearMapToSession(NULL),
	m_pfnRefresh(NULL),
	// 27/10/06	AM		Feature 06091820004288 - Add multiple decode DLLs
	m_pfnSetStreamId(NULL),
	m_pfnGetTagChannel(NULL),
	// 31/10/06	AM		06103009521099	ATLAS API to be modified to allow for the setting of
	//									the recording path.
	m_pfnSetRecordPath(NULL),

	//
	//17/11/06	AM		06111411485599 Multiple decode DLL does not cope with messages to/from server
	//
	m_pfnSendStreamMsgToServer(NULL),
	m_pfnSendStreamMsgToClients(NULL),

	//
	//	28/04/07	AM		07062815580899	The Atlas API does not allow the creation of a session
	//
	m_pfnCreateAtlasSession(NULL),
	m_pfnCloseAtlasSession(NULL),
	m_pfnCloseAtlasSessionEx(NULL),

	//
	// 25/09/07	AM		070912031 - Session DLL converter: make converter distributable
	//
	// For logging
	//
	m_pfnSetLoggingFile(NULL),
	m_pfnLog(NULL),
	//
	// For Data
	//
	m_pfnFlushData(NULL),
	m_pfnGetDataSourcePrefix(NULL),
	//
	// For Laps
	//
	m_pfnAddLapMarkerTime(NULL),
	m_pfnSetOutLapName(NULL),
	m_pfnSetDetailsForLap(NULL),
	m_pfnSetUseLapNames(NULL),
	m_pfnRefreshLaps(NULL),
	//
	// For errors
	//
	m_pfnAddErrorDescription(NULL),
	m_pfnAddErrorGroup(NULL),

//
// 14/12/07	AM			071211013 - ATLAS API: Add SetTeamID(n) method
//
	m_pfnSetTeamID(NULL),
	m_pfnGetTeamIDList(NULL)
{
	//
	// 04/09/07	AM		07090314575899	Exceptions raised when using the ATLAS API
	//
	HINSTANCE hinstLib; 

	// Get a handle to the resource DLL module.
 
	hinstLib = LoadLibrary(_T("ATLASRes")); 
 
	if (hinstLib)
		::AfxSetResourceHandle(hinstLib);


}

// *****************************************************************************
//
// VIRTUAL PUBLIC
//
// 13/04/05	DSp		Feature 05041311164980 - Add api interface to ATLAS to allow decode DLL to add parameter definitions and values
//
CTAtlasApi::~CTAtlasApi()
{
	if (m_hInstance)
	{
		::FreeLibrary(m_hInstance);
	}
}

// *****************************************************************************
//
// PUBLIC
//
// 13/04/05	DSp		Feature 05041311164980 - Add api interface to ATLAS to allow decode DLL to add parameter definitions and values
//
bool CTAtlasApi::Init( LPCTSTR lpszDllFilePath )
{
	m_hInstance = ::LoadLibraryEx( lpszDllFilePath,	NULL, LOAD_WITH_ALTERED_SEARCH_PATH );
	
	if (m_hInstance==NULL)
	{
		return false;
	}

	if (!GetApiFunctions())
	{
		::FreeLibrary(m_hInstance);
		m_hInstance = NULL;
		return false;
	}

	return true;
}

// *****************************************************************************
//
// PRIVATE
//
// 13/04/05	DSp		Feature 05041311164980 - Add api interface to ATLAS to allow decode DLL to add parameter definitions and values
// 09/05/05	DSp		Feature 05050917064380 = Add GetRecordingPath function to ATLAS api
// 10/10/05 GP		Feature 05101114340742 - Add StopRecording to Atlasapi for use in Decode DLL to stop a session that can be restarted
// 17/01/06	GP		Feature 06011217430642 - Add SetSessionFolder and SetSessionFileName functions to AtlasApi.
// 11/09/06 GP		Fault 06091108585242 - Atlas API: Add Refresh() to update new constants.
// 27/10/06	AM		Feature 06091820004288 - Add multiple decode DLLs
// 31/10/06	AM		06103009521099	ATLAS API to be modified to allow for the setting of
//									the recording path.
// 20/10/21	SD		Support for Begin/EndConfig in the public part of Atlas API
//
bool CTAtlasApi::GetApiFunctions()
{
	pfnGetApiFunction_t	pfnGetApiFunction = reinterpret_cast<pfnGetApiFunction_t>(::GetProcAddress( m_hInstance, "GetApiFunction"));
	if (pfnGetApiFunction==NULL)
	{
		return false;
	}

	__try
	{
		m_pfnSendMsgToServer = reinterpret_cast<pfnSendMsgToServer_t>( pfnGetApiFunction( FN_SEND_MSG_TO_SERVER ) );
		m_pfnSendMsgToClients = reinterpret_cast<pfnSendMsgToClients_t>( pfnGetApiFunction( FN_SEND_MSG_TO_CLIENTS ) );

//17/11/06	AM		06111411485599 Multiple decode DLL does not cope with messages to/from server
		m_pfnSendStreamMsgToServer = reinterpret_cast<pfnSendStreamMsgToServer_t>( pfnGetApiFunction( FN_SEND_STREAM_MSG_TO_SERVER ) );
		m_pfnSendStreamMsgToClients = reinterpret_cast<pfnSendStreamMsgToClients_t>( pfnGetApiFunction( FN_SEND_STREAM_MSG_TO_CLIENTS ) );

		m_pfnBeginConfig = reinterpret_cast<pfnBeginConfig_t>( pfnGetApiFunction( FN_BEGIN_CONFIG ) );
		m_pfnEndConfig = reinterpret_cast<pfnEndConfig_t>( pfnGetApiFunction( FN_END_CONFIG ) );
		m_pfnAddParameter = reinterpret_cast<pfnAddParameter_t>( pfnGetApiFunction( FN_ADD_PARAMETER ) );
		m_pfnAddRationalConversion = reinterpret_cast<pfnAddRationalConversion_t>( pfnGetApiFunction( FN_ADD_RATIONAL_CONVERSION ) );
		m_pfnAddTextConversion = reinterpret_cast<pfnAddTextConversion_t>( pfnGetApiFunction( FN_ADD_TEXT_CONVERSION ) );
		m_pfnAddTableConversion = reinterpret_cast<pfnAddTableConversion_t>( pfnGetApiFunction( FN_ADD_TABLE_CONVERSION ) );
		m_pfnAddFormulaConversion = reinterpret_cast<pfnAddFormulaConversion_t>( pfnGetApiFunction( FN_ADD_FORMULA_CONVERSION ) );
		m_pfnAddGroup = reinterpret_cast<pfnAddGroup_t>( pfnGetApiFunction( FN_ADD_GROUP ) );
		m_pfnValidate = reinterpret_cast<pfnValidate_t>( pfnGetApiFunction( FN_VALIDATE ) );

		//
		//	28/04/07	AM		07062815580899	The Atlas API does not allow the creation of a session
		//
		m_pfnCreateAtlasSession = reinterpret_cast<pfnCreateAtlasSession_t>( pfnGetApiFunction( FN_CREATE_ATLAS_SESSION ));
		m_pfnCloseAtlasSession = reinterpret_cast<pfnCloseAtlasSession_t>( pfnGetApiFunction( FN_CLOSE_ATLAS_SESSION ));
		//
		// 22/04/09	AM		090120018 - Facility to generate .ssv files via the Atlas API
		//
		m_pfnCloseAtlasSessionEx = reinterpret_cast<pfnCloseAtlasSessionEx_t>( pfnGetApiFunction( FN_CLOSE_ATLAS_SESSION_EX ));

		m_pfnChannels = reinterpret_cast<pfnChannels_t>( pfnGetApiFunction( FN_CHANNELS ) );
		m_pfnAddEvent = reinterpret_cast<pfnAddEvent_t>( pfnGetApiFunction( FN_ADD_EVENT ) );
		m_pfnAddEventInstance = reinterpret_cast<pfnAddEventInstance_t>( pfnGetApiFunction( FN_ADD_EVENT_INSTANCE ) );
		//
		//	02/04/07	AM		07033008490263 ActiveX: Add a new method: AddEventInstanceDebounce()
		//
		m_pfnAddEventInstanceDebounce = reinterpret_cast<pfnAddEventInstanceDebounce_t>( pfnGetApiFunction( FN_ADD_EVENT_INSTANCE_DEBOUNCE ) );
		m_pfnAddChannelData = reinterpret_cast<pfnAddChannelData_t>( pfnGetApiFunction( FN_ADD_CHANNEL_DATA ) );
		m_pfnAddChannelsData = reinterpret_cast<pfnAddChannelsData_t>( pfnGetApiFunction( FN_ADD_CHANNELS_DATA ) );
		m_pfnAddSynchroChannelData = reinterpret_cast<pfnAddSynchroChannelData_t>( pfnGetApiFunction( FN_ADD_SYNCHRO_CHANNEL_DATA ) );
		m_pfnSessionDetails = reinterpret_cast<pfnSessionDetails_t>( pfnGetApiFunction( FN_SESSION_DETAILS ) );
		m_pfnLap = reinterpret_cast<pfnLap_t>( pfnGetApiFunction( FN_LAP ) );
		m_pfnAddLapData = reinterpret_cast<pfnAddLapData_t>( pfnGetApiFunction( FN_ADD_LAP_DATA ) );

		m_pfnAbortRecording = reinterpret_cast<pfnAbortRecording_t>( pfnGetApiFunction( FN_ABORT_RECORDING ) );
		m_pfnIsServer = reinterpret_cast<pfnIsServer_t>( pfnGetApiFunction( FN_IS_SERVER ) );

		m_pfnCOMCoInitialize = reinterpret_cast<pfnCoInitializeEx_t>( pfnGetApiFunction( FN_COINITIALIZEEX ) );
		m_pfnGetRecordingPath = reinterpret_cast<pfnGetRecordingPath_t>( pfnGetApiFunction( FN_GET_RECORDING_PATH ) );
		m_pfnStopRecording = reinterpret_cast<pfnStopRecording_t>( pfnGetApiFunction(FN_STOP_RECORDING ) );

		m_pfnGetAtlasVersion = reinterpret_cast<pfnGetAtlasVersion_t>( pfnGetApiFunction(FN_GET_ATLAS_VERSION) );
		m_pfnSetSessionFolder = reinterpret_cast<pfnSetSessionFolder_t>(pfnGetApiFunction(FN_SET_SESSION_FOLDER) );
		m_pfnSetSessionFilename	= reinterpret_cast<pfnSetSessionFilename_t>(pfnGetApiFunction(FN_SET_SESSION_FILENAME) );

		m_pfnAddConstantToSession = reinterpret_cast<pfnAddConstantToSession_t>(pfnGetApiFunction(FN_ADD_CONSTANT) );
		m_pfnAddLinearMapToSession = reinterpret_cast<pfnAddLinearMapToSession_t>(pfnGetApiFunction(FN_ADD_LINEAR_MAP) );
		m_pfnAddBiLinearMapToSession = reinterpret_cast<pfnAddBiLinearMapToSession_t>(pfnGetApiFunction(FN_ADD_BILINEAR_MAP) );

		m_pfnRefresh = reinterpret_cast<pfnRefresh_t>(pfnGetApiFunction(FN_REFRESH) );

		// 27/10/06	AM		Feature 06091820004288 - Add multiple decode DLLs
		m_pfnSetStreamId = reinterpret_cast<pfnSetStreamId_t>( pfnGetApiFunction(FN_SET_STREAM_ID) );
		m_pfnGetTagChannel = reinterpret_cast<pfnGetTagChannel_t>( pfnGetApiFunction(FN_GET_TAG_CHANNEL) );
		// 31/10/06	AM		06103009521099	ATLAS API to be modified to allow for the setting of
		//									the recording path.
		m_pfnSetRecordPath = reinterpret_cast<pfnSetRecordPath_t>( pfnGetApiFunction(FN_SET_RECORD_PATH) );

		//
		// 25/09/07	AM		070912031 - Session DLL converter: make converter distributable
		//
		// For logging
		//
		m_pfnSetLoggingFile = reinterpret_cast<pfnSetLoggingFile_t>( pfnGetApiFunction(FN_SET_LOGGING_FILE) );
		m_pfnLog = reinterpret_cast<pfnLog_t>( pfnGetApiFunction(FN_LOG) );
		//
		// For Data
		//
		m_pfnFlushData = reinterpret_cast<pfnFlushData_t>( pfnGetApiFunction(FN_FLUSH_DATA) );
		m_pfnGetDataSourcePrefix = reinterpret_cast<pfnGetDataSourcePrefix_t>( pfnGetApiFunction(FN_GET_DATA_SOURCE_PREFIX) );
		//
		// For Laps
		//
		m_pfnAddLapMarkerTime = reinterpret_cast<pfnAddLapMarkerTime_t>( pfnGetApiFunction(FN_ADD_LAP_MARKER_TIME) );
		m_pfnSetOutLapName = reinterpret_cast<pfnSetOutLapName_t>( pfnGetApiFunction(FN_SET_OUT_LAP_NAME) );
		m_pfnSetDetailsForLap = reinterpret_cast<pfnSetDetailsForLap_t>( pfnGetApiFunction(FN_SET_DETAILS_FOR_LAP) );
		m_pfnSetUseLapNames = reinterpret_cast<pfnSetUseLapNames_t>( pfnGetApiFunction(FN_SET_USE_LAP_NAMES) );
		m_pfnRefreshLaps = reinterpret_cast<pfnRefreshLaps_t>( pfnGetApiFunction(FN_REFRESH_LAPS) );
		//
		// For errors
		//
		m_pfnAddErrorGroup = reinterpret_cast<pfnAddErrorGroup_t>( pfnGetApiFunction(FN_ADD_ERROR_GROUP) );

		//
		// 14/12/07	AM			071211013 - ATLAS API: Add SetTeamID(n) method
		//
		m_pfnSetTeamID = reinterpret_cast<pfnSetTeamID_t>(pfnGetApiFunction(FN_SET_TEAM_ID) );
		m_pfnGetTeamIDList = reinterpret_cast<pfnGetTeamIDList_t>(pfnGetApiFunction(FN_GET_TEAM_ID_LIST) );

		if (	(m_pfnSendMsgToServer==NULL)
			 || (m_pfnSendMsgToClients==NULL)
			 || (m_pfnAddParameter==NULL)
			 || (m_pfnCreateAtlasSession==NULL)
			 || (m_pfnCloseAtlasSession==NULL)
			 || (m_pfnCloseAtlasSessionEx==NULL)
			 || (m_pfnAddRationalConversion==NULL)
			 || (m_pfnAddTextConversion==NULL)
			 || (m_pfnAddTableConversion==NULL)
			 || (m_pfnAddFormulaConversion==NULL)
			 || (m_pfnAddGroup==NULL)
			 || (m_pfnValidate==NULL)
			 || (m_pfnChannels==NULL)
			 || (m_pfnAddEvent==NULL)
			 || (m_pfnAddEventInstance==NULL)
// 02/04/07	AM		07033008490263 ActiveX: Add a new method: AddEventInstanceDebounce()
			 || (m_pfnAddEventInstanceDebounce==NULL)
			 || (m_pfnAddChannelData==NULL)
			 || (m_pfnAddChannelsData==NULL)
			 || (m_pfnAddSynchroChannelData==NULL)
			 || (m_pfnSessionDetails==NULL)
			 || (m_pfnLap==NULL)
			 || (m_pfnAddLapData==NULL)
			 || (m_pfnAbortRecording==NULL)
			 || (m_pfnIsServer==NULL))
		{
			return false;
		}
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
	}

	return true;
}

// *****************************************************************************
//
// PUBLIC
//
// 13/04/05	DSp		Feature 05041311164980 - Add api interface to ATLAS to allow decode DLL to add parameter definitions and values
//
void CTAtlasApi::SendMsgToServer PROTO_SEND_MSG_TO_SERVER
{
	ASSERT( m_pfnSendMsgToServer );

	__try
	{
		m_pfnSendMsgToServer( pwMsgBuffer, dwLength );
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
	}
}

// *****************************************************************************
//
// PUBLIC
//
// 17/11/06	AM		06111411485599 - Multiple decode DLL does not cope with messages to/from server
//
void CTAtlasApi::SendStreamMsgToServer PROTO_SEND_STREAM_MSG_TO_SERVER
{
	ASSERT( m_pfnSendStreamMsgToServer );

	__try
	{
		m_pfnSendStreamMsgToServer( pwMsgBuffer, dwLength, wStreamId );
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
	}
}

// *****************************************************************************
//
// PUBLIC
//
// 13/04/05	DSp		Feature 05041311164980 - Add api interface to ATLAS to allow decode DLL to add parameter definitions and values
//
void CTAtlasApi::SendMsgToClients PROTO_SEND_MSG_TO_CLIENTS
{
	ASSERT( m_pfnSendMsgToClients );

	__try
	{
		m_pfnSendMsgToClients( pwMsgBuffer, dwLength );
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
	}
}

// 31/03/2010	100312008	JS	Add support to record from generic recorder into live data feed recorder
void CTAtlasApi::SendPriorityMsgToClients PROTO_SEND_MSG_TO_CLIENTS
{
	ASSERT( m_pfnSendMsgToClients );

	__try
	{
		m_pfnSendPriorityMsgToClients( pwMsgBuffer, dwLength );
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
	}
}

// *****************************************************************************
//
// PUBLIC
//
// 17/11/06	AM		06111411485599 - Multiple decode DLL does not cope with messages to/from server
//
void CTAtlasApi::SendStreamMsgToClients PROTO_SEND_STREAM_MSG_TO_CLIENTS
{
	ASSERT( m_pfnSendStreamMsgToClients );

	__try
	{
		m_pfnSendStreamMsgToClients( pwMsgBuffer, dwLength, wStreamId );
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
	}
}

// 31/03/2010	100312008	JS	Add support to record from generic recorder into live data feed recorder
void CTAtlasApi::SendStreamPriorityMsgToClients PROTO_SEND_STREAM_MSG_TO_CLIENTS
{
	ASSERT( m_pfnSendStreamPriorityMsgToClients );

	__try
	{
		m_pfnSendStreamPriorityMsgToClients( pwMsgBuffer, dwLength, wStreamId );
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
	}
}

// 20/10/21	SD		Support for Begin/EndConfig in the public part of Atlas API
void CTAtlasApi::BeginConfig(HANDLE hSession)
{
	ASSERT(m_pfnBeginConfig);

	__try
	{
		m_pfnBeginConfig(hSession);
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
	}
}

// 20/10/21	SD		Support for Begin/EndConfig in the public part of Atlas API
void CTAtlasApi::EndConfig(HANDLE hSession)
{
	ASSERT(m_pfnEndConfig);

	__try
	{
		m_pfnEndConfig(hSession);
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
	}
}

// *****************************************************************************
//
// PUBLIC
//
// 13/04/05	DSp		Feature 05041311164980 - Add api interface to ATLAS to allow decode DLL to add parameter definitions and values
//
void CTAtlasApi::AddParameter PROTO_ADD_PARAMETER
{
	ASSERT( m_pfnAddParameter );

	__try
	{
		m_pfnAddParameter(	hSession, 
							lpszIdentifier,
							lpszConversionIdentifier,
							lpszName,
							lpszDscription,
							eDataType,
							dLowerLimit,
							dUpperLimit,
							dwBitMask,
							dwErrorMask,
							lpszFormatOverride,
							dLowerWarningLimit,
							dUpperWarningLimit,
							nGroupCount,
							lpszGroupIdentifiers,
							lpszRawParameterIdentifier );
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
	}
}

// *****************************************************************************
//
// PUBLIC
//
// 13/04/05	DSp		Feature 05041311164980 - Add api interface to ATLAS to allow decode DLL to add parameter definitions and values
//
void CTAtlasApi::AddRationalConversion PROTO_ADD_RATIONAL_CONVERSION
{
	ASSERT( m_pfnAddRationalConversion );

	__try
	{
		m_pfnAddRationalConversion( hSession, 
									lpszIdentifier,
									lpszUnits,
									lpszFormat,
									dCoeff1,
									dCoeff2,
									dCoeff3,
									dCoeff4,
									dCoeff5,
									dCoeff6 );
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
	}
}

// *****************************************************************************
//
// PUBLIC
//
// 13/04/05	DSp		Feature 05041311164980 - Add api interface to ATLAS to allow decode DLL to add parameter definitions and values
//
void CTAtlasApi::AddTextConversion PROTO_ADD_TEXT_CONVERSION
{
	ASSERT( m_pfnAddTextConversion );

	__try
	{
		m_pfnAddTextConversion( hSession, 
								lpszIdentifier,
								lpszUnits,
								lpszFormat,
								nTableSize,
								dValues,
								lpszStringValues,
								lpszDefaultValue );
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
	}
}

// *****************************************************************************
//
// PUBLIC
//
// 13/04/05	DSp		Feature 05041311164980 - Add api interface to ATLAS to allow decode DLL to add parameter definitions and values
//
void CTAtlasApi::AddTableConversion PROTO_ADD_TABLE_CONVERSION
{
	ASSERT( m_pfnAddTableConversion );

	__try
	{
		m_pfnAddTableConversion(	hSession, 
									lpszIdentifier,
									lpszUnits,
									lpszFormat,
									nTableSize,
									dRawValues,
									dCalibratedValues,
									bInterpolated );
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
	}
}

// *****************************************************************************
//
// PUBLIC
//
// 13/04/05	DSp		Feature 05041311164980 - Add api interface to ATLAS to allow decode DLL to add parameter definitions and values
//
void CTAtlasApi::AddFormulaConversion PROTO_ADD_FORMULA_CONVERSION
{
	ASSERT( m_pfnAddFormulaConversion );

	__try
	{
		m_pfnAddFormulaConversion(	hSession, 
									lpszIdentifier,
									lpszUnits,
									lpszFormat,
									lpszFormula );
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
	}
}

// *****************************************************************************
//
// PUBLIC
//
// 13/04/05	DSp		Feature 05041311164980 - Add api interface to ATLAS to allow decode DLL to add parameter definitions and values
//
void CTAtlasApi::AddGroup PROTO_ADD_GROUP
{
	ASSERT( m_pfnAddGroup );

	__try
	{
		m_pfnAddGroup(	hSession, 
						lpszIdentifier,
						lpszDescription,
						nSubGroupCount,
						lpszSubGroupIdentifiers );
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
	}
}

// *****************************************************************************
//
// PUBLIC
//
// 13/04/05	DSp		Feature 05041311164980 - Add api interface to ATLAS to allow decode DLL to add parameter definitions and values
//
bool CTAtlasApi::Validate PROTO_VALIDATE
{
	ASSERT( m_pfnValidate );

	__try
	{
		return m_pfnValidate( hSession, rstrErrors );
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
	}

	return false;
}

// *****************************************************************************
//
// PUBLIC
//
// 13/04/05	DSp		Feature 05041311164980 - Add api interface to ATLAS to allow decode DLL to add parameter definitions and values
//
void CTAtlasApi::AddEvent PROTO_ADD_EVENT
{
	ASSERT( m_pfnAddEvent );

	__try
	{
		m_pfnAddEvent(	hSession, 
						wEventID,
						lpszEventDescription,
						wPriority,
						lpszConv1,
						lpszConv2,
						lpszConv3 );
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
	}
}

// *****************************************************************************
//
// PUBLIC
//
// 13/04/05	DSp		Feature 05041311164980 - Add api interface to ATLAS to allow decode DLL to add parameter definitions and values
//
void CTAtlasApi::AddEventInstance PROTO_ADD_EVENT_INSTANCE
{
	ASSERT( m_pfnAddEventInstance );

	__try
	{
		m_pfnAddEventInstance(	hSession, 
								TimeStamp,
								wEventID,
								wRawVal1,
								wRawVal2,
								wRawVal3 );
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
	}
}

// *****************************************************************************
//
// PUBLIC
//
// 02/04/07	AM		07033008490263 ActiveX: Add a new method: AddEventInstanceDebounce()
//
void CTAtlasApi::AddEventInstanceDebounce PROTO_ADD_EVENT_INSTANCE_DEBOUNCE
{
	ASSERT( m_pfnAddEventInstanceDebounce );

	__try
	{
		m_pfnAddEventInstanceDebounce(	hSession, 
										TimeStamp,
										DebouncePeriod,
										wEventID,
										wRawVal1,
										wRawVal2,
										wRawVal3 );
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
	}
}

// *****************************************************************************
//
// PUBLIC
//
// 13/04/05	DSp		Feature 05041311164980 - Add api interface to ATLAS to allow decode DLL to add parameter definitions and values
// 29/06/05 RDP		TicketID 05061310433263 - Implementation of MESL decoder
//
void CTAtlasApi::Channels PROTO_CHANNELS
{
	ASSERT( m_pfnChannels );

	__try
	{
		m_pfnChannels( hSession, dwCount, astInfo );
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
	}
}

// *****************************************************************************
//
// PUBLIC
//
// 13/04/05	DSp		Feature 05041311164980 - Add api interface to ATLAS to allow decode DLL to add parameter definitions and values
//
void CTAtlasApi::AddChannelData PROTO_ADD_CHANNEL_DATA
{
	ASSERT( m_pfnAddChannelData );

	__try
	{
		m_pfnAddChannelData( hSession, TimeStamp, dwCount, dwChannelID, pvRawData );
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
	}
}

// *****************************************************************************
//
// PUBLIC
//
// 13/04/05	DSp		Feature 05041311164980 - Add api interface to ATLAS to allow decode DLL to add parameter definitions and values
//
void CTAtlasApi::AddChannelsData PROTO_ADD_CHANNELS_DATA
{
	ASSERT( m_pfnAddChannelsData );

	__try
	{
		m_pfnAddChannelsData( hSession, TimeStamp, dwCount, pdwChannelIDs, pvRawData );
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
	}
}

// *****************************************************************************
//
// PUBLIC
//
// 28/04/05	RDP		Feature 05041311164980 - Add api interface to ATLAS to allow decode DLL to add parameter definitions and values
//
void CTAtlasApi::AddSynchroChannelData PROTO_ADD_SYNCHRO_CHANNEL_DATA
{
	ASSERT( m_pfnAddSynchroChannelData );

	__try
	{
		m_pfnAddSynchroChannelData( hSession, TimeStamp, dwChannelID, byLoggedCount, wDeltaScale, pvRawData );
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
	}
}

// *****************************************************************************
//
// PUBLIC
//
// 13/04/05	DSp		Feature 05041311164980 - Add api interface to ATLAS to allow decode DLL to add parameter definitions and values
//
void CTAtlasApi::SessionDetails PROTO_SESSION_DETAILS
{
	ASSERT( m_pfnSessionDetails );

	__try
	{
		m_pfnSessionDetails(	hSession, 
								dwCount,
								eType,
								lpszName,
								lpszValue );
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
	}
}

// *****************************************************************************
//
// PUBLIC
//
// 13/04/05	DSp		Feature 05041311164980 - Add api interface to ATLAS to allow decode DLL to add parameter definitions and values
//
void CTAtlasApi::Lap PROTO_LAP
{
	ASSERT( m_pfnLap );

	__try
	{
		m_pfnLap(	hSession, 
					TimeStamp,
					wLapNumber,
					byTriggerSource );
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
	}
}

void CTAtlasApi::AddLapData PROTO_ADD_LAP_DATA
{
	ASSERT( m_pfnAddLapData );

	__try
	{
		m_pfnAddLapData(	hSession, 
							wLapNumber,
							dwChannelId,
							byRawDataBytes,
							pvRawData );
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
	}
}

// *****************************************************************************
//
// PUBLIC
//
// 13/04/05	DSp		Feature 05041311164980 - Add api interface to ATLAS to allow decode DLL to add parameter definitions and values
//
void CTAtlasApi::AbortRecording PROTO_ABORT_RECORDING
{
	ASSERT( m_pfnAbortRecording );

	__try
	{
		m_pfnAbortRecording( hSession );
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
	}
}
// *****************************************************************************
//
// PUBLIC
//
// 10/10/05	GP		Feature  - Add api interface to ATLAS to allow decode DLL to Stop recording
//
void CTAtlasApi::StopRecording PROTO_STOP_RECORDING
{
	ASSERT( m_pfnAbortRecording );

	__try
	{
		m_pfnStopRecording( hSession );
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
	}
}

// *****************************************************************************
//
// PUBLIC
//
// 13/04/05	DSp		Feature 05041311164980 - Add api interface to ATLAS to allow decode DLL to add parameter definitions and values
//
bool CTAtlasApi::IsServer PROTO_IS_SERVER
{
	ASSERT( m_pfnIsServer );

	__try
	{
		return m_pfnIsServer();
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
	}

	return false;
}

// ************************************************************************** 
//
// 06/05/05	DSp		Feature 05050609375080 = Add fn to CoInitializeEx a data recorder thread
//
HRESULT CTAtlasApi::COMCoInitializeEx PROTO_COINITIALIZEEX
{
	ASSERT( m_pfnCOMCoInitialize );
	if (m_pfnCOMCoInitialize==NULL) return E_NOTIMPL;

	__try
	{
		return m_pfnCOMCoInitialize( hSession, dwCoInit );
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
		return E_UNEXPECTED;
	}
}

// *****************************************************************************
//
// 06/05/05	DSp		Feature 05050609375080 = Add fn to CoInitializeEx a data recorder thread
//
void CTAtlasApi::GetRecordingPath PROTO_GET_RECORDING_PATH
{
	ASSERT( m_pfnGetRecordingPath );
	if (m_pfnGetRecordingPath==NULL)
	{
		rstrFilePath.Empty();
		return;
	}

	__try
	{
		m_pfnGetRecordingPath( hSession, rstrFilePath );
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
		rstrFilePath.Empty();
	}
}

// *****************************************************************************
//
// 17/01/06	GP		Feature 06011217430642 - Add SetSessionFolder and SetSessionFileName functions to AtlasApi.
//					All New funtions should return bool for Implemented.
//
bool CTAtlasApi::GetAtlasVersion PROTO_GET_ATLAS_VERSION
{
	//
	// 04/09/07	AM		07090314575899	Exceptions raised when using the ATLAS API
	//
	// Not really part of the problem but a typographical error spotted during debugging
	// was checking to see if the function pointer m_pfnGetRecordingPath existed rather that
	// the correct one below
	//
	ASSERT( m_pfnGetAtlasVersion );
	if (m_pfnGetAtlasVersion == NULL)
	{
		rdwBuildNumber = 0;
		rstrAtlasVersion.Empty();
		return false;
	}

	__try
	{
		m_pfnGetAtlasVersion(rstrAtlasVersion,rdwBuildNumber );
		return true;
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
		rstrAtlasVersion.Empty();
		return false;
	}
}

// *****************************************************************************
//
// 17/01/06	GP		Feature 06011217430642 - Add SetSessionFolder and SetSessionFileName functions to AtlasApi.
//					All New funtions should return bool for Implemented.
//
bool CTAtlasApi::CreateAtlasSession PROTO_CREATE_ATLAS_SESSION
{
	ASSERT( m_pfnCreateAtlasSession );

	__try
	{
		m_pfnCreateAtlasSession(hSession,
								strRecordingPath,
								strRecordFileName,
								dwMinLapTime,
								strTempRecordingPath,
								strTempRecordFileName);
		return true;
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
		return false;
	}
}

// *****************************************************************************
//
// 17/01/06	GP		Feature 06011217430642 - Add SetSessionFolder and SetSessionFileName functions to AtlasApi.
//					All New funtions should return bool for Implemented.
//
bool CTAtlasApi::CloseAtlasSession PROTO_CLOSE_ATLAS_SESSION
{
	ASSERT( m_pfnCloseAtlasSession );

	__try
	{
		m_pfnCloseAtlasSession(hSession);
		return true;
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
		return false;
	}
}

// *****************************************************************************
//
// 17/01/06	GP		Feature 06011217430642 - Add SetSessionFolder and SetSessionFileName functions to AtlasApi.
//					All New funtions should return bool for Implemented.
// 22/04/09	AM		090120018 - Facility to generate .ssv files via the Atlas API
//
bool CTAtlasApi::CloseAtlasSessionEx PROTO_CLOSE_ATLAS_SESSION_EX
{
	ASSERT( m_pfnCloseAtlasSessionEx );

	__try
	{
		m_pfnCloseAtlasSessionEx(hSession, extension);
		return true;
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
		return false;
	}
}



// *****************************************************************************
//
// 17/01/06	GP		Feature 06011217430642 - Add SetSessionFolder and SetSessionFileName functions to AtlasApi.
//					All New funtions should return bool for Implemented.
//
bool CTAtlasApi::SetSessionFolder PROTO_SET_SESSION_FOLDER
{
	ASSERT( m_pfnSetSessionFolder );
	if(m_pfnSetSessionFolder == NULL)
	{
		return false;;
	}
	__try
	{
		m_pfnSetSessionFolder(hSession,rstrFolderPath );
		return true;
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
		return false;
	}

}
// *****************************************************************************
//
// 17/01/06	GP		Feature 06011217430642 - Add SetSessionFolder and SetSessionFileName functions to AtlasApi.
//					All New funtions should return bool for Implemented.
//
bool CTAtlasApi::SetSessionFilename PROTO_SET_SESSION_FILENAME
{
	ASSERT( m_pfnSetSessionFilename );
	if(m_pfnSetSessionFilename == NULL)
	{
		return false;
	}
	__try
	{
		m_pfnSetSessionFilename(hSession,rstrFilename );
		return true;
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
		return false;
	}
}

bool CTAtlasApi::AddConstantToSession( HANDLE hSession,
									LPCTSTR pszName,
									LPCTSTR pszDescription,
									LPCTSTR pszUnits,
									LPCTSTR pszFormat,
									double dfValue )
{
	ASSERT( m_pfnAddConstantToSession );
	if(m_pfnAddConstantToSession == NULL)
	{
		return false;
	}
	__try
	{
		m_pfnAddConstantToSession(hSession,pszName,pszDescription,pszUnits,pszFormat,dfValue );
		return true;
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
		return false;
	}
}

bool CTAtlasApi::AddLinearMapToSession( HANDLE hSession,
		                    LPCTSTR pszName, 
							LPCTSTR pszDescription, 
							LPCTSTR pszUnits, 
							LPCTSTR pszFormat, 
							DWORD dwNumPoints, 
							double adfBreakPoints[], 
							double adfValues[] )
{
	ASSERT( m_pfnAddLinearMapToSession );
	if(m_pfnAddLinearMapToSession == NULL)
	{
		return false;
	}
	__try
	{
		m_pfnAddLinearMapToSession(hSession,pszName,pszDescription,pszUnits,pszFormat,dwNumPoints,adfBreakPoints,adfValues );
		return true;
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
		return false;
	}
}

bool CTAtlasApi::AddBiLinearMapToSession( HANDLE hSession,
		                        LPCTSTR pszName, 
								LPCTSTR pszDescription, 
								LPCTSTR pszUnits, 
								LPCTSTR pszFormat, 
								DWORD dwNumXPoints, 
								DWORD dwNumYPoints, 
								double adfBreakXPoints[],
								double adfBreakYPoints[], 
								double adfValues[] )
{
	ASSERT( m_pfnAddBiLinearMapToSession );
	if(m_pfnAddBiLinearMapToSession == NULL)
	{
		return false;
	}
	__try
	{
		m_pfnAddBiLinearMapToSession(hSession,pszName,pszDescription,pszUnits,pszFormat,dwNumXPoints,dwNumYPoints,adfBreakXPoints,adfBreakYPoints,adfValues );
		return true;
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
		return false;
	}
}

// *****************************************************************************
//
// 11/09/06 GP		Fault 06091108585242 - Atlas API: Add Refresh() to update new constants.
//
bool CTAtlasApi::Refresh(HANDLE hSession)
{
	ASSERT(m_pfnRefresh);
	if(m_pfnRefresh == NULL)
	{
		return false;
	}
	__try
	{
		m_pfnRefresh(hSession);
		return true;
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
		return false;
	}
}
// *****************************************************************************
//
// 27/10/06 AM		Feature 06091820004288 - Add multiple decode DLL capability
//
bool CTAtlasApi::SetStreamId( HANDLE hSession, WORD wStreamId)
{
	ASSERT(m_pfnSetStreamId);
	if(m_pfnSetStreamId == NULL)
	{
		return false;
	}
	__try
	{
		m_pfnSetStreamId(hSession, wStreamId);
		return true;
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
		return false;
	}
}

// 27/10/06	AM		06103009352299	Fix GetTAGChannel so it is visable
//									through the ATLAS API functions.
//
bool CTAtlasApi::GetTagChannel(HANDLE hSession, BYTE& byChannelId)
{
	ASSERT(m_pfnGetTagChannel);
	if(m_pfnGetTagChannel == NULL)
	{
		return false;
	}
	__try
	{
		m_pfnGetTagChannel(hSession, byChannelId);
		return true;
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
		return false;
	}
}

// 31/10/06	AM		06103009521099	ATLAS API to be modified to allow for the setting of
//									the recording path.
bool CTAtlasApi::SetRecordPath( HANDLE hSession, CString& strRecordPath)
{
	ASSERT(m_pfnSetRecordPath);
	if(m_pfnSetRecordPath == NULL)
	{
		return false;
	}
	__try
	{
		m_pfnSetRecordPath(hSession, strRecordPath);
		return true;
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
		return false;
	}
}

//13/10/2008	KR		080902006 Add missing API function to ATLAS Write test
// For Logging

bool CTAtlasApi::SetLoggingFile PROTO_SET_LOGGING_FILE
{
	ASSERT(m_pfnSetLoggingFile);
	if(m_pfnSetLoggingFile == NULL)
	{
		return false;
	}
	__try
	{
		m_pfnSetLoggingFile( hSession, lpszLoggingFileName );
		return true;
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
		return false;
	}
}

bool CTAtlasApi::Log PROTO_LOG
{
	ASSERT(m_pfnLog);
	if(m_pfnLog == NULL)
	{
		return false;
	}
	__try
	{
		m_pfnLog( hSession, Text );
		return true;
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
		return false;
	}
}

//
// For Data
//
bool CTAtlasApi::FlushData PROTO_FLUSH_DATA
{
	ASSERT(m_pfnFlushData);
	if(m_pfnFlushData == NULL)
	{
		return false;
	}
	__try
	{
		m_pfnFlushData( hSession, sTime );
		return true;
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
		return false;
	}
}

bool CTAtlasApi::GetDataSourcePrefix PROTO_GET_DATA_SOURCE_PREFIX
{
	ASSERT(m_pfnGetDataSourcePrefix);
	if(m_pfnGetDataSourcePrefix == NULL)
	{
		return false;
	}
	__try
	{
		m_pfnGetDataSourcePrefix( hSession, eSourceType, lpszPrefix );
		return true;
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
		return false;
	}
}

//
// For Laps
//
bool CTAtlasApi::AddLapMarkerTime PROTO_ADD_LAP_MARKER_TIME
{
	ASSERT(m_pfnAddLapMarkerTime);
	if(m_pfnAddLapMarkerTime == NULL)
	{
		return false;
	}
	__try
	{
		m_pfnAddLapMarkerTime( hSession,
								dwLapNumber,
								sStartTime,
								byTriggerSource,
								bLog,
								bUseMinLapTime,
								lpszLapName );
		return true;
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
		return false;
	}
}

bool CTAtlasApi::SetOutLapName PROTO_SET_OUT_LAP_NAME
{
	ASSERT(m_pfnSetOutLapName);
	if(m_pfnSetOutLapName == NULL)
	{
		return false;
	}
	__try
	{
		m_pfnSetOutLapName( hSession, lpszName );
		return true;
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
		return false;
	}
}

bool CTAtlasApi::SetDetailsForLap PROTO_SET_DETAILS_FOR_LAP
{
	ASSERT(m_pfnSetDetailsForLap);
	if(m_pfnSetDetailsForLap == NULL)
	{
		return false;
	}
	__try
	{
		m_pfnSetDetailsForLap( hSession, dwLapNo, dwCount, eType, strName, strValue );
		return true;
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
		return false;
	}
}

bool CTAtlasApi::SetUseLapNames PROTO_SET_USE_LAP_NAMES
{
	ASSERT(m_pfnSetUseLapNames);
	if(m_pfnSetUseLapNames == NULL)
	{
		return false;
	}
	__try
	{
		m_pfnSetUseLapNames( hSession, bUseFlag );
		return true;
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
		return false;
	}
}

bool CTAtlasApi::RefreshLaps PROTO_REFRESH_LAPS
{
	ASSERT(m_pfnRefreshLaps);
	if(m_pfnRefreshLaps == NULL)
	{
		return false;
	}
	__try
	{
		m_pfnRefreshLaps( hSession );
		return true;
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
		return false;
	}
}

//
// For errors
//
bool CTAtlasApi::AddErrorGroup PROTO_ADD_ERROR_GROUP
{
	ASSERT(m_pfnAddErrorGroup);
	if(m_pfnAddErrorGroup == NULL)
	{
		return false;
	}
	__try
	{
		hSession; pszGroupName; pszCurrentErrorParam; pszLoggedErrorParam; pszAssociatedDefinition;
		pszText; pszBaseString;
		//m_pfnAddErrorGroup( hSession );
		return true;
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
		return false;
	}
}

//
// 14/12/07	AM			071211013 - ATLAS API: Add SetTeamID(n) method
//
bool CTAtlasApi::SetTeamID( HANDLE hSession, int &swTeamId)
{
	ASSERT(m_pfnSetTeamID);
	if(m_pfnSetTeamID == NULL)
	{
		return false;
	}
	__try
	{
		return m_pfnSetTeamID(hSession, swTeamId);
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
		return false;
	}
}

//
// 14/12/07	AM			071211013 - ATLAS API: Add SetTeamID(n) method
//
bool CTAtlasApi::GetTeamIDList( HANDLE hSession, std::vector<stTeamID> &vsTeamList)
{
	ASSERT(m_pfnGetTeamIDList);
	if(m_pfnGetTeamIDList == NULL)
	{
		return false;
	}
	__try
	{
		return m_pfnGetTeamIDList(hSession, vsTeamList);
	}
	__except(ExceptionFilter( GetExceptionInformation(), m_hInstance ))
	{
		return false;
	}
}



//
// *****************************************************************************
//                       IMPLEMENTATION - DIAGNOSTICS                        
// *****************************************************************************
//

#ifdef _DEBUG
void CTAtlasApi::AssertValid() const
{
	CObject::AssertValid();
}
void CTAtlasApi::Dump( CDumpContext& dc ) const
{
	CObject::Dump( dc );
}
#endif //_DEBUG

