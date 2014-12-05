//=============================================================================
// Copyright © 2010 NaturalPoint, Inc. All Rights Reserved.
// 
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall NaturalPoint, Inc. or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//=============================================================================


/*

SimpleServer.cpp

Illustrates the minimum code required to create and send marker data using
the NatNet server.

*/

#include <stdio.h>
#include <tchar.h>
#include <conio.h>
#include <winsock2.h>
#include <math.h>
#include "NatNetTypes.h"
#include "NatNetServer.h"

#include "mmsystem.h"
#include "NATUtils.h"

#pragma warning( disable : 4996 )

void BuildDescription(sDataDescriptions* pDescription);
void SendDescription(sDataDescriptions* pDescription);
void FreeDescription(sDataDescriptions* pDescription);
void BuildFrame(long FrameNumber, sDataDescriptions* pDataDescriptions, sFrameOfMocapData* pOutFrame);
void SendFrame(sFrameOfMocapData* pFrame);
void FreeFrame(sFrameOfMocapData* pFrame);
void resetServer();
int CreateServer(int iConnectionType);
int HiResSleep(int msecs);

// callbacks
void __cdecl MessageHandler(int msgType, char* msg);
int __cdecl RequestHandler(sPacket* pPacketIn, sPacket* pPacketOut,void* pUserData);

// playing thread
int StartPlayingThread();
int StopPlayingThread();
DWORD WINAPI PlayingThread_Func(void *dummy);

NatNetServer* theServer;    					// The NatNet Server 
sDataDescriptions descriptions; 				// Describes what is sent (Marker sets, rigid bodies, etc)
long g_lCurrentFrame = 0;
bool g_bPlaying = false;
DWORD PlayingThread_ID = NULL;
HANDLE PlayingThread_Handle = NULL;	
int counter = 0;
int counter2 = 0;
float fCounter = 0.0f;

unsigned int MyDataPort = 3130;
unsigned int MyCommandPort = 3131;
unsigned long lAddresses[10];

#define STREAM_RBS 1
#define STREAM_MARKERS 0
#define STREAM_SKELETONS 0
#define STREAM_LABELED_MARKERS 0 
#define PId 3.14159265358979


int _tmain(int argc, _TCHAR* argv[])
{
    // Create a NatNet server
    int iConnectionType = ConnectionType_Multicast;
    //int iConnectionType = ConnectionType_Unicast;
    int iResult = CreateServer(iConnectionType);
    if(iResult != ErrorCode_OK)
    {
        printf("Error initializing server.  See log for details.  Exiting");
        return 1;
    }

    // Create a MarkerSet description
    BuildDescription(&descriptions);

    // OK! Ready to stream data.  Listen for request from clients (RequestHandler())
	printf("\n\nCommands:\nn\tnext frame\ns\tstream frames\nr\treset server\nq\tquit\n\r\tmulticast\nu\tunicast\n\n");
	bool bExit = false;
	while(int c =_getch())
	{
		switch(c)
		{
			case 'n':							// next frame
                {
				sFrameOfMocapData frame;
				BuildFrame(g_lCurrentFrame++, &descriptions, &frame);
				SendFrame(&frame);
				FreeFrame(&frame);
                }
				break;
			case 'q':                           // quit
				bExit = true;		
				break;
            case 's':							// play continuously
                g_lCurrentFrame = 0;
                if(g_bPlaying)
                    StopPlayingThread();
                else
                    StartPlayingThread();
                break;
            case 'r':	                        // reset server
                resetServer();
                break;
            case 'm':	                        // change to multicast
                iResult = CreateServer(ConnectionType_Multicast);
                if(iResult == ErrorCode_OK)
                    printf("Server connection type changed to Multicast.\n\n");
                else
                    printf("Error changing server connection type to Multicast.\n\n");
                break;
            case 'u':	                        // change to unicast
                iResult = CreateServer(ConnectionType_Unicast);
                if(iResult == ErrorCode_OK)
                    printf("Server connection type changed to Unicast.\n\n");
                else
                    printf("Error changing server connection type to Unicast.\n\n");
                break;

			default:
				break;
		}

		if(bExit)
		{
			theServer->Uninitialize();
			FreeDescription(&descriptions);
			break;
		}
	}

	return ErrorCode_OK;
}

