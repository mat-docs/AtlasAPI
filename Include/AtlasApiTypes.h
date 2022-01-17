// *****************************************************************************
//
//             Copyright (c) 1998-2021 McLaren Applied Ltd                    
//
// *****************************************************************************
//
// 13/04/05	DSp		Feature 05041311164980 - Add api interface to ATLAS to allow decode DLL to add parameter definitions and values
// 29/04/05	RDP		TicketID 05040510355888 - Interface to support fixed rate logged data
// 06/05/05	DSp		Feature 05050609375080 = Add fn to CoInitializeEx a data recorder thread
// 09/05/05	DSp		Feature 05050917064380 = Add GetRecordingPath function to ATLAS api
// 20/07/05	RDP		TicketID 05061310433263 - Implement support for lap markers
// 10/10/05 GP		Feature 05101114340742 - Add StopRecording to Atlasapi for use in Decode DLL to stop a session that can be restarted
// 17/01/06	GP		Feature 06011217430642 - Add SetSessionFolder and SetSessionFileName  functions to AtlasApi
//					All New funtions should return bool for Implemented.
// 27/02/06 LAL		TicketID 06022408232563 - Add Heartbeat interface for use by "Generic" DLLs to maintain a timestamp in the system.
// 11/09/06 GP		Fault 06091108585242 - Atlas API: Add Refresh() to update new constants
// 19/10/06	AM		06091820004288 - Add SetStreamId() to allow custom ECUs to supply data on
//					the stream ID (0..15) of their choice.
// 27/10/06	AM		06103009352299	Fix access to GetTAGChannel by adding a prototype for
//									GetTagChannel + an enumerate to access the method
//									pointer.
// 30/10/06	AM		06103009521099	Add a Set recording path capability to the ATLAS API
//									Create a method SetRecordPath and add to enumerate list
// 16/11/06	AM		06111411485599	Multiple decode DLL does not cope with message to/from server
//
#pragma once

//
// 14/12/07	AM			071211013 - ATLAS API: Add SetTeamID(n) method
// 28/09/21	SD		SQLRace support for generic decode dlls
//
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif __cplusplus

typedef __int64 TAG_TIME;

typedef enum tagDATA_TYPE
{
	dtS8 = 0,
	dtU8,
	dtS16,
	dtU16,
	dtS32,
	dtU32,
	dtF32
}
DATA_TYPE;

// 16/03/06	RDP		TicketID 06031515563888 - Lap parameters
typedef enum tagDATA_SOURCE
{
	dsRealTime = 0,
	dsDump,
	dsPeriodic,
	dsBurst,
	dsLap,
	dsConstant,
	// Add new types here!

	dsUnknown
}
DATA_SOURCE;

typedef struct tagCHANNEL_INFO
{
	DWORD	dwId;
	LPCTSTR	pszTagName;
	DATA_TYPE	eType;
	TAG_TIME	Interval;
	DATA_SOURCE	eSource;
}
CHANNEL_INFO;

// 21/11/2008	JRS 71122007	ATLAS API - Unable to add data via AddSynchroChannelData
// JRS:		repackage the samples into a quad again, with the correct endian format
// synchro data channels are identified by having the following Inverval in the above structure.
// this is a horrible hack at the moment until i have found where system monitor assigns this value
TAG_TIME const SYNCHRO_DATA_INTERVAL = 0x441e ;


// 04/05/05	DSp		Feature 05050412430080 - Custom Logger  - Implement SessionDetails function in api
//					Remove unused details
typedef enum tagDETAIL_TYPE
{
	detGeneric = 0,
	detDriverName,
	detCircuitName,
	detSessionDescription,
	detOuting,
	detSession,
	detFirstLap,
	detCar,
	detRace,
	detWeather,
	detTelemetryKey,
	detPitLaneTrigger,
	detLapTriggerOffset,
	detNotes1,
	detNotes2,
	detExtraNotes,
	detECUSerialNo,
	detECUVersion,
	detRestartCount,
	detChassisNumber,
	detDateRecorded
} DETAIL_TYPE;

// 24/01/06 LAL		06010308380174 = Define the possible enumerations for AddEvent priority parameter
typedef enum tagEVENT_PRIORITY
{
	evPriorityHigh = 0,
	evPriorityMedium = 1,
	evPriorityLow = 2
} EVENT_PRIORITY;

