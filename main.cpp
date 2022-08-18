#include "main.h"

/*
======ТЕЛО ПРОГРАММЫ=======
*/

int main(int argc, char *argv[])
{
    int req = 0;
    setlocale(LC_ALL, "Russian");
    switch (createLogF()) //Создание лог файла
    {
    case 0:
        switch (checkArgs(argc)) //Проверка колличестов входных аргументов
        {
        case 0:         
            switch (dnsCheck(argv))//DNS or IP (DNStoIP)
            {
            case 0:
                switch (assembling()) //Сборка пакета
                {
                case 0:
                    while (req <= 5)
                    {
                        req ++;
                        switch (request())//Отправка пакета
                        {
                        case 0:
                            switch (response())//Прием пакета
                            {
                            case 0:
                                continue;
                            
                            case 1:
                                printf ("Error code = 60\n");
                                return 60;
                                break;
                            }                            
                        case 1:
                            printf ("Error code = 50\n");
                            return 50;
                            break;
                        }
                        
                    }
                    
                    break;
                case 1:
                    printf ("Error code = 40\n");
                    return 40;
                    break;
                }                
                break;
            case 1:
                printf ("Error code = 30\n");
                return 30;
                break;
            }
            break;
        case 1:
            printf ("Error code = 20\n");
            return 20;
            break;
        }
        break;
    case 1:
        printf ("Error code = 10\n");
        return 10;
        break;
    }

    //finish(); //Завершение
    return 0;
}