int CreateServer(int iConnectionType)
{
    // release previous server
    if(theServer)
    {
        theServer->Uninitialize();
        delete theServer;
    }

    // create new NatNet server
    theServer = new NatNetServer(iConnectionType);

    // optional - use old multicast group
    //theServer->SetMulticastAddress("224.0.0.1");

    // print version info
    unsigned char ver[4];
    theServer->NatNetVersion(ver);
    printf("NatNet Simple Server (NatNet ver. %d.%d.%d.%d)\n", ver[0], ver[1], ver[2], ver[3]);

    // set callbacks
    theServer->SetErrorMessageCallback(MessageHandler);
    theServer->SetVerbosityLevel(Verbosity_Debug);
    theServer->SetMessageResponseCallback(RequestHandler);	    // Handles requests from the Client

    // get local ip addresses
    char szIPAddress[128];
    in_addr MyAddress[10];
    int nAddresses = NATUtils::GetLocalIPAddresses2((unsigned long *)&MyAddress, 10);
    if(nAddresses < 1)
    {
        printf("Unable to detect local network.  Exiting");
        return 1;
    }
    for(int i=0; i < nAddresses; i++)
    {
        sprintf(szIPAddress, "%d.%d.%d.%d", MyAddress[i].S_un.S_un_b.s_b1, MyAddress[i].S_un.S_un_b.s_b2, MyAddress[i].S_un.S_un_b.s_b3, MyAddress[i].S_un.S_un_b.s_b4);
        printf("IP Address : %s\n", szIPAddress);
    }
    // select first
    sprintf(szIPAddress, "%d.%d.%d.%d", MyAddress[0].S_un.S_un_b.s_b1, MyAddress[0].S_un.S_un_b.s_b2, MyAddress[0].S_un.S_un_b.s_b3, MyAddress[0].S_un.S_un_b.s_b4);

    // Initialize NatNet server with first detected ip address - use NatNet default port assignments
	int retCode = theServer->Initialize(szIPAddress);
    // to use a different port for commands and/or data:
    //int retCode = theServer->Initialize(szIPAddress, MyCommandPort, MyDataPort);
    if(retCode != 0)
    {
        printf("Error initializing server.  See log for details.  Exiting");
        return 1;
    }
    else
    {
        printf("Server initialized on %s.\n", szIPAddress);
    }

    // print address/port info
    char szCommandIP[24];
    char szDataIP[24];
    char szMulticastGroup[24];
    int iDataPort, iCommandPort, iMulticastPort;
    theServer->GetSocketInfo(szDataIP, &iDataPort, szCommandIP, &iCommandPort, szMulticastGroup, &iMulticastPort);
    printf("Command Socket  : %s:%d\n", szCommandIP, iCommandPort);
    printf("Data Socket     : %s:%d\n", szDataIP, iDataPort);
    if(iConnectionType==ConnectionType_Multicast)
    {
        printf("Connection Type : Multicast\n");
        printf("Multicast Group : %s:%d\n", szMulticastGroup, iMulticastPort);
    }
    else
        printf("Connection Type : Unicast\n");


    return ErrorCode_OK;

}

// RequestHandler receives requests from the Client.  Server should
// build and return "request responses" packets in this same thread.
//
//   pPacketIn  - data packet from Client
//   pPacketOut - empty packet, to be filled in and returned to the
//				  Client as the "request response".
//
int __cdecl RequestHandler(sPacket* pPacketIn, sPacket* pPacketOut, void* pUserData)
{
	int iHandled = 1;	// handled

    switch (pPacketIn->iMessage)
    {
    case NAT_PING:
        printf("[SampleServer] received ping from Client.\n");
        printf("[SampleServer] Client App Name : %s\n", pPacketIn->Data.Sender.szName);
        printf("[SampleServer] Client App Version : %d.%d.%d.%d\n", pPacketIn->Data.Sender.Version[0],
            pPacketIn->Data.Sender.Version[1],pPacketIn->Data.Sender.Version[2],pPacketIn->Data.Sender.Version[3]);
        printf("[SampleServer] Client App NatNet Version : %d.%d.%d.%d\n", pPacketIn->Data.Sender.NatNetVersion[0],
            pPacketIn->Data.Sender.NatNetVersion[1],pPacketIn->Data.Sender.NatNetVersion[2],pPacketIn->Data.Sender.NatNetVersion[3]);
        
        // build server info packet
        strcpy(pPacketOut->Data.Sender.szName, "SimpleServer");
        pPacketOut->Data.Sender.Version[0] = 2;
        pPacketOut->Data.Sender.Version[1] = 1;
        pPacketOut->iMessage = NAT_PINGRESPONSE;
        pPacketOut->nDataBytes = sizeof(sSender);
        iHandled = 1;
        break;

    case NAT_REQUEST_MODELDEF:
		printf("[SimpleServer] Received request for data descriptions.\n");
		theServer->PacketizeDataDescriptions(&descriptions, pPacketOut);
        break;

	case NAT_REQUEST_FRAMEOFDATA:
		{
		// note: Client does not typically poll for data, but we accomodate it here anyway
		// note: need to return response on same thread as caller
		printf("[SimpleServer] Received request for frame of data.\n");
		sFrameOfMocapData frame;
		BuildFrame(g_lCurrentFrame, &descriptions, &frame);
		theServer->PacketizeFrameOfMocapData(&frame, pPacketOut);
		FreeFrame(&frame);
		}
        break;

    case NAT_REQUEST:
        printf("[SampleServer] Received request from Client: %s\n", pPacketIn->Data.szData);
        pPacketOut->iMessage = NAT_UNRECOGNIZED_REQUEST;
        if (stricmp(pPacketIn->Data.szData, "TestRequest") == 0)
        {
            pPacketOut->iMessage = NAT_RESPONSE;
            strcpy(pPacketOut->Data.szData, "TestResponse");
            pPacketOut->nDataBytes = ((int)strlen(pPacketOut->Data.szData)) + 1;
        }
        break;

    default:
        pPacketOut->iMessage = NAT_UNRECOGNIZED_REQUEST;
        pPacketOut->nDataBytes = 0;
		iHandled = 0;
        break;
    }

	return iHandled; // 0 = not handled, 1 = handled;
}