// 28/09/07 RCB 070912033 
typedef struct tagDATA_PACKET
{
	DWORD	dwHeader ;		// Header - contains status bits and block id's
	DWORD	dwSequence ;	// Sequence number.
	WORD	awData[1] ;		// Placeholder for the data.
}
DATA_PACKET;

//
// 14/12/07	AM			071211013 - ATLAS API: Add SetTeamID(n) method
//
typedef struct tagSTTeamID
{
	CString strName;
	WORD    wID;
} stTeamID;




// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//
// ATTENTION!
//
// Only add to the end of this enum - atlas and the decode dll need to use the same values
//
enum ApiFn
{
	FN_SEND_MSG_TO_SERVER = 0,
	FN_SEND_MSG_TO_CLIENTS,
	FN_ADD_PARAMETER,
	FN_ADD_RATIONAL_CONVERSION,
	FN_ADD_TEXT_CONVERSION,
	FN_ADD_TABLE_CONVERSION,
	FN_ADD_FORMULA_CONVERSION,
	FN_ADD_GROUP,
	FN_VALIDATE,
	FN_ADD_EVENT,
	FN_ADD_EVENT_INSTANCE,
	FN_CHANNELS,
	FN_ADD_CHANNEL_DATA,
	FN_ADD_CHANNELS_DATA,
	FN_SESSION_DETAILS,
	FN_LAP,
	FN_ABORT_RECORDING,
	FN_IS_SERVER,
	FN_ADD_SYNCHRO_CHANNEL_DATA,
	FN_COINITIALIZEEX,
	FN_GET_RECORDING_PATH,
	FN_STOP_RECORDING,
	FN_ADD_CHANNEL_DATA_EX,
	FN_ADD_CHANNELS_DATA_EX,
	FN_ADD_SYNCHRO_CHANNEL_DATA_EX,
	FN_GET_ATLAS_VERSION,
	FN_SET_SESSION_FOLDER,
	FN_SET_SESSION_FILENAME,
	FN_BEGIN_CONFIG,
	FN_END_CONFIG,
	FN_SET_SERVER_SESSION_ID,
	FN_EMPTY_MESSAGE,
	FN_SEND_CONFIG_VLM,
	FN_HEART_BEAT,
	FN_ADD_LAP_DATA,
	FN_ADD_ERROR_DESCRIPTIONS,
	FN_ADD_ERROR,
	FN_ADD_CONSTANT,
	FN_ADD_LINEAR_MAP,
	FN_ADD_BILINEAR_MAP,
	FN_REFRESH,
	FN_SET_STREAM_ID,
	FN_GET_TAG_CHANNEL,
	FN_SET_RECORD_PATH,
	FN_SEND_STREAM_MSG_TO_SERVER,
	FN_SEND_STREAM_MSG_TO_CLIENTS,
//	02/04/07	AM		07033008490263 ActiveX: Add a new method: AddEventInstanceDebounce()
	FN_ADD_EVENT_INSTANCE_DEBOUNCE,
//	28/04/07	AM		07062815580899	The Atlas API does not allow the creation of a session
	FN_CREATE_ATLAS_SESSION,
	FN_CLOSE_ATLAS_SESSION,

	//
	// 25/09/07	AM		070912031 - Session DLL converter: make converter distributable
	//
	// For logging
	//
	FN_SET_LOGGING_FILE,
	FN_LOG,
	//
	// For data
	//
	FN_FLUSH_DATA,
	FN_GET_DATA_SOURCE_PREFIX,
	//
	// For laps
	//
	FN_ADD_LAP_MARKER_TIME,
	FN_SET_OUT_LAP_NAME,
	FN_SET_DETAILS_FOR_LAP,
	FN_SET_USE_LAP_NAMES,
	FN_REFRESH_LAPS,
	//
	// for Error descriptions
	//
	FN_ADD_ERROR_GROUP,

	// 28/09/07 RCB 070912033 Add method to inject quads into a session
	FN_PROCESS_INPUT_DATA,

	//22/10/07 RCB   071019016  addmethod to override default ssn extension
	FN_CLOSE_ATLAS_SESSION_EX,

	//
	// 14/12/07	AM			071211013 - ATLAS API: Add SetTeamID(n) method
	//
	FN_SET_TEAM_ID,
	FN_GET_TEAM_ID_LIST,

