//#include <LFramework/USB/Host/UsbHostManager.h>
//#include <LFramework/USB/Host/UsbHDevice.h>
//#include <LFramework/Debug.h>
//#include <iostream>
//#include <iomanip>

//#include <thread>
//#include <chrono>
//#include <iomanip>

//#include <LibPad/PlayStation.h>

//#pragma comment(lib, "User32")
//#pragma comment(lib, "Advapi32")
//#pragma comment(lib, "Ole32")
//#pragma comment(lib, "Winusb")
//#pragma comment(lib, "SetupAPI")

//std::string findDevice(){
//    auto devices = UsbHostManager::enumerateDevices();

//    for(auto& device : devices){
//        if(device.vid == 0x0301 && device.pid == 0x1111){
//            return device.path;
//        }
//    }
//    return {};
//}


//enum class UsbRequestType : uint8_t {
//    PollBegin,
//    Poll,
//    EndPoll
//};

//struct UsbRequest {
//    UsbRequestType type;
//    uint8_t data;
//};

//struct UsbResponse {
//    uint8_t data;
//};

//class UsbPadPhy : public PadPhy{
//public:
//    UsbPadPhy(const std::string& path) : _device(path){
//        auto bulkInterface = _device.getInterface(0);
//        _txEp = bulkInterface->getEndpoint(false, 0);
//        _rxEp = bulkInterface->getEndpoint(true, 0);

//        //reset connection
//        auto txFuture = _txEp->transferAsync(nullptr,  0);

//        uint8_t rxBuffer[64];

//        while(true){
//            auto rxFuture = _rxEp->transferAsync(rxBuffer, sizeof(rxBuffer));
//            auto rxSize = rxFuture->wait();
//            if(rxSize == 0){
//                break;
//            }
//        }
//        txFuture->wait();
//    }
//    bool begin(std::uint8_t padId, std::uint8_t& data) override {

//        data = poll({UsbRequestType::PollBegin, padId}).data;
//         //std::cout << "PHY: begin = " << std::hex << "TX: 0x" <<  (int)padId  << " RX: 0x" <<  (int)data << std::endl;
//        if(data != 0xff){
//            //std::cout << "Unexpected response for PollBegin request" << std::endl;
//          //  return false;
//        }
//        return true;
//    }
//    bool shift(std::uint8_t txData, std::uint8_t& rxData) override {

//        rxData = poll({UsbRequestType::Poll, txData}).data;
//        //std::cout << "PHY: shift = " << std::hex << "TX: 0x" <<  (int)txData  << " RX: 0x" <<  (int)rxData << std::endl;
//        return true;
//    }
//    void end() override {
//          //std::cout << "PHY: end" << std::endl;
//        poll({UsbRequestType::EndPoll, 0xff});
//    }
//private:
//    UsbResponse poll(UsbRequest request) {
//        auto txFuture = _txEp->transferAsync(&request, sizeof(request));
//        txFuture->wait();

//        uint8_t rxBuffer[64];

//        auto rxFuture = _rxEp->transferAsync(rxBuffer, sizeof(rxBuffer));
//        auto rxSize = rxFuture->wait();

//        if(rxSize != sizeof(UsbResponse)){
//            std::cout << "Invalid RX size" << std::endl;
//            return {0xff};
//        }else{
//            return *((UsbResponse*)(&rxBuffer[0]));
//        }
//    }
//    LFramework::USB::UsbHDevice _device;
//    LFramework::USB::UsbHEndpoint* _rxEp;
//    LFramework::USB::UsbHEndpoint* _txEp;
//};

//void delayUs(std::size_t delay){
// //std::this_thread::sleep_for(std::chrono::milliseconds(30));
//}


//void printRxBuffer(const PadInterface::CommandRxBuffer& buffer) {
//    for(int i = 0; i < buffer.length; ++i){
//        std::cout << "0x" << std::hex << std::setfill('0') << std::setw(2) << (int)buffer.data[i] << ", ";
//    }
//    std::cout << std::endl;
//}

///*
//#define QUERY(query) \
//        if(pad.query()){ \
//            std::cout << #query " response: \t" ; \
//            printRxBuffer(data); \
//        }else{ \
//            std::cout << #query " FAILED" << std::endl; \
//        }


//        uint8_t cmd[]={ 0x01, 0x43, 0x5a, 0x00, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A };

//        pad.configEnter();
//        pad.setControllerMode(true, false);

//        for(int i = 0; i <= 0xff; ++i){
//            if(i != 0x43){
//                cmd[1] = i;
//                pad.executeCommand(cmd, sizeof(cmd));
//                std::cout << std::hex << std::setw(2) << std::setfill('0') << "[0x" << i << "]: ";
//                printRxBuffer(data);
//            }