// Build a DataSet description (MarkerSets, RigiBodies, Skeletons).
void BuildDescription(sDataDescriptions* pDescription)
{
    pDescription->nDataDescriptions = 0;
    int index = 0;

#if STREAM_MARKERS
    // Marker Set Description
	sMarkerSetDescription* pMarkerSetDescription = new sMarkerSetDescription();
	strcpy(pMarkerSetDescription->szName, "Katie");
	pMarkerSetDescription->nMarkers = 10;
	pMarkerSetDescription->szMarkerNames = new char*[pMarkerSetDescription->nMarkers];
	char szTemp[128];
    for (int i=0; i < pMarkerSetDescription->nMarkers; i++)
	{
		sprintf(szTemp, "Marker %d", i);
		pMarkerSetDescription->szMarkerNames[i] = new char[MAX_NAMELENGTH];
		strcpy(pMarkerSetDescription->szMarkerNames[i], szTemp);
	}
	pDescription->arrDataDescriptions[index].type = Descriptor_MarkerSet;
	pDescription->arrDataDescriptions[index].Data.MarkerSetDescription = pMarkerSetDescription;
    pDescription->nDataDescriptions++;
    index++;
#endif

#if STREAM_RBS
    // Rigid Body Description
    for(int i = 0; i < 3/*sizeof(rbs)/sizeof(RigidBody)*/; i++)
    {
        sRigidBodyDescription* pRigidBodyDescription = new sRigidBodyDescription();
        sprintf(pRigidBodyDescription->szName, "RigidBody %d", i);
        pRigidBodyDescription->ID = i;
        pRigidBodyDescription->offsetx = 0.0f;
        pRigidBodyDescription->offsety = 0.0f;
        pRigidBodyDescription->offsetz = 0.0f;
        pRigidBodyDescription->parentID = 0;
        pDescription->arrDataDescriptions[index].type = Descriptor_RigidBody;
        pDescription->arrDataDescriptions[index].Data.RigidBodyDescription = pRigidBodyDescription;
        pDescription->nDataDescriptions++;

		//rbs[i].m_position = ml::vec3(0.0f);
		//rbs[i].m_orientation = ml::quaternion(0.0f,0.0f,0.0f,1.0f);

        index++;
    }
#endif

#if STREAM_SKELETONS
    // Skeleton description
    for(int i=0; i < 2; i++)
    {
        sSkeletonDescription* pSkeletonDescription = new sSkeletonDescription();
        sprintf(pSkeletonDescription->szName, "Skeleton %d", i);
        pSkeletonDescription->skeletonID = index*100;
        pSkeletonDescription->nRigidBodies = 5;
        for(int j=0; j<pSkeletonDescription->nRigidBodies; j++)
        {
            int RBID = pSkeletonDescription->skeletonID + j;
            sprintf(pSkeletonDescription->RigidBodies[j].szName, "RB%d", RBID);
            pSkeletonDescription->RigidBodies[j].ID = RBID;
            pSkeletonDescription->RigidBodies[j].offsetx = 1.0f;
            pSkeletonDescription->RigidBodies[j].offsety = 2.0f;
            pSkeletonDescription->RigidBodies[j].offsetz = 3.0f;
            pSkeletonDescription->RigidBodies[j].parentID = 2;
        }
        pDescription->arrDataDescriptions[index].type = Descriptor_Skeleton;
        pDescription->arrDataDescriptions[index].Data.SkeletonDescription = pSkeletonDescription;
        pDescription->nDataDescriptions++;
        index++;
    }
#endif


}