	// 12/12/08 RS			081209003 - Atlas API: Copy RDA Settings for vTag
	FN_COPY_RDA_SETTINGS,

	// 27/05/10	SJW			091002012: ATLAS/SQL Race: Add support for recording VTAG sessions to/from SQLRace (as associated sessions)
	FN_CREATE_SQLRACE_SESSION,

	// 15/09/11 KD	110901006 - Atlas API: Update SQLRace RDS settings from VTAG historic session to newly created session.
	//FN_UPDATE_SQLRACE_SSN_RDA_SETTINGS

	// Add new items here at the end!!!
};

// *****************************************************************************
// This is the only function exported explicitly by AtlasApi.DLL
//
#define FN_GET_API_FUNCTION "GetApiFunction"
#define PROTO_GET_API_FUNCTION ( ApiFn eApiFn )
typedef void*	(*pfnGetApiFunction_t) PROTO_GET_API_FUNCTION;

#define FN_CREATE_ATLAS_API_INSTANCE "CreateAtlasApiInstance"
#define PROTO_CREATE_ATLAS_API_INSTANCE ( bool bIsRemote )
typedef void*	(*pfnCreateAtlasApiInstance_t) PROTO_CREATE_ATLAS_API_INSTANCE;

#define FN_DELETE_ATLAS_API_INSTANCE "DeleteAtlasApiInstance"
#define PROTO_DELETE_ATLAS_API_INSTANCE ( void* pInstance )
typedef void	(*pfnDeleteAtlasApiInstance_t) PROTO_DELETE_ATLAS_API_INSTANCE;


// *****************************************************************************
// Pointers to these functions are returned by GetApiFunction
//
// 16/11/06	AM		06111411485599	Multiple decode DLL does not cope with message to/from server
#define PROTO_SEND_MSG_TO_SERVER	   (WORD* pwMsgBuffer,\
										DWORD dwLength)
typedef void (*pfnSendMsgToServer_t) PROTO_SEND_MSG_TO_SERVER;

// 16/11/06	AM		06111411485599	Multiple decode DLL does not cope with message to/from server
#define PROTO_SEND_STREAM_MSG_TO_SERVER	   (WORD* pwMsgBuffer,\
											DWORD dwLength,\
											WORD wStreamId)
typedef void (*pfnSendStreamMsgToServer_t) PROTO_SEND_STREAM_MSG_TO_SERVER;

// *****************************************************************************
// 16/11/06	AM		06111411485599	Multiple decode DLL does not cope with message to/from server
#define PROTO_SEND_MSG_TO_CLIENTS	   (WORD* pwMsgBuffer,\
										DWORD dwLength)
typedef void (*pfnSendMsgToClients_t) PROTO_SEND_MSG_TO_CLIENTS;

// 31/03/2010	100312008	JS	Add support to record from generic recorder into live data feed recorder
#define PROTO_SEND_PRIORITY_MSG_TO_CLIENTS	   (WORD* pwMsgBuffer,\
												DWORD dwLength)
typedef void (*pfnSendPriorityMsgToClients_t) PROTO_SEND_PRIORITY_MSG_TO_CLIENTS;

// *****************************************************************************
// 16/11/06	AM		06111411485599	Multiple decode DLL does not cope with message to/from server
#define PROTO_SEND_STREAM_MSG_TO_CLIENTS	   (WORD* pwMsgBuffer,\
												DWORD dwLength,\
												WORD wStreamId)
typedef void (*pfnSendStreamMsgToClients_t) PROTO_SEND_STREAM_MSG_TO_CLIENTS;

// 31/03/2010	100312008	JS	Add support to record from generic recorder into live data feed recorder
#define PROTO_SEND_STREAM_PRIORITY_MSG_TO_CLIENTS	   (WORD* pwMsgBuffer,\
														DWORD dwLength,\
														WORD wStreamId)
typedef void (*pfnSendStreamPriorityMsgToClients_t) PROTO_SEND_STREAM_PRIORITY_MSG_TO_CLIENTS;

//******************************************************************************
//	28/04/07	AM		07062815580899	The Atlas API does not allow the creation of a session
#define PROTO_CREATE_ATLAS_SESSION		(HANDLE& hSession,\
										CString& strRecordingPath, \
										CString& strRecordFileName, \
										TAG_TIME	dwMinLapTime, \
										CString& strTempRecordingPath, \
										CString& strTempRecordFileName )

