#define BOOTSIZE 512

//�������� ������������� �������� ������ (7500 * 0,000032 � ~= 0,5 ���) ��������. �������� ���������� ��� ������ ������������ ����������
#define BOOTLOADER_TIMEOUT 15000UL
//���������� ������� (��������� BOOTLOADER_TIMEOUT) ��������� ������ �� ������� (0,5 * 4 = 2 ���)
#define BOOTLOADER_WAIT_ATTEMPTS 4