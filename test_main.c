#include <LibPad/PsPad.h>

#include <stdio.h>

int scanUsbDevices();
const char* getUsbDevicePath(int id);
struct PsIoPhy* createUsbPsIoPhy(const char* usbDevicePath);
void destroyUsbPsIoPhy(struct PsIoPhy*);

void printRxBuffer(struct PsIoBuffer* buffer){
    int i;
    for(i = 0; i < buffer->size; ++i){
        printf("0x%02x, ", (int)buffer->data[i]);
    }
    printf("\n");
}

const char* findUsbDevice(){
    scanUsbDevices();
    return getUsbDevicePath(0);
}

int main() {
    struct PsIoPhy* phy;
    struct PsIoPort port;
    int i;
    const char* usbDevicePath = findUsbDevice();

    phy = createUsbPsIoPhy(usbDevicePath);

    port.id = 1;
    port.phy = phy;
    port.rxBuffer.size = 0;

    struct PsPad pad;
    pad.slot = 1;
    pad.port = &port;

    struct PsIoBuffer* rxBuffer = &pad.port->rxBuffer;

    psPadRead(&pad);
    psPadConfigEnter(&pad);
    for(i = 0; i < 5; ++i){
        psPadConfigQueryMode(&pad, i);
        printRxBuffer(rxBuffer);
    }

    psPadConfigExit(&pad);

    destroyUsbPsIoPhy(phy);
    return 0;
}