typedef bool (*pfnCreateAtlasSession_t) PROTO_CREATE_ATLAS_SESSION;

//******************************************************************************
//	28/04/07	AM		07062815580899	The Atlas API does not allow the creation of a session
#define PROTO_CLOSE_ATLAS_SESSION		(HANDLE& hSession )

typedef void (*pfnCloseAtlasSession_t) PROTO_CLOSE_ATLAS_SESSION;


//******************************************************************************
//	28/04/07	RCB		071019016		Allow session extension to be supplied
#define PROTO_CLOSE_ATLAS_SESSION_EX		(HANDLE& hSession, LPCTSTR extension)

typedef void (*pfnCloseAtlasSessionEx_t) PROTO_CLOSE_ATLAS_SESSION_EX;

// *****************************************************************************
// 28/09/21	SD		SQLRace support for generic decode dlls
#define PROTO_BEGIN_CONFIG			   (HANDLE hSession)
typedef void (*pfnBeginConfig_t) PROTO_BEGIN_CONFIG;

// *****************************************************************************
// 28/09/21	SD		SQLRace support for generic decode dlls
#define PROTO_END_CONFIG			   (HANDLE hSession)
typedef void (*pfnEndConfig_t) PROTO_END_CONFIG;

// *****************************************************************************
#define PROTO_ADD_PARAMETER			   (HANDLE hSession,\
										LPCTSTR lpszIdentifier,\
										LPCTSTR lpszConversionIdentifier,\
										LPCTSTR lpszName,\
										LPCTSTR lpszDscription,\
										DATA_TYPE eDataType,\
										double dLowerLimit,\
										double dUpperLimit,\
										DWORD dwBitMask,\
										DWORD dwErrorMask,\
										LPCTSTR lpszFormatOverride,\
										double dLowerWarningLimit,\
										double dUpperWarningLimit,\
										int nGroupCount,\
										LPCTSTR lpszGroupIdentifiers[],\
										LPCTSTR lpszRawParameterIdentifier)
typedef void (*pfnAddParameter_t) PROTO_ADD_PARAMETER;

// *****************************************************************************
#define PROTO_ADD_RATIONAL_CONVERSION  (HANDLE hSession,\
										LPCTSTR lpszIdentifier,\
										LPCTSTR lpszUnits,\
										LPCTSTR lpszFormat,\
										double dCoeff1,\
										double dCoeff2,\
										double dCoeff3,\
										double dCoeff4,\
										double dCoeff5,\
										double dCoeff6)
typedef void (*pfnAddRationalConversion_t) PROTO_ADD_RATIONAL_CONVERSION;

// *****************************************************************************
#define PROTO_ADD_TEXT_CONVERSION	   (HANDLE hSession,\
										LPCTSTR lpszIdentifier,\
										LPCTSTR lpszUnits,\
										LPCTSTR lpszFormat,\
										int nTableSize,\
										double dValues[],\
										LPCTSTR lpszStringValues[],\
										LPCTSTR lpszDefaultValue)
typedef void (*pfnAddTextConversion_t) PROTO_ADD_TEXT_CONVERSION;

// *****************************************************************************
#define PROTO_ADD_TABLE_CONVERSION	   (HANDLE hSession,\
										LPCTSTR lpszIdentifier,\
										LPCTSTR lpszUnits,\
										LPCTSTR lpszFormat,\
										int nTableSize,\
										double dRawValues[],\
										double dCalibratedValues[],\
										bool bInterpolated)
									
typedef void (*pfnAddTableConversion_t) PROTO_ADD_TABLE_CONVERSION;

// *****************************************************************************
#define PROTO_ADD_FORMULA_CONVERSION   (HANDLE hSession,\
										LPCTSTR lpszIdentifier,\
										LPCTSTR lpszUnits,\
										LPCTSTR lpszFormat,\
										LPCTSTR lpszFormula)
typedef void (*pfnAddFormulaConversion_t) PROTO_ADD_FORMULA_CONVERSION;

// *****************************************************************************
#define PROTO_ADD_GROUP				   (HANDLE hSession,\
										LPCTSTR lpszIdentifier,\
										LPCTSTR lpszDescription,\
										int nSubGroupCount,\
										LPCTSTR lpszSubGroupIdentifiers[])
typedef void (*pfnAddGroup_t) PROTO_ADD_GROUP;

