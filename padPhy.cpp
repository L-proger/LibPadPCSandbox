#include <LFramework/USB/Host/UsbHostManager.h>
#include <LFramework/USB/Host/UsbHDevice.h>
#include <LFramework/Debug.h>

#include <iostream>

extern "C" {
#include <LibPad/PsPad.h>
}

std::string findDevice(){
    auto devices = UsbHostManager::enumerateDevices();

    for(auto& device : devices){
        if(device.vid == 0x0301 && device.pid == 0x1111){
            return device.path;
        }
    }
    return {};
}


enum class UsbRequestType : uint8_t {
    PollBegin,
    Poll,
    EndPoll
};

struct UsbRequest {
    UsbRequestType type;
    uint8_t data;
};

struct UsbResponse {
    uint8_t data;
};

static int ioBegin(void* context);
static int ioShift(void* context, uint8_t txData, uint8_t* rxData);
static void ioEnd(void* context);

class UsbPadPhy : public PsIoPhy {
public:
    UsbPadPhy(const std::string& path) : _device(path){
        auto bulkInterface = _device.getInterface(0);
        _txEp = bulkInterface->getEndpoint(false, 0);
        _rxEp = bulkInterface->getEndpoint(true, 0);

        //reset connection
        auto txFuture = _txEp->transferAsync(nullptr,  0);

        uint8_t rxBuffer[64];

        while(true){
            auto rxFuture = _rxEp->transferAsync(rxBuffer, sizeof(rxBuffer));
            auto rxSize = rxFuture->wait();
            if(rxSize == 0){
                break;
            }
        }
        txFuture->wait();

        this->context = this;
        this->begin = &ioBegin;
        this->shift = &ioShift;
        this->end = &ioEnd;
        this->init = nullptr;
    }
    bool implBegin() {
        poll({UsbRequestType::PollBegin, 0xff});
        return true;
    }
    bool implShift(std::uint8_t txData, std::uint8_t& rxData) {

        rxData = poll({UsbRequestType::Poll, txData}).data;
        //std::cout << "PHY: shift = " << std::hex << "TX: 0x" <<  (int)txData  << " RX: 0x" <<  (int)rxData << std::endl;
        return true;
    }
    void implEnd() {
          //std::cout << "PHY: end" << std::endl;
        poll({UsbRequestType::EndPoll, 0xff});
    }
private:
    UsbResponse poll(UsbRequest request) {
        auto txFuture = _txEp->transferAsync(&request, sizeof(request));
        txFuture->wait();

        uint8_t rxBuffer[64];

        auto rxFuture = _rxEp->transferAsync(rxBuffer, sizeof(rxBuffer));
        auto rxSize = rxFuture->wait();

        if(rxSize != sizeof(UsbResponse)){
            std::cout << "Invalid RX size" << std::endl;
            return {0xff};
        }else{
            return *((UsbResponse*)(&rxBuffer[0]));
        }
    }
    LFramework::USB::UsbHDevice _device;
    LFramework::USB::UsbHEndpoint* _rxEp;
    LFramework::USB::UsbHEndpoint* _txEp;
};


int ioBegin(void* context){
    auto _this = (UsbPadPhy*)context;
    return _this->implBegin() ? PS_IO_RESULT_OK : PS_IO_RESULT_FAIL;
}
int ioShift(void* context, uint8_t txData, uint8_t* rxData){
    auto _this = (UsbPadPhy*)context;
    return _this->implShift(txData, *rxData) ? PS_IO_RESULT_OK : PS_IO_RESULT_FAIL;
}
void ioEnd(void* context){
    auto _this = (UsbPadPhy*)context;
    _this->implEnd();
}


static std::vector<std::string> _devices;

extern "C" {

int scanUsbDevices(){
    auto devices = UsbHostManager::enumerateDevices();
    for(auto& device : devices){
        if(device.vid == 0x0301 && device.pid == 0x1111){
            _devices.push_back(device.path);
        }
    }
    return _devices.size();
}

const char* getUsbDevicePath(int id){
    if(id >= (int)_devices.size()){
        return nullptr;
    }
    return _devices[id].c_str();
}

PsIoPhy* createUsbPsIoPhy(const char* usbDevicePath){
    return new UsbPadPhy(usbDevicePath);
}
void destroyUsbPsIoPhy(PsIoPhy* phy){
    delete phy;
}

}
