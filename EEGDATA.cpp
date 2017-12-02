#include "CodeCompletion.h"
#include <iostream>


#include <unistd.h>
#include <cmath>

#include "Iedk.h"
#include "IedkErrorCode.h"
#include "IEmoStateDLL.h"

#include <iostream>
#include <vector>

using std::cout;

const IEE_MotionDataChannel_t targetChannelList[] = {
    IMD_COUNTER,
    IMD_GYROX,
    IMD_GYROY,
    IMD_GYROZ,
    IMD_ACCX,
    IMD_ACCY,
    IMD_ACCZ,
    IMD_MAGX,
    IMD_MAGY,
    IMD_MAGZ,
    IMD_TIMESTAMP
};


std::unique_ptr<void, void(*)(DataHandle)> hMotionData(IEE_MotionDataCreate(), &IEE_MotionDataFree);
/*
EmoEngineEventHandle hEvent;
EmoStateHandle eState;
unsigned int userID;
int state           = 0;
bool ready          = false;

GyroDATA GetGyroDATA()
{
    GyroDATA returnvalue;
    int state = IEE_EngineGetNextEvent(hEvent);

    if (state == EDK_OK)
    {
        IEE_Event_t eventType = IEE_EmoEngineEventGetType(hEvent);
        IEE_EmoEngineEventGetUserId(hEvent, &userID);

        if (eventType == IEE_UserAdded) {
             std::cout << "User added" << std::endl << std::endl;
             userID = 0;
             ready = true;
        }
    }

    int gyroX = 0, gyroY = 0;
    int err = IEE_HeadsetGetGyroDelta(userID, &gyroX, &gyroY);

    if (err == EDK_OK){
        IEE_MotionDataUpdateHandle(0, hMotionData.get());
        unsigned int nSamplesTaken = 0;
        IEE_MotionDataGetNumberOfSample(hMotionData.get(), &nSamplesTaken);

        if (nSamplesTaken != 0) {
            std::vector<double> data(nSamplesTaken);
            for (int sampleIdx = 0; sampleIdx<(int)nSamplesTaken; ++sampleIdx) {
                for (int i = 0;
                    i<sizeof(targetChannelList) / sizeof(IEE_MotionDataChannel_t);
                    i++) {

                    IEE_MotionDataGet(hMotionData.get(), targetChannelList[i],
                        data.data(), data.size());
                    if (i == 1){
                        returnvalue.x = data[sampleIdx];
                        std::cout << "GyroX =" << data[sampleIdx];
                    }
                    if (i == 2)
                    {
                        returnvalue.y = data[sampleIdx];
                        std::cout << "GyroY =" << data[sampleIdx];
                        std::cout << std::endl;
                    }
                }

            }
        }
    }else if (err == EDK_GYRO_NOT_CALIBRATED){
        returnvalue.x = 0;
        returnvalue.y = 0;
        std::cout << "Gyro is not calibrated. Please stay still." << std::endl;
    }else if (err == EDK_CANNOT_ACQUIRE_DATA){
        returnvalue.x = 0;
        returnvalue.y = 0;
        std::cout << "Cannot acquire data" << std::endl;
    }else{
        returnvalue.x = 0;
        returnvalue.y = 0;
        std::cout << "No headset is connected" << std::endl;
    }

    return returnvalue;
}

void EEGINIT()
{
    hEvent = IEE_EmoEngineEventCreate();
    eState = IEE_EmoStateCreate();

    userID = -1;
	
	if (IEE_EngineConnect() != EDK_OK) {
		std::cout << "Emotiv Driver start up failed.";
        return;
	}
	
	std::cout << "Please make sure your headset is on and don't move your head.";
	std::cout << std::endl;
    
    IEE_MotionDataSetBufferSizeInSec(1);
}

void EEGCLOSE()
{
    IEE_EngineDisconnect();
    IEE_EmoStateFree(eState);
    IEE_EmoEngineEventFree(hEvent);
}
*/