// *****************************************************************************
#define PROTO_VALIDATE				   (HANDLE hSession,\
										CString& rstrErrors)
typedef bool (*pfnValidate_t) PROTO_VALIDATE;

// *****************************************************************************
#define PROTO_CHANNELS				   (HANDLE hSession,\
										DWORD dwCount,\
										CHANNEL_INFO astInfo[] )
typedef void	(*pfnChannels_t) PROTO_CHANNELS;

// *****************************************************************************
#define PROTO_ADD_EVENT				   (HANDLE hSession,\
										WORD wEventID,\
										LPCTSTR lpszEventDescription,\
										WORD wPriority,\
										LPCTSTR lpszConv1,\
										LPCTSTR lpszConv2,\
										LPCTSTR lpszConv3)
typedef void (*pfnAddEvent_t) PROTO_ADD_EVENT;

// *****************************************************************************
#define PROTO_ADD_EVENT_INSTANCE	   (HANDLE hSession,\
										TAG_TIME TimeStamp,\
										WORD wEventID,\
										WORD wRawVal1,\
										WORD wRawVal2,\
										WORD wRawVal3)
typedef void (*pfnAddEventInstance_t) PROTO_ADD_EVENT_INSTANCE;

// 02/04/07	AM		07033008490263 ActiveX: Add a new method: AddEventInstanceDebounce()
// *****************************************************************************
#define PROTO_ADD_EVENT_INSTANCE_DEBOUNCE	   (HANDLE hSession,\
										TAG_TIME TimeStamp,\
										TAG_TIME DebouncePeriod,\
										WORD wEventID,\
										WORD wRawVal1,\
										WORD wRawVal2,\
										WORD wRawVal3)
typedef void (*pfnAddEventInstanceDebounce_t) PROTO_ADD_EVENT_INSTANCE_DEBOUNCE;

// *****************************************************************************
#define PROTO_ADD_CHANNEL_DATA		   (HANDLE hSession,\
										TAG_TIME TimeStamp,\
										DWORD dwCount,\
										DWORD dwChannelID,\
										void* pvRawData)
typedef void (*pfnAddChannelData_t) PROTO_ADD_CHANNEL_DATA;

// *****************************************************************************
#define PROTO_ADD_CHANNEL_DATA_EX	   (HANDLE hSession,\
										TAG_TIME TimeStamp,\
										DWORD dwCount,\
										DWORD dwChannelID,\
										void* pvRawData,\
										DWORD dwByteCount)
typedef bool (*pfnAddChannelDataEx_t) PROTO_ADD_CHANNEL_DATA_EX;

// *****************************************************************************
#define PROTO_ADD_CHANNELS_DATA		   (HANDLE hSession,\
										TAG_TIME TimeStamp,\
										DWORD dwCount,\
										DWORD* pdwChannelIDs,\
										void* pvRawData)
typedef void (*pfnAddChannelsData_t) PROTO_ADD_CHANNELS_DATA;

// *****************************************************************************
// This is for the remote API which needs to know how much data to serialize
#define PROTO_ADD_CHANNELS_DATA_EX (HANDLE hSession,\
											TAG_TIME TimeStamp,\
											DWORD dwCount,\
											DWORD* pdwChannelIDs,\
											void* pvRawData,\
											DWORD dwByteCount)
typedef bool (*pfnAddChannelsDataEx_t) PROTO_ADD_CHANNELS_DATA_EX;

// *****************************************************************************
#define PROTO_ADD_SYNCHRO_CHANNEL_DATA	   (HANDLE hSession,\
											TAG_TIME TimeStamp,\
											DWORD dwChannelID,\
											BYTE byLoggedCount,\
											WORD wDeltaScale,\
											void* pvRawData)
typedef void (*pfnAddSynchroChannelData_t) PROTO_ADD_SYNCHRO_CHANNEL_DATA;

// *****************************************************************************
#define PROTO_ADD_SYNCHRO_CHANNEL_DATA_EX   (HANDLE hSession,\
											TAG_TIME TimeStamp,\
											DWORD dwChannelID,\
											BYTE byLoggedCount,\
											WORD wDeltaScale,\
											void* pvRawData,\
											DWORD dwByteCount)
typedef bool (*pfnAddSynchroChannelDataEx_t) PROTO_ADD_SYNCHRO_CHANNEL_DATA_EX;

