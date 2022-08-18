#include <arpa/inet.h> //definitions for internet operations
#include <sys/types.h> //The data types
#include <sys/param.h> //Сдесь хранятся константы
#include <sys/socket.h> //Fot Internet Protocol
#include <sys/stat.h>  //data returned
#include <sys/time.h>  //The time types
#include <netinet/in_systm.h> //The Internet address family in sys
#include <netinet/in.h> //The Internet address family
#include <netinet/ip_icmp.h> //For ICMP
#include <netdb.h> //for network database operations
#include <unistd.h> //standard symbolic constants and types
#include <stdio.h> //standard input/output header 
#include <ctype.h> //Character handling functions
#include <ctime> //Getting the date and time
#include <cstring> //for string 
#include <fcntl.h> //Creating or overwriting a file
#include <string.h> //for string 
#include <sstream> //providing string stream classes
#include <fstream> // To write to a file
#include <errno.h> // To work with error numbers
#include <stdlib.h> //for memory allocation routines
#include <stdint.h> //header defines integer types
#include <iostream> //standard input/output header

#define MAX_PACKET 1024 //Размер буфера для приходящего пакета
#define ICMP_SIZE 64 //Размер посылаемого пакета
#define MaxSize 2000000 // Размер файла лога после которого он будет считаться переполненным



//Функции программы

int createLogF(); //Создание лог файла
int checkArgs(int); //Проверка колличестов входных аргументов
int dnsCheck(char **argv); //DNS or IP (DNStoIP)
int assembling(); //Сборка пакета
int request(); //Отправка пакета
int response(); //Прием пакета
int finish (); //Завершение


//Функции лога

int isLogExist();// Для проверки существования или переполненности лога
int createLog(); // Создание лога
int ErrorOutput(int TypeError);// Вывод ошибок возникших в работе программы
int AddMessageToLog(std::string );// Запись сообщения в лог
void LogfileDiag(int);// Диагностика ошибок лога