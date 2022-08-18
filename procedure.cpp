#include "main.h"

#define DEBUG(MS) printf(MS) 



struct sockaddr_in saServer, from; //Информация о сокете
std::string hostname; //Имя сервера
struct hostent *hp = NULL;//Информация о хосте
u_char *packet [MAX_PACKET], *recvbuf = NULL; //Для ICMP пакета один для отправки, другой для примема 
struct icmp *icp; //Структура для icmp пакета
int req = 0; //Переменная для подсчета отправлений
int sock; //переменная для сокета
struct timeval start, end; // Для фиксации времени
fd_set rfds; //добавляют заданный описатель к набору
struct timeval tv; //для select 
struct ip *ip; //Структра ip
int no_data = 0; //Для счета количество провалов

static uint16_t in_cksum(uint16_t *addr, unsigned len); //Чек сумма

int createLogF(){
    //DEBUG ("Create log\n");

    if (isLogExist() == 1)
    {
       int i = createLog();
       if (i == 1){
           return 1;
       } 
       return 0;
       AddMessageToLog("Start program");
    }
    AddMessageToLog("Start program");
    return 0;
}

int checkArgs(int argc){//Проверка колличестов входных аргументов

    //DEBUG ("Check enter arg\n");

    if (argc != 2) {
	    printf("usage: ip/dns address\n");
        ErrorOutput(20);
		return 1; 
	}

    return 0;
}

int dnsCheck(char * argv[]){

   // DEBUG ("Check dns or ip\n");

    std::string target = argv[1]; //Меняем аргумент из char in str
    std::stringstream ss; //Для преоброзавние строки
    std::string strOut; //Для хранения преобразованной строки
    char hnamebuf[MAXHOSTNAMELEN]; // буфер для hostname
    saServer.sin_family = AF_INET; //Назначаем принадлежность к ipv4

    saServer.sin_addr.s_addr = inet_addr(target.c_str()); //Назначаем адрес
    if (saServer.sin_addr.s_addr != (u_int)-1) //Проверяем тип адреса 
        hostname = target; //Если адрес ip то не меняем адрес
    else  //Если адрес DNS
    {
        hp = gethostbyname(target.c_str()); //Ищем в базе ip входящего DNS
        if (!hp)
        {
            printf ("Unkown host");
            ErrorOutput(30);
            return 1;
        }
        //Переопределям тип и имя
        saServer.sin_family = hp->h_addrtype; 
        bcopy(hp->h_addr, (caddr_t)&saServer.sin_addr, hp->h_length);
        strncpy(hnamebuf, hp->h_name, sizeof(hnamebuf) - 1);
        hostname = hnamebuf;
    }
    ss << "Host IP: " << inet_ntoa (saServer.sin_addr) << std::endl;
    strOut = ss.str();
    printf(strOut.c_str());
    AddMessageToLog(strOut);
    return 0;
}

int assembling(){ //Собираем пакет 
    
    //DEBUG ("Assembling packet\n");

    if ( (recvbuf = (u_char *)malloc((u_int)MAX_PACKET)) == NULL) //Создаем буфер для входящих пакетов
    {
        printf ("malloc error\n");
        ErrorOutput(40);
        ErrorOutput(41);
        return 1;
    }

    if ( (sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) //Создаем сокет
    {
        printf ("Needs to run as superuser!!\n ");
        ErrorOutput(40);
        ErrorOutput(42);
        return 1; /* Needs to run as superuser!! */
    }
    //Определям заголовок пакета
    icp = (struct icmp *)packet;
    icp->icmp_type = ICMP_ECHO; // Тип пакета
    icp->icmp_code = 0; 
    icp->icmp_cksum = 0; //сумма пакета (изменется дальше)
    icp->icmp_seq = 12345; //последоваетльность
    icp->icmp_id = getpid(); //id пакета

    icp->icmp_cksum = in_cksum((unsigned short *)icp,ICMP_SIZE); //сума пакета

    FD_ZERO(&rfds);
    FD_SET(sock, &rfds);
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    return 0;
}

int request(){

    //DEBUG ("Request\n");

    sleep(1);
  	gettimeofday(&start, NULL); //фиксация времени отправки
    //отправляем пакет и проверяем получилось ли
  	int i = sendto(sock, (char *)packet, ICMP_SIZE, 0, (struct sockaddr*)&saServer, (socklen_t)sizeof(struct sockaddr_in));
	
    if (i < 0){
    	printf("sendto error");
        ErrorOutput(50);
        return 1;
    }

    return 0;
}

int response(){

    //DEBUG ("Response\n");

    int fromlen, ret, hlen, end_t;
    std::stringstream ss; //Для преоброзавние строки
    std::string strOut; //Для хранения преобразованной строки
    // блокируемся пока не получим данные или не истечет время
    int retval = select(sock+1, &rfds, NULL, NULL, &tv);
    if (retval == -1)
    {
        perror("select()");
        ErrorOutput(60);
        ErrorOutput(61);
        return 1;
    }
    else if (retval)
    {
        fromlen = sizeof(sockaddr_in);
        if ( (ret = recvfrom(sock, (char *)recvbuf, MAX_PACKET , 0,(struct sockaddr *)&from, (socklen_t*)&fromlen)) < 0)
        {
            perror("recvfrom error");
            ErrorOutput(60);
            return 1;
        }

        // Check the IP header
        ip = (struct ip *)((char*)recvbuf);
        hlen = sizeof( struct ip ); 
        if (ret < (hlen + ICMP_MINLEN)) //Не соответсвие разменра пакета
        { 
            std::cout << "packet too short ( " << ret <<" bytes) from "<< hostname <<" hostname" << std::endl;
            ErrorOutput(60);
            ErrorOutput(62);
            return 1; 
        } 

        icp = (struct icmp *)(recvbuf + hlen); 
        if (icp->icmp_type == ICMP_ECHOREPLY)
        {
            if (icp->icmp_seq != 12345) //не соответсвие последовательности
                printf ("received sequence # %c \n", icp->icmp_seq);
            if (icp->icmp_id != getpid()) //не правильный id пакета
                printf ("received id %c \n", icp->icmp_id);
        }
        else{
            printf ("Recv: not an echo reply \n");
            ErrorOutput(60);
            return 1;
        }
    
        gettimeofday(&end, NULL); //фиксация время получения пакета
        end_t = 1000000*(end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec);

        if(end_t < 1)
            end_t = 1;

        ss << "Ping host: " << inet_ntoa(from.sin_addr) 
                << " time = " << end_t << "usec " << std::endl;
        strOut = ss.str();
	    printf(strOut.c_str());
        AddMessageToLog(strOut);
        return 0;

    }
    else 
    {
        if (no_data == 5){ 
            printf ("No data about node.\n");
            ErrorOutput(60);
            ErrorOutput(63);
            return 1;
        }
        else{
            printf ("No data within one seconds.\n");
            AddMessageToLog("No data within one seconds.\n");
            return 0;
        }
    }

}

static uint16_t in_cksum(uint16_t *addr, unsigned len)
{
    //DEBUG ("Chek Summa\n");
    uint16_t answer = 0;
    /*
    * Algorithm is simple, using a 32 bit accumulator (sum), add
    * sequential 16 bit words to it, and at the end, fold back all the
    * carry bits from the t   16 bits into the lower 16 bits.
    */
    uint32_t sum = 0;
    while (len > 1)  {
        sum += *addr++;
        len -= 2;
    }

    if (len == 1) {
        *(unsigned char *)&answer = *(unsigned char *)addr ;
        sum += answer;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = ~sum;
    return answer;
}