// *****************************************************************************
#define PROTO_SESSION_DETAILS		   (HANDLE hSession,\
										DWORD dwCount,\
										DETAIL_TYPE eType[],\
										LPCTSTR lpszName[],\
										LPCTSTR lpszValue[])
typedef void (*pfnSessionDetails_t) PROTO_SESSION_DETAILS;

// *****************************************************************************
#define PROTO_LAP					   (HANDLE hSession,\
										TAG_TIME TimeStamp,\
										WORD wLapNumber,\
										BYTE byTriggerSource)
typedef void (*pfnLap_t) PROTO_LAP;

// *****************************************************************************
// 31/03/06	RDP		TicketID 06031515563888 - Lap parameters

#define OUT_LAP_NUMBER	0xFFFF

#define PROTO_ADD_LAP_DATA			   (HANDLE hSession,\
										WORD wLapNumber,\
										DWORD dwChannelId,\
										BYTE byRawDataBytes,\
										void* pvRawData)
typedef void (*pfnAddLapData_t) PROTO_ADD_LAP_DATA;

// *****************************************************************************
#define PROTO_ABORT_RECORDING		   (HANDLE hSession)
typedef void (*pfnAbortRecording_t) PROTO_ABORT_RECORDING;

// *****************************************************************************
#define PROTO_IS_SERVER				   (void)
typedef bool (*pfnIsServer_t) PROTO_IS_SERVER;

// *****************************************************************************
#define PROTO_COINITIALIZEEX	   (HANDLE hSession,\
									DWORD dwCoInit)
typedef HRESULT (*pfnCoInitializeEx_t) PROTO_COINITIALIZEEX;

// *****************************************************************************
#define PROTO_GET_RECORDING_PATH	   (HANDLE hSession,\
										CString& rstrFilePath)
typedef HRESULT (*pfnGetRecordingPath_t) PROTO_GET_RECORDING_PATH;

// *****************************************************************************
#define PROTO_STOP_RECORDING		   (HANDLE hSession)
typedef void (*pfnStopRecording_t) PROTO_STOP_RECORDING;

// *****************************************************************************
// 17/01/06	GP		Feature 06011217430642 - Add SetSessionFolder and 
//					SetSessionFileName  functions to AtlasApi
//					All New funtions should return bool for Implemented.
//
// *****************************************************************************
#define PROTO_GET_ATLAS_VERSION			(CString& rstrAtlasVersion,\
										DWORD & rdwBuildNumber)
typedef bool (*pfnGetAtlasVersion_t) PROTO_GET_ATLAS_VERSION;

// *****************************************************************************
#define PROTO_SET_SESSION_FOLDER		(HANDLE hSession,\
										CString& rstrFolderPath)
typedef bool (*pfnSetSessionFolder_t) PROTO_SET_SESSION_FOLDER;

// *****************************************************************************
#define PROTO_SET_SESSION_FILENAME		(HANDLE hSession,\
										CString& rstrFilename)
typedef bool (*pfnSetSessionFilename_t) PROTO_SET_SESSION_FILENAME;

// *****************************************************************************
// 27/02/06 LAL		TicketID 06022408232563 - Add Heartbeat interface for use 
//                  by "Generic" DLLs to maintain a timestamp in the system.
// *****************************************************************************
#define PROTO_HEARTBEAT					(HANDLE hSession, \
	                                    TAG_TIME TimeStamp)
typedef bool (*pfnHeartbeat_t) PROTO_HEARTBEAT;

// *****************************************************************************
// 24/03/06 LAL		TicketID 06022116094488 - Add the AddErrorDescription function to
//					provide an interface for errors into ATLAS.
// *****************************************************************************
#define PROTO_ADD_ERROR_DESCRIPTION		(HANDLE hSession, \
	                                    LPCTSTR pszDescriptionId, \
										LPCTSTR* pszArrErrors, \
										DWORD dwNumberOfErrorBits )
typedef void (*pfnAddErrorDescription_t) PROTO_ADD_ERROR_DESCRIPTION;

// *****************************************************************************
// 24/03/06 LAL		TicketID 06022116094488 - Add the AddErrorDescription function to
//					provide an interface for errors into ATLAS.
// *****************************************************************************
#define PROTO_ADD_ERROR		(HANDLE hSession, \
		                    LPCTSTR pszGroupId, \
						    LPCTSTR pszCurrentErrorParam, \
						    LPCTSTR pszLoggedErrorParam, \
						    LPCTSTR pszAssociatedDefinition, \
						    LPCTSTR pszText, \
						    LPCTSTR pszBaseString )										 
