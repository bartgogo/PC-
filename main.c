#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "serial.h"

#define FILENAME "example.txt" // Ҫ���͵��ļ���

// ��������
int sendFileToLinux(const char *filename, PORT com_port);

int main() {
    PORT com_port;
    int ret;

    // ��ʼ������
    com_port = serial_init(1, 115200, 8, ONESTOPBIT, NOPARITY);
    if (com_port == NULL) {
        printf("Failed to open serial port.\n");
        return 1;
    }

    // �����ļ��� Linux ������
    ret = sendFileToLinux(FILENAME, com_port);
    if (ret != 0) {
        printf("Failed to send file to Linux board.\n");
    } else {
        printf("File sent successfully.\n");
    }

    // �رմ���
    ClosePort(com_port);

    return 0;
}

// �������壺�����ļ��� Linux ������
int sendFileToLinux(const char *filename, PORT com_port) {
    FILE *file;
    char buffer[1024];
    size_t bytes_read;
    int bytes_sent;

    // ���ļ�
    file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Failed to open file '%s'.\n", filename);
        return 1;
    }

    // ��ȡ�ļ����ݲ����͵�����
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        bytes_sent = SendData(com_port, buffer, bytes_read);
        if (bytes_sent != bytes_read) {
            printf("Failed to send file data.\n");
            fclose(file);
            return 1;
        }
        // �ȴ�һ��ʱ�䣬�Ա� Linux �������ܹ�������յ�����
        Sleep(10);
    }

    // �ر��ļ�
    fclose(file);

    return 0;
}














/*
#include <stdio.h>
#include "serial.h"
#include <windows.h>

int serial_test()
{
    PORT COM1;
    char buff[1024] = {0};
    int rcv_len = 0;

    printf("open com1\n");
    COM1 = serial_init(1, 115200, 8, 1, 0);

    while(1)
    {
        Serial_SendData(COM1, "hello world\n", 13);
        memset(buff, 0, 1024);
        rcv_len = Serial_ReciveData(COM1, buff, 1024);
        printf("rcv:%s\n", buff);
        Sleep(1);
    }
}
int main()
{
    serial_test();
    while (1)
    {
    }

    return 0;
}
*/