//        }
//*/


//void scanCommands(PadInterface& pad) {
//     const auto& data = pad.getRxBuffer();

//    //uint8_t cmd[]={ 0x01, 0x43, 0x00, 0x00, 0x00 };

//     uint8_t cmd[5] = {};//={ 0x01, 0x43, 0x00, 0x00, 0x00 };
//     cmd[0] = 0x01;
//        cmd[1] = 0x42;

//    for(int i = 0; i < 0xff; ++i){
//        cmd[0]  = i;
//        pad.executeRawCommand(cmd, sizeof(cmd));
//        std::cout << std::hex << std::setw(2) << std::setfill('0') << "[0x" << i << "]: ";
//        printRxBuffer(data);
//        std::this_thread::sleep_for(std::chrono::milliseconds(1));
//    }
//}

//struct MouseState {
//    std::int8_t dx;
//    std::int8_t dy;
//    bool lmbPressed;
//    bool rmbPressed;

//    bool operator == (const MouseState& other){
//        return (dx == other.dx) && (dy == other.dy) && (lmbPressed == other.lmbPressed) && (rmbPressed == other.rmbPressed);
//    }

//    bool operator != (const MouseState& other){
//        return (dx != other.dx) || (dy != other.dy) || (lmbPressed != other.lmbPressed) || (rmbPressed != other.rmbPressed);
//    }
//};

//bool readMouse(PadInterface& pad, MouseState& result) {
//    pad.read();
//    const auto& data = pad.getRxBuffer();

//    if(data.data[1] != 0x12){
//        std::cout << "Not mouse connected!" << std::endl;
//        return false;
//    }else{
//        auto buttons = data.data[4];
//        result.lmbPressed = (buttons & (1 << 3)) == 0;
//        result.rmbPressed = (buttons & (1 << 2)) == 0;
//        result.dx =  data.data[5];
//        result.dy =  data.data[6];
//    }
//}

//int main() {

//    lfDebug() << "Test";
//    auto devicePath = findDevice();

//    if(!devicePath.empty()){
//        std::cout << "Device found!" << std::endl;

//        UsbPadPhy phy(devicePath);
//        PadInterface pad(&phy);

//        const auto& data = pad.getRxBuffer();


//        //pad.configEnter();
//       // pad.setControllerMode(true, false);

//        //configQueryAct b0 any except 0 - fail


//        //0xff, 0xf3, 0x5a, 0x00, 0x00, 0x01, 0x02, 0x00, 0x0a,  //configQueryAct b1 = 0
//        //0xff, 0xf3, 0x5a, 0x00, 0x00, 0x01, 0x01, 0x01, 0x14,  //configQueryAct b1 = 1


//        //InfoActFunc 1 == continuous-rotary-vibrator
//        //InfoActSub  2 == fast-rotation, 1 == slow-rotation
//        //InfoActSize -> maybe control bytes count? 0 == 1 bit, 1 == 1 byte
//        //Current (ma) -> divided by 10.  value 10 == 10*10 = 100mA


//       // scanCommands(pad);



//        uint8_t cmdReapMtap[35] = {};
//                cmdReapMtap[0] = 0x01;
//                cmdReapMtap[1] = 0x42;
//                cmdReapMtap[2] = 0x01;

//               /* cmdReapMtap[3] = 0x42;
//                cmdReapMtap[11] = 0x42;
//                cmdReapMtap[19] = 0x42;
//                cmdReapMtap[27] = 0x42;*/




//      //  pad.executeRawCommand(cmd, sizeof(cmd));
//    //    printRxBuffer(data);

//        for(int i = 1; i < 0x5; ++i){
//            pad.configQueryMode()
//            pad.executeRawCommand(cmdReapMtap, sizeof(cmdReapMtap));

//            std::cout << i << ": ";
//            printRxBuffer(data);
//        }



//        /*MouseState state = {};

//        while(true){

//            MouseState newState;
//            readMouse(pad, newState);

//            if(state != newState){
//                std::cout << "LMB: " << std::boolalpha << newState.lmbPressed << " RMB: " << newState.rmbPressed << " DX: " << (int)newState.dx << " DY: " << (int)newState.dy << std::endl;
//            }

//            state = newState;

//            //pad.configQueryComb();
//            //pad.configQueryAct(1);
//           // printRxBuffer(data);

//            std::this_thread::sleep_for(std::chrono::milliseconds(20));
//        }*/

//    }else{
//        std::cout << "Failed to find device" << std::endl;
//    }

//    return 0;
//}