typedef void (*pfnAddErr_t) PROTO_ADD_ERROR;

// *****************************************************************************
// 19/06/06	LAL		Feature 06061614410174 - Add functions to support 
//					programmatic addition of maps and constants.
// *****************************************************************************
#define PROTO_ADD_CONSTANT_TO_SESSION (HANDLE hSession, \
	                                  LPCTSTR pszName, \
									  LPCTSTR pszDescription, \
									  LPCTSTR pszUnits, \
									  LPCTSTR pszFormat, \
									  double dfValue )
typedef void (*pfnAddConstantToSession_t) PROTO_ADD_CONSTANT_TO_SESSION;

#define PROTO_ADD_LINEAR_MAP_TO_SESSION (HANDLE hSession, \
	                                    LPCTSTR pszName, \
										LPCTSTR pszDescription, \
										LPCTSTR pszUnits, \
										LPCTSTR pszFormat, \
										DWORD dwNumPoints, \
										double adfBreakPoints[], \
										double adfValues[] )
typedef void (*pfnAddLinearMapToSession_t) PROTO_ADD_LINEAR_MAP_TO_SESSION;

#define PROTO_ADD_BILINEAR_MAP_TO_SESSION (HANDLE hSession, \
	                                    LPCTSTR pszName, \
										LPCTSTR pszDescription, \
										LPCTSTR pszUnits, \
										LPCTSTR pszFormat, \
										DWORD dwNumXPoints, \
										DWORD dwNumYPoints, \
                                        double adfBreakXPoints[], \
										double adfBreakYPoints[], \
										double adfValues[] )
typedef void (*pfnAddBiLinearMapToSession_t) PROTO_ADD_BILINEAR_MAP_TO_SESSION;

// *****************************************************************************
// 11/09/06 GP		Fault 06091108585242 - Atlas API: Add Refresh() to
//					update new constants
// *****************************************************************************
#define PROTO_REFRESH					(HANDLE hSession)
typedef void (*pfnRefresh_t) PROTO_REFRESH;


// *****************************************************************************
// 19/09/06 AM		06091820004288 - Atlas API: Add Set stream ID to
//					allow custom ECUs to select the stream they will
//					provide data on.
// *****************************************************************************
#define PROTO_SET_STREAM_ID				(HANDLE hSession, WORD wStreamId)
typedef bool (*pfnSetStreamId_t) PROTO_SET_STREAM_ID;


// *****************************************************************************
// 27/09/06 AM		06091820004288 - Atlas API: Add GetTAGChannel to
//					allow custom ECUs to select the stream they will
//					provide data on.
// *****************************************************************************
#define PROTO_GET_TAG_CHANNEL				(HANDLE hSession, BYTE& byChannelId)
typedef bool (*pfnGetTagChannel_t) PROTO_GET_TAG_CHANNEL;

// *****************************************************************************
// 27/09/06 AM		06091820004288 - Atlas API: Add SetRecordPath to
//					allow custom ECUs to select file to record the data to.
// *****************************************************************************
#define PROTO_SET_RECORD_PATH				(HANDLE hSession, \
												CString& strRecordPath)
typedef bool (*pfnSetRecordPath_t) PROTO_SET_RECORD_PATH;


// *****************************************************************************
	//
	// 25/09/07	AM		070912031 - Session DLL converter: make converter distributable
	//
	// For logging
	//
#define PROTO_SET_LOGGING_FILE	( HANDLE hSession, LPCSTR lpszLoggingFileName )
typedef bool (*pfnSetLoggingFile_t) PROTO_SET_LOGGING_FILE;

#define	PROTO_LOG ( HANDLE hSession, const LPCSTR Text )
typedef bool (*pfnLog_t) PROTO_LOG;

	//
	// For data
	//
#define PROTO_FLUSH_DATA ( HANDLE hSession, TAG_TIME sTime )
typedef bool (*pfnFlushData_t) PROTO_FLUSH_DATA;

#define PROTO_GET_DATA_SOURCE_PREFIX ( HANDLE hSession, DATA_SOURCE eSourceType, CString & lpszPrefix )
typedef bool (*pfnGetDataSourcePrefix_t) PROTO_GET_DATA_SOURCE_PREFIX;

	//
	// For laps
	//
