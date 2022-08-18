 #include "main.h"


char* username;  // Переменная под имя пользователя
char log_buff[128]; //Переменная под сборку пути к логу
bool Overflow;   // Переменная показывающая не переполнен ли лог
int Errornum;  // Переменная под хранение номера ошибки

int isLogExist()                                                                   // Функция проверки наличия и переполнения лога
{
    struct stat logStat;  // Создание структуры информации о файле
    username = getenv("USER"); // Получение имени пользователя
    sprintf(log_buff, "%s%s%s", "/home/",username, "/Desktop/Ping/Ping_log.txt" ); // Сборка пути
    if (  stat((char *)log_buff, &logStat) == -1 ) // Заполнение структуры информации о файле
    {
        return 1;// Возвращаем 1 говоря что файла не существует
    }
    else if (logStat.st_size > MaxSize)
    {
        std::cout << "Файл лога переполнился и будет перезаписан";
        Overflow = true;
        return 1;
    }
    return 0;// Возвращаем 0 говоря что файла существует                                                                   // Возвращаем 0 говоря что файла существует
}

int createLog()                                                                     // Функция создания папки и лога
{
    char path_buff[128];//Переменная под сборку команды
	int result;// Переменная под результат создания папки


	sprintf(path_buff, "%s%s%s","mkdir -p /home/", username, "/Desktop/Ping" );// Сборка команды создания директории
	result = system(path_buff);// Создание директории
	if (result == -1) // Обработка ошибки
	{
		ErrorOutput(10);
		return 1;
	}

	if (Overflow)
	{
	   result = open((char*)log_buff, O_RDWR | O_TRUNC); // Перезапись файла лога
	}
	else
	{
		result = open((char*)log_buff, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);// Создание файла
	}
	if (result == -1) // Обработка ошибки
	{
		Errornum = errno;
		LogfileDiag(Errornum);
		ErrorOutput(11);
		return 1;
	}
	close(result);
	return 0;

}

int ErrorOutput(int TypeError) // Функция записи ошибки в лог
{

    switch(TypeError)
	{
		case 10:
		{
			std::cout <<"Ошибка создания директории для лога"<<std::endl;
			break;
		}
		case 11:
		{
			std::cout <<"Ошибка создания файла лога"<<std::endl;
			break;
		}
		case 12:
		{
			std::cout <<"Ошибка записи в лог"<<std::endl;
			break;
		}
		case 20:
		{
			AddMessageToLog("ERROR Неверное количество входных аргументов");
			break;
		}
		case 30:
		{
			AddMessageToLog("ERROR Доменное имя не соответствует никакому IP");
			break;
			
		}
		case 40:
		{
			AddMessageToLog("ERROR Пакет не собран");
			break;       
		}
		case 41:
		{
			AddMessageToLog("ERROR malloc");
			break;       
		}
		case 42:
		{
			AddMessageToLog("ERROR Недостаточно прав");
			break;       
		}
		case 50:
		{
			AddMessageToLog("ERROR При отправке возникла ошибка");
			break;
		}
		case 60:
		{
			AddMessageToLog("ERROR Пакет не получен");
			break;
		}
		case 61:
		{
			AddMessageToLog("ERROR select()");
			break;
		}
		case 62:
		{
			AddMessageToLog("ERROR Не соответсвие разменра пакета ");
			break;
		}
		case 63:
		{
			AddMessageToLog("ERROR Нет данных о ноде");
			break;
		}

	}
	return 0;

}

void LogfileDiag(int TypeError)
{
	switch(TypeError)// Вывод сообщения в зависимости от значения errno
	{
		case 1:
		{
			std::cout <<"Операция не разрешена";
			break;
		}
		case 12:
		{
			std::cout <<"Не достаточно памяти";
			break;
		}
		case 13:
		{
			std::cout <<"Доступ запрещен";
			break;
		}
		case 26:
		{
			std::cout <<"Текстовый файл занят";
			break;
		}
	 default:
		{
			std::cout <<"Неопознанная ошибка код errno = " <<  TypeError;
			break;
		}
	}
}

int AddMessageToLog(std::string Message) // Функция добавления записи в лог
{
    char buff[512];// Буфер для формирования записи в лог
    char *date;// Переменная для получения текущей даты
    time_t now;// Структура для получения времени
    date = NULL;

    char *Mess = new char[Message.length() + 1];// Создание массива под содержимое сообщения
    strcpy(Mess, Message.c_str());// Копирование сообщения в массив

    time(&now);// Заполняем структуру
    date = ctime(&now);
    date[strlen(date)-1] = '\0';
    sprintf(buff, "[%s] %s", date, Mess);// Формирование записи в лог
           
    std::fstream fs;
    fs.open(log_buff, std::fstream::app);// Открытие файла
    if (!fs.is_open())
    {
          Errornum = errno;
          LogfileDiag(Errornum);
          ErrorOutput(12);
          return 1;
    }
    fs << buff << std::endl;
    fs.close();       
    delete [] Mess;// Удаление массива   
    return 0;
  
}