// Send DataSet description to Client
void SendDescription(sDataDescriptions* pDescription)
{
	sPacket packet;
	theServer->PacketizeDataDescriptions(pDescription, &packet);
	theServer->SendPacket(&packet);
}

// Release any memory we allocated for our example
void FreeDescription(sDataDescriptions* pDescription)
{
    /*
    for(int i=0; i< pDescription->nDataDescriptions; i++)
    {
        if(pDescription->arrDataDescriptions[i].type == Descriptor_MarkerSet)
        {
            for(int i=0; i< pMarkerSetDescription->nMarkers; i++)
            {
                delete[] pMarkerSetDescription->szMarkerNames[i];
            }
            delete elete[] pMarkerSetDescription->szMarkerNames;
            sMarkerSetDescription* pMarkerSetDescription = pDescription->arrDataDescriptions[0].Data.MarkerSetDescription;
        }
        else if(pDescription->arrDataDescriptions[i].type == Descriptor_RigidBody)
        {
        */

}

// Build frame of MocapData
void BuildFrame(long FrameNumber, sDataDescriptions* pModels, sFrameOfMocapData* pOutFrame)
{	
	if(!pModels)
	{
		printf("No models defined - nothing to send.\n");
		return;
	}

	ZeroMemory(pOutFrame, sizeof(sFrameOfMocapData));
    pOutFrame->iFrame = FrameNumber;
    pOutFrame->fLatency = (float)GetTickCount(); //vllt nicht ganz so gut
    pOutFrame->nOtherMarkers = 0;
    pOutFrame->nMarkerSets = 0;
	pOutFrame->nRigidBodies = 0;
	pOutFrame->nLabeledMarkers = 0;

	const float rotanglesin = (float)sin((double)FrameNumber*(PId/720.0));
	const float rotanglecos = (float)cos((double)FrameNumber*(PId/720.0));
    for(int i=0; i < pModels->nDataDescriptions; i++)
    {
#if STREAM_MARKERS
        // MarkerSet data
        if(pModels->arrDataDescriptions[i].type == Descriptor_MarkerSet)
        {
            // add marker data
            int index = pOutFrame->nMarkerSets;
	        sMarkerSetDescription* pMS = pModels->arrDataDescriptions[i].Data.MarkerSetDescription;
	        strcpy(pOutFrame->MocapData[index].szName, pMS->szName);
	        pOutFrame->MocapData[index].nMarkers = pMS->nMarkers;
	        pOutFrame->MocapData[index].Markers = new MarkerData[pOutFrame->MocapData[0].nMarkers];
	        for(int iMarker=0; iMarker < pOutFrame->MocapData[index].nMarkers; iMarker++)
	        {	    
                double rads = (double)counter * PId / 180.0f;
		        pOutFrame->MocapData[index].Markers[iMarker][0] = (float) sin(rads) + (10*iMarker);		// x
		        pOutFrame->MocapData[index].Markers[iMarker][1] = (float) cos(rads) + (20*iMarker);		// y
		        pOutFrame->MocapData[index].Markers[iMarker][2] = (float) tan(rads) + (30*iMarker);		// z
                counter++;
                counter %= 360;
	        }
            pOutFrame->nMarkerSets++;

        }
#endif

#if STREAM_RBS
        // RigidBody data
        if(pModels->arrDataDescriptions[i].type == Descriptor_RigidBody)
        {
            sRigidBodyDescription* pMS = pModels->arrDataDescriptions[i].Data.RigidBodyDescription;
            int index = pOutFrame->nRigidBodies;
            sRigidBodyData* pRB = &pOutFrame->RigidBodies[index];

            pRB->ID = 0;
            pRB->x = 0.0f;
            pRB->y = 0.0f;
            pRB->z = 0.0f;

			pRB->qz = rotanglesin;
			pRB->qw = rotanglecos;

			pRB->nMarkers = 3;
            pRB->Markers = new MarkerData[pRB->nMarkers];
            pRB->MarkerIDs = new int[pRB->nMarkers];
            pRB->MarkerSizes = new float[pRB->nMarkers];
            pRB->MeanError = 0.0f;
			pRB->params = FrameNumber; //visible!!!!
            for(int iMarker=0; iMarker < pRB->nMarkers; iMarker++)
            {	    
                pRB->Markers[iMarker][0] = iMarker + 0.1f;		// x
                pRB->Markers[iMarker][1] = iMarker + 0.2f;		// y
                pRB->Markers[iMarker][2] = iMarker + 0.3f;		// z
                pRB->MarkerIDs[iMarker] = iMarker + 200;
                pRB->MarkerSizes[iMarker] = 77.0f;
            }
            pOutFrame->nRigidBodies++;
            counter++;
           
        }
#endif
    }

#if STREAM_LABELED_MARKERS
	// add marker data
	pOutFrame->nLabeledMarkers = 10;
	for(int iMarker=0; iMarker < 10; iMarker++)
	{	    
		sMarker* pMarker = &pOutFrame->LabeledMarkers[iMarker];
		pMarker->ID = iMarker+100;
		pMarker->x = (float) iMarker;
		pMarker->y = (float) (counter2 * iMarker);
		pMarker->z = (float) iMarker;
		pMarker->size = 5.0f;
	}
	counter2++;
	counter2 %= 100;
#endif

}