#define PROTO_ADD_LAP_MARKER_TIME ( HANDLE hSession,\
									DWORD dwLapNumber, \
									TAG_TIME sStartTime, \
									BYTE byTriggerSource, \
									bool bLog, \
									bool bUseMinLapTime, \
									LPCSTR lpszLapName )
typedef bool (*pfnAddLapMarkerTime_t) PROTO_ADD_LAP_MARKER_TIME;

#define	PROTO_SET_OUT_LAP_NAME ( HANDLE hSession, LPCSTR lpszName )
typedef bool (*pfnSetOutLapName_t) PROTO_SET_OUT_LAP_NAME;

#define	PROTO_SET_DETAILS_FOR_LAP (HANDLE hSession,\
										DWORD dwLapNo, \
										DWORD dwCount,\
										DETAIL_TYPE eType[],\
										CString strName[],\
										CString strValue[])
typedef bool (*pfnSetDetailsForLap_t) PROTO_SET_DETAILS_FOR_LAP;

#define	PROTO_SET_USE_LAP_NAMES ( HANDLE hSession, bool bUseFlag )
typedef bool (*pfnSetUseLapNames_t) PROTO_SET_USE_LAP_NAMES;

#define	PROTO_REFRESH_LAPS ( HANDLE hSession )
typedef bool (*pfnRefreshLaps_t) PROTO_REFRESH_LAPS;

	//
	// for Error descriptions
	//
#define	PROTO_ADD_ERROR_GROUP ( HANDLE hSession, LPCTSTR pszGroupName, \
								LPCTSTR pszCurrentErrorParam, \
								LPCTSTR pszLoggedErrorParam, \
								LPCTSTR pszAssociatedDefinition, \
								LPCTSTR pszText, \
								LPCTSTR pszBaseString )
typedef bool (*pfnAddErrorGroup_t) PROTO_ADD_ERROR_GROUP;


// *****************************************************************************
// 28/09/07 RCB 
//                  
// *****************************************************************************
//#define PROTO_PROCESS_INPUT_DATA   (HANDLE hSession, \
//									DATA_PACKET* pstDataPacket, \
//									WORD wPacketSize)

#define PROTO_PROCESS_INPUT_DATA (HANDLE hSession, DATA_PACKET* pstDataPacket, WORD wPacketSize) 

typedef bool (*pfnProcessInputData_t) PROTO_PROCESS_INPUT_DATA; 


//
// 14/12/07	AM			071211013 - ATLAS API: Add SetTeamID(n) method
//
#define PROTO_SET_TEAM_ID (HANDLE hSession, int& swTeamId)

typedef bool (*pfnSetTeamID_t) PROTO_SET_TEAM_ID;

//
// 14/12/07	AM			071211013 - ATLAS API: Add SetTeamID(n) method
//
#define PROTO_GET_TEAM_ID_LIST (HANDLE hSession, std::vector<stTeamID> &vsTeamList)

typedef bool (*pfnGetTeamIDList_t) PROTO_GET_TEAM_ID_LIST;

// 12/12/08 RS	081209003 - Atlas API: Copy RDA Settings for vTag
#define PROTO_COPY_RDA_SETTINGS (long hFromSession, HANDLE hToSession)
typedef bool (*pfnCopyRDASettings_t) PROTO_COPY_RDA_SETTINGS;

// 27/05/10	SJW	091002012: ATLAS/SQL Race: Add support for recording VTAG sessions to/from SQLRace (as associated sessions)
#define PROTO_CREATE_SQLRACE_SESSION		(HANDLE& hSession, \
											CString& strConnectionString, \
											CString& strKey, \
											CString& strSessionIdentifier, \
											CString& strTypeIdentifier, \
											TAG_TIME dwMinLapTime)

typedef bool (*pfnCreateSqlRaceSession_t) PROTO_CREATE_SQLRACE_SESSION;

// 15/09/11 KD	110901006 - Atlas API: Update SQLRace RDS settings from VTAG historic session to newly created session.
//#define PROTO_UPDATE_SQLRACE_RDA_SETTINGS (long hFromSession, HANDLE hDestSession)

//typedef bool (*pfnUpdateSqlRaceSsnRDASettings_t) PROTO_UPDATE_SQLRACE_RDA_SETTINGS;

#ifdef __cplusplus
}
#endif __cplusplus