// Packetize and send a single frame of mocap data to the Client
void SendFrame(sFrameOfMocapData* pFrame)
{
	sPacket packet; 
	theServer->PacketizeFrameOfMocapData(pFrame, &packet);
	theServer->SendPacket(&packet);
}

// Release any memory we allocated for this sample
void FreeFrame(sFrameOfMocapData* pFrame)
{
    for(int i=0; i < pFrame->nMarkerSets; i++)
    {
        delete[] pFrame->MocapData[i].Markers;
    }

    for(int i=0; i < pFrame->nOtherMarkers; i++)
    {
        delete[] pFrame->OtherMarkers;
    }

    for(int i=0; i< pFrame->nRigidBodies; i++)
    {
        delete[] pFrame->RigidBodies[i].Markers;
        delete[] pFrame->RigidBodies[i].MarkerIDs;
        delete[] pFrame->RigidBodies[i].MarkerSizes;
    }

}

// PlayingThread_Func streams data at ~60hz 
DWORD WINAPI PlayingThread_Func(void * empty)
{
    while (1)
    {
        sFrameOfMocapData frame;
        BuildFrame(g_lCurrentFrame, &descriptions, &frame);
        SendFrame(&frame);
        FreeFrame(&frame);
        printf("Sent Frame %d", g_lCurrentFrame);
        g_lCurrentFrame++;
        HiResSleep(10);
    }

    return ErrorCode_OK;
}

int StartPlayingThread()
{
    SECURITY_ATTRIBUTES security_attribs;
    security_attribs.nLength = sizeof(SECURITY_ATTRIBUTES);
    security_attribs.lpSecurityDescriptor = NULL;
    security_attribs.bInheritHandle = TRUE;

    PlayingThread_Handle = CreateThread(&security_attribs, 0, PlayingThread_Func, NULL, 0, &PlayingThread_ID);
    if(PlayingThread_Handle ==NULL)
    {
        printf("Error creating play thread.");
        return -1;
    }
    else
        g_bPlaying = true;

    return ErrorCode_OK;
}

int StopPlayingThread()
{
    if (PlayingThread_Handle != NULL)
        TerminateThread(PlayingThread_Handle, 0);

    PlayingThread_Handle = NULL;
    g_bPlaying = false;

    return ErrorCode_OK;
}

void resetServer()
{
    int iSuccess = 0;
    char szIPAddress[128];
    theServer->IPAddress_LongToString(lAddresses[0], szIPAddress);	// use first IP detected

    printf("\nRe-initting Server\n\n");

    iSuccess = theServer->Uninitialize();
    if(iSuccess != 0)
        printf("Error uninitting server.\n");

    iSuccess = theServer->Initialize(szIPAddress);
    if(iSuccess != 0)
        printf("Error re-initting server.\n");

}

// MessageHandler receives NatNet error mesages
void __cdecl MessageHandler(int msgType, char* msg)
{
    printf("\n%s\n", msg);
}

// higer res sleep than standard.
int HiResSleep(int msecs)
{
	HANDLE hTempEvent = CreateEvent( 0, true, FALSE, _T("TEMP_EVENT") );
	timeSetEvent( msecs, 1, (LPTIMECALLBACK)hTempEvent, 0, TIME_ONESHOT | TIME_CALLBACK_EVENT_SET );
	WaitForSingleObject( hTempEvent, msecs );
	CloseHandle( hTempEvent );
	return 1;
}
